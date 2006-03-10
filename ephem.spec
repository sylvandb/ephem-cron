%define name	x10ephem
%define version	0.50
%define release 1

Summary:	x10ephem - Solar Ephemeris Data for lighting control
Name:		%{name}
Version:	%{version}
Release:	%{release}
Group:		Applications/Console
Copyright:	GPL
URL:		http://www.drwren.com/ephem/
Vendor:		Chris Wren <c@drwren.com>
Source:		%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-root


%description
This code computes sunrise and sunset times. There is a library and an
assortment of utilities. The most useful utility is x10events. This
program parses crontab files and updates the time fields to track sun
events given instructions in the comments.  Combined with an X10
interface like HEYU or X10 this allows the scheduling lighting events
that follow the changing seasons and DST automatically.  This
application does not depend on X10, it can be used for any crontab
entry you like.

%prep
%setup

%build
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root)
%doc CHANGELOG
%doc COPYING
%doc FAQ
%doc BUGS
%doc README
%doc sample.cron
/usr/bin/today
/usr/bin/year
/usr/bin/x10events
/usr/include/x10ephem.h
/usr/lib/libx10ephem-%{version}.a

%clean 
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%post

%postun

