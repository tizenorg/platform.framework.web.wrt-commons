ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h
   COMMAND ${CMAKE_SOURCE_DIR}/modules/custom_handler_dao/orm/gen_db_md5.sh
   ARGS ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h
        ${CMAKE_SOURCE_DIR}/modules/custom_handler_dao/orm/custom_handler_db
   DEPENDS ${CMAKE_SOURCE_DIR}/modules/custom_handler_dao/orm/custom_handler_db
        ${CMAKE_SOURCE_DIR}/modules/custom_handler_dao/orm/gen_db_md5.sh
   COMMENT "Generating WRT custom handlers database checksum"
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_custom_handler.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_custom_handler.db
   COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/orm/custom_handler_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/wrt_custom_handler_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.wrt_custom_handler.db ".read ${CMAKE_CURRENT_BINARY_DIR}/wrt_custom_handler_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt_custom_handler.db
   DEPENDS ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h ${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/orm/custom_handler_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/orm/custom_handler_db
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt_custom_handler.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.wrt_custom_handler.db-journal
   )

ADD_CUSTOM_TARGET(Sqlite3DbCustomHandler ALL DEPENDS .wrt_custom_handler.db .wrt_custom_handler.db-journal)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/wrt_custom_handler_db.sql DESTINATION share/wrt-engine/)

###############################################################################

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(CUSTOM_HANDLER_DAO_DEPS
    glib-2.0
    REQUIRED)

SET(CUSTOM_HANDLER_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/include
    ${PROJECT_SOURCE_DIR}/modules/custom_handler_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
)


SET(CUSTOM_HANDLER_DAO_RO_SOURCES
    dao/CustomHandlerDatabase.cpp
    dao/custom_handler_dao_read_only.cpp
)

SET(CUSTOM_HANDLER_DAO_RW_SOURCES
    dao/custom_handler_dao.cpp
)


INCLUDE_DIRECTORIES(${CUSTOM_HANDLER_DAO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${CUSTOM_HANDLER_DAO_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_CUSTOM_HANDLER_DAO_RO_LIB} SHARED ${CUSTOM_HANDLER_DAO_RO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_CUSTOM_HANDLER_DAO_RO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_CUSTOM_HANDLER_DAO_RO_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h")
TARGET_LINK_LIBRARIES(${TARGET_CUSTOM_HANDLER_DAO_RO_LIB} ${TARGET_CUSTOM_HANDLER_DAO_LIB})

ADD_LIBRARY(${TARGET_CUSTOM_HANDLER_DAO_RW_LIB} SHARED ${CUSTOM_HANDLER_DAO_RW_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_CUSTOM_HANDLER_DAO_RW_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
SET_TARGET_PROPERTIES(${TARGET_CUSTOM_HANDLER_DAO_RW_LIB} PROPERTIES COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/custom_handler_dao/database_checksum_custom_handler.h")
TARGET_LINK_LIBRARIES(${TARGET_CUSTOM_HANDLER_DAO_RW_LIB} ${TARGET_CUSTOM_HANDLER_DAO_RO_LIB})

INSTALL(TARGETS ${TARGET_CUSTOM_HANDLER_DAO_RO_LIB} DESTINATION lib)
INSTALL(TARGETS ${TARGET_CUSTOM_HANDLER_DAO_RW_LIB} DESTINATION lib)

INSTALL(FILES
    include/wrt-commons/custom-handler-dao-ro/common_dao_types.h
    include/wrt-commons/custom-handler-dao-ro/CustomHandlerDatabase.h
    include/wrt-commons/custom-handler-dao-ro/custom_handler_dao_read_only.h
    DESTINATION include/dpl-efl/wrt-commons/custom-handler-dao-ro
)

INSTALL(FILES
    include/wrt-commons/custom-handler-dao-rw/custom_handler_dao.h
    DESTINATION include/dpl-efl/wrt-commons/custom-handler-dao-rw
)
