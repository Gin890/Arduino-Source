/*  White Object Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <map>
#include "Common/Cpp/FixedLimitVector.tpp"
#include "Kernels/Waterfill/Kernels_Waterfill.h"
#include "Kernels/Waterfill/Kernels_Waterfill_Session.h"
#include "CommonFramework/ImageTools/BinaryImage_FilterRgb32.h"
#include "CommonFramework/ImageMatch/SubObjectTemplateMatcher.h"
#include "PokemonLA_WhiteObjectDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{

using namespace Kernels::Waterfill;



void find_overworld_white_objects(
    const std::vector<std::pair<WhiteObjectDetector&, bool>>& detectors,
    const ConstImageRef& image
){
    std::set<Color> threshold_set;
    for (const auto& item : detectors){
        const std::set<Color>& thresholds = item.first.thresholds();
        threshold_set.insert(thresholds.begin(), thresholds.end());
    }

//    FixedLimitVector<CompressRgb32ToBinaryRangeFilter> filters(threshold_set.size());
//    for (Color filter : threshold_set){
//        filters.emplace_back(image.width(), image.height(), (uint32_t)filter, 0xffffffff);
//    }
//    compress_rgb32_to_binary_range(image, filters.data(), filters.size());

    {
        std::vector<std::pair<uint32_t, uint32_t>> filters;
        for (Color filter : threshold_set){
            filters.emplace_back((uint32_t)filter, 0xffffffff);
        }
        std::vector<PackedBinaryMatrix2> matrix = compress_rgb32_to_binary_range(image, filters);

#if 1
        std::unique_ptr<WaterfillSession> session = make_WaterfillSession();
        for (size_t c = 0; c < filters.size(); c++){
            session->set_source(matrix[c]);
            auto finder = session->make_iterator(50);
            WaterfillObject object;
            while (finder->find_next(object, false)){
                for (const auto& detector : detectors){
                    const std::set<Color>& thresholds = detector.first.thresholds();
                    if (thresholds.find((Color)filters[c].first) != thresholds.end()){
                        detector.first.process_object(image, object);
                    }
                }
            }
        }
#endif
    }

    for (const auto& detector : detectors){
        detector.first.finish();
    }
}


void WhiteObjectDetector::merge_heavily_overlapping(double tolerance){
    std::multimap<size_t, ImagePixelBox> boxes;
    for (const ImagePixelBox& box : m_detections){
        boxes.emplace(box.area(), box);
    }
    m_detections.clear();
//    cout << "boxes.size() = " << boxes.size() << endl;

    double ratio = 1.0 + tolerance;

    while (!boxes.empty()){
        auto current = boxes.begin();
        size_t limit = (size_t)(current->first * ratio);
        auto candidate = current;
        ++candidate;
        while (candidate != boxes.end()){
            if (candidate->first > limit){
                ++candidate;
                continue;
            }
            size_t overlap = current->second.overlap_with(candidate->second);
            if ((double)overlap * ratio > current->first){
                current->second.merge_with(candidate->second);
                candidate = boxes.erase(candidate);
            }else{
                ++candidate;
            }
        }
        m_detections.emplace_back(current->second);
        current = boxes.erase(current);
    }
//    cout << "m_detections.size() = " << m_detections.size() << endl;
}




WhiteObjectWatcher::WhiteObjectWatcher(
    VideoOverlay& overlay,
    const ImageFloatBox& box,
    std::vector<std::pair<WhiteObjectDetector&, bool>> detectors
)
    : VisualInferenceCallback("WhiteObjectWatcher")
    , m_box(box)
    , m_overlays(overlay)
    , m_detectors(detectors)
{}

void WhiteObjectWatcher::make_overlays(VideoOverlaySet& items) const{
    items.add(COLOR_RED, m_box);
}
bool WhiteObjectWatcher::process_frame(const QImage& frame, WallClock timestamp){
    for (auto& detector : m_detectors){
        detector.first.clear();
    }

    find_overworld_white_objects(m_detectors, extract_box_reference(frame, m_box));
    m_overlays.clear();

    for (auto& detector : m_detectors){
        for (const ImagePixelBox& obj : detector.first.detections()){
            ImageFloatBox box = translate_to_parent(frame, m_box, obj);
            m_overlays.add(detector.first.color(), box);
        }
    }
    for (auto& detector : m_detectors){
        if (detector.second && !detector.first.detections().empty()){
            return true;
        }
    }
    return false;
}





}
}
}
