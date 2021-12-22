/*  Overworld Trigger
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "NintendoSwitch/Commands/NintendoSwitch_PushButtons.h"
#include "PokemonBDSP/PokemonBDSP_Settings.h"
#include "PokemonBDSP_OverworldTrigger.h"
#include <iostream>

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


OverworldTrigger::OverworldTrigger()
    : GroupOption("Trigger Method")
    , TRIGGER_METHOD(
        "<b>Maneuver:</b><br>How to trigger an encounter",
        {
            "Move left/right. (no bias)",
            "Move left/right. (bias left)",
            "Move left/right. (bias right)",
            "Move up/down. (no bias)",
            "Move up/down. (bias up)",
            "Move up/down. (bias down)",
            "Sweet scent",
        }, 0
    )
    , MOVE_DURATION(
        "<b>Move Duration:</b><br>Move in each direction for this long before turning around.",
        "1 * TICKS_PER_SECOND"
    )
    , SWEET_SCENT_POKEMON_LOCATION(
        "<b>Sweet Scent Pokemon Location:</b><br>Which Pokemon in the party knows Sweet Scent.",
        {
          "1st",
          "2nd",
          "3rd",
          "4th",
          "2nd last",
          "Last",
        }, 0
    )
{
    PA_ADD_OPTION(TRIGGER_METHOD);
    PA_ADD_OPTION(MOVE_DURATION);
    PA_ADD_OPTION(SWEET_SCENT_POKEMON_LOCATION);
}

void OverworldTrigger::run_trigger_non_stop(const BotBaseContext& context) const{
    while (true) {
        run_trigger_once(context);
        // If encounter using sweet scent, then it is guaranteed to encounter a pokemon.
        // No need to run it repeatedly.
        if (TRIGGER_METHOD == 6) {
            // Wait a while to let the battle encounter callbacks know a battle
            // has started.
            const uint16_t after_sweet_scent_wait_time = 500;
            pbf_wait(context, after_sweet_scent_wait_time);
            break;
        }
    }
}

void OverworldTrigger::run_trigger_once(const BotBaseContext& context) const{
    switch (TRIGGER_METHOD){
    case 0:
        pbf_move_left_joystick(context, 0, 128, MOVE_DURATION, 0);
        pbf_move_left_joystick(context, 255, 128, MOVE_DURATION, 0);
        break;
    case 1:
        pbf_move_left_joystick(context, 0, 128, MOVE_DURATION + 25, 0);
        pbf_move_left_joystick(context, 255, 128, MOVE_DURATION, 0);
        break;
    case 2:
        pbf_move_left_joystick(context, 255, 128, MOVE_DURATION + 25, 0);
        pbf_move_left_joystick(context, 0, 128, MOVE_DURATION, 0);
        break;
    case 3:
        pbf_move_left_joystick(context, 128, 0, MOVE_DURATION, 0);
        pbf_move_left_joystick(context, 128, 255, MOVE_DURATION, 0);
        break;
    case 4:
        pbf_move_left_joystick(context, 128, 0, MOVE_DURATION + 25, 0);
        pbf_move_left_joystick(context, 128, 255, MOVE_DURATION, 0);
        break;
    case 5:
        pbf_move_left_joystick(context, 128, 255, MOVE_DURATION + 25, 0);
        pbf_move_left_joystick(context, 128, 0, MOVE_DURATION, 0);
        break;
    case 6: // Sweet scent
        // Go to menu
        const uint16_t overworld_to_menu_delay = 50;
        pbf_press_button(context, BUTTON_X, 20, overworld_to_menu_delay);
        // Go to pokemon page
        const uint16_t MENU_TO_POKEMON_DELAY = 200; // GameSettings::instance().MENU_TO_POKEMON_DELAY;
        pbf_mash_button(context, BUTTON_ZL, 30);
        if (MENU_TO_POKEMON_DELAY > 30){
            pbf_wait(context, MENU_TO_POKEMON_DELAY - 30);
        }

        // Go to the pokemon that knows Sweet Scent
        const size_t location = SWEET_SCENT_POKEMON_LOCATION;
        const uint16_t change_pokemon_delay = 20;
        if (location >= 1 && location <= 3) {
            const size_t move_down_times = location;
            for(size_t i = 0; i < move_down_times; ++i) {
                pbf_press_dpad(context, DPAD_DOWN, 1, change_pokemon_delay);
            }
        } else if (location >= 1) { // for location 4 and 5
            const size_t move_down_times = 6 - location;
            for(size_t i = 0; i < move_down_times; ++i) {
                pbf_press_dpad(context, DPAD_UP, 1, change_pokemon_delay);
            }
        }

        // Open the pokemon menu of the first pokemon
        const uint16_t pokemon_to_pokemon_menu_delay = 30;
        pbf_press_button(context, BUTTON_ZL, 20, pokemon_to_pokemon_menu_delay);
        // Move down one menuitem to select "Sweet Scent"
        const uint16_t move_pokemon_menu_item_delay = 30;
        pbf_press_dpad(context, DPAD_DOWN, 1, move_pokemon_menu_item_delay);
        // Use sweet scent
        pbf_mash_button(context, BUTTON_ZL, 30);
        break;
    }
}



}
}
}
