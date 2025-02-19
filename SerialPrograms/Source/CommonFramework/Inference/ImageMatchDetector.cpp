/*  Image Match Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <cmath>
#include "CommonFramework/ImageTools/ImageStats.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "CommonFramework/VideoPipeline/VideoOverlay.h"
#include "CommonFramework/Tools/ErrorDumper.h"
#include "ImageMatchDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{



ImageMatchDetector::ImageMatchDetector(
    QImage reference_image, const ImageFloatBox& box,
    double max_rmsd, bool scale_brightness,
    Color color
)
    : m_reference_image(std::move(reference_image))
    , m_average_brightness(image_stats(m_reference_image).average)
    , m_max_rmsd(max_rmsd)
    , m_scale_brightness(scale_brightness)
    , m_color(color)
    , m_box(box)
{
    m_reference_image = extract_box_copy(m_reference_image, m_box);
}

double ImageMatchDetector::rmsd(const QImage& frame) const{
    if (frame.isNull()){
        return 1000;
    }
    ConstImageRef image = extract_box_reference(frame, m_box);
    QImage scaled = image.scaled_to_qimage(m_reference_image.width(), m_reference_image.height());

#if 0
    if (image.width() != (size_t)scaled.width() || image.height() != (size_t)scaled.height()){
        cout << image.width() << " x " << image.height() << " - " << scaled.width() << " x " << scaled.height() << endl;
        dump_image(global_logger_tagged(), ProgramInfo(), "ImageMatchDetector-rmsd", frame);
    }
#endif

    if (m_scale_brightness){
        FloatPixel image_brightness = ImageMatch::pixel_average(scaled, m_reference_image);
        FloatPixel scale = m_average_brightness / image_brightness;
        if (std::isnan(scale.r)) scale.r = 1.0;
        if (std::isnan(scale.g)) scale.g = 1.0;
        if (std::isnan(scale.b)) scale.b = 1.0;
        scale.bound(0.8, 1.2);
        ImageMatch::scale_brightness(scaled, scale);
    }

//    cout << "asdf" << endl;
    double ret = ImageMatch::pixel_RMSD(m_reference_image, scaled);
//    cout << "rmsd = " << ret << endl;
    return ret;
}

void ImageMatchDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(m_color, m_box);
}
bool ImageMatchDetector::detect(const QImage& screen) const{
    return rmsd(screen) <= m_max_rmsd;
}



ImageMatchWatcher::ImageMatchWatcher(
    QImage reference_image, const ImageFloatBox& box,
    double max_rmsd, bool scale_brightness,
    std::chrono::milliseconds hold_duration,
    Color color
)
    : ImageMatchDetector(std::move(reference_image), box, max_rmsd, scale_brightness, color)
    , VisualInferenceCallback("ImageMatchWatcher")
    , m_hold_duration(hold_duration)
    , m_last_match(false)
    , m_start_of_match(WallClock::min())
{}

void ImageMatchWatcher::make_overlays(VideoOverlaySet& items) const{
    ImageMatchDetector::make_overlays(items);
}
bool ImageMatchWatcher::process_frame(const QImage& frame, WallClock){
    if (!detect(frame)){
        m_last_match = false;
        return false;
    }
    auto now = current_time();
    if (!m_last_match){
        m_last_match = true;
        m_start_of_match = now;
        return false;
    }
    return now - m_start_of_match >= m_hold_duration;
}






}
