
SET(ETC_DIR ${PROJECT_SOURCE_DIR}/etc)

INSTALL(FILES
    ${ETC_DIR}/wrt_commons_reset_db.sh
    ${ETC_DIR}/wrt_commons_create_clean_db.sh
    DESTINATION /usr/bin
    )
