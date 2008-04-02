Summary: Evolvotron Interactive Art Generator
Name: evolvotron
Version: 0.5.1
Release: 1
License: GPL
URL: http://sourceforge.net/projects/evolvotron
Packager: <wickedsmoke@users.sf.net>
Group: Applications/Graphics
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build
%if 0%{?fedora_version}
BuildRequires: gcc-c++ boost-devel python qt-devel 
%else
BuildRequires: gcc-c++ boost-devel python qt3-devel 
%endif

%description
Evolvotron is interactive "generative art" software to evolve images/textures/patterns through an iterative process of random mutation and user-selection driven evolution. 

%prep
%setup -q -n %{name}

%build
%if 0%{?fedora_version}
source /etc/profile.d/qt.sh
%endif
%if 0%{?mandriva_version}
export QTDIR=%{_prefix}/lib/qt3
export PATH=$QTDIR/bin:$PATH  
%endif
%if 0%{?suse_version}
export QTDIR=/usr/%{_lib}/qt3
export PATH=$QTDIR/bin:$PATH  
%endif
./configure
make

%install
# make install
mkdir -p $RPM_BUILD_ROOT/usr/bin
install -s -m 755 evolvotron/evolvotron $RPM_BUILD_ROOT/usr/bin

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/evolvotron
%doc README USAGE CHANGES

%changelog
* Fri Mar 28 2008 Karl Robillard <wickedsmoke@users.sf.net>
  - Initial package release.
