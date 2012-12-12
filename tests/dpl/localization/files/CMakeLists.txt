INSTALL(FILES
    ${DPL_TESTS_DIR}/localization/files/one
    DESTINATION
    /opt/share/widget/tests/localization/widget1/locales/pl-en
  )

INSTALL(FILES
    ${DPL_TESTS_DIR}/localization/files/one
    ${DPL_TESTS_DIR}/localization/files/two
    DESTINATION
    /opt/share/widget/tests/localization/widget2/locales/pl-en
  )

INSTALL(FILES
    ${DPL_TESTS_DIR}/localization/files/two
    DESTINATION
    /opt/share/widget/tests/localization/widget2/locales/en-en
  )

