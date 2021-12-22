/*  Overworld Trigger
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_OverworldTrigger_H
#define PokemonAutomation_PokemonBDSP_OverworldTrigger_H

#include "CommonFramework/Options/BatchOption.h"
#include "CommonFramework/Options/EnumDropdownOption.h"
#include "NintendoSwitch/Options/TimeExpressionOption.h"
#include "CommonFramework/Tools/ConsoleHandle.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{

// Program component to trigger an overworld encounter.
class OverworldTrigger : public GroupOption{
public:
    OverworldTrigger();

    // Run the encounter commands non stop.
    // Need to launch other async code to detect when battle starts, otherwise
    // it will run forever.
    void run_trigger_non_stop(const BotBaseContext& context) const;

    // Run one basic unit of encounter command, like move up and down once.
    void run_trigger_once(const BotBaseContext& context) const;

public:
    EnumDropdownOption TRIGGER_METHOD;
    TimeExpressionOption<uint16_t> MOVE_DURATION;
    EnumDropdownOption SWEET_SCENT_POKEMON_LOCATION;
};



}
}
}
#endif
