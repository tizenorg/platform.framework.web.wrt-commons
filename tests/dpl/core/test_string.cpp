/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        test_string.cpp
 * @author      Piotr Marcinkiewicz (p.marcinkiew@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of string tests
 */
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <dpl/test/test_runner.h>
#include <dpl/string.h>
#include <dpl/sstream.h>

RUNNER_TEST_GROUP_INIT(DPL)

unsigned char GetBaseCode(int index);
unsigned char GetBaseCode(int index)
{
    /* aaaack but it's fast and const should make it shared text page. */
    static const unsigned char pr2six[256] = {
        /* ASCII table */
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
        64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
        64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };
    return pr2six[index];
}

/* Function adapted from APR library (http://apr.apache.org/) */
int wbxml_base64_decode(const char *buffer, char **result);
int wbxml_base64_decode(const char *buffer, char **result)
{
    int nbytesdecoded = 0, nprbytes = 0;
    const char *bufin = NULL;
    char *bufout = NULL;

    if ((buffer == NULL) || (result == NULL)) {
        return 0;
    }

    /* Initialize output buffer */
    *result = NULL;

    bufin = buffer;
    while (GetBaseCode(*(bufin++)) <= 63) {}

    nprbytes = (bufin - buffer) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    /* Malloc result buffer */
    if ((*result = (char*) malloc(nbytesdecoded + 1)) == NULL) {
        return 0;
    }
    memset(*result, 0, nbytesdecoded + 1);

    bufout = *result;
    bufin = buffer;

    while (nprbytes > 4) {
        *(bufout++) =
            (char)(GetBaseCode(*bufin) << 2 | GetBaseCode(bufin[1]) >> 4);
        *(bufout++) =
            (char)(GetBaseCode(bufin[1]) << 4 | GetBaseCode(bufin[2]) >> 2);
        *(bufout++) = (char)(GetBaseCode(bufin[2]) << 6 | GetBaseCode(bufin[3]));
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        *(bufout++) =
            (char)(GetBaseCode(*bufin) << 2 | GetBaseCode(bufin[1]) >> 4);
    }
    if (nprbytes > 2) {
        *(bufout++) =
            (char)(GetBaseCode(bufin[1]) << 4 | GetBaseCode(bufin[2]) >> 2);
    }
    if (nprbytes > 3) {
        *(bufout++) = (char)(GetBaseCode(bufin[2]) << 6 | GetBaseCode(bufin[3]));
    }

    nbytesdecoded -= (4 - nprbytes) & 3;

    return nbytesdecoded;
}

//#define TEST_CONVERSION(in_string, out_string, buffer_type, function

const char utf32Encoded[] =
    "RDAAAI0wAABvMAAAazAAAHswAAB4MAAAaDAAAAAwAABhMAAAijAAAGwwAACLMAAAkjAAAAAwAACP\
MAAASzAAAIgwAABfMAAAjDAAAF0wAAAAMAAAZDAAAG0wAABqMAAAiTAAAIAwAAAAMAAARjAAAJAw\
AABuMAAASjAAAE8wAACEMAAAfjAAAAAwAABRMAAAdTAAAFMwAABIMAAAZjAAAAAwAABCMAAAVTAA\
AE0wAACGMAAAgTAAAH8wAABXMAAAADAAAJEwAAByMAAAgjAAAFswAABZMAAACgAAANsFAADaBQAA\
IAAAANQFAADqBQAA6AUAAOEFAADnBQAAIAAAAOAFAADkBQAA5QUAACAAAADiBQAA3AUAACAAAADS\
BQAA1QUAANYFAADcBQAAIAAAAOcFAADYBQAA3wUAACwAAAAgAAAA6QUAANMFAADXBQAA4wUAACAA\
AADQBQAA6gUAACAAAADmBQAA0QUAANkFAAAgAAAA3AUAAN4FAADZBQAA3QUAAAoAAACk0AAApMIA\
AFjHAAAgAAAA4KwAACDHAABwyAAAdKwAAEDHAAAgAAAAhccAACDCAAB8sAAArLkAACAAAADMuQAA\
mLAAAHzFAAAgAAAAWNUAAOCsAAAgAAAAudIAAMS8AABc1QAAIAAAADCuAAAgwgAAQMcAACAAAABE\
1QAAlMYAAFjOAAAgAAAASsUAAOSyAAAKAAAAUAAAAGMAAABoAAAAbgAAAAUBAAAHAQAAIAAAAHcA\
AAAgAAAAdAAAABkBAAAgAAAAQgEAAPMAAABkAAAAegEAACAAAABqAAAAZQAAAHwBAABhAAAAIAAA\
AGwAAAB1AAAAYgAAACAAAABvAAAAWwEAAG0AAAAgAAAAcwAAAGsAAAByAAAAegAAAHkAAABEAQAA\
IAAAAGYAAABpAAAAZwAAAC4AAAAKAAAAQgAAAGwAAABvAAAAdwAAAHoAAAB5AAAAIAAAAG4AAABp\
AAAAZwAAAGgAAAB0AAAALQAAAGYAAAByAAAAdQAAAG0AAABwAAAAcwAAACAAAAB2AAAAZQAAAHgA\
AAAnAAAAZAAAACAAAABKAAAAYQAAAGMAAABrAAAAIAAAAFEAAAAuAAAACgAAAEYGAAA1BgAAIAAA\
AC0GAABDBgAASgYAAEUGAAAgAAAARAYAAEcGAAAgAAAAMwYAADEGAAAgAAAAQgYAACcGAAA3BgAA\
OQYAACAAAABIBgAAMAYAAEgGAAAgAAAANAYAACMGAABGBgAAIAAAADkGAAA4BgAASgYAAEUGAAAg\
AAAARQYAAEMGAAAqBgAASAYAACgGAAAgAAAAOQYAAEQGAABJBgAAIAAAACsGAABIBgAAKAYAACAA\
AAAjBgAALgYAADYGAAAxBgAAIAAAAEgGAABFBgAAOgYAAEQGAABBBgAAIAAAACgGAAAsBgAARAYA\
AC8GAAAgAAAAIwYAADIGAAAxBgAAQgYAACAAAAAKAAAAEgQAACAAAABHBAAAMAQAAEkEAAAwBAAA\
RQQAACAAAABOBAAAMwQAADAEAAAgAAAANgQAADgEAAA7BAAAIAAAADEEAABLBAAAIAAAAEYEAAA4\
BAAAQgQAAEAEAABDBAAAQQQAAD8AAAAgAAAAFAQAADAEAAAsAAAAIAAAAD0EAAA+BAAAIAAAAEQE\
AAAwBAAAOwQAAEwEAABIBAAAOAQAADIEAABLBAAAOQQAACAAAABNBAAAOgQAADcEAAA1BAAAPAQA\
AD8EAAA7BAAATwQAAEAEAAAhAAAACgAAAKQDAACsAwAAxwMAALkDAADDAwAAxAMAALcDAAAgAAAA\
sQMAALsDAADOAwAAwAMAALcDAAC+AwAAIAAAALIDAACxAwAAxgMAAK4DAADCAwAAIAAAAMgDAAC3\
AwAAvAMAAK0DAAC9AwAAtwMAACAAAACzAwAAtwMAACwAAAAgAAAAtAMAAMEDAACxAwAAwwMAALoD\
AAC1AwAAuwMAAK8DAAC2AwAAtQMAALkDAAAgAAAAxQMAAMADAACtAwAAwQMAACAAAAC9AwAAyQMA\
ALgDAADBAwAAvwMAAM0DAAAgAAAAugMAAMUDAAC9AwAAzAMAAMIDAAAKAAAAVgAAAGkAAABjAAAA\
dAAAAG8AAAByAAAAIAAAAGoAAABhAAAAZwAAAHQAAAAgAAAAegAAAHcAAAD2AAAAbAAAAGYAAAAg\
AAAAQgAAAG8AAAB4AAAAawAAAOQAAABtAAAAcAAAAGYAAABlAAAAcgAAACAAAABxAAAAdQAAAGUA\
AAByAAAAIAAAAPwAAABiAAAAZQAAAHIAAAAgAAAAZAAAAGUAAABuAAAAIAAAAGcAAAByAAAAbwAA\
AN8AAABlAAAAbgAAACAAAABTAAAAeQAAAGwAAAB0AAAAZQAAAHIAAAAgAAAARAAAAGUAAABpAAAA\
YwAAAGgAAAAKAAAAlokAAM6RAAAhcQAAUJYAAONeAAAM/wAAl3oAABZZAAAJZwAAzYUAAClZAAAK\
AAAACgAAAAAAAAA=";

const char utf8Encoded[] =
    "44GE44KN44Gv44Gr44G744G444Go44CA44Gh44KK44Gs44KL44KS44CA44KP44GL44KI44Gf44KM\
44Gd44CA44Gk44Gt44Gq44KJ44KA44CA44GG44KQ44Gu44GK44GP44KE44G+44CA44GR44G144GT\
44GI44Gm44CA44GC44GV44GN44KG44KB44G/44GX44CA44KR44Gy44KC44Gb44GZCteb15og15TX\
qteo16HXpyDXoNek16Ug16LXnCDXkteV15bXnCDXp9eY158sINep15PXl9ejINeQ16og16bXkdeZ\
INec157XmdedCu2CpOyKpOydmCDqs6DsnKDsobDqsbTsnYAg7J6F7Iig64G866asIOunjOuCmOyV\
vCDtlZjqs6Ag7Yq567OE7ZWcIOq4sOyIoOydgCDtlYTsmpTsuZgg7JWK64ukClBjaG7EhcSHIHcg\
dMSZIMWCw7NkxbogamXFvGEgbHViIG/Fm20gc2tyennFhCBmaWcuCkJsb3d6eSBuaWdodC1mcnVt\
cHMgdmV4J2QgSmFjayBRLgrZhti1INit2YPZitmFINmE2Ycg2LPYsSDZgtin2LfYuSDZiNiw2Ygg\
2LTYo9mGINi52LjZitmFINmF2YPYqtmI2Kgg2LnZhNmJINir2YjYqCDYo9iu2LbYsSDZiNmF2LrZ\
hNmBINio2KzZhNivINij2LLYsdmCIArQkiDRh9Cw0YnQsNGFINGO0LPQsCDQttC40Lsg0LHRiyDR\
htC40YLRgNGD0YE/INCU0LAsINC90L4g0YTQsNC70YzRiNC40LLRi9C5INGN0LrQt9C10LzQv9C7\
0Y/RgCEKzqTOrM+HzrnPg8+EzrcgzrHOu8+Oz4DOt86+IM6yzrHPhs6uz4Igz4jOt868zq3Ovc63\
IM6zzrcsIM60z4HOsc+DzrrOtc67zq/Ots61zrkgz4XPgM6tz4Egzr3Pic64z4HOv8+NIM66z4XO\
vc+Mz4IKVmljdG9yIGphZ3QgenfDtmxmIEJveGvDpG1wZmVyIHF1ZXIgw7xiZXIgZGVuIGdyb8Of\
ZW4gU3lsdGVyIERlaWNoCuimlumHjueEoemZkOW7o++8jOeql+WkluacieiXjeWkqQoKAA==";

const char asciiEncodedIso1[] =
    "ISIjJCUmJygpKissLS4vMDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZ\
WltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fgA=";

const char asciiEncodedUtf32[] =
    "IQAAACIAAAAjAAAAJAAAACUAAAAmAAAAJwAAACgAAAApAAAAKgAAACsAAAAsAAAALQAAAC4AAAAv\
AAAAMAAAADEAAAAyAAAAMwAAADQAAAA1AAAANgAAADcAAAA4AAAAOQAAADoAAAA7AAAAPAAAAD0A\
AAA+AAAAPwAAAEAAAABBAAAAQgAAAEMAAABEAAAARQAAAEYAAABHAAAASAAAAEkAAABKAAAASwAA\
AEwAAABNAAAATgAAAE8AAABQAAAAUQAAAFIAAABTAAAAVAAAAFUAAABWAAAAVwAAAFgAAABZAAAA\
WgAAAFsAAABcAAAAXQAAAF4AAABfAAAAYAAAAGEAAABiAAAAYwAAAGQAAABlAAAAZgAAAGcAAABo\
AAAAaQAAAGoAAABrAAAAbAAAAG0AAABuAAAAbwAAAHAAAABxAAAAcgAAAHMAAAB0AAAAdQAAAHYA\
AAB3AAAAeAAAAHkAAAB6AAAAewAAAHwAAAB9AAAAfgAAAAAAAAA=";

/*
Name: String_ConverterFromASCII
Description: tests construction of string from ascii data
Expected: data stored in buffer should match expected
*/
RUNNER_TEST(String_ConverterFromASCII)
{
    char* inStr = NULL;
    int inSize = wbxml_base64_decode(asciiEncodedIso1, &inStr);
    RUNNER_ASSERT(inSize > 0);
    RUNNER_ASSERT(NULL != inStr);
    inStr[inSize] = '\0';
    {
        DPL::String asciiString = DPL::FromASCIIString(inStr);

        std::string result = DPL::ToUTF8String(asciiString);

        RUNNER_ASSERT(strlen(inStr) == result.size());

        RUNNER_ASSERT(0 == memcmp(inStr, result.c_str(), result.size()));
    }

    free(inStr);
}

/*
Name: String_ConverterFromUTF8
Description: tests construction of string from UTF-8 data
Expected: data stored in buffer should match expected
*/
RUNNER_TEST(String_ConverterFromUTF8)
{
    char* inStr = NULL;
    int inSize = wbxml_base64_decode(asciiEncodedIso1, &inStr);
    RUNNER_ASSERT(inSize > 0);
    RUNNER_ASSERT(NULL != inStr);
    {
        DPL::String asciiString = DPL::FromUTF8String(inStr);

        std::string result = DPL::ToUTF8String(asciiString);

        RUNNER_ASSERT(strlen(inStr) == result.size());

        RUNNER_ASSERT(0 == memcmp(inStr, result.c_str(), result.size()));
    }

    free(inStr);
}

/*
Name: String_ConverterFromUTF32
Description: tests construction of string from UTF-32 data
Expected: data stored in buffer should match expected
*/
RUNNER_TEST(String_ConverterFromUTF32)
{
    wchar_t* inStr = NULL;
    int inSize =
        wbxml_base64_decode(utf32Encoded, reinterpret_cast<char**>(&inStr));
    RUNNER_ASSERT(inSize > 0);
    RUNNER_ASSERT(NULL != inStr);
    char* outStr = NULL;
    int outSize = wbxml_base64_decode(utf8Encoded, &outStr);
    RUNNER_ASSERT(outSize > 0);
    RUNNER_ASSERT(NULL != outStr);
    outStr[outSize] = '\0';
    {
        DPL::String utfString = DPL::FromUTF32String(inStr);
        std::string result = DPL::ToUTF8String(utfString);

        RUNNER_ASSERT(strlen(outStr) == result.size());
        RUNNER_ASSERT(0 == memcmp(outStr, result.c_str(), result.size()));

        RUNNER_ASSERT(inSize / sizeof(wchar_t) - 1 == utfString.size());
        RUNNER_ASSERT(0 ==
                      memcmp(inStr, &(utfString[0]), utfString.size() *
                             sizeof(wchar_t)));
    }

    free(inStr);
}

template<typename DelimiterType>
void String_TokenizeReal(const DelimiterType& delimiter)
{
    DPL::String str(L".##..abc.#.");
    std::vector<DPL::String> tokens;
    DPL::Tokenize(str, delimiter, std::back_inserter(tokens));

    std::vector<DPL::String> expectedTokens;
    for (int i = 0; i < 5; i++) {
        expectedTokens.push_back(L"");
    }
    expectedTokens.push_back(L"abc");
    for (int i = 0; i < 3; i++) {
        expectedTokens.push_back(L"");
    }

    RUNNER_ASSERT(expectedTokens == tokens);
    tokens.clear();
    expectedTokens.clear();

    DPL::Tokenize(str, delimiter, std::back_inserter(tokens), true);
    expectedTokens.push_back(L"abc");
    RUNNER_ASSERT(expectedTokens == tokens);
}

/*
Name: String_Tokenize
Description: tests of string splitting
Expected: returned substring should match expected values
*/
RUNNER_TEST(String_Tokenize)
{
    String_TokenizeReal(L"#.");
    String_TokenizeReal(L".#");
    String_TokenizeReal(L".....####.###..");
    String_TokenizeReal(DPL::String(L".#"));

    std::vector<std::string> tokens;
    DPL::Tokenize(std::string("abc.def"), '.', std::back_inserter(tokens));
    std::vector<std::string> expectedTokens;
    expectedTokens.push_back("abc");
    expectedTokens.push_back("def");

    RUNNER_ASSERT(tokens == expectedTokens);
}

template <typename TemplateArgumentCharTraits>
void TestInStreams(
    std::basic_string<typename TemplateArgumentCharTraits::char_type,
                      TemplateArgumentCharTraits> argumentInString,
    std::basic_string<typename TemplateArgumentCharTraits::char_type,
                      TemplateArgumentCharTraits> argumentResultString)
{
    typedef std::basic_string<typename TemplateArgumentCharTraits::char_type,
                              TemplateArgumentCharTraits>
    String;
    std::basic_istringstream<typename TemplateArgumentCharTraits::char_type,
                             TemplateArgumentCharTraits>
    istream(argumentInString);
    int intValue = 0;
    double doubleValue = 0.0;
    float floatValue = 0.0;
    String stringValue;

    istream >> intValue;
    RUNNER_ASSERT(!istream.fail());
    istream >> doubleValue;
    RUNNER_ASSERT(!istream.fail());
    istream >> floatValue;
    RUNNER_ASSERT(!istream.fail());
    istream >> stringValue;
    RUNNER_ASSERT(!istream.fail());

    RUNNER_ASSERT(1 == intValue);
    RUNNER_ASSERT(fabs(1.1f - doubleValue) < 0.00001);
    RUNNER_ASSERT(fabs(1.1f - floatValue) < 0.00001);
    RUNNER_ASSERT(argumentResultString == stringValue);
}

template <typename TemplateArgumentCharTraits>
void TestOutStreams(
    std::basic_string<typename TemplateArgumentCharTraits::char_type,
                      TemplateArgumentCharTraits> argumentInString,
    std::basic_string<typename TemplateArgumentCharTraits::char_type,
                      TemplateArgumentCharTraits> argumentResultString)
{
    typedef std::basic_string<typename TemplateArgumentCharTraits::char_type,
                              TemplateArgumentCharTraits>
    String;

    std::basic_ostringstream<typename TemplateArgumentCharTraits::char_type,
                             TemplateArgumentCharTraits>
    ostream;

    int intValue = 1;
    double doubleValue = 1.1;
    float floatValue = 1.1f;
    String stringValue = argumentInString;

    ostream << intValue;
    RUNNER_ASSERT(!ostream.fail());
    ostream << doubleValue;
    RUNNER_ASSERT(!ostream.fail());
    ostream << floatValue;
    RUNNER_ASSERT(!ostream.fail());
    ostream << stringValue;
    RUNNER_ASSERT(!ostream.fail());

    RUNNER_ASSERT(ostream.str() == argumentResultString);
}

/*
Name: String_Streams
Description: tests of input/output stream
Expected: returned substrign should match expected values
*/
RUNNER_TEST(String_Streams)
{
    TestInStreams<std::char_traits<char> >("1 1.1 1.1 test", "test");
    TestInStreams<std::char_traits<wchar_t> >(L"1 1.1 1.1 test", L"test");
    TestInStreams<DPL::CharTraits>(L"1 1.1 1.1 test", L"test");
    TestOutStreams<std::char_traits<char> >("test", "11.11.1test");
    TestOutStreams<std::char_traits<wchar_t> >(L"test", L"11.11.1test");
    TestOutStreams<DPL::CharTraits>(L"test", L"11.11.1test");
}

/*
Name: String_CompareCaseSensitive
Description: tests case sensitive comparision
Expected: strings should be equal
*/
RUNNER_TEST(String_CompareCaseSensitive)
{
    RUNNER_ASSERT(
        DPL::StringCompare(
            DPL::FromUTF32String(L"Ala Makota ma żołądkówkę"),
            DPL::FromUTF32String(L"Ala Makota ma żołądkówkę")) == 0);
}

/*
Name: String_CompareCaseInsensitive
Description: tests case insensitive comparision
Expected: strings should be equal
*/
RUNNER_TEST(String_CompareCaseInsensitive)
{
    RUNNER_ASSERT(
        DPL::StringCompare(
            DPL::FromUTF32String(L"Ala Makota ma żołądkówkę"),
            DPL::FromUTF32String(L"AlA MakOTA ma ŻoŁąDKÓwkę"),
            true) == 0);
}

