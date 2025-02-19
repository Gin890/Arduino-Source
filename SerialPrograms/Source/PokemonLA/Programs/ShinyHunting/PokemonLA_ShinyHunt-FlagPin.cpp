/*  Shiny Hunt - Fixed Point
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonLA/PokemonLA_Settings.h"
#include "PokemonLA/Inference/Sounds/PokemonLA_ShinySoundDetector.h"
#include "PokemonLA/Programs/PokemonLA_GameEntry.h"
#include "PokemonLA/Programs/PokemonLA_RegionNavigation.h"
#include "PokemonLA/Programs/PokemonLA_FlagNavigationAir.h"
#include "PokemonLA_ShinyHunt-FlagPin.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{



ShinyHuntFlagPin_Descriptor::ShinyHuntFlagPin_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonLA:ShinyHunt-FlagPin",
        STRING_POKEMON + " LA", "Shiny Hunt - Flag Pin",
        "ComputerControl/blob/master/Wiki/Programs/PokemonLA/ShinyHunt-FlagPin.md",
        "Repeatedly travel to a flag pin to shiny hunt " + STRING_POKEMON + " around it.",
        FeedbackType::REQUIRED, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


ShinyHuntFlagPin::ShinyHuntFlagPin(const ShinyHuntFlagPin_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , ENROUTE_DISTANCE(
        "<b>Enroute Distance:</b><br>"
        "You are considered \"enroute\" if you are further than this distance from the flag.<br><br>"
        "<font color=\"red\">If you wish to ignore enroute shinies, scroll down to "
        "\"Enroute Shiny Action\" and set it to ignore shinies. "
        "Keep in mind that the shiny sound radius is 30 and you will need some headroom.</font>",
        60
    )
    , SHINY_DETECTED_ENROUTE(
        "Enroute Shiny Action",
        "This applies if a shiny is detected while enroute to the flag. (defined as being more than the \"Enroute Distance\" specified above)",
        "0 * TICKS_PER_SECOND"
    )
    , SHINY_DETECTED_DESTINATION(
        "Destination Shiny Action",
        "This applies if a shiny is detected at or near the flag. (defined as being less than the \"Enroute Distance\" specified above)",
        "0 * TICKS_PER_SECOND"
    )
    , NOTIFICATION_STATUS("Status Update", true, false, std::chrono::seconds(3600))
    , NOTIFICATIONS({
        &NOTIFICATION_STATUS,
        &SHINY_DETECTED_ENROUTE.NOTIFICATIONS,
        &SHINY_DETECTED_DESTINATION.NOTIFICATIONS,
        &NOTIFICATION_PROGRAM_FINISH,
//        &NOTIFICATION_ERROR_RECOVERABLE,
        &NOTIFICATION_ERROR_FATAL,
    })
    , m_advanced_options(
        "<font size=4><b>Advanced Options:</b> You should not need to touch anything below here.</font>"
    )
    , STOP_DISTANCE(
        "<b>Stop Distance:</b><br>"
        "Reset the game when you come within this distance of the flag. "
        "Don't set this too small. The navigation is not precise enough to land directly on the flag.",
        20
    )
    , FLAG_REACHED_DELAY(
        "<b>Target Reached Delay:</b><br>"
        "Once you have reached the flag, wait this many seconds to ensure everything loads and that any shinies are heard before resetting.",
        1.0, 0, 60
    )
    , NAVIGATION_TIMEOUT(
        "<b>Navigation Timeout:</b><br>Give up and reset if flag is not reached after this many seconds.",
        180, 0
    )
{
    PA_ADD_STATIC(SHINY_REQUIRES_AUDIO);
    PA_ADD_OPTION(TRAVEL_LOCATION);
    PA_ADD_OPTION(ENROUTE_DISTANCE);
    PA_ADD_OPTION(SHINY_DETECTED_ENROUTE);
    PA_ADD_OPTION(SHINY_DETECTED_DESTINATION);
    PA_ADD_OPTION(NOTIFICATIONS);
    PA_ADD_STATIC(m_advanced_options);
    PA_ADD_OPTION(STOP_DISTANCE);
    PA_ADD_OPTION(FLAG_REACHED_DELAY);
    PA_ADD_OPTION(NAVIGATION_TIMEOUT);
}

class ShinyHuntFlagPin::Stats : public StatsTracker, public ShinyStatIncrementer{
public:
    Stats()
        : attempts(m_stats["Attempts"])
        , errors(m_stats["Errors"])
        , shinies(m_stats["Shinies"])
    {
        m_display_order.emplace_back("Attempts");
        m_display_order.emplace_back("Errors", true);
        m_display_order.emplace_back("Shinies", true);
    }
    virtual void add_shiny() override{
        shinies++;
    }

    std::atomic<uint64_t>& attempts;
    std::atomic<uint64_t>& errors;
    std::atomic<uint64_t>& shinies;
};

std::unique_ptr<StatsTracker> ShinyHuntFlagPin::make_stats() const{
    return std::unique_ptr<StatsTracker>(new Stats());
}


void ShinyHuntFlagPin::run_iteration(SingleSwitchProgramEnvironment& env, BotBaseContext& context){
    Stats& stats = env.stats<Stats>();
    stats.attempts++;

    {
        std::atomic<double> flag_distance(10000);

        float shiny_coefficient = 1.0;
        ShinyDetectedActionOption* shiny_action = nullptr;

        ShinySoundDetector shiny_detector(env.console.logger(), env.console, [&](float error_coefficient) -> bool{
            //  Warning: This callback will be run from a different thread than this function.
            stats.shinies++;
            shiny_coefficient = error_coefficient;
            if (flag_distance.load(std::memory_order_acquire) <= ENROUTE_DISTANCE){
                shiny_action = &SHINY_DETECTED_DESTINATION;
            }else{
                shiny_action = &SHINY_DETECTED_ENROUTE;
            }
            return on_shiny_callback(env, env.console, *shiny_action, error_coefficient);
        });

        int ret = run_until(
            env.console, context,
            [&](BotBaseContext& context){
                goto_camp_from_jubilife(env, env.console, context, TRAVEL_LOCATION);
                FlagNavigationAir session(
                    env, env.console, context,
                    STOP_DISTANCE,
                    FLAG_REACHED_DELAY,
                    std::chrono::seconds(NAVIGATION_TIMEOUT)
                );
                session.set_distance_callback([&](double distance){
                    flag_distance.store(distance, std::memory_order_release);
                });
                session.run_session();
            },
            {{shiny_detector}}
        );
        shiny_detector.throw_if_no_sound();
        if (ret == 0){
            on_shiny_sound(env, env.console, context, *shiny_action, shiny_coefficient);
        }
    }

    pbf_press_button(context, BUTTON_HOME, 20, GameSettings::instance().GAME_TO_HOME_DELAY);
    reset_game_from_home(env, env.console, context, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST);
}


void ShinyHuntFlagPin::program(SingleSwitchProgramEnvironment& env, BotBaseContext& context){
    Stats& stats = env.stats<Stats>();

    //  Connect the controller.
    pbf_press_button(context, BUTTON_LCLICK, 5, 5);

    while (true){
        env.update_stats();
        send_program_status_notification(
            env.logger(), NOTIFICATION_STATUS,
            env.program_info(),
            "",
            stats.to_str()
        );
        try{
            run_iteration(env, context);
        }catch (OperationFailedException&){
            stats.errors++;
            pbf_press_button(context, BUTTON_HOME, 20, GameSettings::instance().GAME_TO_HOME_DELAY);
            reset_game_from_home(env, env.console, context, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST);
        }
    }

    env.update_stats();
    send_program_finished_notification(
        env.logger(), NOTIFICATION_PROGRAM_FINISH,
        env.program_info(),
        "",
        stats.to_str()
    );
}





}
}
}
