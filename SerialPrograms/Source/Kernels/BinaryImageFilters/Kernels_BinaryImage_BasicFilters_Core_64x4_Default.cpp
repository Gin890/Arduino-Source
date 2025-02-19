/*  Binary Image Basic Filters (Default)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Kernels/BinaryMatrix/Kernels_BinaryMatrix_Arch_64x4_Default.h"
#include "Kernels_BinaryImage_BasicFilters_Routines.h"
#include "Kernels_BinaryImage_BasicFilters_Default.h"

namespace PokemonAutomation{
namespace Kernels{



void compress_rgb32_to_binary_range_64x4_Default(
    const uint32_t* image, size_t bytes_per_row,
    PackedBinaryMatrix_IB& matrix0, uint32_t mins0, uint32_t maxs0
){
    Compressor_RgbRange_Default compressor0(mins0, maxs0);
    compress_rgb32_to_binary(
        image, bytes_per_row,
        static_cast<PackedBinaryMatrix_64x4_Default&>(matrix0).get(), compressor0
    );
}
void compress_rgb32_to_binary_range_64x4_Default(
    const uint32_t* image, size_t bytes_per_row,
    CompressRgb32ToBinaryRangeFilter* filters, size_t filter_count
){
    compress_rgb32_to_binary<PackedBinaryMatrix_64x4_Default, Compressor_RgbRange_Default>(
        image, bytes_per_row, filters, filter_count
    );
}



void filter_rgb32_64x4_Default(
    const PackedBinaryMatrix_IB& matrix,
    uint32_t* image, size_t bytes_per_row,
    uint32_t replace_with, bool replace_if_zero
){
    BinaryFilter_RgbRange_Default filter(replace_with, replace_if_zero);
    filter_rgb32(static_cast<const PackedBinaryMatrix_64x4_Default&>(matrix).get(), image, bytes_per_row, filter);
}




}
}
