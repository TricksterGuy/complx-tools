#include "ValidationHelper.hpp"
#include <wx/base64.h>

const wxString IntegralChars = "0123456789ABCDEFabcdefx-";
const wxString HexadecimalChars = "0123456789ABCDEFabcdefx";
const wxString DecimalChars = "0123456789-";
const wxString Base64Chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/=";

bool ValidateHexValue(const wxString& str)
{
    if (str.IsEmpty())
        return false;

    // Reject strings that don't begin with x
    if (str[0] != 'x')
        return false;

    // Reject x
    if (str.Length() == 1)
        return false;

    // Reject x-34, xxxxx
    for (size_t i = 1; i < str.Length(); i++)
        if (!isxdigit(str[i]))
            return false;

    return true;
}

bool ValidateDecimalValue(const wxString& str)
{
    if (str.IsEmpty())
        return false;

    // Reject a192
    if (str[0] == '-' && str.Length() == 1)
        return false;

    if (str[0] != '-' && !isdigit(str[0]))
        return false;

    for (size_t i = 1; i < str.Length(); i++)
        if (!isdigit(str[i]))
            return false;

    return true;
}

bool ValidateBase64Value(const wxString& str)
{
    size_t err = -1;
	auto buffer = wxBase64Decode(str, wxBase64DecodeMode_Strict, &err);

	return (buffer.IsEmpty() || err != static_cast<size_t>(-1));
}

int16_t ParseIntegralValueOrDie(const wxString& str)
{
    long convert;
    if (str[0] == 'x')
    {
        bool ret = str.Mid(1).ToCULong(reinterpret_cast<unsigned long*>(&convert), 16);
        wxASSERT(ret);
        return convert;
    }
    else
    {
        bool ret = str.ToCLong(&convert, 10);
        wxASSERT(ret);
        return convert;
    }
}
