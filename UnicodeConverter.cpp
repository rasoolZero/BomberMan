#include "UnicodeConverter.h"
#ifdef _WIN32
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> UnicodeConverter::converter;
#else //unix
std::wstring_convert<std::codecvt_utf8<wchar_t>> UnicodeConverter::converter;
#endif // _WIN32
std::string UnicodeConverter::to_utf8(const std::wstring& wide_str)
{
    return converter.to_bytes(wide_str);
}

std::wstring UnicodeConverter::to_wide(const std::string& str)
{
    return converter.from_bytes(str);
}
