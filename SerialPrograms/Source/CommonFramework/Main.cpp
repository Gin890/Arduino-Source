
#include <QApplication>
#include "Common/Cpp/Exceptions.h"
#include "PersistentSettings.h"
#include "Tests/CommandLineTests.h"
#include "CrashDump.h"
#include "Environment/HardwareValidation.h"
#include "Logging/LoggerQt.h"
#include "Logging/OutputRedirector.h"
#include "Tools/StatsDatabase.h"
#include "Integrations/SleepyDiscordRunner.h"
#include "GlobalSettingsPanel.h"
#include "Windows/DpiScaler.h"
#include "Windows/MainWindow.h"

//#include <QTextStream>

#include <iostream>
using std::cout;
using std::endl;


using namespace PokemonAutomation;

Q_DECLARE_METATYPE(std::string)


int main(int argc, char *argv[]){
    setup_crash_handler();

//#if QT_VERSION_MAJOR == 5 // AA_EnableHighDpiScaling is deprecated in Qt6
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

    QApplication application(argc, argv);

    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<std::string>("std::string");
    
    OutputRedirector redirect_stdout(std::cout, "stdout", Color());
    OutputRedirector redirect_stderr(std::cerr, "stderr", COLOR_RED);

    // Read program settings from json file: SerialPrograms-Settings.json.
    try{
        PERSISTENT_SETTINGS().read();
    }catch (const FileException& error){
        global_logger_tagged().log(error.message(), COLOR_RED);
    }catch (const ParseException& error){
        global_logger_tagged().log(error.message(), COLOR_RED);
    }

    if (GlobalSettings::instance().COMMAND_LINE_TEST_MODE){
        return run_command_line_tests();
    }

    // Check whether the hardware is powerful enough to run this program.
    if (!check_hardware()){
        return 1;
    }

#if 0
    application.connect(
        &application, &QGuiApplication::primaryScreenChanged,
        &application, [&](QScreen* screen){
            cout << "asdf" << endl;
        }
    );
#endif

    int ret;
    {
        MainWindow w;
        w.show();
        ret = application.exec();
    }

    // Write program settings back to the json file.
    PERSISTENT_SETTINGS().write();

#ifdef PA_SLEEPY
    Integration::SleepyDiscordRunner::sleepy_terminate();
#endif

    return ret;
}
