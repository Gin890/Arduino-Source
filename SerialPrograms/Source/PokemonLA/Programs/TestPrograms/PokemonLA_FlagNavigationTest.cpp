/*  Flag Navigation Test
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Globals.h"
#include "PokemonLA/Programs/PokemonLA_FlagNavigationAir.h"
#include "PokemonLA_FlagNavigationTest.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


FlagNavigationTest_Descriptor::FlagNavigationTest_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonLA:FlagNavigationTest",
        STRING_POKEMON + " LA", "Flag Navigation Test",
        "",
        "Navigate to the flag pin.",
        FeedbackType::REQUIRED, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


FlagNavigationTest::FlagNavigationTest(const FlagNavigationTest_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , STOP_DISTANCE(
        "<b>Stop Distance:</b><br>"
        "You have reached the flag when you come within this distance of it. "
        "Don't set this too small. The navigation is not precise enough to land directly on the flag.",
        20
    )
    , FLAG_REACHED_DELAY(
        "<b>Target Reached Delay:</b><br>"
        "Once you have reached the flag, wait this many seconds to ensure everything loads and that any shinies are heard before resetting.",
        1.0, 0, 60
    )
    , NAVIGATION_TIMEOUT(
        "<b>Navigation Timeout:</b><br>Give up if flag is not reached after this many seconds.",
        180, 0
    )
{
    PA_ADD_OPTION(STOP_DISTANCE);
    PA_ADD_OPTION(FLAG_REACHED_DELAY);
    PA_ADD_OPTION(NAVIGATION_TIMEOUT);
//    PA_ADD_OPTION(SHINY_DETECTED);
}


void FlagNavigationTest::program(SingleSwitchProgramEnvironment& env, BotBaseContext& context){

    FlagNavigationAir session(
        env, env.console, context,
//        SHINY_DETECTED.stop_on_shiny(),
        STOP_DISTANCE,
        FLAG_REACHED_DELAY,
        std::chrono::seconds(NAVIGATION_TIMEOUT)
    );
    session.run_session();

}



}
}
}
