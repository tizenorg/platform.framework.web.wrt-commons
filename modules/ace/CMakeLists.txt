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
# Path for all tests binary files

######################################################################
#DB ace
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h
    COMMAND ${CMAKE_SOURCE_DIR}/modules/ace/orm/gen_db_md5.sh
    ARGS ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h
         ${CMAKE_SOURCE_DIR}/modules/ace/orm/ace_db
    DEPENDS ${CMAKE_SOURCE_DIR}/modules/ace/orm/ace_db
            ${CMAKE_SOURCE_DIR}/modules/ace/orm/gen_db_md5.sh
    COMMENT "Generating ACE database checksum"
    )
ADD_CUSTOM_COMMAND( OUTPUT .ace.db
  COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.ace.db
  COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/ace/orm -E ${PROJECT_SOURCE_DIR}/modules/ace/orm/ace_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/ace_db.sql
  COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.ace.db ".read ${CMAKE_CURRENT_BINARY_DIR}/ace_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.ace.db
  DEPENDS ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h ${PROJECT_SOURCE_DIR}/modules/ace/orm/ace_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/ace/orm/ace_db
  )

ADD_CUSTOM_COMMAND( OUTPUT .ace.db-journal
  COMMAND touch
  ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.ace.db-journal
  )

ADD_CUSTOM_TARGET(Sqlite3DbACE ALL DEPENDS .ace.db .ace.db-journal)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/ace_db.sql
    DESTINATION share/wrt-engine/
    )

###########################################################

INCLUDE(FindPkgConfig)

SET(ACE_TEST_PATH "/usr/apps/org.tizen.policy")

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/config.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/config.dtd
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/bondixml.xsd
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/demo.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/WACPolicy.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/UnrestrictedPolicy.xml
    DESTINATION /usr/etc/ace
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE)

add_subdirectory(dao)

SET(ACE_LIB_DEPS_BASIC
    ecore
    appcore-efl
    openssl
    sqlite3
    heynoti
    dlog
    vconf
    db-util
    libpcrecpp
    icu-uc
    libxml-2.0
    )

IF(SMACK_ENABLED)
    LIST(APPEND ACE_LIB_DEPS_BASIC libprivilege-control)
ENDIF(SMACK_ENABLED)

PKG_CHECK_MODULES(ACE_LIB_DEPS ${ACE_LIB_DEPS_BASIC} REQUIRED)

SET(WRT_ACE_DIR ${PROJECT_SOURCE_DIR}/modules/ace)

SET(ACE_SOURCES
  ${WRT_ACE_DIR}/engine/PolicyEvaluator.cpp
  ${WRT_ACE_DIR}/engine/PolicyInformationPoint.cpp
  ${WRT_ACE_DIR}/engine/CombinerImpl.cpp
  ${WRT_ACE_DIR}/engine/parser.cpp
  ${WRT_ACE_DIR}/engine/PolicyEnforcementPoint.cpp
  ${WRT_ACE_DIR}/engine/SettingsLogic.cpp
  ${WRT_ACE_DIR}/engine/Attribute.cpp
  ${WRT_ACE_DIR}/engine/Condition.cpp
  ${WRT_ACE_DIR}/engine/NodeFactory.cpp
  ${WRT_ACE_DIR}/engine/Policy.cpp
  ${WRT_ACE_DIR}/engine/Rule.cpp
  ${WRT_ACE_DIR}/engine/Serializer.cpp
  ${WRT_ACE_DIR}/engine/Subject.cpp
  ${WRT_ACE_DIR}/engine/TreeNode.cpp
  ${WRT_ACE_DIR}/engine/ConfigurationManager.cpp
)

INCLUDE_DIRECTORIES(${ACE_LIB_DEPS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${WRT_ACE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/core/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/db/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/log/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/event/include)

SET(WITH_ACE_SETTINGS_SERVER_SOURCES
   ${WITH_ACE_SETTINGS_SERVER_NONE_SOURCES}
   )

ADD_LIBRARY(${TARGET_ACE_LIB} SHARED
    ${ACE_SOURCES}
    ${WITH_ACE_SETTINGS_SERVER_SOURCES}
)

SET_TARGET_PROPERTIES(${TARGET_ACE_LIB} PROPERTIES
    SOVERSION ${VERSION})

SET_TARGET_PROPERTIES(${TARGET_ACE_LIB} PROPERTIES
    COMPILE_FLAGS -fPIC)

TARGET_LINK_LIBRARIES(${TARGET_ACE_LIB}
#    ${TARGET_WRT_DAO_RW_LIB} #For now WRT has to be RW - Global dao write is needed
    ${TARGET_ACE_DAO_RW_LIB}
    ${ACE_LIB_DEPS_LIBRARIES}
    ${DPL_EFL_LIBRARY}
)

INSTALL(TARGETS ${TARGET_ACE_LIB}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES
    include/dpl/ace/AbstractPolicyEnforcementPoint.h
    include/dpl/ace/AbstractTreeElement.h
    include/dpl/ace/Attribute.h
    include/dpl/ace/AsyncVerdictResultListener.h
    include/dpl/ace/Combiner.h
    include/dpl/ace/CombinerImpl.h
    include/dpl/ace/ConfigurationManager.h
    include/dpl/ace/Constants.h
    include/dpl/ace/Effect.h
    include/dpl/ace/PermissionTriple.h
    include/dpl/ace/Policy.h
    include/dpl/ace/PolicyEffect.h
    include/dpl/ace/PolicyEnforcementPoint.h
    include/dpl/ace/PolicyEvaluator.h
    include/dpl/ace/PolicyEvaluatorFactory.h
    include/dpl/ace/PolicyInformationPoint.h
    include/dpl/ace/PolicyResult.h
    include/dpl/ace/Preference.h
    include/dpl/ace/PromptDecision.h
    include/dpl/ace/Request.h
    include/dpl/ace/SettingsLogic.h
    include/dpl/ace/Subject.h
    include/dpl/ace/TreeNode.h
    include/dpl/ace/UserDecision.h
    include/dpl/ace/WRT_INTERFACE.h
    include/dpl/ace/Verdict.h
    DESTINATION
    include/dpl-efl/dpl/ace
    )

