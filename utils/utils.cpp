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
 
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <unistd.h>
#include <cstring>
#include <istream>
#include <iterator>
#include <errno.h>

#include <iostream>

#include "utils.hpp"

#ifdef __unix__
// für fn_DatumUebernehmen
#include <utime.h>
#endif // __unix__
#ifdef _WIN32 // both 32 and 64 bit
 #include <chrono>
#endif // _WIN32

std::string fn_Int_To_StringU(uint64_t i)
{
    if(i == 0)
    {
        return std::string("0");
    }
    else
    {
        std::string ret;
        char zeichen;
        while(i != 0)
        {
            zeichen = static_cast<char>(i % 10) + 48;
            ret.insert(0, 1, zeichen);
            i /= 10;
        }
        return ret;
    }
}
std::string fn_Int_To_String(int64_t i)
{
    if(i == 0)
    {
        return std::string("0");
    }
    else
    {
        std::string ret;
        bool minuszeichen;
        if(i < 0) {
            i = 0 - i;
            minuszeichen = true;
        }
        else {
            minuszeichen = false;
        }
        char zeichen;
        while(i != 0)
        {
            zeichen = static_cast<char>(i % 10) + 48;
            ret.insert(0, 1, zeichen);
            i /= 10;
        }
        if(minuszeichen) {
            ret.insert(0, 1, '-');
        }
        return ret;
    }
}

//-------------------------------------------------------------------------------------------------------------------------

template<typename Tstr>
std::string fn_FormatToHex(Tstr string, bool kleinbuchstaben)
{
    std::string str_ret;
    typename Tstr::iterator it_zeichen = string.begin();
    if(string.empty()) {
        return str_ret;
    }
    if(kleinbuchstaben) {
        uint8_t umwandlung = (((*it_zeichen) & 0xf0) >> 4);
        if(umwandlung > 9) {
            str_ret.append(1, 'a' - 10 + umwandlung);
        }
        else {
            str_ret.append(1, '0' + umwandlung);
        }
        umwandlung = (*it_zeichen) & 0x0f;
        if(umwandlung > 9) {
            str_ret.append(1, 'a' - 10 + umwandlung);
        }
        else {
            str_ret.append(1, '0' + umwandlung);
        }
        for(it_zeichen++; it_zeichen != string.end(); it_zeichen++) {
            str_ret.append(1, ' ');
            umwandlung = (((*it_zeichen) & 0xf0) >> 4);
            if(umwandlung > 9) {
                str_ret.append(1, 'a' - 10 + umwandlung);
            }
            else {
                str_ret.append(1, '0' + umwandlung);
            }
            umwandlung = (*it_zeichen) & 0x0f;
            if(umwandlung > 9) {
                str_ret.append(1, 'a' - 10 + umwandlung);
            }
            else {
                str_ret.append(1, '0' + umwandlung);
            }
        }
    }
    else {
        uint8_t umwandlung = (((*it_zeichen) & 0xf0) >> 4);
        if(umwandlung > 9) {
            str_ret.append(1, 'A' - 10 + umwandlung);
        }
        else {
            str_ret.append(1, '0' + umwandlung);
        }
        umwandlung = (*it_zeichen) & 0x0f;
        if(umwandlung > 9) {
            str_ret.append(1, 'A' - 10 + umwandlung);
        }
        else {
            str_ret.append(1, '0' + umwandlung);
        }
        for(it_zeichen++; it_zeichen != string.end(); it_zeichen++) {
            str_ret.append(1, ' ');
            umwandlung = (((*it_zeichen) & 0xf0) >> 4);
            if(umwandlung > 9) {
                str_ret.append(1, 'A' - 10 + umwandlung);
            }
            else {
                str_ret.append(1, '0' + umwandlung);
            }
            umwandlung = (*it_zeichen) & 0x0f;
            if(umwandlung > 9) {
                str_ret.append(1, 'A' - 10 + umwandlung);
            }
            else {
                str_ret.append(1, '0' + umwandlung);
            }
        }
    }
    return str_ret;
}
// Instantiations
template std::string fn_FormatToHex(std::string string, bool kleinbuchstaben);
template std::string fn_FormatToHex(std::basic_string<uint8_t> string, bool kleinbuchstaben);

//-----------------------------------------------------------------------------------------------------------------------

bool fn_str_begins_with(std::string str, std::string begin)
{
	if(str.size() >= begin.size()) {
		for(unsigned int i1 = 0; i1 < begin.size(); i1++) {
			if(str[i1] != begin[i1]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
bool fn_str_ends_with(std::string str, std::string end)
{
	if(str.size() >= end.size()) {
		unsigned int offset = str.size() - end.size();
		for(unsigned int i1 = 0; i1 < end.size(); i1++) {
			if(str[i1+offset] != end[i1]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
void fn_str_replace_char(std::string &str, char suchen, std::string ersetzen)
{
	for(unsigned int izeichen = 0; izeichen < str.length(); ) {
		if(str[izeichen] == suchen) {
			str.replace(izeichen, 1, ersetzen);
			izeichen += ersetzen.length();
		}
		else {
			izeichen++;
		}
	}
}

//------------------------------------------------------------------------------------------------------------

namespace utils {


//----------------------------------------------------------------------------------------------------------------------

uint8_t Hex_To_uint8(char hex1, char hex2)
{
    uint8_t ret = 0;
    if((hex1 > 64) && (hex1 < 71)) //A-F
    {
        hex1 -= 65;
        hex1 += 10;
    }
    else if((hex1 > 96) && (hex1 < 103)) //a - f
    {
        hex1 -= 97;
        hex1 += 10;
    }
    else if((hex1 > 47) && (hex1 < 58)) //0-9
    {
        hex1 -= 48;
    }
    else
    {
        return 0;
    }
    if((hex2 > 64) && (hex2 < 71)) //A-F
    {
        hex2 -= 65;
        hex2 += 10;
    }
    else if((hex2 > 96) && (hex2 < 103)) //a - f
    {
        hex2 -= 97;
        hex2 += 10;
    }
    else if((hex2 > 47) && (hex2 < 58)) //0-9
    {
        hex2 -= 48;
    }
    else
    {
        return 0;
    }
    ret = hex2;
    ret += hex1 * 16;
    return ret;
}

std::string Format_Defaultdate_stdString(long long int datum, char ms_trennzeichen)
{
    time_t sec = datum / 1000;
    long int ms = datum - ((long long int)sec) * 1000;
    char* c_datum = new char[100];
    tm* tmZeit = std::localtime(&sec);
    std::strftime(c_datum, 100, "%d.%m.%Y %X", tmZeit);
    std::string ret(c_datum);
    delete c_datum;
    ret.append(1, ms_trennzeichen);
    if(ms >= 100)
    {
    }
    else if(ms >= 10)
    {
        ret.append("0");
    }
    else
    {
        ret.append("00");
    }
    ret.append(Int_To_String(ms));
    return ret;
}
std::string Format_Sec_Standarddatum_stdString(time_t datum)
{
    char c_datum[100];
    tm* tmZeit = std::localtime(&datum);
    std::strftime(c_datum, 100, "%d.%m.%Y %X", tmZeit);
    return std::string(c_datum);
}

std::string Int_To_String(int64_t i)
{
    if(i == 0)
    {
        return std::string("0");
    }
    else
    {
        std::string ret;
        bool minuszeichen;
        if(i < 0) {
            i = 0 - i;
            minuszeichen = true;
        }
        else {
            minuszeichen = false;
        }
        char zeichen;
        while(i != 0)
        {
            zeichen = static_cast<char>(i % 10) + 48;
            ret.insert(0, 1, zeichen);
            i /= 10;
        }
        if(minuszeichen) {
            ret.insert(0, 1, '-');
        }
        return ret;
    }
}
std::string Int_To_String_Zeros(long long int i, unsigned long int breite)
{
    if(i == 0)
    {
        return std::string(breite, '0');
    }
    else
    {
        std::string ret;
        bool minuszeichen;
        if(i < 0) {
            i = 0 - i;
            minuszeichen = true;
        }
        else {
            minuszeichen = false;
        }
        char zeichen;
        while(i != 0)
        {
            long long int i_durch_10 = i / 10;
            zeichen = i - i_durch_10 * 10 + 48;
            ret.insert(0, 1, zeichen);
            i = i_durch_10;
        }
        if(minuszeichen) {
            int anzahl_vornullen = ((int)breite) - ((int)ret.size()) - 1;
            if(anzahl_vornullen > 0) {
                ret.insert(0, anzahl_vornullen, '0');
            }
            ret.insert(0, 1, '-');
        }
        else {
            int anzahl_vornullen = ((int)breite) - ((int)ret.size());
            if(anzahl_vornullen > 0) {
                ret.insert(0, anzahl_vornullen, '0');
            }
        }
        return ret;
    }
}

std::string Int_To_String_Hex(int64_t i, bool lowercase)
{
    if(i == 0)
    {
        return std::string("0");
    }
    else
    {
        std::string ret;
        bool minuszeichen;
        if(i < 0) {
            i = 0 - i;
            minuszeichen = true;
        }
        else {
            minuszeichen = false;
        }
        char zeichen;
        while(i != 0)
        {
            zeichen = static_cast<char>(i % 16) + 48;
            if(zeichen > '9') {
                zeichen += (lowercase ? 'a' : 'A') - '9' - 1;
            }
            ret.insert(0, 1, zeichen);
            i /= 16;
        }
        if(minuszeichen) {
            ret.insert(0, 1, '-');
        }
        return ret;
    }
}

//-----------------------------------------------------------------------------------------------------------------------

string_to_int_converter::string_to_int_converter()
{
    Init();
}
void string_to_int_converter::Init()
{
    fehler = 0;
}

long long int string_to_int_converter::ConvertToLLI(std::string rohstring)
{
    return ConvertToLLIFromSIt(rohstring.begin(), rohstring.end());
}
long long int string_to_int_converter::ConvertToLLI_Hex(std::string rohstring)
{
    return ConvertToLLIFromSIt_Hex(rohstring.begin(), rohstring.end());
}
long long int string_to_int_converter::ConvertToLLI_HexOk(std::string rohstring)
{
    return ConvertToLLIFromSIt_HexOk(rohstring.begin(), rohstring.end());
}
long long int string_to_int_converter::ConvertToLLIFromSIt(std::string::iterator begin, std::string::iterator end)
{
    if(begin == end) {
        fehler++;
        return 0;
    }
    else {
        long long int ret = 0;
        std::string::iterator it_zeichen = begin;
        bool negativ;
        if(*it_zeichen == '-') {
            negativ = true;
            it_zeichen++;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else if(*it_zeichen == '+') {
            negativ = false;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else {
            negativ = false;
        }
        for(; it_zeichen != end; it_zeichen++) {
            long long int neue_ziffer = (*it_zeichen) - '0';
            if((neue_ziffer < 0) || (neue_ziffer > 9)) {
                fehler++;
                return 0;
            }
            ret = ret * 10 + neue_ziffer;
        }
        if(negativ) {
            return 0 - ret;
        }
        else {
            return ret;
        }
    }
}
long long int string_to_int_converter::ConvertToLLIFromSIt_Hex(std::string::iterator begin, std::string::iterator end)
{
    if(begin == end) {
        fehler++;
        return 0;
    }
    else {
        long long int ret = 0;
        for(std::string::iterator it_zeichen = begin; it_zeichen != end; it_zeichen++) {
            ret <<= 4;
            if((*it_zeichen >= '0') && (*it_zeichen <= '9')) {
                ret |= (long long int)((*it_zeichen) - '0') & 0x0f;
            }
            else if((*it_zeichen >= 'A') && (*it_zeichen <= 'F')) {
                ret |= (long long int)((*it_zeichen) - 55) & 0x0f; // 'A' ist Nr. 65
            }
            else if((*it_zeichen >= 'a') && (*it_zeichen <= 'f')) {
                ret |= (long long int)((*it_zeichen) - 87) & 0x0f; // 'a' ist Nr. 97
            }
            else {
                fehler++;
                return 0;
            }
        }
        return ret;
    }
}
long long int string_to_int_converter::ConvertToLLIFromSIt_HexOk(std::string::iterator begin, std::string::iterator end)
{
    if(begin == end) {
        fehler++;
        return 0;
    }
    else {
        long long int ret = 0;
        if(*begin == '0') {
			begin++;
			if(*begin == 'x') {
				begin++;
				if(begin == end) {
					fehler++;
					return 0;
				}
				for(std::string::iterator it_zeichen = begin; it_zeichen != end; it_zeichen++) {
					ret <<= 4;
					if((*it_zeichen >= '0') && (*it_zeichen <= '9')) {
						ret |= (long long int)((*it_zeichen) - '0') & 0x0f;
					}
					else if((*it_zeichen >= 'A') && (*it_zeichen <= 'F')) {
						ret |= (long long int)((*it_zeichen) - 55) & 0x0f; // 'A' ist Nr. 65
					}
					else if((*it_zeichen >= 'a') && (*it_zeichen <= 'f')) {
						ret |= (long long int)((*it_zeichen) - 87) & 0x0f; // 'a' ist Nr. 97
					}
					else {
						fehler++;
						return 0;
					}
				}
				return ret;
			}
			else {
				begin--;
			}
		}
        
        std::string::iterator it_zeichen = begin;
        bool negativ;
        if(*it_zeichen == '-') {
            negativ = true;
            it_zeichen++;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else if(*it_zeichen == '+') {
            negativ = false;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else {
            negativ = false;
        }
        for(; it_zeichen != end; it_zeichen++) {
            long long int neue_ziffer = (*it_zeichen) - '0';
            if((neue_ziffer < 0) || (neue_ziffer > 9)) {
                fehler++;
                return 0;
            }
            ret = ret * 10 + neue_ziffer;
        }
        if(negativ) {
            return 0 - ret;
        }
        else {
            return ret;
        }
    }
}
long long int string_to_int_converter::ConvertToLLIFromSIt(std::string::iterator begin, std::string::iterator end, char tausender_zeichen)
{
    if(begin == end) {
        fehler++;
        return 0;
    }
    else {
        long long int ret = 0;
        std::string::iterator it_zeichen = begin;
        bool negativ;
        if(*it_zeichen == '-') {
            negativ = true;
            it_zeichen++;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else if(*it_zeichen == '+') {
            negativ = false;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else {
            negativ = false;
        }
        for(; it_zeichen != end; it_zeichen++) {
            if(*it_zeichen != tausender_zeichen) {
                long long int neue_ziffer = (*it_zeichen) - '0';
                if((neue_ziffer < 0) || (neue_ziffer > 9)) {
                    fehler++;
                    return 0;
                }
                ret = ret * 10 + neue_ziffer;
            }
        }
        if(negativ) {
            return 0 - ret;
        }
        else {
            return ret;
        }
    }
}
double string_to_int_converter::ConvertToDouble(std::string rohstring, char punct_char, char tausender_zeichen)
{
    return ConvertToDoubleFromSIt(rohstring.begin(), rohstring.end(), punct_char, tausender_zeichen);
}
double string_to_int_converter::ConvertToDoubleR(std::string rohstring, char punct_char, char tausender_zeichen)
{
    return ConvertToDoubleRFromSIt(rohstring.begin(), rohstring.end(), punct_char, tausender_zeichen);
}
double string_to_int_converter::ConvertToDoubleFromSIt(std::string::iterator begin, std::string::iterator end, char punct_char, char tausender_zeichen)
{
    if(begin == end) {
        fehler++;
        return 0;
    }
    else {
        double ret = 0;
        std::string::iterator it_zeichen = begin;
        bool negativ;
        if(*it_zeichen == '-') {
            negativ = true;
            it_zeichen++;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else if(*it_zeichen == '+') {
            negativ = false;
            if(it_zeichen == end) {
                fehler++;
                return 0;
            }
        }
        else {
            negativ = false;
        }
        for(; it_zeichen != end; it_zeichen++) {
            if(*it_zeichen == punct_char) {
                double kommastelle_fac = 1;
                for(it_zeichen++; it_zeichen != end; it_zeichen++) {
                    if(*it_zeichen == 'e') {
                        it_zeichen++;
                        if(it_zeichen == end) {
                            fehler++;
                            return 0;
                        }
                        else {
                            if(*it_zeichen == '+') {
                                it_zeichen++;
                                ret *= std::pow(10, ConvertToLLIFromSIt(it_zeichen, end));
                            }
                            else if(*it_zeichen == '-') {
                                it_zeichen++;
                                ret *= std::pow(0.1, ConvertToLLIFromSIt(it_zeichen, end));
                            }
                            else {
                                fehler++;
                                return 0;
                            }
                        }
                        break;
                    }
                    else {
                        kommastelle_fac *= 0.1;
                        ret = ret + ((double)((*it_zeichen) - '0')) * kommastelle_fac;
                    }
                }
                break;
            }
            else if(*it_zeichen == 'e') {
                it_zeichen++;
                if(it_zeichen == end) {
                    fehler++;
                    return 0;
                }
                else {
                    if(*it_zeichen == '+') {
                        it_zeichen++;
                        ret *= std::pow(10, ConvertToLLIFromSIt(it_zeichen, end));
                    }
                    else if(*it_zeichen == '-') {
                        it_zeichen++;
                        ret *= std::pow(0.1, ConvertToLLIFromSIt(it_zeichen, end));
                    }
                    else {
                        fehler++;
                        return 0;
                    }
                }
                break;
            }
            else if(*it_zeichen != tausender_zeichen) {
                double neue_ziffer = (*it_zeichen) - '0';
                if((neue_ziffer < 0) || (neue_ziffer > 9)) {
                    fehler++;
                    return 0;
                }
                ret = ret * 10 + neue_ziffer;
            }
        }
        if(negativ) {
            return 0 - ret;
        }
        else {
            return ret;
        }
    }
}
double string_to_int_converter::ConvertToDoubleRFromSIt(std::string::iterator begin, std::string::iterator end, char punct_char, char tausender_zeichen)
{
	std::string::iterator it_slash;
	for(it_slash = begin; it_slash != end; it_slash++) {
		if(*it_slash == '/') {
			break;
		}
	}
	if(it_slash == end) {
		return ConvertToDoubleFromSIt(begin, end, punct_char, tausender_zeichen);
	}
	else {
		long long int zaehler = ConvertToLLIFromSIt(begin, it_slash);
		it_slash++;
		long long int nenner = ConvertToLLIFromSIt(it_slash, end);
		if(nenner == 0) {
			fehler++;
			return 0;
		}
		else {
			return double(zaehler) / nenner;
		}
	}
}
bool string_to_int_converter::MistakeHappened()
{
    return (fehler != 0);
}

//-----------------------------------------------------------------------------------------------------------------------

string_to_int_limited_converter::string_to_int_limited_converter()
{
    Init();
}
void string_to_int_limited_converter::Init()
{
    limitfehler = 0;
}

long long int string_to_int_limited_converter::ConvertToLLI_Limited(std::string rohstring, long long int min, long long int max)
{
    return ConvertToLLIFromSIt_Limited(rohstring.begin(), rohstring.end(), min, max);
}
long long int string_to_int_limited_converter::ConvertToLLI_Hex_Limited(std::string rohstring, long long int min, long long int max)
{
    return ConvertToLLIFromSIt_Hex_Limited(rohstring.begin(), rohstring.end(), min, max);
}
long long int string_to_int_limited_converter::ConvertToLLI_HexOk_Limited(std::string rohstring, long long int min, long long int max)
{
    return ConvertToLLIFromSIt_HexOk_Limited(rohstring.begin(), rohstring.end(), min, max);
}
long long int string_to_int_limited_converter::ConvertToLLIFromSIt_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max)
{
    long long int pruef = ConvertToLLIFromSIt(begin, end);
    if(pruef < min) {
        pruef = min;
        limitfehler++;
    }
    else if(pruef > max) {
        pruef = max;
        limitfehler++;
    }
    return pruef;
}
long long int string_to_int_limited_converter::ConvertToLLIFromSIt_Hex_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max)
{
    long long int pruef = ConvertToLLIFromSIt_Hex(begin, end);
    if(pruef < min) {
        pruef = min;
        limitfehler++;
    }
    else if(pruef > max) {
        pruef = max;
        limitfehler++;
    }
    return pruef;
}
long long int string_to_int_limited_converter::ConvertToLLIFromSIt_HexOk_Limited(std::string::iterator begin, std::string::iterator end, long long int min, long long int max)
{
    long long int pruef = ConvertToLLIFromSIt_HexOk(begin, end);
    if(pruef < min) {
        pruef = min;
        limitfehler++;
    }
    else if(pruef > max) {
        pruef = max;
        limitfehler++;
    }
    return pruef;
}
double string_to_int_limited_converter::ConvertToDouble_Limited(std::string rohstring, double min, double max, char punct_char, char tausender_zeichen)
{
    double pruef = ConvertToDouble(rohstring, punct_char, tausender_zeichen);
    if(pruef < min) {
        pruef = min;
        limitfehler++;
    }
    else if(pruef > max) {
        pruef = max;
        limitfehler++;
    }
    return pruef;
}
double string_to_int_limited_converter::ConvertToDoubleR_Limited(std::string rohstring, double min, double max, char punct_char, char tausender_zeichen)
{
    double pruef = ConvertToDoubleR(rohstring, punct_char, tausender_zeichen);
    if(pruef < min) {
        pruef = min;
        limitfehler++;
    }
    else if(pruef > max) {
        pruef = max;
        limitfehler++;
    }
    return pruef;
}

bool string_to_int_limited_converter::LimitMistakeHappened()
{
    return (limitfehler != 0);
}

//-----------------------------------------------------------------------------------------------------------------------

bool GetFileString(std::string dateiname, std::string* value, bool binary)
{
    std::ifstream datei_istream;
    if(binary) {
        datei_istream.open(dateiname.c_str(), std::ios_base::in | std::ios_base::binary);
    }
    else {
        datei_istream.open(dateiname.c_str(), std::ios_base::in);
    }
    if(datei_istream) {
        datei_istream.unsetf(std::ios::skipws); // No white space skipping!
        value->clear();
        std::copy(
            std::istream_iterator<char>(datei_istream),
            std::istream_iterator<char>(), // allgemeiner End-Iterator
            std::back_inserter(*value)); // dieser Iterator ist keine Referenz. Stattdessen ruft er bei einer
                            // Zuweisung (*value).push_back(.) auf und macht nichts, wenn it++ aufgerufen wird.
        return true;
    }
    else {
        return false;
    }
}
bool SetFileString(std::string dateiname, const std::string &value)
{
	std::ofstream datei_ostream;
    datei_ostream.open(dateiname.c_str(), std::ios_base::out | std::ios_base::binary);
    if(datei_ostream) {
		datei_ostream.write(value.c_str(), value.length());
        return true;
    }
    else {
        return false;
    }
}

bool str_begins_with(const std::string &str, const std::string &begin)
{
	if(str.size() >= begin.size()) {
		for(unsigned int i1 = 0; i1 < begin.size(); i1++) {
			if(str[i1] != begin[i1]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
bool str_ends_with(const std::string &str, const std::string &end)
{
	if(str.size() >= end.size()) {
		unsigned int offset = str.size() - end.size();
		for(unsigned int i1 = 0; i1 < end.size(); i1++) {
			if(str[i1+offset] != end[i1]) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
void str_replace_char(std::string &str, char suchen, std::string ersetzen)
{
	for(unsigned int izeichen = 0; izeichen < str.length(); ) {
		if(str[izeichen] == suchen) {
			str.replace(izeichen, 1, ersetzen);
			izeichen += ersetzen.length();
		}
		else {
			izeichen++;
		}
	}
}
std::vector<std::string> str_split(const std::string &str, const std::string &split)
{
	std::vector<std::string> splits;
	
	if(split.empty()) {
		std::cout << "Warnung: str_split: split = \"\"." << std::endl;
		splits.push_back(split);
		return splits;
	}
	
	std::size_t splitstr_begin = 0;
	do {
		std::size_t splitstr_end = str.find(split, splitstr_begin);
		if(splitstr_end == std::string::npos) {
			splits.push_back(str.substr(splitstr_begin, str.size() - splitstr_begin)); // Den Rest anhängen
			return splits;
		}
		else {
			splits.push_back(str.substr(splitstr_begin, splitstr_end - splitstr_begin)); // substr(pos, laenge)
			splitstr_begin = splitstr_end + split.size();
		}
	} while(splitstr_begin != str.size());
	return splits;
}
#define FN_STR_ISWHITE(zeichen) ((zeichen == ' ') || (zeichen == '\t') || (zeichen == '\n') || (zeichen == '\r'))
std::string str_remove_sidewhites(const std::string &text)
{
	if(text.empty()) {
		return text;
	}
	unsigned int begin, last;
	for(begin = 0; begin < text.length(); begin++) {
		if(!FN_STR_ISWHITE(text[begin])) {
			break;
		}
	}
	for(last = text.length() - 1; last > 0; last--) {
		if(!FN_STR_ISWHITE(text[last])) {
			break;
		}
	}
	if((last == 0) && FN_STR_ISWHITE(text[0])) {
		return std::string();
	}
	return text.substr(begin, last - begin + 1);
}
std::string str_to_upper(std::string str)
{
	bool b_umlaut = false;
	for(std::string::iterator it_zeichen = str.begin(); it_zeichen != str.end(); it_zeichen++) {
		if((*it_zeichen >= 'a') && (*it_zeichen <= 'z')) {
			*it_zeichen = *it_zeichen - 'a' + 'A';
		}
		else if(*it_zeichen == '\xc3') {
			b_umlaut = true;
		}
		else if(b_umlaut) {
			if(*it_zeichen == '\xa4') { // ä
				*it_zeichen = '\x84';
			}
			else if(*it_zeichen == '\xb6') { // ö
				*it_zeichen = '\x96';
			}
			else if(*it_zeichen == '\xbc') { // ü
				*it_zeichen = '\x9c';
			}
			b_umlaut = false;
		}
	}
	return str;
}
bool str_replace_one(std::string &str, const std::string &search, const std::string &replace)
{
    std::size_t pos = str.find(search);
    if(pos == std::string::npos) {
        return false;
    }
    else {
        str.replace(pos, search.length(), replace);
        return true;
    }
}

std::string vectorstring_unsplit(const std::vector<std::string> &array, char zeichen)
{
	if(array.empty()) {
		return std::string();
	}
	std::string ret = array[0];
	for(unsigned int i1 = 1; i1 < array.size(); i1++) {
		ret.append(1, zeichen);
		ret.append(array[i1]);
	}
	return ret;
}

//-----------------------------------------------------------------------------------------------------------------------

void Char_To_UTF8String(uint32_t zeichen, std::string* str_ret)
{
    // siehe de.wikipedia.org/wiki/UTF-8
    if(zeichen >= 2097152) { // invalid characters
        str_ret->append("err");
    }
    else if(zeichen >= 65536) {
        str_ret->append(1, 0xf0 | ((zeichen >> 18) & 0x07)); // 0xf0 = 1111 0000; 0x07 = 0000 0111
        str_ret->append(1, 0x80 | ((zeichen >> 12) & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
        str_ret->append(1, 0x80 | ((zeichen >> 6) & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
        str_ret->append(1, 0x80 | (zeichen & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
    }
    else if(zeichen >= 2048) {
        str_ret->append(1, 0xe0 | ((zeichen >> 12) & 0x0F)); // 0xe0 = 1110 0000; 0x0F = 0000 1111
        str_ret->append(1, 0x80 | ((zeichen >> 6) & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
        str_ret->append(1, 0x80 | (zeichen & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
    }
    else if(zeichen >= 128) {
        str_ret->append(1, 0xc0 | ((zeichen >> 6) & 0x1F)); // 0xc0 = 1100 0000; 0x1F = 0001 1111
        str_ret->append(1, 0x80 | (zeichen & 0x3F)); // 0x80 = 1000 0000; 0x3F = 0011 1111
        //std::cout << int32_t(str_ret[0]) << " " << int32_t(str_ret[1]) << std::endl;
    }
    else {
        str_ret->append(1, zeichen);
    }
}
std::string Char_To_UTF8String(uint32_t zeichen)
{
    std::string ret;
    Char_To_UTF8String(zeichen, &ret);
    return ret;
}

//-----------------------------------------------------------------------------------------------------------------------

uint64_t GetTime_Microseconds()
{
    // clock_gettime kommt von pthread -> bei Windows nicht benutzbar
   #ifdef _WIN32
    //return clock() * 1000;
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
   #else
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((uint64_t)(ts.tv_sec))*1000000 + ((uint64_t)(ts.tv_nsec/1000));
   #endif
}
long long int GetTime_Milliseconds()
{
    // clock_gettime kommt von pthread -> bei Windows nicht benutzbar
   #ifdef _WIN32
    //return clock() * 1000;
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
   #else
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((long long int)(ts.tv_sec))*1000 + ((long long int)(ts.tv_nsec/1000000));
   #endif
}
double GetTimeAsDouble()
{
	return double(GetTime_Microseconds()) / 1000000;
}
double GetClockAsDouble()
{
	static uint64_t start_zeit = GetTime_Microseconds();
	return double(GetTime_Microseconds() - start_zeit) / 1000000;
}
int32_t GetClock_Milliseconds_I32()
{
	static long long int start_zeit = GetTime_Milliseconds();
	return int32_t(GetTime_Milliseconds() - start_zeit);
}

} // namespace utils
