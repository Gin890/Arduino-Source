/*  Battle Won Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_BattleWonDetector_H
#define PokemonAutomation_PokemonSwSh_BattleWonDetector_H

#include "Common/Cpp/FixedLimitVector.h"
#include "CommonFramework/VideoPipeline/VideoFeed.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"
#include "CommonFramework/Inference/VisualDetector.h"
#include "PokemonSwSh_BattleDialogDetector.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


class ExperienceGainDetector : public StaticScreenDetector{
public:
    ~ExperienceGainDetector();
    ExperienceGainDetector(Color color = COLOR_RED);

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool detect(const QImage& screen) const override;

private:
    Color m_color;
    BattleDialogDetector m_dialog;
    FixedLimitVector<std::pair<ImageFloatBox, ImageFloatBox>> m_rows;
};


class ExperienceGainWatcher : public ExperienceGainDetector, public VisualInferenceCallback{
public:
    ExperienceGainWatcher(Color color = COLOR_RED);

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(const QImage& frame, WallClock timestamp) override final;
};


}
}
}
#endif
