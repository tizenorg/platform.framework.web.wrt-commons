%bcond_with x
Name:       wrt-commons
Summary:    Wrt common library
Version:    0.2.160
Release:    1
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: 	wrt-commons.manifest
BuildRequires:  cmake
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(libssl)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gio-2.0)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(libpcrecpp)
BuildRequires:  pkgconfig(icu-i18n)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(libiri)
BuildRequires:  pkgconfig(libidn)
BuildRequires:  pkgconfig(minizip)
BuildRequires:  pkgconfig(libtzplatform-config)
Requires:       libtzplatform-config
%description
Wrt common library

%package devel
Summary:    Wrt common library development headers
Group:      Development/Libraries
Requires:   %{name} = %{version}

%description devel
Wrt common library development headers

%prep
%setup -q
cp %{SOURCE1001} .

%define with_tests 0
%if "%{WITH_TESTS}" == "ON" || "%{WITH_TESTS}" == "Y" || "%{WITH_TESTS}" == "YES" || "%{WITH_TESTS}" == "TRUE" || "%{WITH_TESTS}" == "1"
    %define with_tests 1
%endif

%define with_child 0
%if "%{WITH_CHILD}" == "ON" || "%{WITH_CHILD}" == "Y" || "%{WITH_CHILD}" == "YES" || "%{WITH_CHILD}" == "TRUE" || "%{WITH_CHILD}" == "1"
    %define with_child 1
%endif

%build
%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

export LDFLAGS+="-Wl,--rpath=%{_libdir} -Wl,--hash-style=both -Wl,--as-needed"

%cmake . -DVERSION=%{version} \
        -DDPL_LOG="OFF"      \
        -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} \
        %{?WITH_TESTS:-DWITH_TESTS=%WITH_TESTS} \
%if %{with x}
         -DX11_SUPPORT=On \
%else
         -DX11_SUPPORT=Off \
%endif
        %{?WITH_CHILD:-DWITH_CHILD=%WITH_CHILD}
make %{?jobs:-j%jobs}

%install
%make_install


%postun -p /sbin/ldconfig

%post 
/sbin/ldconfig
mkdir -p ${TZ_SYS_RW_WIDGET}/system
mkdir -p ${TZ_SYS_RW_WIDGET}/user
mkdir -p ${TZ_SYS_RW_WIDGET}/exec
mkdir -p ${TZ_SYS_RW_WIDGET}/data/Public
mkdir -p %{_libdir}/wrt-plugins

#Don't reset DB when install on QEMU (during other packages building witch GBS)
if [ -z "$EMULATOR_ARCHS" ]; then
    if [ -z ${2} ]; then
        echo "This is new install of wrt-commons"
        echo "Calling /usr/bin/wrt_commons_reset_db.sh"
        %{_bindir}/wrt_commons_reset_db.sh
    else
        # Find out old and new version of databases
        WRT_OLD_DB_VERSION=`sqlite3 %{TZ_SYS_DB}/.wrt.db ".tables" | grep "DB_VERSION_"`
        WRT_NEW_DB_VERSION=`cat %{TZ_SYS_RO_WRT_ENGINE}/wrt_db.sql | tr '[:blank:]' '\n' | grep DB_VERSION_`
        echo "OLD wrt database version ${WRT_OLD_DB_VERSION}"
        echo "NEW wrt database version ${WRT_NEW_DB_VERSION}"

        if [ ${WRT_OLD_DB_VERSION} -a ${WRT_NEW_DB_VERSION} ]
        then
            if [ ${WRT_NEW_DB_VERSION} = ${WRT_OLD_DB_VERSION} ]
            then
                echo "Equal database detected so db installation ignored"
            else
                echo "Calling /usr/bin/wrt_commons_reset_db.sh"
                %{_bindir}/wrt_commons_reset_db.sh
            fi
        else
            echo "Calling /usr/bin/wrt_commons_reset_db.sh"
            %{_bindir}/wrt_commons_reset_db.sh
        fi
    fi
fi

mkdir -p %{TZ_SYS_ACE_CONF}
mkdir -p %{TZ_SYS_RO_APP}/org.tizen.policy

# DBUS services fix
# WARNING: THIS IS TEMPORARY SOLUTION, AS THIS SHOULD NOT BE OUR
# RESPONSIBILITY!!! WE HAVE TO CONTACT TO DBUS MAINAINERS

if [ -f /var/lib/dbus/machine-id ]; then
    echo "machine-id exists"
else
    if [ -f /usr/var/lib/dbus/machine-id ]; then
        echo "machine-id exists"
    else
        echo "Creating machine-id"
        mkdir -p /usr/var/lib/dbus/
        dbus-uuidgen > /usr/var/lib/dbus/machine-id
        dbus-uuidgen --ensure=/usr/var/lib/dbus/machine-id
    fi
    mkdir -p /var/lib/dbus/
    cp /usr/var/lib/dbus/machine-id /var/lib/dbus/
fi

# Until the Tizen 3.0 smack domains have been defined, all
# files must use floor or else the system will fall on its face.
# Reenable the following using whatever ends up being the accepted
# domain structure once that has been established.
# chsmack -a 'wrt-commons::db_wrt' /opt/dbspace/.wrt.db
# chsmack -a 'wrt-commons::db_wrt' /opt/dbspace/.wrt.db-journal
# chsmack -a 'wrt-commons::db_wrt' /opt/usr/dbspace/.wrt_custom_handler.db
# chsmack -a 'wrt-commons::db_wrt' /opt/usr/dbspace/.wrt_custom_handler.db-journal
# chsmack -a '*' /opt/usr/dbspace/.wrt_i18n.db
# chsmack -a '*' /opt/usr/dbspace/.wrt_i18n.db-journal

echo "[WRT] wrt-commons postinst done ..."

%files
%manifest %{name}.manifest
%license  LICENSE
%{_libdir}/*.so.*
%{_datadir}/wrt-engine/*
%attr(755,root,root) %{_bindir}/wrt_commons_create_clean_db.sh
%attr(755,root,root) %{_bindir}/wrt_commons_reset_db.sh
%if %{with_tests}
    %attr(755,root,root) %{_bindir}/wrt-commons-tests-*
    %attr(755,root,root) %{_bindir}/wrt_dao_tests_prepare_db.sh
    %attr(755,root,root) %{_bindir}/wrt_db_localization_prepare.sh
    %{_datadir}/dbus-1/services/org.tizen.DBusTestService.service
    %{TZ_SYS_SHARE}/wrt/wrt-commons/tests/*
    %{TZ_SYS_RW_WIDGET}/tests/localization/*
%endif

%files devel
%manifest %{name}.manifest
%{_libdir}/*.so
%{_includedir}/dpl-efl/*
%{_libdir}/pkgconfig/*.pc
