# Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-example.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-wac-2.0.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-example2.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-example3.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy_example.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/general-test.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/interceptpolicy.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-test.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/policy-test-gsettings.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attre_config.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example1.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example2.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example3.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example4.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example5.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example6.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example7.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/attr_policy-example8.xml
    ${PROJECT_SOURCE_DIR}/tests/ace/test-configuration/undefined-test.xml
    DESTINATION /usr/etc/ace
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
  )

ADD_SUBDIRECTORY(CMTest)
