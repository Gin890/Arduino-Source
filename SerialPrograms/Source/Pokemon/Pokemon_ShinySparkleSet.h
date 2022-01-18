/*  Shiny Sparkle Set
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_Pokemon_ShinySparkleSet_H
#define PokemonAutomation_Pokemon_ShinySparkleSet_H

#include <string>
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "CommonFramework/Inference/VisualInferenceCallback.h"

namespace PokemonAutomation{
    class Logger;
}

namespace PokemonAutomation{
namespace Pokemon{


class ShinySparkleSet{
public:
    virtual ~ShinySparkleSet() = default;
    virtual void clear() = 0;

    virtual std::string to_str() const = 0;

    virtual void read_from_image(const QImage& image) = 0;
    virtual void draw_boxes(
        VideoOverlaySet& overlays,
        const QImage& frame,
        const ImageFloatBox& inference_box
    ) const = 0;

};



class ShinySparkleTracker : public VisualInferenceCallback{
public:
    ShinySparkleTracker(
        Logger& logger, VideoOverlay& overlay,
        ShinySparkleSet& sparkle_set,
        const ImageFloatBox& box
    );

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& frame,
        std::chrono::system_clock::time_point timestamp
    ) override;

private:
    ImageFloatBox m_box;
    Logger& m_logger;
    ShinySparkleSet& m_current_sparkles;
    VideoOverlaySet m_overlays;
};





}
}
#endif
