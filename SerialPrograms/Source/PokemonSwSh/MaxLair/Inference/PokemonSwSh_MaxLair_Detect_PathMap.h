/*  Max Lair Detect Path Map
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_MaxLair_Detect_PathMap_H
#define PokemonAutomation_PokemonSwSh_MaxLair_Detect_PathMap_H

#include <deque>
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/VideoPipeline/VideoOverlay.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "Pokemon/Pokemon_Types.h"
#include "PokemonSwSh/MaxLair/Framework/PokemonSwSh_MaxLair_State.h"

namespace PokemonAutomation{
    class BotBaseContext;
    class ProgramEnvironment;
namespace NintendoSwitch{
namespace PokemonSwSh{
namespace MaxLairInternal{
using namespace Pokemon;


bool read_type_array(
    ConsoleHandle& console,
    const ConstImageRef& screen,
    const ImageFloatBox& box,
    std::deque<InferenceBoxScope>& hits,
    size_t count,
    PokemonType* type, ImagePixelBox* boxes
);


bool read_path(
    ProgramEnvironment& env, ConsoleHandle& console, BotBaseContext& context,
    PathMap& path,
    const ImageFloatBox& box
);


}
}
}
}
#endif
