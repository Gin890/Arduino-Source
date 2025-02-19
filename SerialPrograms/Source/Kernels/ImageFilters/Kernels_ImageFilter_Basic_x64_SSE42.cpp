/*  Image Filters Basic (x64 SSE4.2)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifdef PA_AutoDispatch_08_Nehalem

#include <stdint.h>
#include <cstddef>
#include <nmmintrin.h>
#include "Kernels/Kernels_x64_SSE41.h"
#include "Kernels/PartialWordAccess/Kernels_PartialWordAccess_x64_SSE41.h"
#include "Kernels_ImageFilter_Basic_Routines.h"

namespace PokemonAutomation{
namespace Kernels{


class ImageFilter_RgbRange_x64_SSE42{
public:
    static const size_t VECTOR_SIZE = 4;

public:
    ImageFilter_RgbRange_x64_SSE42(uint32_t mins, uint32_t maxs, uint32_t replacement, bool invert)
        : m_replacement(_mm_set1_epi32(replacement))
        , m_invert(invert ? _mm_set1_epi32(-1) : _mm_setzero_si128())
        , m_mins(_mm_set1_epi32(mins ^ 0x80808080))
        , m_maxs(_mm_set1_epi32(maxs ^ 0x80808080))
        , m_count(_mm_setzero_si128())
    {}

    PA_FORCE_INLINE size_t count() const{
        return reduce32_x64_SSE41(m_count);
    }

    PA_FORCE_INLINE void process_full(uint32_t* out, const uint32_t* in){
        __m128i pixel = _mm_loadu_si128((const __m128i*)in);
        pixel = process_word(pixel);
        _mm_storeu_si128((__m128i*)out, pixel);
    }
    PA_FORCE_INLINE void process_partial(uint32_t* out, const uint32_t* in, size_t left){
        PartialWordAccess_x64_SSE41 loader(left * sizeof(uint32_t));
        __m128i pixel = loader.load(in);
        pixel = process_word(pixel);
        do{
            out[0] = _mm_cvtsi128_si32(pixel);
            pixel = _mm_srli_si128(pixel, 4);
            out++;
        }while(--left);
    }

private:
    PA_FORCE_INLINE __m128i process_word(__m128i pixel){
        __m128i adj = _mm_xor_si128(pixel, _mm_set1_epi8((uint8_t)0x80));
        __m128i cmp0 = _mm_cmpgt_epi8(m_mins, adj);
        __m128i cmp1 = _mm_cmpgt_epi8(adj, m_maxs);
        cmp0 = _mm_or_si128(cmp0, cmp1);
        cmp0 = _mm_cmpeq_epi32(cmp0, _mm_setzero_si128());
        m_count = _mm_sub_epi32(m_count, cmp0);
        cmp0 = _mm_xor_si128(cmp0, m_invert);
        return _mm_blendv_epi8(m_replacement, pixel, cmp0);
    }

private:
    const __m128i m_replacement;
    const __m128i m_invert;
    const __m128i m_mins;
    const __m128i m_maxs;
    __m128i m_count;
};



size_t filter_rgb32_range_x64_SSE42(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    uint32_t* out0, size_t bytes_per_row0, uint32_t mins0, uint32_t maxs0, uint32_t replacement0, bool invert0
){
    ImageFilter_RgbRange_x64_SSE42 filter0(mins0, maxs0, replacement0, invert0);
    filter_rbg32(image, bytes_per_row, width, height, filter0, out0, bytes_per_row0);
    return filter0.count();
}
void filter_rgb32_range_x64_SSE42(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    FilterRgb32RangeFilter* filter, size_t filter_count
){
    filter_rbg32<ImageFilter_RgbRange_x64_SSE42>(
        image, bytes_per_row, width, height, filter, filter_count
    );
}








class ToBlackWhite_RgbRange_x64_SSE42{
public:
    static const size_t VECTOR_SIZE = 4;

public:
    ToBlackWhite_RgbRange_x64_SSE42(uint32_t mins, uint32_t maxs, bool in_range_black)
        : m_in_range_black(in_range_black ? _mm_set1_epi32(-1) : _mm_setzero_si128())
        , m_mins(_mm_set1_epi32(mins ^ 0x80808080))
        , m_maxs(_mm_set1_epi32(maxs ^ 0x80808080))
        , m_count(_mm_setzero_si128())
    {}

    PA_FORCE_INLINE size_t count() const{
        return reduce32_x64_SSE41(m_count);
    }

    PA_FORCE_INLINE void process_full(uint32_t* out, const uint32_t* in){
        __m128i pixel = _mm_loadu_si128((const __m128i*)in);
        pixel = process_word(pixel);
        _mm_storeu_si128((__m128i*)out, pixel);
    }
    PA_FORCE_INLINE void process_partial(uint32_t* out, const uint32_t* in, size_t left){
        PartialWordAccess_x64_SSE41 loader(left * sizeof(uint32_t));
        __m128i pixel = loader.load(in);
        pixel = process_word(pixel);
        do{
            out[0] = _mm_cvtsi128_si32(pixel);
            pixel = _mm_srli_si128(pixel, 4);
            out++;
        }while(--left);
    }

private:
    PA_FORCE_INLINE __m128i process_word(__m128i pixel){
        __m128i adj = _mm_xor_si128(pixel, _mm_set1_epi8((uint8_t)0x80));
        __m128i cmp0 = _mm_cmpgt_epi8(m_mins, adj);
        __m128i cmp1 = _mm_cmpgt_epi8(adj, m_maxs);
        cmp0 = _mm_or_si128(cmp0, cmp1);
        cmp0 = _mm_cmpeq_epi32(cmp0, _mm_setzero_si128());
        m_count = _mm_sub_epi32(m_count, cmp0);
        cmp0 = _mm_xor_si128(cmp0, m_in_range_black);
        return _mm_or_si128(cmp0, _mm_set1_epi32(0xff000000));
    }

private:
    const __m128i m_in_range_black;
    const __m128i m_mins;
    const __m128i m_maxs;
    __m128i m_count;
};



size_t to_blackwhite_rgb32_range_x64_SSE42(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    uint32_t* out0, size_t bytes_per_row0, uint32_t mins0, uint32_t maxs0, bool in_range_black0
){
    ToBlackWhite_RgbRange_x64_SSE42 filter0(mins0, maxs0, in_range_black0);
    filter_rbg32(image, bytes_per_row, width, height, filter0, out0, bytes_per_row0);
    return filter0.count();
}
void to_blackwhite_rgb32_range_x64_SSE42(
    const uint32_t* image, size_t bytes_per_row, size_t width, size_t height,
    ToBlackWhiteRgb32RangeFilter* filter, size_t filter_count
){
    to_blackwhite_rbg32<ToBlackWhite_RgbRange_x64_SSE42>(
        image, bytes_per_row, width, height, filter, filter_count
    );
}








}
}
#endif
