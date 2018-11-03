#
# spec file for package goat
#

Name:           goat
Version:        
Release:        0
Summary:        Database frontend
License:        GPL-3.0
Group:          Development/Database
Url:            https://github.com/mispp/goat
Source0:        %{name}-%{version}.tar.bz2
BuildRequires:  libQt5Concurrent-devel
BuildRequires:  libQt5Core-devel
BuildRequires:  libQt5Gui-devel
BuildRequires:  libQt5Sql-devel
BuildRequires:  libQt5Widgets-devel
BuildRequires:  postgresql
BuildRequires:  libQt5Sql5-postgresql
BuildRequires:  libqt5-qtbase-common-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Tool for querying databases

%prep
%setup -q

%build
qmake-qt5
make

%install
make install INSTALL_ROOT=%{buildroot} %{?_smp_mflags}

%files
%doc README.md LICENSE
%{_bindir}/goat

%changelog
