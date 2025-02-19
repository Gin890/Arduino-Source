/*  Shiny Hunt - Custom Path
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonLA_ShinyHuntCustomPath_H
#define PokemonAutomation_PokemonLA_ShinyHuntCustomPath_H

#include "CommonFramework/Notifications/EventNotificationsTable.h"
#include "CommonFramework/Options/BooleanCheckBoxOption.h"
#include "CommonFramework/Options/EnumDropdownOption.h"
#include "CommonFramework/Options/FloatingPointOption.h"
#include "CommonFramework/Options/SimpleIntegerOption.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"
#include "PokemonLA/PokemonLA_Locations.h"
#include "PokemonLA/PokemonLA_TravelLocations.h"
#include "PokemonLA/Options/PokemonLA_ShinyDetectedAction.h"
#include "PokemonLA/Options/PokemonLA_TravelLocation.h"
#include "PokemonLA/Options/PokemonLA_CustomPathTable.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{



class ShinyHuntCustomPath_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    ShinyHuntCustomPath_Descriptor();
};


class ShinyHuntCustomPath : public SingleSwitchProgramInstance{
public:
    ShinyHuntCustomPath(const ShinyHuntCustomPath_Descriptor& descriptor);

    virtual std::unique_ptr<StatsTracker> make_stats() const override;
    virtual void program(SingleSwitchProgramEnvironment& env, BotBaseContext& context) override;

private:
    // Run the custom path on overworld.
    void run_path(SingleSwitchProgramEnvironment& env, BotBaseContext& context);

    // Do one action (while ignoring listen-related actions)
    void do_non_listen_action(ConsoleHandle& console, BotBaseContext& context, size_t action_index);

private:
    class Stats;

    ShinyRequiresAudioText SHINY_REQUIRES_AUDIO;

//    TravelLocationOption TRAVEL_LOCATION;

    CustomPathTable PATH;

    BooleanCheckBoxOption TEST_PATH;

    ShinyDetectedActionOption SHINY_DETECTED_ENROUTE;
    ShinyDetectedActionOption SHINY_DETECTED_DESTINATION;

    EventNotificationOption NOTIFICATION_STATUS;
    EventNotificationsOption NOTIFICATIONS;
};





}
}
}
#endif
