dnl configure.ac
dnl
dnl Autoconf configuration for the MingW32 build of rivwebcapi.
dnl
dnl   (C) Copyright 2015-2018 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License version 2 as
dnl   published by the Free Software Foundation.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public
dnl   License along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl
dnl Use autoconf to process this into a configure script

AC_INIT([rivwebcapi],m4_esyscmd_s(cat ../../PACKAGE_VERSION))
AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

#
# Basic Compiler Checks
#
AC_PROG_CC
AC_PROG_LIBTOOL

#
# Determine Library API Versioning
#
# From http://www.gnu.org/software/libtool/manual.html#Updating-version-info
#
#   1. Start with version information of 0:0:0 for each libtool library.
#   2. Update the version information only immediately before a public
#      release of your software. More frequent updates are unnecessary,
#      and only guarantee that the current interface number gets larger
#      faster.
#   3. If the library source code has changed at all since the last update,
#      then increment 'REVISION' (c:r:a becomes c:r+1:a).
#   4. If any interfaces have been added, removed, or changed since the last
#      update, increment 'CURRENT', and set 'REVISION' to 0.
#   5. If any interfaces have been added since the last public release, then
#      increment 'AGE'.
#   6. If any interfaces have been removed since the last public release,
#      then set 'AGE' to 0.
RIVWEBCAPI_CURRENT=1
RIVWEBCAPI_REVISION=0
RIVWEBCAPI_AGE=1
RIVWEBCAPI_POINT=$(($RIVWEBCAPI_CURRENT - $RIVWEBCAPI_AGE))

AC_SUBST(INTERFACE_RIVWEBCAPI_CURRENT,$RIVWEBCAPI_CURRENT)
AC_SUBST(INTERFACE_RIVWEBCAPI_REVISION,$RIVWEBCAPI_REVISION)
AC_SUBST(INTERFACE_RIVWEBCAPI_AGE,$RIVWEBCAPI_AGE)
AC_SUBST(INTERFACE_RIVWEBCAPI_MAJOR,$RIVWEBCAPI_POINT)
AC_SUBST(INTERFACE_RIVWEBCAPI_MINOR,$RIVWEBCAPI_AGE)
AC_SUBST(INTERFACE_RIVWEBCAPI_POINT,$RIVWEBCAPI_REVISION)

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)
if test $ar_gcc_os = mingw32 ; then
  AC_DEFINE(MINGW32,"yes")
fi

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)

#
# Check for LibCurl
#
AC_CHECK_HEADER(curl/curl.h,[],[AC_MSG_ERROR([*** LibCurl not found ***])])

#
# Check for Expat
#
AC_CHECK_HEADER(expat.h,[],[AC_MSG_ERROR([*** Expat not found ***])])

AC_CONFIG_FILES([ \
    rivwebcapi/Makefile \
    tests/Makefile \
    rivwebcapi.pc \
    Makefile \
    ])
AC_OUTPUT()

AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
