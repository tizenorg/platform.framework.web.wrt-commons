SET(TARGET_WRT_DAO_DB "Sqlite3DbWRT")

ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h
   COMMAND ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/gen_db_md5.sh
   ARGS ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h
        ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/wrt_db
        ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/iana_db
   DEPENDS ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/wrt_db
        ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/iana_db
        ${CMAKE_SOURCE_DIR}/modules/widget_dao/orm/gen_db_md5.sh
   COMMENT "Generating WRT database checksum"
   )

ADD_CUSTOM_COMMAND( OUTPUT .wrt.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt.db
   COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/widget_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/widget_dao/orm/wrt_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/wrt_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.wrt.db ".read ${CMAKE_CURRENT_BINARY_DIR}/wrt_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.wrt.db
   DEPENDS ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h ${PROJECT_SOURCE_DIR}/modules/widget_dao/orm/wrt_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/widget_dao/orm/wrt_db ${PROJECT_SOURCE_DIR}/modules/widget_dao/orm/iana_db
   )
 
ADD_CUSTOM_COMMAND( OUTPUT .wrt.db-journal
   COMMAND touch
   ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.wrt.db-journal
   )
 
ADD_CUSTOM_TARGET(${TARGET_WRT_DAO_DB} ALL DEPENDS .wrt.db .wrt.db-journal)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/wrt_db.sql
    DESTINATION share/wrt-engine/
    )

###############################################################################

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(WRT_DAO_DEPS
    ecore
    appcore-efl
    libxml-2.0
    openssl
    REQUIRED)

set(WRT_DAO_RO_SOURCES
    dao/config_parser_data.cpp
    dao/common_dao_types.cpp
    dao/feature_dao_read_only.cpp
    dao/global_dao_read_only.cpp
    dao/path_builder.cpp
    dao/plugin_dao_read_only.cpp
    dao/property_dao_read_only.cpp
    dao/widget_dao_read_only.cpp
    dao/webruntime_database.cpp
    dao/WrtDatabase.cpp
    dao/widget_dao_types.cpp
)

set(WRT_DAO_RW_SOURCES
    dao/feature_dao.cpp
    dao/global_dao.cpp
    dao/plugin_dao.cpp
    dao/property_dao.cpp
    dao/widget_dao.cpp
)

SET(WRT_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/widget_dao/include
    ${PROJECT_SOURCE_DIR}/modules/event/include
    ${PROJECT_SOURCE_DIR}/modules/widget_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${PROJECT_SOURCE_DIR}/modules/localization/include
)

INCLUDE_DIRECTORIES(${WRT_DAO_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${WRT_DAO_DEPS_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_WRT_DAO_RO_LIB} SHARED
            ${WRT_DAO_RO_SOURCES}
)
SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RO_LIB} PROPERTIES
    SOVERSION ${API_VERSION} 
 VERSION ${VERSION})

SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RO_LIB} PROPERTIES
                      COMPILE_FLAGS -fPIC)

SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RO_LIB} PROPERTIES
                      COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h")

target_link_libraries(${TARGET_WRT_DAO_RO_LIB}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_DB_EFL}
    ${WRT_DAO_DEPS_LIBRARIES})
ADD_DEPENDENCIES(${TARGET_WRT_DAO_RO_LIB} ${TARGET_WRT_DAO_DB})

ADD_LIBRARY(${TARGET_WRT_DAO_RW_LIB} SHARED ${WRT_DAO_RW_SOURCES})

SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RW_LIB} PROPERTIES
    SOVERSION ${API_VERSION} 
 VERSION ${VERSION})

SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RW_LIB} PROPERTIES COMPILE_FLAGS -fPIC)

SET_TARGET_PROPERTIES(${TARGET_WRT_DAO_RW_LIB} PROPERTIES
                     COMPILE_FLAGS "-include ${CMAKE_BINARY_DIR}/modules/widget_dao/database_checksum.h")

target_link_libraries(${TARGET_WRT_DAO_RW_LIB}
    ${TARGET_WRT_DAO_RO_LIB})
ADD_DEPENDENCIES(${TARGET_WRT_DAO_RW_LIB} ${TARGET_WRT_DAO_DB})

INSTALL(TARGETS ${TARGET_WRT_DAO_RO_LIB}
    DESTINATION ${LIB_INSTALL_DIR})

INSTALL(TARGETS ${TARGET_WRT_DAO_RW_LIB}
    DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES
    include/dpl/wrt-dao-ro/config_parser_data.h
    include/dpl/wrt-dao-ro/common_dao_types.h
    include/dpl/wrt-dao-ro/feature_dao_read_only.h
    include/dpl/wrt-dao-ro/feature_model.h
    include/dpl/wrt-dao-ro/global_config.h
    include/dpl/wrt-dao-ro/global_dao_read_only.h
    include/dpl/wrt-dao-ro/path_builder.h
    include/dpl/wrt-dao-ro/plugin_dao_read_only.h
    include/dpl/wrt-dao-ro/property_dao_read_only.h
    include/dpl/wrt-dao-ro/widget_config.h
    include/dpl/wrt-dao-ro/vconf_config.h
    include/dpl/wrt-dao-ro/widget_dao_read_only.h
    include/dpl/wrt-dao-ro/wrt_db_types.h
    include/dpl/wrt-dao-ro/WrtDatabase.h
    include/dpl/wrt-dao-ro/widget_dao_types.h
    DESTINATION include/dpl-efl/dpl/wrt-dao-ro
    )

INSTALL(FILES
    include/dpl/wrt-dao-rw/feature_dao.h
    include/dpl/wrt-dao-rw/global_dao.h
    include/dpl/wrt-dao-rw/plugin_dao.h
    include/dpl/wrt-dao-rw/property_dao.h
    include/dpl/wrt-dao-rw/widget_dao.h
    DESTINATION include/dpl-efl/dpl/wrt-dao-rw
    )
