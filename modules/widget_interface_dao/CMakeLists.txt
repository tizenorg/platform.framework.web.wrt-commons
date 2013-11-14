SET(TARGET_WIDGET_INTERFACE_DAO_DB "Sqlite3DbWidgetInterface")

ADD_CUSTOM_COMMAND( OUTPUT .widget_interface.db
   COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.widget_interface.db
   COMMAND gcc -Wall -I${PROJECT_SOURCE_DIR}/modules/db/include -I${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/orm -E ${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/orm/widget_interface_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/widget_interface_db.sql
   COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.widget_interface.db ".read ${CMAKE_CURRENT_BINARY_DIR}/widget_interface_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.widget_interface.db
   DEPENDS ${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/orm/widget_interface_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/orm/widget_interface_db
   )

ADD_CUSTOM_TARGET(${TARGET_WIDGET_INTERFACE_DAO_DB} ALL DEPENDS .widget_interface.db)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/widget_interface_db.sql DESTINATION share/wrt-engine/)

INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(WIDGET_INTERFACE_DAO_DEPS
    glib-2.0
    REQUIRED)

SET(WIDGET_INTERFACE_DAO_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/include
    ${PROJECT_SOURCE_DIR}/modules/widget_interface_dao/orm
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${PROJECT_SOURCE_DIR}/modules/widget_dao/include
)

SET(WIDGET_INTERFACE_DAO_SOURCES
    dao/widget_interface_dao.cpp
)

INCLUDE_DIRECTORIES(SYSTEM ${WIDGET_INTERFACE_DAO_DEPS_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES(${WIDGET_INTERFACE_DAO_INCLUDE_DIRS})

ADD_LIBRARY(${TARGET_WIDGET_INTERFACE_DAO_LIB} SHARED ${WIDGET_INTERFACE_DAO_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_WIDGET_INTERFACE_DAO_LIB} PROPERTIES SOVERSION ${API_VERSION} VERSION ${VERSION})
TARGET_LINK_LIBRARIES(${TARGET_WIDGET_INTERFACE_DAO_LIB} ${TARGET_DPL_EFL} ${TARGET_DPL_DB_EFL} ${TARGET_WRT_DAO_RO_LIB} ${WIDGET_INTERFACE_DAO_DEPS_LIBRARIES})
ADD_DEPENDENCIES(${TARGET_WIDGET_INTERFACE_DAO_LIB} ${TARGET_WIDGET_INTERFACE_DAO_DB})

INSTALL(TARGETS ${TARGET_WIDGET_INTERFACE_DAO_LIB} DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES
    include/wrt-commons/widget-interface-dao/widget_interface_dao.h
    DESTINATION include/dpl-efl/wrt-commons/widget-interface-dao
)
