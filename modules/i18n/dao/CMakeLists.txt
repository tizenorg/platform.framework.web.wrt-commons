SET(TARGET_I18N_DAO_DB "Sqlite3DbI18n")

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_BINARY_DIR}/modules/i18n/dao/database_checksum_i18n.h
   COMMAND ${CMAKE_SOURCE_DIR}/modules/i18n/dao/orm/gen_db_md5.sh
   ARGS ${CMAKE_BINARY_DIR}/modules/i18n/dao/database_checksum_i18n.h
        ${CMAKE_SOURCE_DIR}/modules/i18n/dao/orm/iana_db
   DEPENDS ${CMAKE_SOURCE_DIR}/modules/i18n/dao/orm/iana_db
        ${CMAKE_SOURCE_DIR}/modules/i18n/dao/orm/gen_db_md5.sh
   COMMENT "Generating WRT i18n database checksum"
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_i18n.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_i18n.db
   COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/i18n/dao/database_checksum_i18n.h -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/i18n/dao/orm -E ${PROJECT_SOURCE_DIR}/modules/i18n/dao/orm/i18n_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/wrt_i18n_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.wrt_i18n.db ".read ${CMAKE_CURRENT_BINARY_DIR}/wrt_i18n_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_i18n.db
   DEPENDS ${CMAKE_BINARY_DIR}/modules/i18n/dao/database_checksum_i18n.h ${PROJECT_SOURCE_DIR}/modules/i18n/dao/orm/i18n_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/i18n/dao/orm/iana_db
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_i18n.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.wrt_i18n.db-journal
   )

ADD_CUSTOM_TARGET(${TARGET_I18N_DAO_DB} ALL DEPENDS .wrt_i18n.db .wrt_i18n.db-journal)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/wrt_i18n_db.sql DESTINATION share/wrt-engine/)

###############################################################################

SET(I18N_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/i18n/dao/include
    ${PROJECT_SOURCE_DIR}/modules/i18n/dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
)


SET(I18N_DAO_RO_SOURCES
    src/i18n_database.cpp
    src/i18n_dao_read_only.cpp
)

INCLUDE_DIRECTORIES(${I18N_DAO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${I18N_DAO_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_I18N_DAO_RO_LIB} SHARED ${I18N_DAO_RO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_I18N_DAO_RO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_I18N_DAO_RO_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/i18n/dao/database_checksum_i18n.h")
TARGET_LINK_LIBRARIES(${TARGET_I18N_DAO_RO_LIB} ${TARGET_DPL_DB_EFL})
ADD_DEPENDENCIES(${TARGET_I18N_DAO_RO_LIB} ${TARGET_I18N_DAO_DB})

INSTALL(TARGETS ${TARGET_I18N_DAO_RO_LIB} DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES
    include/wrt-commons/i18n-dao-ro/i18n_database.h
    include/wrt-commons/i18n-dao-ro/i18n_dao_read_only.h
    DESTINATION include/dpl-efl/wrt-commons/i18n-dao-ro
)
