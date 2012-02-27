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
 * @file    TestSuite05.cpp
 * @author  unknown
 * @version 1.0
 * @brief   Test cases for Attribute class.
 */

#include <iostream>
#include <list>
#include <set>
#include <string>

#include <dpl/test/test_runner.h>

#include <dpl/ace/Attribute.h>
#include <dpl/ace-dao-ro/BaseAttribute.h>

#define TESTSUITE05(n) \
RUNNER_TEST(ts05_attr_tests_ ## n)

class AT : public Attribute {
public:
    AT(std::string nm)
      : Attribute(nm)
    {}

    static std::string* uriAuthorityStatic(const std::string *input) {
        AT at("Micky");
        return at.uriAuthority(input);
    }

    static std::string* uriHostStatic(const std::string *input) {
        AT at("Pluto");
        return at.uriHost(input);
    }

    static std::string* uriSchemeStatic(const std::string *input) {
        AT at("Donald");
        return at.uriScheme(input);
    }

    static std::string* uriSchemeAuthorityStatic(const std::string *input) {
        AT at("Winnie the Pooh");
        return at.uriSchemeAuthority(input);
    }

    static std::string* uriPathStatic(const std::string *input) {
        AT at("Hannibal");
        return at.uriPath(input);
    }

    static bool markTest(){
        bool result = true;
        for(int i =0; i<128; ++i){
            if( i == '-' || i == '_' || i == '.'|| i == '!'|| i == '~' || i == '*' || i == '\'' || i == ')' || i == '(' ){
                if (!mark[i]){
                    result = false;
                    break;
                }
            }
            else{
                if(mark[i]){
                    result =false;
                    break;
                }
            }
        }
        return result;
    }

    static bool digitTest(){
        bool result = true;
        for(int i =0; i<128; ++i){
            if( i > 47 && i < 58 ){
                if (!digit[i]){
                    result = false;
                    break;
                }
            }
            else{
                if(digit[i]){
                    result =false;
                    break;
                }
            }
        }
        return result;
    }

    static bool alphaTest(){
        bool result = true;
        for(int i =0; i<128; ++i){
            if( ( i>64 && i<91 ) || ( i>96 && i<123  ) ) {
                if (!alpha[i]){
                    result = false;
                    break;
                }
            }
            else{
                if(alpha[i]){
                    result =false;
                    break;
                }
            }
        }
        return result;
    }

    static bool isEscapedStatic(const char esc[3]) {
        AT at("Swinka");
        return at.isEscaped(esc);
    }
};

bool assertEqual(const std::string * actual, const char * intended) {
    if (actual == NULL || intended == NULL) {
        if (intended == NULL && actual == NULL) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string temp(intended);

    if (temp == *actual) {
        return true;
    }
    else {
        return false;
    }
}

bool assertTrue(bool condition){
    return condition;
}

bool assertFalse(bool condition){
    return !condition;
}

TESTSUITE05(01_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://www.wp.pl");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "www.wp.pl"));
    delete outcome;
}

TESTSUITE05(02_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://authority?path/asdf");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "authority"));
    delete outcome;
}

TESTSUITE05(03_uriAuthority){
    std::string * outcome = NULL;
    std::string query("abcd"); //This should be interpreted as schema
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(04_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://authority/asdf");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "authority"));
    delete outcome;
}

TESTSUITE05(05_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://user@host:20?ds");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "user@host:20"));
    delete outcome;
}

TESTSUITE05(06_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://hostname:23");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "hostname:23"));
    delete outcome;
}

TESTSUITE05(07_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://user@host:port");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "user@host:port"));
    delete outcome;
}

TESTSUITE05(08_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://1user@host:port"); //This is a VALID URI
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "1user@host:port"));
    delete outcome;
}

TESTSUITE05(09_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://abc%30"); //This is not a valid uri
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "abc%30"));
    delete outcome;
}

TESTSUITE05(10_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http:///asd"); //This is not a valid uri
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(11_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://?asd"); //This is not a valid uri
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(12_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://%34%56%67%ab");
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "%34%56%67%ab"));
    delete outcome;
}

TESTSUITE05(13_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://<>"); //This is not a valid uri
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(14_uriAuthority){
    std::string * outcome = NULL;
    std::string query("http://\\/"); //This is not a valid uri
    outcome = AT::uriAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(15_uriHost){
    std::string * outcome = NULL;
    std::string query("http://user@host:23");
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "host"));
    delete outcome;
}

TESTSUITE05(16_uriHost){
    std::string * outcome = NULL;
    std::string query("http://user@host:name"); //This is not a valid uri
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(17_uriHost){
    std::string * outcome = NULL;
    std::string query("http::::"); //This is a valid uri
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(18_uriHost){
    std::string * outcome = NULL;
    std::string query("..%%%."); //This is not a valid uri
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(19_uriHost){
    std::string * outcome = NULL;
    std::string query("ftp://abds.eu/fda");
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "abds.eu"));
    delete outcome;
}

TESTSUITE05(20_uriHost){
    std::string * outcome = NULL;
    std::string query("abs%14ccc");
    //This is a valid uri because it's interpreted as a path not a host
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(21_uriHost){
    std::string * outcome = NULL;
    std::string query("http://abc@123.2.23.213:982");
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "123.2.23.213"));
    delete outcome;
}

TESTSUITE05(22_uriHost){
    std::string * outcome = NULL;
    std::string query("http://abc@1233.2.23.213:982");
    //Hostname is invalid, but uri is valid
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(23_uriHost){
    std::string * outcome = NULL;
    std::string query("http://ab%23c@host"); //Valid escaped characters
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "host"));
    delete outcome;
}

TESTSUITE05(24_uriHost){
    std::string * outcome = NULL;
    std::string query("http://ab%23c@host%34"); //Invalid escaped characters in hostname
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(25_uriHost){
    std::string * outcome = NULL;
    std::string query("http://ab%GGc@host"); //Wrong character %
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(26_uriHost){
    std::string * outcome = NULL;
    std::string query("http://www.example.pl");
    outcome = AT::uriHostStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "www.example.pl"));
    delete outcome;
}

TESTSUITE05(27_uriScheme){
    std::string * outcome = NULL;
    std::string query("http://host");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "http"));
    delete outcome;
}

TESTSUITE05(28_uriScheme){
    std::string * outcome = NULL;
    //Wrong character '1' in scheme , it's not an URI because two slashes are not acceptable
    //in any other place than in separation between scheme and pat
    std::string query("1http://host");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(29_uriScheme){
    std::string * outcome = NULL;
    std::string query("ftp+a-fdf.ads://host");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "ftp+a-fdf.ads"));
    delete outcome;
}

TESTSUITE05(30_uriScheme){
    std::string * outcome = NULL;
    //Scheme cannot start with plus, it's not an URI because two slashes are not acceptable
    //in any other place than in separation between scheme and path
    std::string query("+++://host");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(31_uriScheme){
    std::string * outcome = NULL;
    std::string query("aaaac"); //It's a path not a scheme'a
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(32_uriScheme){
    std::string * outcome = NULL;
    //no escaped characters in schema, it's not an URI because two slashes are not acceptable
    //in any other place than in separation between scheme and path
    std::string query("ftpa%34fdfads://host");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(33_uriScheme){
    std::string * outcome = NULL;
    //no escaped characters in schema, it's not an URI because two slashes are not acceptable
    //in any other place than in separation between scheme and path
    std::string query("meaninglessstring://host%34");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "meaninglessstring"));
    delete outcome;
}

TESTSUITE05(34_uriScheme){
    std::string * outcome = NULL;
    std::string query("meaninglessstring2://");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "meaninglessstring2"));
    delete outcome;
}

TESTSUITE05(35_uriScheme){
    std::string * outcome = NULL;
    std::string query("http://www.samsung.com/ace/bondi#5");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "http"));
    delete outcome;
}

TESTSUITE05(36_uriScheme){
    std::string * outcome = NULL;
    std::string query("www.samsung.com");
    outcome = AT::uriSchemeStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ""));
    delete outcome;
}

TESTSUITE05(37_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("http://www.samsung.com");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "http://www.samsung.com"));
    delete outcome;
}

TESTSUITE05(38_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("ftp23://www.samsung.com/avc%23");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "ftp23://www.samsung.com"));
    delete outcome;
}

TESTSUITE05(39_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("ftp++://anonymous@hostname:12/avc%23");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "ftp++://anonymous@hostname:12"));
    delete outcome;
}

TESTSUITE05(40_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("32ftp://anonymous@hostname:12/avc%23");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(41_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("http://aaabbb?acd");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "http://aaabbb"));
    delete outcome;
}

TESTSUITE05(42_uriSchemeAuthority){
    std::string * outcome = NULL;
    std::string query("http://aaabbb/acd;sdf;sdf");
    outcome = AT::uriSchemeAuthorityStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "http://aaabbb"));
    delete outcome;
}

TESTSUITE05(43_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority//invalidpath");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, NULL));
    delete outcome;
}

TESTSUITE05(44_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/validpath");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "validpath"));
    delete outcome;
}

TESTSUITE05(45_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/validpath;param;param");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "validpath;param;param"));
    delete outcome;
}

TESTSUITE05(46_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/validpath;param;param?query");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "validpath;param;param"));
    delete outcome;
}

TESTSUITE05(47_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/validpath;?param;param?query");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "validpath;"));
    delete outcome;
}

TESTSUITE05(48_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/validpath;param?;param?query");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "validpath;param"));
    delete outcome;
}

TESTSUITE05(49_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/valid:path?query");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "valid:path"));
    delete outcome;
}

TESTSUITE05(50_uriPath){
    std::string * outcome = NULL;
    std::string query("ftp://authority/:::");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, ":::"));
    delete outcome;
}

TESTSUITE05(51_uriPath){
    std::string * outcome = NULL;
    std::string query("/path1/path2?abc#fragment");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "path1/path2"));
    delete outcome;
}

TESTSUITE05(52_uriPath){
    std::string * outcome = NULL;
    std::string query("http://www.samsung.com/normalpath/path2?query");
    outcome = AT::uriPathStatic(&query);
    RUNNER_ASSERT(assertEqual(outcome, "normalpath/path2"));
    delete outcome;
}

TESTSUITE05(53_markTest){
    RUNNER_ASSERT(AT::markTest());
}

TESTSUITE05(54_digitTest){
    RUNNER_ASSERT(AT::digitTest());
}

TESTSUITE05(55_alphaTest){
    RUNNER_ASSERT(AT::alphaTest());
}

TESTSUITE05(56_escapedTest){
    const char * query = "%23";
    RUNNER_ASSERT(assertTrue(AT::isEscapedStatic(query)));
    query = "%ab";
    RUNNER_ASSERT(assertTrue(AT::isEscapedStatic(query)));

    query = "%a";
    RUNNER_ASSERT(assertFalse(AT::isEscapedStatic(query)));

    query = "%rw";
    RUNNER_ASSERT(assertFalse(AT::isEscapedStatic(query)));

    query = NULL;
    RUNNER_ASSERT(assertFalse(AT::isEscapedStatic(query)));

    query = "abc";
    RUNNER_ASSERT(assertFalse(AT::isEscapedStatic(query)));

    query = "%bc";
    RUNNER_ASSERT(assertTrue(AT::isEscapedStatic(query)));

    query = "%DF";
    RUNNER_ASSERT(assertTrue(AT::isEscapedStatic(query)));
}

