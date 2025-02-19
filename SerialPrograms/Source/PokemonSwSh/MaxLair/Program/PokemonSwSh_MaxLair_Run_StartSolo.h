/*  Max Lair Run Start (Solo)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_MaxLair_Run_StartSolo_H
#define PokemonAutomation_PokemonSwSh_MaxLair_Run_StartSolo_H

#include "CommonFramework/Tools/ConsoleHandle.h"
#include "PokemonSwSh/Inference/PokemonSwSh_QuantityReader.h"
#include "PokemonSwSh/MaxLair/Options/PokemonSwSh_MaxLair_Options_Hosting.h"
#include "PokemonSwSh/MaxLair/Framework/PokemonSwSh_MaxLair_Stats.h"
#include "PokemonSwSh/MaxLair/Framework/PokemonSwSh_MaxLair_StateTracker.h"

namespace PokemonAutomation{
    class BotBaseContext;
    class ProgramEnvironment;
namespace NintendoSwitch{
namespace PokemonSwSh{
namespace MaxLairInternal{


bool wait_for_a_player(
    ConsoleHandle& console, BotBaseContext& context,
    const QImage& entrance,
    WallClock time_limit
);
bool wait_for_lobby_ready(
    ConsoleHandle& console, BotBaseContext& context,
    const QImage& entrance,
    size_t min_players,
    size_t start_players,
    WallClock time_limit
);
bool start_adventure(
    ConsoleHandle& console, BotBaseContext& context,
    size_t consoles,
    const QImage& entrance
);


bool start_raid_self_solo(
    ConsoleHandle& console, BotBaseContext& context,
    GlobalStateTracker& state_tracker,
    QImage& entrance, size_t boss_slot,
    ReadableQuantity999& ore
);

bool start_raid_host_solo(
    ProgramEnvironment& env, ConsoleHandle& console, BotBaseContext& context,
    GlobalStateTracker& state_tracker,
    QImage& entrance, size_t boss_slot,
    HostingSettings& settings,
    const PathStats& path_stats,
    const StatsTracker& session_stats,
    ReadableQuantity999& ore
);


}
}
}
}
#endif
