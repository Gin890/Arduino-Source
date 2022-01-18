/*  Shiny Sparkle Set
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QImage>
#include "CommonFramework/Logging/Logger.h"
#include "Pokemon_ShinySparkleSet.h"

namespace PokemonAutomation{
namespace Pokemon{


ShinySparkleTracker::ShinySparkleTracker(
    Logger& logger, VideoOverlay& overlay,
    ShinySparkleSet& sparkle_set,
    const ImageFloatBox& box
)
    : m_box(box)
    , m_logger(logger)
    , m_current_sparkles(sparkle_set)
    , m_overlays(overlay)
{}
void ShinySparkleTracker::make_overlays(VideoOverlaySet& items) const{
    items.add(COLOR_RED, m_box);
}
bool ShinySparkleTracker::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    QImage image = extract_box(frame, m_box);
    m_current_sparkles.read_from_image(image);
    m_overlays.clear();
    m_current_sparkles.draw_boxes(m_overlays, frame, m_box);
    std::string log_str = m_current_sparkles.to_str();
    if (!log_str.empty()){
        m_logger.log(log_str, COLOR_BLUE);
    }
    return false;
}



}
}
