#ifndef VALIDATION_HELPER_HPP
#define VALIDATION_HELPER_HPP

#include <wx/string.h>

bool ValidateHexValue(const wxString& str);
bool ValidateDecimalValue(const wxString& str);
bool ValidateBase64Value(const wxString& str);

int16_t ParseIntegralValueOrDie(const wxString& str);

extern const wxString IntegralChars;
extern const wxString HexadecimalChars;
extern const wxString DecimalChars;
extern const wxString Base64Chars;

#endif
