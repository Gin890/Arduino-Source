/*  Clothing Buyer
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonLA_ClothingBuyer.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


ClothingBuyer_Descriptor::ClothingBuyer_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonLA:ClothingBuyer",
        STRING_POKEMON + " LA", "Clothing Buyer",
        "ComputerControl/blob/master/Wiki/Programs/PokemonLA/ClothingBuyer.md",
        "Buy out all the clothing in the store.",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}


ClothingBuyer::ClothingBuyer(const ClothingBuyer_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , CATEGORY_ROTATION(
        "<b>Rotate Categories:</b><br>This slows down the program, but ensures it will cover all categories.",
        true
    )
{
    PA_ADD_OPTION(CATEGORY_ROTATION);
}


void ClothingBuyer::program(SingleSwitchProgramEnvironment& env, BotBaseContext& context){
    //  Connect the controller.
    pbf_press_button(context, BUTTON_LCLICK, 5, 5);

    while (true){
        // If this clothing is not bought:
        // Press A to pop purchase dialogue
        // Press A to confirm purchase
        // Press B to not wear it
        // Press B to clear dialogue

        // If this clothing is already bought:
        // Press A to do nothing
        // Press A to do nothing
        // Press B to be asked whether to leave menu
        // Press B to not leave

        pbf_press_button(context, BUTTON_A, 10, 120);
        pbf_press_button(context, BUTTON_A, 10, 130);
        pbf_press_button(context, BUTTON_B, 10, 120);
        pbf_press_button(context, BUTTON_B, 10, 60);

        // Move to the next clothing in the same category
        pbf_press_dpad(context, DPAD_DOWN, 10, 40);

        // Move to the next category
        if (CATEGORY_ROTATION){
            pbf_press_button(context, BUTTON_R, 10, 40);
        }
    }
}




}
}
}
