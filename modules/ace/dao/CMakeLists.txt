
SET(ACE_DAO_DEPS_LIST
    ecore
    appcore-efl
    openssl
    vconf
    db-util
    libpcrecpp
    icu-uc
    libxml-2.0
    )

PKG_CHECK_MODULES(ACE_DAO_DEPS ${ACE_DAO_DEPS_LIST} REQUIRED)

set(ACE_SRC_DIR ${PROJECT_SOURCE_DIR}/modules/ace/dao)

set(ACE_DAO_RO_SOURCES
    ${ACE_SRC_DIR}/AceDAOReadOnly.cpp
    ${ACE_SRC_DIR}/AceDAOUtilities.cpp
    ${ACE_SRC_DIR}/AceDAOConversions.cpp
    ${ACE_SRC_DIR}/BaseAttribute.cpp
    ${ACE_SRC_DIR}/AceDatabase.cpp
    ${ACE_SRC_DIR}/PromptModel.cpp
)

set(ACE_DAO_RW_SOURCES
    ${ACE_SRC_DIR}/AceDAO.cpp
)

INCLUDE_DIRECTORIES(${ACE_SRC_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/log/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/db/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/core/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/ace/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/ace/orm)
INCLUDE_DIRECTORIES(${ACE_DAO_DEPS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/ace/orm)

ADD_LIBRARY(${TARGET_ACE_DAO_RO_LIB} SHARED
    ${ACE_DAO_RO_SOURCES}
)

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RO_LIB} PROPERTIES
    SOVERSION ${VERSION})

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RO_LIB} PROPERTIES
    COMPILE_FLAGS -fPIC)

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RO_LIB} PROPERTIES
    COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h")
 
target_link_libraries(${TARGET_ACE_DAO_RO_LIB}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_DB_EFL}
    ${ACE_DAO_DEPS_LIBRARY}
    ${ACE_DAO_DEPS_LDFLAGS}
)

ADD_LIBRARY(${TARGET_ACE_DAO_RW_LIB} SHARED
    ${ACE_DAO_RW_SOURCES}
)

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RW_LIB} PROPERTIES
    SOVERSION ${VERSION})

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RW_LIB} PROPERTIES
    COMPILE_FLAGS -fPIC)

SET_TARGET_PROPERTIES(${TARGET_ACE_DAO_RW_LIB} PROPERTIES
    COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/ace/database_checksum_ace.h")

target_link_libraries(${TARGET_ACE_DAO_RW_LIB}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_DB_EFL}
    ${TARGET_ACE_DAO_RO_LIB}
)

INSTALL(TARGETS ${TARGET_ACE_DAO_RO_LIB}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(TARGETS ${TARGET_ACE_DAO_RW_LIB}
    DESTINATION lib
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/PromptModel.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/PreferenceTypes.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/BaseAttribute.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/BasePermission.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/VerdictTypes.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/TimedVerdict.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/IRequest.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/ValidityTypes.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/AceDAOReadOnly.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/AceDatabase.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/wrt_db_types.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/common_dao_types.h
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-ro/AceDAOConversions.h
    DESTINATION include/dpl-efl/dpl/ace-dao-ro
)

INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/modules/ace/include/dpl/ace-dao-rw/AceDAO.h
    DESTINATION include/dpl-efl/dpl/ace-dao-rw
)
