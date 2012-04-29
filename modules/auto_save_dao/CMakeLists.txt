ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h
   COMMAND ${CMAKE_SOURCE_DIR}/modules/auto_save_dao/orm/gen_db_md5.sh
   ARGS ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h
        ${CMAKE_SOURCE_DIR}/modules/auto_save_dao/orm/autosave_db
   DEPENDS ${CMAKE_SOURCE_DIR}/modules/auto_save_dao/orm/autosave_db
        ${CMAKE_SOURCE_DIR}/modules/auto_save_dao/orm/gen_db_md5.sh
   COMMENT "Generating WRT auto save database checksum"
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_autosave.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_autosave.db
   COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/auto_save_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/auto_save_dao/orm/autosave_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/wrt_autosave_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.wrt_autosave.db ".read ${CMAKE_CURRENT_BINARY_DIR}/wrt_autosave_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_autosave.db
   DEPENDS ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h ${PROJECT_SOURCE_DIR}/modules/auto_save_dao/orm/autosave_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/auto_save_dao/orm/autosave_db
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_autosave.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.wrt_autosave.db-journal
   )

ADD_CUSTOM_TARGET(Sqlite3DbAutoSave ALL DEPENDS .wrt_autosave.db .wrt_autosave.db-journal)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/wrt_autosave_db.sql DESTINATION share/wrt-engine/)

###############################################################################

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(AUTO_SAVE_DAO_DEPS
    glib-2.0
    REQUIRED)

SET(AUTO_SAVE_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/auto_save_dao/include
    ${PROJECT_SOURCE_DIR}/modules/auto_save_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${AUTO_SAVE_DAO_DEPS_INCLUDE_DIRS}
)

SET(AUTO_SAVE_DAO_SOURCES
    dao/common_dao_types.cpp
    dao/AutoSaveDatabase.cpp
)

SET(AUTO_SAVE_DAO_RO_SOURCES
    dao/auto_save_dao_read_only.cpp
)

SET(AUTO_SAVE_DAO_RW_SOURCES
    dao/auto_save_dao.cpp
)

INCLUDE_DIRECTORIES(${AUTO_SAVE_DAO_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_AUTO_SAVE_DAO_LIB} SHARED ${AUTO_SAVE_DAO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h")
TARGET_LINK_LIBRARIES(${TARGET_AUTO_SAVE_DAO_LIB} ${TARGET_DPL_EFL} ${TARGET_DPL_DB_EFL} ${AUTO_SAVE_DAO_DEPS_LIBRARIES})

ADD_LIBRARY(${TARGET_AUTO_SAVE_DAO_RO_LIB} SHARED ${AUTO_SAVE_DAO_RO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_RO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_RO_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h")
TARGET_LINK_LIBRARIES(${TARGET_AUTO_SAVE_DAO_RO_LIB} ${TARGET_AUTO_SAVE_DAO_LIB})

ADD_LIBRARY(${TARGET_AUTO_SAVE_DAO_RW_LIB} SHARED ${AUTO_SAVE_DAO_RW_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_RW_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_AUTO_SAVE_DAO_RW_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/auto_save_dao/database_checksum_autosave.h")
TARGET_LINK_LIBRARIES(${TARGET_AUTO_SAVE_DAO_RW_LIB} ${TARGET_AUTO_SAVE_DAO_RO_LIB})

INSTALL(TARGETS ${TARGET_AUTO_SAVE_DAO_LIB} DESTINATION lib)
INSTALL(TARGETS ${TARGET_AUTO_SAVE_DAO_RO_LIB} DESTINATION lib)
INSTALL(TARGETS ${TARGET_AUTO_SAVE_DAO_RW_LIB} DESTINATION lib)

INSTALL(FILES
    include/wrt-commons/auto-save-dao/common_dao_types.h
    include/wrt-commons/auto-save-dao/AutoSaveDatabase.h
    DESTINATION include/dpl-efl/wrt-commons/auto-save-dao
)

INSTALL(FILES
    include/wrt-commons/auto-save-dao-ro/auto_save_dao_read_only.h
    DESTINATION include/dpl-efl/wrt-commons/auto-save-dao-ro
)

INSTALL(FILES
    include/wrt-commons/auto-save-dao-rw/auto_save_dao.h
    DESTINATION include/dpl-efl/wrt-commons/auto-save-dao-rw
)
