/*  Binary Image
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/Exceptions.h"
#include "Common/Cpp/FixedLimitVector.tpp"
#include "Kernels/BinaryImageFilters/Kernels_BinaryImage_BasicFilters.h"
#include "BinaryImage_FilterRgb32.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{



PackedBinaryMatrix2 compress_rgb32_to_binary_min(
    const ConstImageRef& image,
    uint8_t min_red,
    uint8_t min_green,
    uint8_t min_blue
){
    return compress_rgb32_to_binary_range(
        image,
        255, 255,
        min_red, 255,
        min_green, 255,
        min_blue, 255
    );
}
PackedBinaryMatrix2 compress_rgb32_to_binary_max(
    const ConstImageRef& image,
    uint8_t max_red,
    uint8_t max_green,
    uint8_t max_blue
){
    return compress_rgb32_to_binary_range(
        image,
        255, 255,
        0, max_red,
        0, max_green,
        0, max_blue
    );
}
PackedBinaryMatrix2 compress_rgb32_to_binary_range(
    const ConstImageRef& image,
    uint8_t min_red, uint8_t max_red,
    uint8_t min_green, uint8_t max_green,
    uint8_t min_blue, uint8_t max_blue
){
    return compress_rgb32_to_binary_range(
        image,
        255, 255,
        min_red, max_red,
        min_green, max_green,
        min_blue, max_blue
    );
}
PackedBinaryMatrix2 compress_rgb32_to_binary_range(
    const ConstImageRef& image,
    uint8_t min_alpha, uint8_t max_alpha,
    uint8_t min_red, uint8_t max_red,
    uint8_t min_green, uint8_t max_green,
    uint8_t min_blue, uint8_t max_blue
){
    PackedBinaryMatrix2 ret(image.width(), image.height());
    Kernels::compress_rgb32_to_binary_range(
        image.data(), image.bytes_per_row(), ret,
        ((uint32_t)min_alpha << 24) | ((uint32_t)min_red << 16) | ((uint32_t)min_green << 8) | (uint32_t)min_blue,
        ((uint32_t)max_alpha << 24) | ((uint32_t)max_red << 16) | ((uint32_t)max_green << 8) | (uint32_t)max_blue
    );
    return ret;
}
PackedBinaryMatrix2 compress_rgb32_to_binary_range(
    const ConstImageRef& image,
    uint32_t mins, uint32_t maxs
){
    PackedBinaryMatrix2 ret(image.width(), image.height());
    Kernels::compress_rgb32_to_binary_range(
        image.data(), image.bytes_per_row(),
        ret, mins, maxs
    );
    return ret;
}
std::vector<PackedBinaryMatrix2> compress_rgb32_to_binary_range(
    const ConstImageRef& image,
    const std::vector<std::pair<uint32_t, uint32_t>>& filters
){
    std::vector<PackedBinaryMatrix2> ret;
    FixedLimitVector<Kernels::CompressRgb32ToBinaryRangeFilter> vec(filters.size());
    for (size_t c = 0; c < filters.size(); c++){
        ret.emplace_back(image.width(), image.height());
        vec.emplace_back(ret[c], filters[c].first, filters[c].second);
    }
    compress_rgb32_to_binary_range(
        image.data(), image.bytes_per_row(),
        vec.data(), vec.size()
    );
    return ret;
}



void filter_rgb32(
    const PackedBinaryMatrix2& matrix,
    const ImageRef& image,
    Color replace_with,
    bool replace_if_zero
){
    if (matrix.width() > image.width()){
        throw InternalProgramError(nullptr, PA_CURRENT_FUNCTION, "Image width is too small.");
    }
    if (matrix.height() > image.height()){
        throw InternalProgramError(nullptr, PA_CURRENT_FUNCTION, "Image height is too small.");
    }
    Kernels::filter_rgb32(
        matrix,
        image.data(), image.bytes_per_row(),
        (uint32_t)replace_with, replace_if_zero
    );
}















}
