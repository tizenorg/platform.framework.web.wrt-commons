
ADD_CUSTOM_COMMAND( OUTPUT dpl_orm_test_db.sql
  COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test.db
  COMMAND gcc -Wall -I${DPL_DB_INCLUDE_DIR} -E ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test_db_sql_generator.h | grep --invert-match "^#" > ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test_db.sql
  COMMAND sqlite3 ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test.db ".read ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test_db.sql" || rm -f ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test.db
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test_db_sql_generator.h ${CMAKE_CURRENT_SOURCE_DIR}/dpl_orm_test_db
)

ADD_CUSTOM_TARGET( Sqlite3Db ALL DEPENDS dpl_orm_test_db.sql )
