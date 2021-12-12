/*  Tesseract Wrapper
 * 
 *  From: https://github.com/PokemonAutomation/
 * 
 */

#ifndef PokemonAutomation_TesseractPA_H
#define PokemonAutomation_TesseractPA_H

#include <stdint.h>

//#define TESSERACT_STATIC

#ifdef TESSERACT_STATIC
#define TESSERACT_EXPORT
#else

#ifdef _WIN32

#ifdef _WINDLL
#define TESSERACT_EXPORT __declspec(dllexport)
#else
#define TESSERACT_EXPORT __declspec(dllimport)
#endif // end for _WINDLL


#ifdef __cplusplus
extern "C" {
#endif

// We build a custom Tesseract library,
// https://github.com/PokemonAutomation/Tesseract-OCR_for_Windows
// to ship with SerialProgram on Windows, in the form of tesseractPA.dll.
// The following C API is a wrapper for the raw Tesseract API. The wrapper
// along with Tesseract itself is implemented in tesseractPA.dll.
struct TesseractAPI_internal;
TESSERACT_EXPORT TesseractAPI_internal* TesseractAPI_construct(
    const char* path, const char* language
);
TESSERACT_EXPORT void TesseractAPI_destroy(TesseractAPI_internal* api);

//TESSERACT_EXPORT char* TesseractAPI_read_file(TesseractAPI_internal* api, const char* filepath);
TESSERACT_EXPORT char* TesseractAPI_read_bitmap(
    TesseractAPI_internal* api,
    const unsigned char* data,
    size_t width, size_t height,
    size_t bytes_per_pixel, size_t bytes_per_line,
    size_t ppi
);
TESSERACT_EXPORT void Tesseract_delete(char* text);


#ifdef __cplusplus
}
#endif

// end for _WIN32
#elif defined(UNIX_LINK_TESSERACT)

// We hope to use our own Tesseract build in future for Unix systems too.
// But right now to run on Linux and Mac we need to use external Tesseract library API directly.
// So the Tesseract API wrapper is defined here to match Windows.

#include <tesseract/baseapi.h>
#include <iostream>

struct TesseractAPI_internal{
    tesseract::TessBaseAPI m_api;

    TesseractAPI_internal(const char* path, const char* language){
        if (m_api.Init(path, language)){
            throw "Could not initialize TesseractAPI.";
        }
    }
};

inline TesseractAPI_internal* TesseractAPI_construct(const char* path, const char* language){
    try{
        return new TesseractAPI_internal(path, language);
    }catch (const char* err){
        std::cout << err << std::endl;
    }
    return nullptr;
}

inline void TesseractAPI_destroy(TesseractAPI_internal* api){
    delete api;
}

inline char* TesseractAPI_read_bitmap(
    TesseractAPI_internal* api,
    const unsigned char* data,
    size_t width, size_t height,
    size_t bytes_per_pixel, size_t bytes_per_line,
    size_t ppi
){
    api->m_api.SetImage(data, (int)width, (int)height, (int)bytes_per_pixel, (int)bytes_per_line);
    api->m_api.SetSourceResolution((int)ppi);
    return api->m_api.GetUTF8Text();
}
inline void Tesseract_delete(char* text){
    delete[] text;
}

// end for UNIX_LINK_TESSERACT
#else

// #define TESSERACT_EXPORT __attribute__((visibility("default")))

#endif 
#endif // end for TESSERACT_STATIC


class TesseractString{
public:
    ~TesseractString(){
#ifdef PA_TESSERACT
        if (m_str != nullptr){
            Tesseract_delete(m_str);
        }
#endif
    }
    TesseractString(const TesseractString&) = delete;
    void operator=(const TesseractString&) = delete;
    TesseractString(TesseractString&& x)
        : m_str(x.m_str)
    {
        x.m_str = nullptr;
    }
    void operator=(TesseractString&& x){
        m_str = x.m_str;
        x.m_str = nullptr;
    }

public:
    const char* c_str() const{
        return m_str;
    }

private:
    TesseractString(char* str)
        : m_str(str)
    {}

    friend class TesseractAPI;
    char* m_str;
};


class TesseractAPI{
public:
    ~TesseractAPI(){
#ifdef PA_TESSERACT
        if (m_api != nullptr){
            TesseractAPI_destroy(m_api);
        }
#endif
    }
    TesseractAPI(const TesseractAPI&) = delete;
    void operator=(const TesseractAPI&) = delete;
    TesseractAPI(TesseractAPI&& x)
        : m_api(x.m_api)
    {
        x.m_api = nullptr;
    }
    void operator=(TesseractAPI&& x){
        m_api = x.m_api;
        x.m_api = nullptr;
    }

public:
    TesseractAPI(const char* path, const char* language)
#ifdef PA_TESSERACT
        : m_api(TesseractAPI_construct(path, language))
#endif
    {}

    bool valid() const{ return m_api != nullptr; }

//    TesseractString read(const char* filepath){
//#ifdef PA_TESSERACT
//        return TesseractAPI_read_file(m_api, filepath);
//#else
//        return nullptr;
//#endif
//    }
    TesseractString read32(
        const unsigned char* data,
        size_t width, size_t height,
        size_t bytes_per_line, size_t ppi = 100
    ){
#ifdef PA_TESSERACT
        return TesseractAPI_read_bitmap(
            m_api,
            data,
            width, height,
            sizeof(uint32_t),
            bytes_per_line,
            ppi
        );
#else
        return nullptr;
#endif
    }

private:
    TesseractAPI_internal* m_api = nullptr;
};




#endif

