@ECHO OFF

REM build_win32.bat
REM
REM Build Rivendell for Win32.
REM
REM (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
REM
REM   This program is free software; you can redistribute it and/or modify
REM   it under the terms of the GNU General Public License as
REM   published by the Free Software Foundation; either version 2 of
REM   the License, or (at your option) any later version.
REM
REM   This program is distributed in the hope that it will be useful,
REM   but WITHOUT ANY WARRANTY; without even the implied warranty of
REM   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM   GNU General Public License for more details.
REM
REM   You should have received a copy of the GNU General Public
REM   License along with this program; if not, write to the Free Software
REM   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

REM
REM Stupid DOS trick to get the package version
REM
copy helpers\win32_frag1.txt + package_version + helpers\win32_frag2.txt helpers\win32_version.bat
call helpers\win32_version.bat
del helpers\win32_version.bat

REM
REM Build It
REM
qmake -o Makefile rivendell.pro
nmake


REM End of build_win32.bat
