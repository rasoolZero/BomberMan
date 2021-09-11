#ifndef UNICODE_CONVERTER
#define UNICODE_CONVERTER
#include <codecvt>
#include <locale>

class UnicodeConverter {
public:
  static std::string to_utf8(const std::wstring &wide_str);
  static std::wstring to_wide(const std::string &str);

private:
  UnicodeConverter();
#ifdef _WIN32
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#else  // unix
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
#endif // _WIN32
};

/*static std::string to_utf8(const std::wstring& wide_str) {
#ifdef _WIN32
        static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#else //unix
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
#endif // _WIN32
        return converter.to_bytes(wide_str);
}

static std::wstring to_wide(const std::string& str) {
#ifdef _WIN32
        static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#else //unix
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
#endif // _WIN32
        return converter.from_bytes(str);
}*/
#endif // !UNICODE_CONVERTER
