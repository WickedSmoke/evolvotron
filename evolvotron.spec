Summary: Evolvotron Interactive Art Generator
Name: evolvotron
Version: 0.8.2
Release: %autorelease
License: GPL-2.0-or-later
URL: http://sourceforge.net/projects/evolvotron
Source: https://github.com/WickedSmoke/evolvotron/archive/refs/tags/v%{version}.tar.gz
BuildRequires: gcc-c++ boost-devel qt6-qtbase-devel

%global debug_package %{nil}

%description
Evolvotron is interactive "generative art" software to evolve images/textures/patterns through an iterative process of random mutation and user-selection driven evolution. 

%prep
%setup -q -n %{name}-%{version}

%build
qmake6 VERSION_NUMBER=%{version} main.pro
make -j 4

%install
mkdir -p $RPM_BUILD_ROOT/usr/bin $RPM_BUILD_ROOT/usr/share/man/man1
install -m 755 evolvotron/evolvotron $RPM_BUILD_ROOT/usr/bin
install -m 755 evolvotron_mutate/evolvotron_mutate $RPM_BUILD_ROOT/usr/bin
install -m 755 evolvotron_render/evolvotron_render $RPM_BUILD_ROOT/usr/bin
install -m 644 man/man1/evolvotron.1 $RPM_BUILD_ROOT/usr/share/man/man1
install -m 644 man/man1/evolvotron_mutate.1 $RPM_BUILD_ROOT/usr/share/man/man1
install -m 644 man/man1/evolvotron_render.1 $RPM_BUILD_ROOT/usr/share/man/man1
install -D -m 644 dist/icon-48.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/48x48/apps/evolvotron.png
install -D -m 644 dist/icon-128.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/128x128/apps/evolvotron.png
install -D -m 644 dist/evolvotron.desktop $RPM_BUILD_ROOT/usr/share/applications/evolvotron.desktop

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/evolvotron
%{_bindir}/evolvotron_mutate
%{_bindir}/evolvotron_render
%{_mandir}/man1/evolvotron.1*
%{_mandir}/man1/evolvotron_mutate.1*
%{_mandir}/man1/evolvotron_render.1*
%{_datadir}/icons/hicolor/48x48/apps/evolvotron.png
%{_datadir}/icons/hicolor/128x128/apps/evolvotron.png
%{_datadir}/applications/evolvotron.desktop

%changelog
* Tue Apr 23 2024 Karl Robillard <wickedsmoke@users.sf.net> 0.8.2-1
  - Update to build with Qt 6.
* Sun Dec 18 2022 Karl Robillard <wickedsmoke@users.sf.net> 0.7.2-1
  - Update for v0.7.2
* Fri Mar 28 2008 Karl Robillard <wickedsmoke@users.sf.net>
  - Initial package release.
