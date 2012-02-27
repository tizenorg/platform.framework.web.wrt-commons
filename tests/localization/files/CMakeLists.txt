INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/tests/localization/files/one
    DESTINATION
    /opt/apps/widget/tests/localization/widget1/locales/pl-en
  )

INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/tests/localization/files/one
    ${PROJECT_SOURCE_DIR}/tests/localization/files/two
    DESTINATION
    /opt/apps/widget/tests/localization/widget2/locales/pl-en
  )

INSTALL(FILES
    ${PROJECT_SOURCE_DIR}/tests/localization/files/two
    DESTINATION
    /opt/apps/widget/tests/localization/widget2/locales/en-en
  )

