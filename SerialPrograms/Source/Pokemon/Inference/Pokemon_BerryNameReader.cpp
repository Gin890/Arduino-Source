/*  Berry Name Reader
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/OCR/OCR_Routines.h"
#include "Pokemon_BerryNameReader.h"

namespace PokemonAutomation{
namespace Pokemon{


BerryNameReader& BerryNameReader::instance(){
    static BerryNameReader reader;
    return reader;
}


BerryNameReader::BerryNameReader()
    : SmallDictionaryMatcher("Pokemon/BerryNameOCR.json")
{}

#if 0
OCR::StringMatchResult BerryNameReader::read_substring(
    LoggerQt& logger,
    Language language,
    const ConstImageRef& image
) const{
    QString text = OCR::ocr_read(language, image);
    OCR::StringMatchResult ret = match_substring(language, text);
    ret.log(logger, MAX_LOG10P);
    ret.clear_beyond_log10p(MAX_LOG10P);
    return ret;
}
#endif

OCR::StringMatchResult BerryNameReader::read_substring(
    LoggerQt& logger,
    Language language,
    const ConstImageRef& image,
    const std::vector<OCR::TextColorRange>& text_color_ranges,
    double min_text_ratio, double max_text_ratio
) const{
    return match_substring_from_image_multifiltered(
        &logger, language, image, text_color_ranges,
        MAX_LOG10P, MAX_LOG10P_SPREAD, min_text_ratio, max_text_ratio
    );
}



}
}
