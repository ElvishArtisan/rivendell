dnl acinclude.m4
dnl
dnl Local Autoconf macro definitions for LibRHpi
dnl
dnl   (C) Copyright 2006,2016 Fred Gleason <fredg@paravelsystems.com>
dnl
dnl   BNV_HAVE_QT Macro Copyright (C) 2001, 2002, 2003, 2005, 2006 
dnl   Bastiaan Veelo
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

dnl AR_GCC_TARGET()
dnl
dnl Get gcc(1)'s idea of the target architecture, distribution and os.
dnl The following variables are set:
dnl   $ar_gcc_arch = Target Architecture (i586, XF86_64, etc)
dnl   $ar_gcc_distro = Target Distribution (suse, slackware, etc)
dnl   $ar_gcc_os = Target Operating System (linux, solaris, etc)
dnl
AC_DEFUN([AR_GCC_TARGET],[AC_REQUIRE([AC_PROG_CC])]
  [
  AC_MSG_CHECKING(target architecture)
  ar_gcc_arch=$(./get_target.sh $CC $AWK arch)
  ar_gcc_distro=$(./get_target.sh $CC $AWK distro)
  ar_gcc_os=$(./get_target.sh $CC $AWK os)
  AC_MSG_RESULT([$ar_gcc_arch-$ar_gcc_distro-$ar_gcc_os])
  ]
)

dnl AR_GET_DISTRO()
dnl
dnl Try to determine the name and version of the distribution running
dnl on the host machine, based on entries in '/etc/'.
dnl The following variables are set:
dnl   $ar_distro_name = Distribution Name (SuSE, Debian, etc)
dnl   $ar_distro_version = Distribution Version (10.3, 3.1, etc)
dnl   $ar_distro_major = Distribution Version Major Number (10, 3, etc)
dnl   $ar_distro_minor = Distribution Version Minor Number (3, 1, etc)
dnl
AC_DEFUN([AR_GET_DISTRO],[]
  [
  AC_MSG_CHECKING(distribution)
  ar_distro_name=$(./get_distro.pl NAME)
  ar_distro_version=$(./get_distro.pl VERSION)
  ar_distro_major=$(./get_distro.pl MAJOR)
  ar_distro_minor=$(./get_distro.pl MINOR)
  AC_MSG_RESULT([$ar_distro_name $ar_distro_version])
  ]
)
