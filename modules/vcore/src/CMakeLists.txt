INCLUDE(FindPkgConfig)

PKG_CHECK_MODULES(VCORE_DEPS
    cert-svc
    ecore
    appcore-efl
    libxml-2.0
    libsoup-2.4
    openssl
    xmlsec1
    tapi
    REQUIRED)

SET(VCORE_DIR
    ${PROJECT_SOURCE_DIR}/modules/vcore
    )

SET(VCORE_SRC_DIR
    ${VCORE_DIR}/src/vcore
    )

SET(VCORE_SOURCES
    ${VCORE_SRC_DIR}/Base64.cpp
    ${VCORE_SRC_DIR}/CachedCRL.cpp
    ${VCORE_SRC_DIR}/CachedOCSP.cpp
    ${VCORE_SRC_DIR}/Certificate.cpp
    ${VCORE_SRC_DIR}/CertificateCacheDAO.cpp
    ${VCORE_SRC_DIR}/CertificateCollection.cpp
    ${VCORE_SRC_DIR}/CertificateConfigReader.cpp
    ${VCORE_SRC_DIR}/CertificateLoader.cpp
    ${VCORE_SRC_DIR}/CertificateVerifier.cpp
    ${VCORE_SRC_DIR}/Config.cpp
    ${VCORE_SRC_DIR}/CRL.cpp
    ${VCORE_SRC_DIR}/Database.cpp
    ${VCORE_SRC_DIR}/DeveloperModeValidator.cpp
    ${VCORE_SRC_DIR}/OCSP.cpp
    ${VCORE_SRC_DIR}/OCSPCertMgrUtil.cpp
    ${VCORE_SRC_DIR}/OCSPUtil.c
    ${VCORE_SRC_DIR}/ReferenceValidator.cpp
    ${VCORE_SRC_DIR}/RevocationCheckerBase.cpp
    ${VCORE_SRC_DIR}/SaxReader.cpp
    ${VCORE_SRC_DIR}/SignatureFinder.cpp
    ${VCORE_SRC_DIR}/SignatureReader.cpp
    ${VCORE_SRC_DIR}/SignatureValidator.cpp
    ${VCORE_SRC_DIR}/SoupMessageSendBase.cpp
    ${VCORE_SRC_DIR}/SoupMessageSendSync.cpp
    ${VCORE_SRC_DIR}/SoupMessageSendAsync.cpp
    ${VCORE_SRC_DIR}/VerificationStatus.cpp
    ${VCORE_SRC_DIR}/ValidatorFactories.cpp
    ${VCORE_SRC_DIR}/VCore.cpp
    ${VCORE_SRC_DIR}/XmlsecAdapter.cpp
    )

SET(VCORE_INCLUDES
    ${PROJECT_SOURCE_DIR}/modules/core/include
    ${PROJECT_SOURCE_DIR}/modules/log/include
    ${PROJECT_SOURCE_DIR}/modules/db/include
    ${VCORE_DEPS_INCLUDE_DIRS}
    ${VCORE_SRC_DIR}
    ${VCORE_DIR}/src
    ${VCORE_DIR}/src/orm
    ${VCORE_DIR}/src/legacy
    ${CMAKE_BINARY_DIR}/modules/vcore/src
    )

ADD_DEFINITIONS(${VCORE_DEPS_CFLAGS})
ADD_DEFINITIONS(${VCORE_DEPS_CFLAGS_OTHER})
ADD_DEFINITIONS("-DSEPARATED_SINGLETON_IMPLEMENTATION")

INCLUDE_DIRECTORIES(${VCORE_INCLUDES})

ADD_LIBRARY(${TARGET_VCORE_LIB} SHARED ${VCORE_SOURCES})
SET_TARGET_PROPERTIES(${TARGET_VCORE_LIB} PROPERTIES
    SOVERSION ${VERSION})

ADD_DEPENDENCIES(${TARGET_VCORE_LIB} Sqlite3DbWTF)

SET_TARGET_PROPERTIES(${TARGET_VCORE_LIB} PROPERTIES
  COMPILE_FLAGS -fPIC)

TARGET_LINK_LIBRARIES(${TARGET_VCORE_LIB}
    ${VCORE_DEPS_LIBRARIES}
    ${TARGET_DPL_EFL}
    ${TARGET_DPL_DB_EFL}
  )

INSTALL(TARGETS ${TARGET_VCORE_LIB}
    DESTINATION lib
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

INSTALL(FILES
    ${VCORE_SRC_DIR}/Base64.h
    ${VCORE_SRC_DIR}/CachedCRL.h
    ${VCORE_SRC_DIR}/CachedOCSP.h
    ${VCORE_SRC_DIR}/Certificate.h
    ${VCORE_SRC_DIR}/CertificateCacheDAO.h
    ${VCORE_SRC_DIR}/CertificateCollection.h
    ${VCORE_SRC_DIR}/CertificateConfigReader.h
    ${VCORE_SRC_DIR}/CertificateLoader.h
    ${VCORE_SRC_DIR}/CertificateStorage.h
    ${VCORE_SRC_DIR}/CertificateVerifier.h
    ${VCORE_SRC_DIR}/CertStoreType.h
    ${VCORE_SRC_DIR}/Config.h
    ${VCORE_SRC_DIR}/CRL.h
    ${VCORE_SRC_DIR}/Database.h
    ${VCORE_SRC_DIR}/DeveloperModeValidator.h
    ${VCORE_SRC_DIR}/IAbstractResponseCache.h
    ${VCORE_SRC_DIR}/OCSP.h
    ${VCORE_SRC_DIR}/OCSPCertMgrUtil.h
    ${VCORE_SRC_DIR}/ParserSchema.h
    ${VCORE_SRC_DIR}/ReferenceValidator.h
    ${VCORE_SRC_DIR}/RevocationCheckerBase.h
    ${VCORE_SRC_DIR}/SaxReader.h
    ${VCORE_SRC_DIR}/scoped_gpointer.h
    ${VCORE_SRC_DIR}/SignatureData.h
    ${VCORE_SRC_DIR}/SignatureFinder.h
    ${VCORE_SRC_DIR}/SignatureReader.h
    ${VCORE_SRC_DIR}/SignatureValidator.h
    ${VCORE_SRC_DIR}/SoupMessageSendBase.h
    ${VCORE_SRC_DIR}/SoupMessageSendSync.h
    ${VCORE_SRC_DIR}/SoupMessageSendAsync.h
    ${VCORE_SRC_DIR}/SSLContainers.h
    ${VCORE_SRC_DIR}/VerificationStatus.h
    ${VCORE_SRC_DIR}/ValidatorCommon.h
    ${VCORE_SRC_DIR}/ValidatorFactories.h
    ${VCORE_SRC_DIR}/VCore.h
    ${VCORE_SRC_DIR}/XmlsecAdapter.h
    DESTINATION include/dpl-efl/dpl/vcore/vcore
    PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    )

