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
#include <string>

#include <dpl/test/test_runner.h>
#include <dpl/event/event_delivery.h>
#include <dpl/wrt-dao-ro/global_config.h>
#include <vcore/ReferenceValidator.h>
#include <vcore/SignatureFinder.h>
#include <vcore/SignatureReader.h>
#include <vcore/SignatureValidator.h>
#include <vcore/OCSP.h>
#include <vcore/CachedOCSP.h>
#include "TestEnv.h"
#include <vcore/SSLContainers.h>
#include <vcore/Base64.h>
#include <vcore/CertificateLoader.h>
#include <vcore/CRL.h>
#include <vcore/CachedCRL.h>
#include <vcore/RevocationCheckerBase.h>
#include "TestCRL.h"
#include <vcore/CertificateCacheDAO.h>

namespace {

const std::string widget_path =
    "/opt/apps/widget/tests/vcore_widget_uncompressed/";
const std::string keys_path = "/opt/apps/widget/tests/vcore_keys/";
const std::string widget_store_path = "/opt/apps/widget/tests/vcore_widgets/";
const std::string cert_store_path = "/opt/apps/widget/tests/vcore_certs/";
const std::string crl_URI = "http://localhost/my.crl";

const std::string anka_ec_key_type = "urn:oid:1.2.840.10045.3.1.7";
const std::string anka_ec_public_key =
        "BGi9RmTUjpqCpQjx6SSiKdfmtjQBFNSN7ghm6TuaH9r4x73WddeLxLioH3VEmFLC+QLiR"\
        "kPxDxL/6YmQdgfGrqk=";

const std::string rsa_modulus =
        "ocwjKEFaPxLNcPTz2PtT2Gyu5jzkWaPo4thjZo3rXuNbD4TzjY02UGnTxvflNeORLpSS1"\
        "PeYr/1E/Nhr7qQAzj9g0DwW7p8zQEdOUi3v76VykeB0pFJH+0Fxp6LVBX9Z+EvZk+dbOy"\
        "GJ4Njm9B6M09axXlV11Anj9B/HYUDfDX8=";
const std::string rsa_exponent = "AQAB";

const std::string magda_dsa_p =
        "2BYIQj0ePUVxzrdBT41eCblraa9Dqag7QXFMCRM2PtyS22JPDKuV77tBc/jg0V3htHWdR"\
        "q9n6/kQDwrP7FIPoLATLIiC3oAYWj46Mr6d9k/tt/JZU6PvULmB2k1wrrmvKUi+U+I5Ro"\
        "qe8ui8lqR9pp9u2WCh2QmFfCohKNjN5qs=";
const std::string magda_dsa_q = "4p4JcDqz+S7CbWyd8txApZw0sik=";
const std::string magda_dsa_g =
        "AQrLND1ZGFvzwBpPPXplmPh1ijPx1O2gQEvPvyjR88guWcGqQc0m7dTb6PEvbI/oZ0o91"\
        "k7VEkfthURnNR1WtOLT8dmAuKQfwTQLPwCwUM/QiuWSlCyKLTE4Ev8aOG7ZqWudsKm/td"\
        "n9pUNGtcod1wo1ZtP7PfEJ6rYZGQDOlz8=";

const std::string googleCA =
"MIICPDCCAaUCEHC65B0Q2Sk0tjjKewPMur8wDQYJKoZIhvcNAQECBQAwXzELMAkG"
"A1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFz"
"cyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTk2"
"MDEyOTAwMDAwMFoXDTI4MDgwMTIzNTk1OVowXzELMAkGA1UEBhMCVVMxFzAVBgNV"
"BAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFzcyAzIFB1YmxpYyBQcmlt"
"YXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MIGfMA0GCSqGSIb3DQEBAQUAA4GN"
"ADCBiQKBgQDJXFme8huKARS0EN8EQNvjV69qRUCPhAwL0TPZ2RHP7gJYHyX3KqhE"
"BarsAx94f56TuZoAqiN91qyFomNFx3InzPRMxnVx0jnvT0Lwdd8KkMaOIG+YD/is"
"I19wKTakyYbnsZogy1Olhec9vn2a/iRFM9x2Fe0PonFkTGUugWhFpwIDAQABMA0G"
"CSqGSIb3DQEBAgUAA4GBALtMEivPLCYATxQT3ab7/AoRhIzzKBxnki98tsX63/Do"
"lbwdj2wsqFHMc9ikwFPwTtYmwHYBV4GSXiHx0bH/59AhWM1pF+NEHJwZRDmJXNyc"
"AA9WjQKZ7aKQRUzkuxCkPfAyAw7xzvjoyVGM5mKf5p/AfbdynMk2OmufTqj/ZA1k";

const std::string google2nd =
"MIIDIzCCAoygAwIBAgIEMAAAAjANBgkqhkiG9w0BAQUFADBfMQswCQYDVQQGEwJV"
"UzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xNzA1BgNVBAsTLkNsYXNzIDMgUHVi"
"bGljIFByaW1hcnkgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQwNTEzMDAw"
"MDAwWhcNMTQwNTEyMjM1OTU5WjBMMQswCQYDVQQGEwJaQTElMCMGA1UEChMcVGhh"
"d3RlIENvbnN1bHRpbmcgKFB0eSkgTHRkLjEWMBQGA1UEAxMNVGhhd3RlIFNHQyBD"
"QTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA1NNn0I0Vf67NMf59HZGhPwtx"
"PKzMyGT7Y/wySweUvW+Aui/hBJPAM/wJMyPpC3QrccQDxtLN4i/1CWPN/0ilAL/g"
"5/OIty0y3pg25gqtAHvEZEo7hHUD8nCSfQ5i9SGraTaEMXWQ+L/HbIgbBpV8yeWo"
"3nWhLHpo39XKHIdYYBkCAwEAAaOB/jCB+zASBgNVHRMBAf8ECDAGAQH/AgEAMAsG"
"A1UdDwQEAwIBBjARBglghkgBhvhCAQEEBAMCAQYwKAYDVR0RBCEwH6QdMBsxGTAX"
"BgNVBAMTEFByaXZhdGVMYWJlbDMtMTUwMQYDVR0fBCowKDAmoCSgIoYgaHR0cDov"
"L2NybC52ZXJpc2lnbi5jb20vcGNhMy5jcmwwMgYIKwYBBQUHAQEEJjAkMCIGCCsG"
"AQUFBzABhhZodHRwOi8vb2NzcC50aGF3dGUuY29tMDQGA1UdJQQtMCsGCCsGAQUF"
"BwMBBggrBgEFBQcDAgYJYIZIAYb4QgQBBgpghkgBhvhFAQgBMA0GCSqGSIb3DQEB"
"BQUAA4GBAFWsY+reod3SkF+fC852vhNRj5PZBSvIG3dLrWlQoe7e3P3bB+noOZTc"
"q3J5Lwa/q4FwxKjt6lM07e8eU9kGx1Yr0Vz00YqOtCuxN5BICEIlxT6Ky3/rbwTR"
"bcV0oveifHtgPHfNDs5IAn8BL7abN+AqKjbc1YXWrOU/VG+WHgWv";

const std::string google3rd =
"MIIDITCCAoqgAwIBAgIQL9+89q6RUm0PmqPfQDQ+mjANBgkqhkiG9w0BAQUFADBM"
"MQswCQYDVQQGEwJaQTElMCMGA1UEChMcVGhhd3RlIENvbnN1bHRpbmcgKFB0eSkg"
"THRkLjEWMBQGA1UEAxMNVGhhd3RlIFNHQyBDQTAeFw0wOTEyMTgwMDAwMDBaFw0x"
"MTEyMTgyMzU5NTlaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlh"
"MRYwFAYDVQQHFA1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKFApHb29nbGUgSW5jMRcw"
"FQYDVQQDFA53d3cuZ29vZ2xlLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkC"
"gYEA6PmGD5D6htffvXImttdEAoN4c9kCKO+IRTn7EOh8rqk41XXGOOsKFQebg+jN"
"gtXj9xVoRaELGYW84u+E593y17iYwqG7tcFR39SDAqc9BkJb4SLD3muFXxzW2k6L"
"05vuuWciKh0R73mkszeK9P4Y/bz5RiNQl/Os/CRGK1w7t0UCAwEAAaOB5zCB5DAM"
"BgNVHRMBAf8EAjAAMDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly9jcmwudGhhd3Rl"
"LmNvbS9UaGF3dGVTR0NDQS5jcmwwKAYDVR0lBCEwHwYIKwYBBQUHAwEGCCsGAQUF"
"BwMCBglghkgBhvhCBAEwcgYIKwYBBQUHAQEEZjBkMCIGCCsGAQUFBzABhhZodHRw"
"Oi8vb2NzcC50aGF3dGUuY29tMD4GCCsGAQUFBzAChjJodHRwOi8vd3d3LnRoYXd0"
"ZS5jb20vcmVwb3NpdG9yeS9UaGF3dGVfU0dDX0NBLmNydDANBgkqhkiG9w0BAQUF"
"AAOBgQCfQ89bxFApsb/isJr/aiEdLRLDLE5a+RLizrmCUi3nHX4adpaQedEkUjh5"
"u2ONgJd8IyAPkU0Wueru9G2Jysa9zCRo1kNbzipYvzwY4OA8Ys+WAi0oR1A04Se6"
"z5nRUP8pJcA2NhUzUnC+MY+f6H/nEQyNv4SgQhqAibAxWEEHXw==";

const std::string certVerisign =
"MIIG+DCCBeCgAwIBAgIQU9K++SSnJF6DygHkbKokdzANBgkqhkiG9w0BAQUFADCB"
"vjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL"
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTswOQYDVQQLEzJUZXJtcyBvZiB1c2Ug"
"YXQgaHR0cHM6Ly93d3cudmVyaXNpZ24uY29tL3JwYSAoYykwNjE4MDYGA1UEAxMv"
"VmVyaVNpZ24gQ2xhc3MgMyBFeHRlbmRlZCBWYWxpZGF0aW9uIFNTTCBTR0MgQ0Ew"
"HhcNMTAwNTI2MDAwMDAwWhcNMTIwNTI1MjM1OTU5WjCCASkxEzARBgsrBgEEAYI3"
"PAIBAxMCVVMxGTAXBgsrBgEEAYI3PAIBAhMIRGVsYXdhcmUxGzAZBgNVBA8TElYx"
"LjAsIENsYXVzZSA1LihiKTEQMA4GA1UEBRMHMjQ5Nzg4NjELMAkGA1UEBhMCVVMx"
"DjAMBgNVBBEUBTk0MDQzMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHFA1N"
"b3VudGFpbiBWaWV3MSIwIAYDVQQJFBk0ODcgRWFzdCBNaWRkbGVmaWVsZCBSb2Fk"
"MRcwFQYDVQQKFA5WZXJpU2lnbiwgSW5jLjEmMCQGA1UECxQdIFByb2R1Y3Rpb24g"
"U2VjdXJpdHkgU2VydmljZXMxGTAXBgNVBAMUEHd3dy52ZXJpc2lnbi5jb20wggEi"
"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCj+PvvK+fZOXwno0yT/OTy2Zm9"
"ehnZjTtO/X2IWBEa3jG30C52uHFQI4NmXiQVNvJHkBaAj0ilVjvGdxXmkyyFsugt"
"IWOTZ8pSKdX1tmGFIon6Ko9+lBFkVkudA1ogAUbtTB8IcdeOlpK78T4SjdVMhY18"
"150YzSw6hRKlw52wBaDxtGZElvOth41K7TUcaDnQVzz5SBPW5MUhi7AWrdoSk17O"
"BozOzmB/jkYDVDnwLcbR89SLHEOle/idSYSDQUmab3y0JS8RyQV1+DB70mnFALnD"
"fLiL47nMQQCGxXgp5voQ2YmSXhevKmEJ9vvtC6C7yv2W6yomfS/weUEce9pvAgMB"
"AAGjggKCMIICfjCBiwYDVR0RBIGDMIGAghB3d3cudmVyaXNpZ24uY29tggx2ZXJp"
"c2lnbi5jb22CEHd3dy52ZXJpc2lnbi5uZXSCDHZlcmlzaWduLm5ldIIRd3d3LnZl"
"cmlzaWduLm1vYmmCDXZlcmlzaWduLm1vYmmCD3d3dy52ZXJpc2lnbi5ldYILdmVy"
"aXNpZ24uZXUwCQYDVR0TBAIwADAdBgNVHQ4EFgQU8oBwK/WBXCZDWi0dbuDgPyTK"
"iJIwCwYDVR0PBAQDAgWgMD4GA1UdHwQ3MDUwM6AxoC+GLWh0dHA6Ly9FVkludGwt"
"Y3JsLnZlcmlzaWduLmNvbS9FVkludGwyMDA2LmNybDBEBgNVHSAEPTA7MDkGC2CG"
"SAGG+EUBBxcGMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3LnZlcmlzaWduLmNv"
"bS9ycGEwKAYDVR0lBCEwHwYIKwYBBQUHAwEGCCsGAQUFBwMCBglghkgBhvhCBAEw"
"HwYDVR0jBBgwFoAUTkPIHXbvN1N6T/JYb5TzOOLVvd8wdgYIKwYBBQUHAQEEajBo"
"MCsGCCsGAQUFBzABhh9odHRwOi8vRVZJbnRsLW9jc3AudmVyaXNpZ24uY29tMDkG"
"CCsGAQUFBzAChi1odHRwOi8vRVZJbnRsLWFpYS52ZXJpc2lnbi5jb20vRVZJbnRs"
"MjAwNi5jZXIwbgYIKwYBBQUHAQwEYjBgoV6gXDBaMFgwVhYJaW1hZ2UvZ2lmMCEw"
"HzAHBgUrDgMCGgQUS2u5KJYGDLvQUjibKaxLB4shBRgwJhYkaHR0cDovL2xvZ28u"
"dmVyaXNpZ24uY29tL3ZzbG9nbzEuZ2lmMA0GCSqGSIb3DQEBBQUAA4IBAQB9VZxB"
"wDMRGyhFWYkY5rwUVGuDJiGeas2xRJC0G4+riQ7IN7pz2a2BhktmZ5HbxXL4ZEY4"
"yMN68DEVErhtKiuL02ng27alhlngadKQzSL8pLdmQ+3jEwm9nva5C/7pbeqy+qGF"
"is4IWNYOc4HKNkABxXm5v0ouys8HPNkTLFLep0gLqRXW3gYN2XbKUWMs7z7hJpkY"
"GxP8YQSxi513O2dWVCXB8S6erIz9E/bcfdXoCPyQdn42y3IEoJvPvBS3S55fD4+Q"
"Q43GPhumSg9a6S3hnyw8DX5OiUGmqgQrtSeDRsNmWqtWizEQbe+fotZpEn/7zYTa"
"tk1ni/k5jDH/QeuG";

const std::string crlExampleCertificate =
"MIIFlDCCBHygAwIBAgIBADANBgkqhkiG9w0BAQUFADBDMRIwEAYKCZImiZPyLGQB"
"GRYCZXMxGDAWBgoJkiaJk/IsZAEZFghpcmlzZ3JpZDETMBEGA1UEAxMKSVJJU0dy"
"aWRDQTAeFw0wNTA2MjgwNTAyMjhaFw0xNTA2MjYwNTAyMjhaMEMxEjAQBgoJkiaJ"
"k/IsZAEZFgJlczEYMBYGCgmSJomT8ixkARkWCGlyaXNncmlkMRMwEQYDVQQDEwpJ"
"UklTR3JpZENBMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1CQiWlff"
"ajoMSTuismKqLQ+Mt33Tq4bBpCZvCBXhqan1R0ksILPtK1L7C8QWqPk6AZZpuNmY"
"cNVtJGc8ksgDWvX0EB3GKwZTZ8RrSRlSEe9Otq+Ur7S9uxM1JMmCr6zZTMFANzBS"
"4btnduV78C09IhFYG4OW8IPhNrbfPaeOR+PRPAa/qdSONAwTrM1sZkIvGpAkBWM6"
"Pn7TK9BAK6GLvwgii780fWj3Cwgmp8EDCTievBbWj+z8/apMEy9R0vyB2dWNNCnk"
"6q8VvrjgMsJt33O3BqOoBuZ8R/SS9OFWLFSU3s7cfrRaUSJk/Mx8OGFizRkcXSzX"
"0Nidcg7hX5i78wIDAQABo4ICkTCCAo0wDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E"
"FgQUnUJkLlupXvH/bMg8NtPxtkOYrRowawYDVR0jBGQwYoAUnUJkLlupXvH/bMg8"
"NtPxtkOYrRqhR6RFMEMxEjAQBgoJkiaJk/IsZAEZFgJlczEYMBYGCgmSJomT8ixk"
"ARkWCGlyaXNncmlkMRMwEQYDVQQDEwpJUklTR3JpZENBggEAMA4GA1UdDwEB/wQE"
"AwIBxjARBglghkgBhvhCAQEEBAMCAAcwOwYJYIZIAYb4QgENBC4WLElSSVNHcmlk"
"IENlcnRpZmljYXRpb24gQXV0aG9yaXR5IENlcnRpZmljYXRlMIGZBgNVHR8EgZEw"
"gY4wLqAsoCqGKGh0dHA6Ly93d3cuaXJpc2dyaWQuZXMvcGtpL2NybC9jYWNybC5w"
"ZW0wXKBaoFiGVmxkYXA6Ly9sZGFwLmlyaXNncmlkLmVzOjEzODAvY249SVJJU0dy"
"aWRDQSxkYz1pcmlzZ3JpZCxkYz1lcz9jZXJ0aWZpY2F0ZVJldm9jYXRpb25MaXN0"
"MDcGCWCGSAGG+EIBAwQqFihodHRwOi8vd3d3LmlyaXNncmlkLmVzL3BraS9jcmwv"
"Y2FjcmwucGVtME4GCWCGSAGG+EIBCARBFj9odHRwOi8vd3d3LmlyaXNncmlkLmVz"
"L3BraS9wb2xpY3kvMS4zLjYuMS40LjEuNzU0Ny4yLjIuNC4xLjEuMS8waQYDVR0g"
"BGIwYDBeBg0rBgEEAbp7AgIEAQEBME0wSwYIKwYBBQUHAgEWP2h0dHA6Ly93d3cu"
"aXJpc2dyaWQuZXMvcGtpL3BvbGljeS8xLjMuNi4xLjQuMS43NTQ3LjIuMi40LjEu"
"MS4xLzANBgkqhkiG9w0BAQUFAAOCAQEAaqRfyLER+P2QOZLLdz66m7FGsgtFsAEx"
"wiNrIChFWfyHVZG7Ph1fn/GDD5LMsrU23lx3NBN5/feHuut1XNYKNs8vtV07D70r"
"DKjUlPbmWV0B+/GDxe1FDGop/tKQfyHSUaBuauXChFU/2INu5lhBerNl7QxNJ1ws"
"cWGiT7R+L/2EjgzWgH1V/0zmIOMep6kY7MUs8rlyF0O5MNFs232cA1trl9kvhAGU"
"9p58Enf5DWMrh17SPH586yIJeiWZtPez9G54ftY+XIqfn0X0zso0dnoXNJQYS043"
"/5vSnoHdRx/EmN8yjeEavZtC48moN0iJ38eB44uKgCD77rZW5s1XqA==";

//class TestCleanup
//{
//  public:
//    explicit TestCleanup(bool bCheckForFakeVerification = false)
//    {
//        if (bCheckForFakeVerification) {
//            bool bUnsetEnvVar = true;
//
//            m_strEnvVar = "CHECK_ONLY_DOMAIN_INSTEAD_OF_VALIDATION";
//            if (getenv(m_strEnvVar.c_str()) != NULL) {
//                bUnsetEnvVar = false;
//            } else {
//                setenv(m_strEnvVar.c_str(), "1", 0);
//            }
//        }
//    }
//
//    ~TestCleanup()
//    {
//        if (!m_strRootCAPath.empty()) {
//            removeCertGivenByFilename(m_strRootCAPath.c_str());
//        }
//
//        if (!m_strEnvVar.empty()) {
//            unsetenv(m_strEnvVar.c_str());
//        }
//    }
//
//    void setRootCAPath(const std::string& strRootCAPath)
//    {
//        m_strRootCAPath = strRootCAPath;
//    }
//
//  private:
//    std::string           m_strRootCAPath;
//    std::string           m_strEnvVar;
//};
//
//class PolicyChanger : public DPL::Event::EventListener<AceUpdateResponseEvent>
//{
//  public:
//    PolicyChanger()
//    {
//        DPL::Event::EventDeliverySystem::AddListener<AceUpdateResponseEvent>(this);
//    }
//
//    ~PolicyChanger()
//    {
//        DPL::Event::EventDeliverySystem::RemoveListener<AceUpdateResponseEvent>(this);
//    }
//
//    void OnEventReceived(const AceUpdateResponseEvent& event)
//    {
//        if (0 != event.GetArg0()) {
//            LogError("Policy change failed");
//        }
//        Assert(0 == event.GetArg0() && "Policy change failed");
//        LoopControl::finish_wait_for_wrt_init();
//    }
//
//    void updatePolicy(const std::string& path)
//    {
//        AceUpdateRequestEvent event(path);
//        DPL::Event::EventDeliverySystem::Publish(event);
//        LoopControl::wait_for_wrt_init();
//    }
//};

} // namespace anonymous

using namespace ValidationCore;

//////////////////////////////////////////////////
////////  VALIDATION CORE TEST SUITE  ////////////
//////////////////////////////////////////////////

RUNNER_TEST(test01_signature_finder)
{
    SignatureFileInfoSet signatureSet;
    SignatureFinder signatureFinder(widget_path);
    RUNNER_ASSERT_MSG(
        SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
        "SignatureFinder failed");
    RUNNER_ASSERT_MSG(signatureSet.size() == 3,
                      "Some signature has not been found");

    SignatureFileInfo first = *(signatureSet.begin());
    RUNNER_ASSERT_MSG(
        std::string("author-signature.xml") == first.getFileName(),
        "Author Signature");
    RUNNER_ASSERT_MSG(-1 == first.getFileNumber(), "Wrong signature number.");
    first = *(signatureSet.rbegin());
    RUNNER_ASSERT_MSG(std::string("signature22.xml") == first.getFileName(),
                      "Wrong signature fileName.");
    RUNNER_ASSERT_MSG(22 == first.getFileNumber(), "Wrong signature number.");
}

RUNNER_TEST(test02_signature_reader)
{
    SignatureFileInfoSet signatureSet;
    SignatureFinder signatureFinder(widget_path);
    RUNNER_ASSERT_MSG(
        SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
        "SignatureFinder failed");

    SignatureFileInfoSet::reverse_iterator iter = signatureSet.rbegin();

    for (; iter != signatureSet.rend(); ++iter) {
        SignatureData data(widget_path + iter->getFileName(),
                           iter->getFileNumber());
        SignatureReader xml;
        xml.initialize(data, WrtDB::GlobalConfig::GetSignatureXmlSchema());
        xml.read(data);
    }
}

RUNNER_TEST(test03_signature_validator)
{
    SignatureFileInfoSet signatureSet;
    SignatureFinder signatureFinder(widget_path);
    RUNNER_ASSERT_MSG(
        SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
        "SignatureFinder failed");

    SignatureFileInfoSet::reverse_iterator iter = signatureSet.rbegin();

    for (; iter != signatureSet.rend(); ++iter) {
        SignatureData data(widget_path + iter->getFileName(),
                           iter->getFileNumber());
        SignatureReader xml;
        xml.initialize(data, WrtDB::GlobalConfig::GetSignatureXmlSchema());
        xml.read(data);

        SignatureValidator validator(
            false,
            false,
            false);

        if (data.isAuthorSignature()) {
            RUNNER_ASSERT_MSG(
                SignatureValidator::SIGNATURE_DISREGARD ==
                    validator.check(data, widget_path),
                "Validation failed");
        } else {
            RUNNER_ASSERT_MSG(
                SignatureValidator::SIGNATURE_VERIFIED ==
                    validator.check(data, widget_path),
                "Validation failed");
        }
    }
}

RUNNER_TEST(test05_signature_reference)
{
    SignatureFileInfoSet signatureSet;
    SignatureFinder signatureFinder(widget_path);
    RUNNER_ASSERT_MSG(
        SignatureFinder::NO_ERROR == signatureFinder.find(signatureSet),
        "SignatureFinder failed");

    SignatureFileInfoSet::reverse_iterator iter = signatureSet.rbegin();

    for (; iter != signatureSet.rend(); ++iter) {
        SignatureData data(widget_path + iter->getFileName(),
                           iter->getFileNumber());
        SignatureReader xml;
        xml.initialize(data, WrtDB::GlobalConfig::GetSignatureXmlSchema());
        xml.read(data);

        SignatureValidator sval(
            false,
            false,
            false);
        sval.check(data, widget_path);

        ReferenceValidator val(widget_path);
        RUNNER_ASSERT(
            ReferenceValidator::NO_ERROR == val.checkReferences(data));
    }
}

RUNNER_TEST(test07t01_base64)
{
    std::string strraw = "1234567890qwertyuiop[]asdfghjkl;'zxcvbnm,.";
    std::string strenc =
        "MTIzNDU2Nzg5MHF3ZXJ0eXVpb3BbXWFzZGZnaGprbDsnenhjdmJubSwu";

    Base64Encoder encoder;
    encoder.reset();
    encoder.append(strraw);
    encoder.finalize();
    RUNNER_ASSERT_MSG(strenc == encoder.get(), "Error in Base64Encoder.");

    Base64Decoder decoder;
    decoder.reset();
    decoder.append(strenc);
    RUNNER_ASSERT(decoder.finalize());
    RUNNER_ASSERT_MSG(strraw == decoder.get(), "Error in Base64Decoder.");
}

RUNNER_TEST(test07t02_base64)
{
    const size_t MAX = 40;
    char buffer[MAX];
    for (size_t i = 0; i<MAX; ++i) {
        buffer[i] = static_cast<char>(i);
    }

    std::string raw(&buffer[0], &buffer[MAX]);

    RUNNER_ASSERT(MAX == raw.size());

    Base64Encoder encoder;
    encoder.reset();
    encoder.append(raw);
    encoder.finalize();
    std::string enc = encoder.get();

    Base64Decoder decoder;
    decoder.reset();
    decoder.append(enc);
    RUNNER_ASSERT(decoder.finalize());
    RUNNER_ASSERT_MSG(raw == decoder.get(), "Error in Base64 conversion.");
}

RUNNER_TEST(test07t03_base64)
{
    std::string invalid = "1234)";

    Base64Decoder decoder;
    decoder.reset();
    decoder.append(invalid);
    RUNNER_ASSERT(false == decoder.finalize());
}

RUNNER_TEST(test07t04_base64)
{
    std::string invalid = "12234";

    Base64Decoder decoder;
    decoder.reset();

    bool exception = false;
    Try {
        std::string temp = decoder.get();
    } Catch(Base64Decoder::Exception::NotFinalized) {
        exception = true;
    }

    RUNNER_ASSERT_MSG(exception, "Base64Decoder does not throw error.");
}

RUNNER_TEST(test08t01_Certificate)
{
    Certificate cert(certVerisign, Certificate::FORM_BASE64);

    DPL::OptionalString result;

    result = cert.getCommonName(Certificate::FIELD_SUBJECT);
    RUNNER_ASSERT_MSG(!result.IsNull(), "No common name");
    RUNNER_ASSERT_MSG(*result == DPL::FromUTF8String("www.verisign.com"),
                      "CommonName mismatch");

    result = cert.getCommonName(Certificate::FIELD_ISSUER);
    RUNNER_ASSERT_MSG(!result.IsNull(), "No common name");
    RUNNER_ASSERT_MSG(result == DPL::FromUTF8String(
            "VeriSign Class 3 Extended Validation SSL SGC CA"),
            "CommonName mismatch");

    result = cert.getCountryName();
    RUNNER_ASSERT_MSG(!result.IsNull(), "No country");
    RUNNER_ASSERT_MSG(*result == DPL::FromUTF8String("US"),
                      "Country mismatch");
}

RUNNER_TEST(test08t02_Certificate)
{
    Certificate cert(certVerisign, Certificate::FORM_BASE64);

    Certificate::Fingerprint fin =
        cert.getFingerprint(Certificate::FINGERPRINT_SHA1);

    unsigned char buff[20] = {
        0xb9, 0x72, 0x1e, 0xd5, 0x49,
        0xed, 0xbf, 0x31, 0x84, 0xd8,
        0x27, 0x0c, 0xfe, 0x03, 0x11,
        0x19, 0xdf, 0xc2, 0x2b, 0x0a};
    RUNNER_ASSERT_MSG(fin.size() == 20, "Wrong size of fingerprint");

    for (size_t i = 0; i<20; ++i) {
        RUNNER_ASSERT_MSG(fin[i] == buff[i], "Fingerprint mismatch");
    }
}

RUNNER_TEST(test08t03_Certificate)
{
    Certificate cert(certVerisign, Certificate::FORM_BASE64);

    Certificate::AltNameSet nameSet = cert.getAlternativeNameDNS();

    RUNNER_ASSERT(nameSet.size() == 8);

    DPL::String str = DPL::FromUTF8String("verisign.com");
    RUNNER_ASSERT(nameSet.find(str) != nameSet.end());

    str = DPL::FromUTF8String("fake.com");
    RUNNER_ASSERT(nameSet.find(str) == nameSet.end());

}

RUNNER_TEST(test09t01_CertificateCollection)
{
    CertificateList list;
    list.push_back(CertificatePtr(
        new Certificate(google2nd, Certificate::FORM_BASE64)));
    list.push_back(CertificatePtr(
        new Certificate(googleCA, Certificate::FORM_BASE64)));
    list.push_back(CertificatePtr(
        new Certificate(google3rd, Certificate::FORM_BASE64)));

    CertificateCollection collection;
    collection.load(list);

    bool exception = false;

    Try {
        RUNNER_ASSERT(collection.isChain());
    } Catch (CertificateCollection::Exception::WrongUsage) {
        exception = true;
    }

    RUNNER_ASSERT_MSG(exception, "Exception expected!");

    RUNNER_ASSERT_MSG(collection.sort(), "Sort failed");

    RUNNER_ASSERT(collection.isChain());

    std::string encoded = collection.toBase64String();

    collection.clear();

    RUNNER_ASSERT_MSG(collection.size() == 0, "Function clear failed.");

    collection.load(encoded);

    RUNNER_ASSERT_MSG(collection.sort(), "Sort failed");

    list = collection.getChain();

    RUNNER_ASSERT(
        DPL::ToUTF8String(*(list.front().Get()->getCommonName())) ==
            "www.google.com");
    RUNNER_ASSERT(
        DPL::ToUTF8String(*(list.back().Get()->getOrganizationName())) ==
            "VeriSign, Inc.");
}

RUNNER_TEST(test51t01_ocsp_validation_negative)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pCert1;
    CertificatePtr pCert2;
    CertificatePtr pRootCert;
    std::string caRootPath(keys_path + "ocsp_rootca.crt"),
        certLevel0Path(keys_path + "ocsp_level0deprecated.crt"),
        certLevel1Path(keys_path + "ocsp_level1.crt"),
        certLevel2Path(keys_path + "ocsp_level2.crt");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_rootca.crt");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    if (!pCert0) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_level0.crt");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    pCert1 = RevocationCheckerBase::loadPEMFile(certLevel1Path.c_str());
    if (!pCert1) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_level1.crt");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert1));

    pCert2 = RevocationCheckerBase::loadPEMFile(certLevel2Path.c_str());
    if (!pCert2) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_level2.crt");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert2));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error from store exception");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test51t02_ocsp_validation_positive)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pCert1;
    CertificatePtr pCert2;
    CertificatePtr pRootCert;
    std::string caRootPath(keys_path + "ocsp_rootca.crt"),
        certLevel1Path(keys_path + "ocsp_level1.crt"),
        certLevel2Path(keys_path + "ocsp_level2.crt");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_rootca.crt");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert1 = RevocationCheckerBase::loadPEMFile(certLevel1Path.c_str());
    if (!pCert1) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_level1.crt");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert1));

    pCert2 = RevocationCheckerBase::loadPEMFile(certLevel2Path.c_str());
    if (!pCert2) {
        RUNNER_ASSERT_MSG(false, "Couldn't load ocsp_level2.crt");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert2));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error from store exception");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test51t04_ocsp_request)
{
    CertificateList lTrustedCerts;

    lTrustedCerts.push_back(CertificatePtr(
        new Certificate(google3rd, Certificate::FORM_BASE64)));
    lTrustedCerts.push_back(CertificatePtr(
        new Certificate(google2nd, Certificate::FORM_BASE64)));
    lTrustedCerts.push_back(CertificatePtr(
        new Certificate(googleCA, Certificate::FORM_BASE64)));

    CertificateCollection chain;
    chain.load(lTrustedCerts);
    chain.sort();

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(OCSP::SHA1);
    ocsp.setTrustedStore(lTrustedCerts);
    VerificationStatus result = ocsp.checkEndEntity(chain);

    RUNNER_ASSERT(VERIFICATION_STATUS_GOOD == result);
}

RUNNER_TEST(test51t05_cached_ocsp_validation_negative)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pCert1;
    CertificatePtr pCert2;
    CertificatePtr pRootCert;
    std::string caRootPath(keys_path + "ocsp_rootca.crt"),
        certLevel0Path(keys_path + "ocsp_level0deprecated.crt"),
        certLevel1Path(keys_path + "ocsp_level1.crt"),
        certLevel2Path(keys_path + "ocsp_level2.crt");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    RUNNER_ASSERT_MSG(pRootCert, "Couldn't load ocsp_rootca.crt");
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    RUNNER_ASSERT_MSG(pCert0, "Couldn't load ocsp_level0.crt");
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    pCert1 = RevocationCheckerBase::loadPEMFile(certLevel1Path.c_str());
    RUNNER_ASSERT_MSG(pCert1, "Couldn't load ocsp_level1.crt");
    lOCSPCertificates.push_back(CertificatePtr(pCert1));

    pCert2 = RevocationCheckerBase::loadPEMFile(certLevel2Path.c_str());
    RUNNER_ASSERT_MSG(pCert2, "Couldn't load ocsp_level2.crt");
    lOCSPCertificates.push_back(CertificatePtr(pCert2));

    CachedOCSP ocsp;

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());

    VerificationStatus status = ocsp.check(collection);

    RUNNER_ASSERT_MSG(status != VERIFICATION_STATUS_GOOD,
                      "Caught OCSP verification error exception");

    OCSPCachedStatusList respList;
    CertificateCacheDAO::getOCSPStatusList(&respList);
    unsigned len = respList.size();

    status = ocsp.check(collection);

    RUNNER_ASSERT_MSG(status != VERIFICATION_STATUS_GOOD,
                      "Caught OCSP verification error exception");

    respList.clear();
    CertificateCacheDAO::getOCSPStatusList(&respList);
    RUNNER_ASSERT_MSG(respList.size() == len && len > 0,
                      "Caught OCSP cache error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test51t06_cached_ocsp_validation_positive)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pCert1;
    CertificatePtr pCert2;
    CertificatePtr pRootCert;
    std::string caRootPath(keys_path + "ocsp_rootca.crt"),
        certLevel1Path(keys_path + "ocsp_level1.crt"),
        certLevel2Path(keys_path + "ocsp_level2.crt");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    RUNNER_ASSERT_MSG(pRootCert, "Couldn't load ocsp_rootca.crt");
    lOCSPCertificates.push_back(pRootCert);

    pCert1 = RevocationCheckerBase::loadPEMFile(certLevel1Path.c_str());
    RUNNER_ASSERT_MSG(pCert1, "Couldn't load ocsp_level1.crt");
    lOCSPCertificates.push_back(CertificatePtr(pCert1));

    pCert2 = RevocationCheckerBase::loadPEMFile(certLevel2Path.c_str());
    RUNNER_ASSERT_MSG(pCert2, "Couldn't load ocsp_level2.crt");
    lOCSPCertificates.push_back(CertificatePtr(pCert2));

    CachedOCSP ocsp;

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());

    VerificationStatus status = ocsp.check(collection);

    RUNNER_ASSERT_MSG(status == VERIFICATION_STATUS_GOOD,
                      "Caught OCSP verification error exception");

    OCSPCachedStatusList respList;
    CertificateCacheDAO::getOCSPStatusList(&respList);
    unsigned len = respList.size();

    status = ocsp.check(collection);

    RUNNER_ASSERT_MSG(status == VERIFICATION_STATUS_GOOD,
                      "Caught OCSP verification error exception");

    respList.clear();
    CertificateCacheDAO::getOCSPStatusList(&respList);
    RUNNER_ASSERT_MSG(respList.size() == len && len > 0,
                      "Caught OCSP cache error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test61_crl_test_revocation_no_crl)
{
    //Clear CRL cache so there is no CRL for those certificates URI.
    CertificateCacheDAO::clearCertificateCache();
    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "1second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "1third_level.pem"));

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    RUNNER_ASSERT_MSG(status.isCRLValid == false,
                      "Some certificate have no CRL extension!");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test62_crl_test_revocation_set1)
{
    CertificateCacheDAO::clearCertificateCache();

    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "1second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "1third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl1.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(status.isRevoked);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test63_crl_test_revocation_set1)
{
    CertificateCacheDAO::clearCertificateCache();

    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "1second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "1third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl1.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(status.isRevoked);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test64_crl_test_revocation_set2)
{
    CertificateCacheDAO::clearCertificateCache();

    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "2second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "2third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl1.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(!status.isRevoked);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test65_crl_test_revocation_set2)
{
    CertificateCacheDAO::clearCertificateCache();

    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "2second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "2third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl2.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(status.isRevoked);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test66_crl_update_expired_lists)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateLoader loader;
    loader.loadCertificateFromRawData(google2nd);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    TestCRL crl;
    RUNNER_ASSERT_MSG(
            crl.updateList(loader.getCertificatePtr(), CRL::UPDATE_ON_EXPIRED),
            "CRL update on expired succeeded");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test67_crl_update_lists_on_demand)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateLoader loader;
    loader.loadCertificateFromRawData(google2nd);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    TestCRL crl;
    RUNNER_ASSERT_MSG(
            crl.updateList(loader.getCertificatePtr(), CRL::UPDATE_ON_DEMAND),
            "CRL update on demand succeeded");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test68_cached_crl_test_positive)
{
    CertificateCacheDAO::clearCertificateCache();

    TestCRL crl;

    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "2second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "2third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl1.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);

    CachedCRL cached;
    VerificationStatus cached_status = cached.check(collection);
    CRLCachedDataList list;
    CertificateCacheDAO::getCRLResponseList(&list);
    unsigned len = list.size();

    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(!status.isRevoked &&
                  cached_status == VERIFICATION_STATUS_GOOD);

    cached_status = cached.check(collection);
    list.clear();
    CertificateCacheDAO::getCRLResponseList(&list);

    RUNNER_ASSERT(len == list.size());
    RUNNER_ASSERT(!status.isRevoked &&
                  cached_status == VERIFICATION_STATUS_GOOD);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test69_cached_crl_test_negative)
{
    CertificateCacheDAO::clearCertificateCache();

    //Prepare certificate chain
    TestCRL crl;
    std::string cacertStr(crl.getFileContent(cert_store_path + "cacert.pem"));
    std::string certAStr(
            crl.getFileContent(cert_store_path + "2second_level.pem"));
    std::string certBStr(
            crl.getFileContent(cert_store_path + "2third_level.pem"));
    crl.addCRLToStore(cert_store_path + "cacrl2.pem", crl_URI);

    CertificateLoader loader;
    CertificateList certList;
    CertificateCollection collection;
    RUNNER_ASSERT(loader.loadCertificateFromRawData(cacertStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certAStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());
    RUNNER_ASSERT(loader.loadCertificateFromRawData(certBStr) ==
                  CertificateLoader::NO_ERROR);
    RUNNER_ASSERT(!!loader.getCertificatePtr());
    certList.push_back(loader.getCertificatePtr());

    collection.load(certList);

    CRL::RevocationStatus status = crl.checkCertificateChain(collection);
    CachedCRL cached;
    VerificationStatus cached_status = cached.check(collection);
    CRLCachedDataList list;
    CertificateCacheDAO::getCRLResponseList(&list);
    unsigned len = list.size();

    RUNNER_ASSERT(status.isCRLValid);
    RUNNER_ASSERT(status.isRevoked &&
                  cached_status == VERIFICATION_STATUS_REVOKED);

    cached_status = cached.check(collection);
    list.clear();
    CertificateCacheDAO::getCRLResponseList(&list);

    RUNNER_ASSERT(len == list.size());
    RUNNER_ASSERT(status.isRevoked &&
                  cached_status == VERIFICATION_STATUS_REVOKED);

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test70_ocsp_local_validation_positive)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pRootCert;
    std::string caRootPath(cert_store_path + "cacert.pem"),
        certLevel0Path(cert_store_path + "1second_level.pem");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load cacert.pem");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    if (!pCert0) {
        RUNNER_ASSERT_MSG(false, "Couldn't load 1second_level.pem");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error - check if "
                      "dpl-tests-vcore-ocsp-server.sh is running!");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test71_ocsp_local_validation_positive)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pRootCert;
    std::string caRootPath(cert_store_path + "cacert.pem"),
        certLevel0Path(cert_store_path + "3second_level.pem");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load cacert.pem");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    if (!pCert0) {
        RUNNER_ASSERT_MSG(false, "Couldn't load 3second_level.pem");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error - check if "
                      "dpl-tests-vcore-ocsp-server.sh is running!");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test72_ocsp_local_validation_revoked)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pRootCert;
    std::string caRootPath(cert_store_path + "cacert.pem"),
        certLevel0Path(cert_store_path + "2second_level.pem");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load cacert.pem");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    if (!pCert0) {
        RUNNER_ASSERT_MSG(false, "Couldn't load 2second_level.pem");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error - check if "
                      "dpl-tests-vcore-ocsp-server.sh is running!");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_REVOKED),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_UNKNOWN),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

RUNNER_TEST(test73_ocsp_local_validation_error_unknown_cert)
{
    CertificateCacheDAO::clearCertificateCache();

    CertificateList lOCSPCertificates;
    CertificatePtr certificatePtr;
    CertificatePtr pCert0;
    CertificatePtr pCert1;
    CertificatePtr pRootCert;
    std::string caRootPath(cert_store_path + "cacert.pem"),
        certLevel0Path(cert_store_path + "1second_level.pem"),
        certLevel1Path(cert_store_path + "1third_level.pem");

    pRootCert = RevocationCheckerBase::loadPEMFile(caRootPath.c_str());
    if (!pRootCert) {
        RUNNER_ASSERT_MSG(false, "Couldn't load cacerr.pem");
    }
    lOCSPCertificates.push_back(pRootCert);

    pCert0 = RevocationCheckerBase::loadPEMFile(certLevel0Path.c_str());
    if (!pCert0) {
        RUNNER_ASSERT_MSG(false, "Couldn't load 1second_level.pem");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert0));

    pCert1 = RevocationCheckerBase::loadPEMFile(certLevel1Path.c_str());
    if (!pCert1) {
        RUNNER_ASSERT_MSG(false, "Couldn't load 1third_level.pem");
    }
    lOCSPCertificates.push_back(CertificatePtr(pCert1));

    OCSP ocsp;
    ocsp.setDigestAlgorithmForCertId(ValidationCore::OCSP::SHA1);
    ocsp.setDigestAlgorithmForRequest(ValidationCore::OCSP::SHA1);

    CertificateCollection collection;
    collection.load(lOCSPCertificates);
    RUNNER_ASSERT(collection.sort());
    CertificateList sorted = collection.getChain();

    ocsp.setTrustedStore(sorted);
    VerificationStatusSet status = ocsp.validateCertificateList(sorted);

    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_CONNECTION_FAILED),
                      "Caught OCSP connection error - check if "
                      "dpl-tests-vcore-ocsp-server.sh is running!");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_GOOD),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_REVOKED),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(status.contains(VERIFICATION_STATUS_VERIFICATION_ERROR),
                      "Caught OCSP verification error exception");
    RUNNER_ASSERT_MSG(!status.contains(VERIFICATION_STATUS_UNKNOWN),
                          "Caught OCSP verification error exception");

    CertificateCacheDAO::clearCertificateCache();
}

