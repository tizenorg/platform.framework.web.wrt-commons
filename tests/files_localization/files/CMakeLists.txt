INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/one
    DESTINATION
    /opt/share/widget/tests/localization/widget1/res/wgt/locales/pl-en
  )

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/one
    ${CMAKE_CURRENT_SOURCE_DIR}/two
    DESTINATION
    /opt/share/widget/tests/localization/widget2/res/wgt/locales/pl-en
  )

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/two
    DESTINATION
    /opt/share/widget/tests/localization/widget2/res/wgt/locales/en-en
  )

