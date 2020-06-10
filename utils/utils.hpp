/******************************************************************
 * Copyright (C) 2020 Matthias Rosenthal
 * 
 * This file is part of The Interactive 8051 Disassembler.
 * 
 * The Interactive 8051 Disassembler is licensed under Creative
 * Commons-Attribution-Noncommercial-NoDerivative (CC BY-NC-ND).
 * See https://creativecommons.org/licenses/by-nc-nd/4.0/legalcode
 * 
 * Additionally, The Interactive 8051 Disassembler including
 * binaries generated from its source code is only allowed to be
 * used for non-commercial purposes.
 *
 * The Interactive 8051 Disassembler is distributed in the hope 
 * that it will be useful, but WITHOUT ANY WARRANTY; without 
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR 
 * A PARTICULAR PURPOSE.
 *****************************************************************/
 
#ifndef P_UTILS_HPP_INCLUDED
#define P_UTILS_HPP_INCLUDED

#include <inttypes.h>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cmath>
#include <stdexcept>
#include <chrono>

#include "../consts.hpp"

std::string fn_Int_To_StringU(uint64_t i);
std::string fn_Int_To_String(int64_t i);

template<typename Tstr> // implemented for std::string and std::basic_string<uint8_t>
std::string fn_FormatToHex(Tstr string, bool kleinbuchstaben); // Turns "abc" into "61 62 63"

inline void fn_usleep(long long int us) {
	timespec t;
	t.tv_sec = us / 1000000;
	t.tv_nsec = (us % 1000000) * 1000;
	nanosleep(&t, NULL);
}

#define DATE_MIN_YEAR (1800)
#define DATE_MAX_YEAR (2200)

namespace utils {

uint8_t Hex_To_uint8(char hex1, char hex2); // Upper and lower chars are possible

std::string Format_Defaultdate_stdString(long long int datum, char ms_trennzeichen = ':');
std::string Format_Sec_Standarddatum_stdString(time_t datum);

std::string Int_To_String(int64_t i);
// Before an optional minus sign zeros are inserted so that the width equals to the width parameter.
std::string Int_To_String_Zeros(long long int i, unsigned long int width);

std::string Int_To_String_Hex(int64_t i, bool lowercase = true);

// suffix _HexOk: "0x" at the begin is discarded and only the rest of the string is read as hex format.
// suffix R: also fractions possible (p. e. 5/3).
class string_to_int_converter
{
private:
    int fehler;

public:
    string_to_int_converter();
private:
    void Init();
public:

    long long int ConvertToLLI(std::string rohstring);
    long long int ConvertToLLI_Hex(std::string rohstring);
    long long int ConvertToLLI_HexOk(std::string rohstring); // only hex if starting with 0x
    long long int ConvertToLLIFromSIt(std::string::iterator begin, std::string::iterator end);
    long long int ConvertToLLIFromSIt_Hex(std::string::iterator begin, std::string::iterator end);
    long long int ConvertToLLIFromSIt_HexOk(std::string::iterator begin, std::string::iterator end); // only hex if starting with 0x
    long long int ConvertToLLIFromSIt(std::string::iterator begin, std::string::iterator end, char tausender_zeichen);
    // Possible values when punct_char is '.': "5e-3", "5.2e+3", ".e-4" and "+5".
    double ConvertToDoubleFromSIt(std::string::iterator begin, std::string::iterator end, char punct_char = '.', char tausender_zeichen = ',');
    double ConvertToDoubleRFromSIt(std::string::iterator begin, std::string::iterator end, char punct_char = '.', char tausender_zeichen = ',');
    double ConvertToDouble(std::string rohstring, char punct_char = '.', char tausender_zeichen = ',');
    double ConvertToDoubleR(std::string rohstring, char punct_char = '.', char tausender_zeichen = ',');

    bool MistakeHappened();
    int GetError() {
		return fehler;
	}
};
class string_to_int_limited_converter:
    public string_to_int_converter
{
private:
    int limitfehler;

public:
    string_to_int_limited_converter();
private:
    void Init();
public:

    long long int ConvertToLLI_Limited(std::string rohstring, long long int min, long long int max);
    long long int ConvertToLLI_Hex_Limited(std::string rohstring, long long int min, long long int max);
    long long int ConvertToLLI_HexOk_Limited(std::string rohstring, long long int min, long long int max);
    long long int ConvertToLLIFromSIt_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max);
    long long int ConvertToLLIFromSIt_Hex_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max);
    long long int ConvertToLLIFromSIt_HexOk_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max);
    double ConvertToDouble_Limited(std::string rohstring, double min, double max, char punct_char = '.', char tausender_zeichen = ',');
    double ConvertToDoubleR_Limited(std::string rohstring, double min, double max, char punct_char = '.', char tausender_zeichen = ',');

    bool LimitMistakeHappened();
    int GetLimiterror() {
		return limitfehler;
	}
};

bool GetFileString(std::string filename, std::string* value, bool binary = false); // value is overwritten, not appended.
bool SetFileString(std::string filename, const std::string &value);

bool str_begins_with(const std::string &str, const std::string &begin);
bool str_ends_with(const std::string &str, const std::string &end);
// chars durch chars ersetzen geht mit algorithm: std::replace
void str_replace_char(std::string &str, char suchen, std::string ersetzen);
std::vector<std::string> str_split(const std::string &str, const std::string &split); // The returned vector
										// contains at least one string (that is empty), even if str is empty.
std::string str_to_upper(std::string str); // auch ä, ö, ü
bool str_replace_one(std::string &str, const std::string &search, const std::string &replace); // true if found

std::string vectorstring_unsplit(const std::vector<std::string> &array, char zeichen);

std::string Char_To_UTF8String(uint32_t character);
void Char_To_UTF8String(uint32_t character, std::string* str_ret); // appends the result to str_ret

inline std::basic_string<uint8_t> StdStringToUstring(std::string str) {
	return std::basic_string<uint8_t>((uint8_t*)str.data(), str.length());
}
inline std::string StdUstringToString(std::basic_string<uint8_t> str) {
	return std::string((char*)str.data(), str.length());
}
std::string str_remove_sidewhites(const std::string &text); // Removes tabulators, spaces, line endings and \r at the first and at the last position.

inline bool is_hexchar(char c) {
	return ((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f'));
}

uint64_t GetTime_Microseconds(); // 100 years need 52 bit
long long int GetTime_Milliseconds(); // 100 years beed 42 bit -> long long int required (-> 64 bit)
double GetTimeAsDouble();

} // namespace utils

#endif // P_UTILS_HPP_INCLUDED
