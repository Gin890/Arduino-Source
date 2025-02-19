/*  Image Filter
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_CommonFramework_ImageFilter_H
#define PokemonAutomation_CommonFramework_ImageFilter_H

#include <vector>
#include "Common/Cpp/Color.h"

class QImage;

namespace PokemonAutomation{

class ConstImageRef;

//  If `invert` is false, replace the color outside of the range [mins, maxs] with the color `replace_with`.
//  If `invert` is true, replace the color range [mins, maxs] with `replace_with`.
//  Returns the # of pixels inside the range [mins, maxs].
size_t filter_rgb32_range(QImage& image, uint32_t mins, uint32_t maxs, Color replace_with, bool invert);



//  Run multiple filters at once. This is more memory efficient than making
//  multiple calls to one filter at a time.
struct FilterRgb32Range{
    uint32_t mins;
    uint32_t maxs;
    Color replace_with;
    bool invert;
};
std::vector<std::pair<QImage, size_t>> filter_rgb32_range(
    const ConstImageRef& image,
    const std::vector<FilterRgb32Range>& filters
);





//  Convert the image to black and white.
//  Inside [mins, maxs] is white, otherwise it's black.
//  Set "in_range_black" to true to invert the colors.
size_t to_blackwhite_rgb32_range(QImage& image, uint32_t mins, uint32_t maxs, bool in_range_black);



//  Run multiple filters at once. This is more memory efficient than making
//  multiple calls to one filter at a time.
struct BlackWhiteRgb32Range{
    uint32_t mins;
    uint32_t maxs;
    bool in_range_black;
};
std::vector<std::pair<QImage, size_t>> to_blackwhite_rgb32_range(
    const ConstImageRef& image,
    const std::vector<BlackWhiteRgb32Range>& filters
);




}
#endif
