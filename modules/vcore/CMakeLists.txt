#DB vcore
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_BINARY_DIR}/modules/vcore/src/database_checksum_vcore.h
    COMMAND ${CMAKE_SOURCE_DIR}/modules/vcore/src/orm/gen_db_md5.sh
    ARGS ${CMAKE_BINARY_DIR}/modules/vcore/src/database_checksum_vcore.h
         ${CMAKE_SOURCE_DIR}/modules/vcore/src/orm/vcore_db
    DEPENDS ${CMAKE_SOURCE_DIR}/modules/vcore/src/orm/vcore_db
            ${CMAKE_SOURCE_DIR}/modules/vcore/src/orm/gen_db_md5.sh
    COMMENT "Generating VCORE database checksum"
    )

ADD_CUSTOM_COMMAND( OUTPUT .vcore.db
  COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/.vcore.db
  COMMAND gcc -Wall -include ${CMAKE_BINARY_DIR}/modules/vcore/src/database_checksum_vcore.h -I${PROJECT_SOURCE_DIR}/modules/db/include/ -I${PROJECT_SOURCE_DIR}/modules/vcore/src/orm -E ${PROJECT_SOURCE_DIR}/modules/vcore/src/orm/vcore_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_BINARY_DIR}/vcore_db.sql
  COMMAND sqlite3 ${CMAKE_CURRENT_BINARY_DIR}/.vcore.db ".read ${CMAKE_CURRENT_BINARY_DIR}/vcore_db.sql" || rm -f ${CMAKE_CURRENT_BINARY_DIR}/.vcore.db
  DEPENDS ${CMAKE_BINARY_DIR}/modules/vcore/src/database_checksum_vcore.h ${PROJECT_SOURCE_DIR}/modules/vcore/src/orm/vcore_db_sql_generator.h ${PROJECT_SOURCE_DIR}/modules/vcore/src/orm/vcore_db
  )

ADD_CUSTOM_COMMAND( OUTPUT .vcore.db-journal
  COMMAND touch
  ARGS  ${CMAKE_CURRENT_BINARY_DIR}/.vcore.db-journal
  )

ADD_CUSTOM_TARGET(Sqlite3DbVCORE ALL DEPENDS .vcore.db .vcore.db-journal)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/vcore_db.sql
    DESTINATION share/wrt-engine/
    )

ADD_SUBDIRECTORY(src)
