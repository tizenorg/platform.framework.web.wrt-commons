SET(TARGET_SECURITY_ORIGIN_DAO_DB "Sqlite3DbSecurityOrigin")

ADD_CUSTOM_COMMAND( OUTPUT .security_origin.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.security_origin.db
   COMMAND gcc -Wall -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/security_origin_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/security_origin_dao/orm/security_origin_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/security_origin_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.security_origin.db ".read ${CMAKE_CURRENT_BINARY_DIR}/security_origin_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.security_origin.db
   DEPENDS ${PROJECT_SOURCE_DIR}/modules/security_origin_dao/orm/security_origin_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/security_origin_dao/orm/security_origin_db
   )

ADD_CUSTOM_COMMAND( OUTPUT .security_origin.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.security_origin.db-journal
   )

ADD_CUSTOM_TARGET(${TARGET_SECURITY_ORIGIN_DAO_DB} ALL DEPENDS .security_origin.db .security_origin.db-journal)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/security_origin_db.sql DESTINATION share/wrt-engine/)

###############################################################################

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(SECURITY_ORIGIN_DAO_DEPS
    glib-2.0
    REQUIRED)

SET(SECURITY_ORIGIN_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/security_origin_dao/include
    ${PROJECT_SOURCE_DIR}/modules/security_origin_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${PROJECT_SOURCE_DIR}/modules/widget_dao/include
)

SET(SECURITY_ORIGIN_DAO_SOURCES
    dao/security_origin_dao_types.cpp
    dao/security_origin_database.cpp
    dao/security_origin_dao.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${SECURITY_ORIGIN_DAO_DEPS_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES(${SECURITY_ORIGIN_DAO_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_SECURITY_ORIGIN_DAO_LIB} SHARED ${SECURITY_ORIGIN_DAO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_SECURITY_ORIGIN_DAO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
TARGET_LINK_LIBRARIES(${TARGET_SECURITY_ORIGIN_DAO_LIB} ${TARGET_DPL_EFL} ${TARGET_DPL_DB_EFL} ${TARGET_WRT_DAP_RO_LIB} ${SECURITY_ORIGIN_DAO_DEPS_LIBRARIES})
ADD_DEPENDENCIES(${TARGET_SECURITY_ORIGIN_DAO_LIB} ${TARGET_SECURITY_ORIGIN_DAO_DB})

INSTALL(TARGETS ${TARGET_SECURITY_ORIGIN_DAO_LIB} DESTINATION lib)

INSTALL(FILES
    include/wrt-commons/security-origin-dao/security_origin_dao_types.h
    include/wrt-commons/security-origin-dao/security_origin_database.h
    include/wrt-commons/security-origin-dao/security_origin_dao.h
    DESTINATION include/dpl-efl/wrt-commons/security-origin-dao
)

