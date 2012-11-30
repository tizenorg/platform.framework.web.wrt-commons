#git:framework/web/wrt-commons wrt-commons 0.2.76
Name:       wrt-commons
Summary:    Wrt common library
Version:    0.2.76
Release:    1
Group:      Development/Libraries
License:    Apache License, Version 2.0
URL:        N/A
Source0:    %{name}-%{version}.tar.gz
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

%build
export LDFLAGS+="-Wl,--rpath=%{_libdir} -Wl,--hash-style=both -Wl,--as-needed"

cmake . -DVERSION=%{version} \
        -DDPL_LOG="OFF"      \
        -DCMAKE_INSTALL_PREFIX=%{_prefix} \
        -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}
make %{?jobs:-j%jobs}

%install
mkdir -p %{buildroot}/usr/share/license
cp %{name} %{buildroot}/usr/share/license/
%make_install

%clean
rm -rf %{buildroot}

%post
mkdir -p /opt/share/widget/system
mkdir -p /opt/share/widget/user
mkdir -p /opt/share/widget/exec
mkdir -p /opt/share/widget/data/Public
mkdir -p /usr/lib/wrt-plugins

if [ -z ${2} ]; then
    echo "This is new install of wrt-commons"
    echo "Calling /usr/bin/wrt_commons_reset_db.sh"
    /usr/bin/wrt_commons_reset_db.sh
else
    # Find out old and new version of databases
    WRT_OLD_DB_VERSION=`sqlite3 /opt/dbspace/.wrt.db ".tables" | grep "DB_VERSION_"`
    WRT_NEW_DB_VERSION=`cat /usr/share/wrt-engine/wrt_db.sql | tr '[:blank:]' '\n' | grep DB_VERSION_`
    echo "OLD wrt database version ${WRT_OLD_DB_VERSION}"
    echo "NEW wrt database version ${WRT_NEW_DB_VERSION}"

    if [ ${WRT_OLD_DB_VERSION} -a ${WRT_NEW_DB_VERSION} ]
    then
        if [ ${WRT_NEW_DB_VERSION} = ${WRT_OLD_DB_VERSION} ]
        then
            echo "Equal database detected so db installation ignored"
        else
            echo "Calling /usr/bin/wrt_commons_reset_db.sh"
            /usr/bin/wrt_commons_reset_db.sh
        fi
    else
        echo "Calling /usr/bin/wrt_commons_reset_db.sh"
        /usr/bin/wrt_commons_reset_db.sh
    fi
fi

mkdir -p /usr/etc/ace
mkdir -p /usr/apps/org.tizen.policy

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

# Set Smack label for db files
chsmack -a 'wrt-commons::db_wrt' /opt/dbspace/.wrt.db
chsmack -a 'wrt-commons::db_wrt' /opt/dbspace/.wrt.db-journal
chsmack -a 'wrt-commons::db_wrt_autosave' /opt/dbspace/.wrt_autosave.db
chsmack -a 'wrt-commons::db_wrt_autosave' /opt/dbspace/.wrt_autosave.db-journal

echo "[WRT] wrt-commons postinst done ..."

%files
%manifest wrt-commons.manifest
%{_libdir}/*.so
%{_libdir}/*.so.*
/usr/share/wrt-engine/*
%attr(775,root,root) %{_bindir}/wrt_commons_reset_db.sh
%attr(775,root,root) %{_bindir}/wrt_commons_create_clean_db.sh
%{_datadir}/license/%{name}

%files devel
%{_includedir}/dpl-efl/*
%{_libdir}/pkgconfig/*.pc
