SET(TARGET_CERTIFICATE_DAO_DB "Sqlite3DbCertificate")

ADD_CUSTOM_COMMAND( OUTPUT .certificate.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.certificate.db
   COMMAND gcc -Wall -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/certificate_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/certificate_dao/orm/certificate_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/certificate_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.certificate.db ".read ${CMAKE_CURRENT_BINARY_DIR}/certificate_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.certificate.db
   DEPENDS ${PROJECT_SOURCE_DIR}/modules/certificate_dao/orm/certificate_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/certificate_dao/orm/certificate_db
   )

ADD_CUSTOM_COMMAND( OUTPUT .certificate.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.certificate.db-journal
   )

ADD_CUSTOM_TARGET(${TARGET_CERTIFICATE_DAO_DB} ALL DEPENDS .certificate.db .certificate.db-journal)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/certificate_db.sql DESTINATION share/wrt-engine/)

###############################################################################

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(CERTIFICATE_DAO_DEPS
    glib-2.0
    REQUIRED)

SET(CERTIFICATE_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/certificate_dao/include
    ${PROJECT_SOURCE_DIR}/modules/certificate_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${PROJECT_SOURCE_DIR}/modules/widget_dao/include
)

SET(CERTIFICATE_DAO_SOURCES
    dao/certificate_dao_types.cpp
    dao/certificate_dao.cpp
    dao/certificate_database.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${CERTIFICATE_DAO_DEPS_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES(${CERTIFICATE_DAO_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_CERTIFICATE_DAO_LIB} SHARED ${CERTIFICATE_DAO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_CERTIFICATE_DAO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
TARGET_LINK_LIBRARIES(${TARGET_CERTIFICATE_DAO_LIB} ${TARGET_DPL_EFL} ${TARGET_DPL_DB_EFL} ${TARGET_WRT_DAP_RO_LIB} ${CERTIFICATE_DAO_DEPS_LIBRARIES})
ADD_DEPENDENCIES(${TARGET_CERTIFICATE_DAO_LIB} ${TARGET_CERTIFICATE_DAO_DB})

INSTALL(TARGETS ${TARGET_CERTIFICATE_DAO_LIB} DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES
    include/wrt-commons/certificate-dao/certificate_dao_types.h
    include/wrt-commons/certificate-dao/certificate_database.h
    include/wrt-commons/certificate-dao/certificate_dao.h
    DESTINATION include/dpl-efl/wrt-commons/certificate-dao
)

