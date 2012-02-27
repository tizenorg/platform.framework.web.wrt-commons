Name:       dpl
Summary:    Design patterns library
Version:    0.1.57
Release:    1
Group:      System/Libraries
License:    TBD
Source0:    %{name}-%{version}.tar.bz2
Requires(post):  /sbin/ldconfig
Requires(postun):  /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(heynoti)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(libssl)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  pkgconfig(libpcrecpp)
BuildRequires:  pkgconfig(icu-uc)

BuildRoot:  %{_tmppath}/%{name}-%{version}-build

%description
Design patterns library is a collection of useful C++ utilities to easily develop window applications.

%package efl
Summary:    Design patterns library - EFL based
Group:      System/Libraries

%description efl
Design patterns library is a collection of useful C++ utilities to easily develop window applications.

%package efl-devel
Summary:    Design patterns library - EFL based developer files
Group:      Development/Libraries
Requires:   %{name}-efl = %{version}-%{release}

%description efl-devel
Design patterns library is a collection of useful C++ utilities to easily develop window applications. The most important part of library is full MVC support. It also supports event-based architecture, adds wrappers for many packages and provides many basic C++ utilities as RAII objects, singletons, and many other.

%prep
%setup -q -n %{name}-%{version}

%build

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DDPL_LOG=ON \
	-DDISABLE_GTK=ON

# Call make instruction with smp support
make %{?jobs:-j%jobs}

# >> build post
# << build post
%install
rm -rf %{buildroot}
%make_install

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files efl
%defattr(-,root,root,-)
%{_libdir}/lib*-efl.so*
# << files

%files efl-devel
%defattr(-,root,root,-)
%{_includedir}/dpl-efl/*
%{_libdir}/pkgconfig/*-efl.pc

