# acinclude.m4
#
# Local Autoconf macro definitions
#
#   (C) Copyright 2006-2025 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

# AR_GCC_TARGET()
#
# Get gcc(1)'s idea of the target architecture, distribution and os.
# The following variables are set:
#   $ar_gcc_arch = Target Architecture (i586, XF86_64, etc)
#   $ar_gcc_distro = Target Distribution (suse, slackware, etc)
#   $ar_gcc_os = Target Operating System (linux, solaris, etc)
#
AC_DEFUN([AR_GCC_TARGET],[AC_REQUIRE([AC_PROG_CC])]
  [
  AC_MSG_CHECKING(target architecture)
  ar_gcc_arch=$(./get_target.sh $CC $AWK arch)
  ar_gcc_distro=$(./get_target.sh $CC $AWK distro)
  ar_gcc_os=$(./get_target.sh $CC $AWK os)
  AC_MSG_RESULT([$ar_gcc_arch-$ar_gcc_distro-$ar_gcc_os])
  ]
)

# AR_GET_DISTRO()
#
# Try to determine the name and version of the distribution running
# on the host machine, based on entries in '/etc/'.
# The following variables are set:
#   $ar_distro_name = Distribution Name (SuSE, Debian, etc)
#   $ar_distro_version = Distribution Version (10.3, 3.1, etc)
#   $ar_distro_major = Distribution Version Major Number (10, 3, etc)
#   $ar_distro_minor = Distribution Version Minor Number (3, 1, etc)
#   $ar_distro_pretty_name = Full Distribution Name (Ubuntu 20.04.2 LTS, etc)
#   $ar_distro_id = All lowercase identifier (ubuntu, debian, centos, etc)
#   $ar_distro_id_like = Identifier(s) of similar distros (rhel fedora, etc)
#
AC_DEFUN([AR_GET_DISTRO],[]
  [
  AC_MSG_CHECKING(distribution)
  ar_distro_name=$(./get_distro.pl NAME)
  ar_distro_version=$(./get_distro.pl VERSION)
  ar_distro_major=$(./get_distro.pl MAJOR)
  ar_distro_minor=$(./get_distro.pl MINOR)
  ar_distro_pretty_name=$(./get_distro.pl PRETTY_NAME)
  ar_distro_id=$(./get_distro.pl ID)
  ar_distro_id_like=$(./get_distro.pl ID_LIKE)
  AC_MSG_RESULT([$ar_distro_pretty_name $ar_distro_version])
  ]
)

#
# AR_PYTHON_MODULE(modname,[ACTION-IF-DETECTED],[ACTION-IF-NOT-DETECTED])
#
# Check for the existence of the {modname} python3 module.
#
# On successful detection, ACTION-IF-DETECTED is executed if present. If
# the detection fails, then ACTION-IF-NOT-DETECTED is triggered.
#
# Based on code from the AX_PYTHON_MODULE() macro by Andrew Collier.
#
#   Copyright (c) 2008 Andrew Collier
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
#
AC_DEFUN([AR_PYTHON_MODULE],[
    PYTHON="/usr/bin/python3"
    PYTHON_NAME=`basename $PYTHON`
    AC_MSG_CHECKING($PYTHON_NAME module: $1)
    $PYTHON -c "import $1" 2>/dev/null
    if test $? -eq 0;
    then
        AC_MSG_RESULT(yes)
        $2
    else
        AC_MSG_RESULT(no)
        $3
    fi
])

# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_count_cpus.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_COUNT_CPUS([ACTION-IF-DETECTED],[ACTION-IF-NOT-DETECTED])
#
# DESCRIPTION
#
#   Attempt to count the number of logical processor cores (including
#   virtual and HT cores) currently available to use on the machine and
#   place detected value in CPU_COUNT variable.
#
#   On successful detection, ACTION-IF-DETECTED is executed if present. If
#   the detection fails, then ACTION-IF-NOT-DETECTED is triggered. The
#   default ACTION-IF-NOT-DETECTED is to set CPU_COUNT to 1.
#
# LICENSE
#
#   Copyright (c) 2014,2016 Karlson2k (Evgeny Grin) <k2k@narod.ru>
#   Copyright (c) 2012 Brian Aker <brian@tangent.org>
#   Copyright (c) 2008 Michael Paul Bailey <jinxidoru@byu.net>
#   Copyright (c) 2008 Christophe Tournayre <turn3r@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

##serial 22

  AC_DEFUN([AX_COUNT_CPUS],[dnl
      AC_REQUIRE([AC_CANONICAL_HOST])dnl
      AC_REQUIRE([AC_PROG_EGREP])dnl
      AC_MSG_CHECKING([the number of available CPUs])
      CPU_COUNT="0"

      # Try generic methods

      # 'getconf' is POSIX utility, but '_NPROCESSORS_ONLN' and
      # 'NPROCESSORS_ONLN' are platform-specific
      command -v getconf >/dev/null 2>&1 && \
        CPU_COUNT=`getconf _NPROCESSORS_ONLN 2>/dev/null || getconf NPROCESSORS_ONLN 2>/dev/null` || CPU_COUNT="0"
      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null || ! command -v nproc >/dev/null 2>&1]],[[: # empty]],[dnl
        # 'nproc' is part of GNU Coreutils and is widely available
        CPU_COUNT=`OMP_NUM_THREADS='' nproc 2>/dev/null` || CPU_COUNT=`nproc 2>/dev/null` || CPU_COUNT="0"
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null]],[[: # empty]],[dnl
        # Try platform-specific preferred methods
        AS_CASE([[$host_os]],dnl
          [[*linux*]],[[CPU_COUNT=`lscpu -p 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+,' -c` || CPU_COUNT="0"]],dnl
          [[*darwin*]],[[CPU_COUNT=`sysctl -n hw.logicalcpu 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[freebsd*]],[[command -v sysctl >/dev/null 2>&1 && CPU_COUNT=`sysctl -n kern.smp.cpus 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[netbsd*]], [[command -v sysctl >/dev/null 2>&1 && CPU_COUNT=`sysctl -n hw.ncpuonline 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[solaris*]],[[command -v psrinfo >/dev/null 2>&1 && CPU_COUNT=`psrinfo 2>/dev/null | $EGREP -e '^@<:@0-9@:>@.*on-line' -c 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[mingw*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]],dnl
          [[msys*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]],dnl
          [[cygwin*]],[[CPU_COUNT=`ls -qpU1 /proc/registry/HKEY_LOCAL_MACHINE/HARDWARE/DESCRIPTION/System/CentralProcessor/ 2>/dev/null | $EGREP -e '^@<:@0-9@:>@+/' -c` || CPU_COUNT="0"]]dnl
        )dnl
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null || ! command -v sysctl >/dev/null 2>&1]],[[: # empty]],[dnl
        # Try less preferred generic method
        # 'hw.ncpu' exist on many platforms, but not on GNU/Linux
        CPU_COUNT=`sysctl -n hw.ncpu 2>/dev/null` || CPU_COUNT="0"
      ])dnl

      AS_IF([[test "$CPU_COUNT" -gt "0" 2>/dev/null]],[[: # empty]],[dnl
      # Try platform-specific fallback methods
      # They can be less accurate and slower then preferred methods
        AS_CASE([[$host_os]],dnl
          [[*linux*]],[[CPU_COUNT=`$EGREP -e '^processor' -c /proc/cpuinfo 2>/dev/null` || CPU_COUNT="0"]],dnl
          [[*darwin*]],[[CPU_COUNT=`system_profiler SPHardwareDataType 2>/dev/null | $EGREP -i -e 'number of cores:'|cut -d : -f 2 -s|tr -d ' '` || CPU_COUNT="0"]],dnl
          [[freebsd*]],[[CPU_COUNT=`dmesg 2>/dev/null| $EGREP -e '^cpu@<:@0-9@:>@+: '|sort -u|$EGREP -e '^' -c` || CPU_COUNT="0"]],dnl
          [[netbsd*]], [[CPU_COUNT=`command -v cpuctl >/dev/null 2>&1 && cpuctl list 2>/dev/null| $EGREP -e '^@<:@0-9@:>@+ .* online ' -c` || \
                           CPU_COUNT=`dmesg 2>/dev/null| $EGREP -e '^cpu@<:@0-9@:>@+ at'|sort -u|$EGREP -e '^' -c` || CPU_COUNT="0"]],dnl
          [[solaris*]],[[command -v kstat >/dev/null 2>&1 && CPU_COUNT=`kstat -m cpu_info -s state -p 2>/dev/null | $EGREP -c -e 'on-line'` || \
                           CPU_COUNT=`kstat -m cpu_info 2>/dev/null | $EGREP -c -e 'module: cpu_info'` || CPU_COUNT="0"]],dnl
          [[mingw*]],[AS_IF([[CPU_COUNT=`reg query 'HKLM\\Hardware\\Description\\System\\CentralProcessor' 2>/dev/null | $EGREP -e '\\\\@<:@0-9@:>@+$' -c`]],dnl
                        [[: # empty]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]])],dnl
          [[msys*]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]],dnl
          [[cygwin*]],[[test "$NUMBER_OF_PROCESSORS" -gt "0" 2>/dev/null && CPU_COUNT="$NUMBER_OF_PROCESSORS"]]dnl
        )dnl
      ])dnl

      AS_IF([[test "x$CPU_COUNT" != "x0" && test "$CPU_COUNT" -gt 0 2>/dev/null]],[dnl
          AC_MSG_RESULT([[$CPU_COUNT]])
          m4_ifvaln([$1],[$1],)dnl
        ],[dnl
          m4_ifval([$2],[dnl
            AS_UNSET([[CPU_COUNT]])
            AC_MSG_RESULT([[unable to detect]])
            $2
          ], [dnl
            CPU_COUNT="1"
            AC_MSG_RESULT([[unable to detect (assuming 1)]])
          ])dnl
        ])dnl
      ])dnl

dnl Attempt to find a Qt6 installation.
dnl
dnl By default, this will look for a system-wide install, at
dnl '/usr/lib/<machine>/qt6/qt6.conf', with <machine> being gcc(1)'s
dnl idea of the machine's arch/os/distro --e.g. 'x86_64-linux-gnu'. This can
dnl overridden by defining a 'QT6_PATH' environmental variable containing
dnl the path to the configuration file of the desired Qt6 installation.
dnl
dnl If an installation is successfully detected, the following variables
dnl are set:
dnl
dnl prefix_CFLAGS - Flags to pass to the C++ compiler
dnl
dnl prefix_LIBS - Linker arguments
dnl
dnl prefix_MOC - Path to the appropriate moc(1)
dnl
AC_DEFUN([AQ_FIND_QT6],[AC_REQUIRE([AC_PROG_CXX])]
  [
  AC_MSG_CHECKING([for ]$1)

  dnl
  dnl Load the Qt6 configuration
  dnl
  AC_ARG_VAR(QT6_PATH,[path to the location of the Qt6 installation's configuration (default: "/usr/lib/<machine>/qt6/qt6.conf")])

  if test -n "$QT6_PATH" ; then
    config_path=$QT6_PATH
    if ! test -f $config_path ; then
      config_path=""
    fi
  else
    config_path=/usr/lib/$(gcc -dumpmachine)/qt6/qt6.conf
    if ! test -f $config_path ; then
      config_path=/usr/lib/$(gcc -dumpmachine)/qtchooser/qt6.conf
      if ! test -f $config_path ; then
        config_path=""
      fi
    fi
  fi
  if test $config_path ; then
      qt_var=$(grep ^Prefix\= $config_path)
      qt_Prefix=${qt_var:7}
      qt_var=$(grep ^ArchData\= $config_path)
      qt_ArchData=${qt_var:9}
      qt_var=$(grep ^Binaries\= $config_path)
      qt_Binaries=${qt_var:9}
      qt_var=$(grep ^Data\= $config_path)
      qt_Data=${qt_var:5}
      qt_var=$(grep ^Documentation\= $config_path)
      qt_Documentation=${qt_var:14}
      qt_var=$(grep ^Headers\= $config_path)
      qt_Headers=${qt_var:8}
      qt_var=$(grep ^HostBinaries\= $config_path)
      qt_HostBinaries=${qt_var:13}
      qt_var=$(grep ^HostData\= $config_path)
      qt_HostData=${qt_var:9}
      qt_var==$(grep ^HostLibraries\= $config_path)
      qt_HostLibraries=${qt_var:15}
      qt_var=$(grep ^Libraries\= $config_path)
      qt_Libraries=${qt_var:10}
      qt_var=$(grep ^LibraryExecutables\= $config_path)
      qt_LibraryExecutables=${qt_var:19}
      qt_var=$(grep ^Plugins\= $config_path)
      qt_Plugins=${qt_var:8}
      qt_var=$(grep ^Qml2Imports\= $config_path)
      qt_Qml2Imports=${qt_var:12}
      qt_var=$(grep ^Settings\= $config_path)
      qt_Settings=${qt_var:9}
      qt_var=$(grep ^Translations\= $config_path)
      qt_Translations=${qt_var:13}
      qt_libs="-L "$qt_Prefix$qt_Libraries
      qt_cppflags="-I"$qt_Prefix"/"$qt_Headers
      qt_libs="-L"$qt_Prefix"/"$qt_Libraries
      qt_moc=$qt_Prefix"/"$qt_Binaries"/moc"
      if test -n "$QT6_MOC" ; then
        qt_moc=$QT6_MOC
      fi
      if test -x $qt_moc ; then
        for module in $2
        do
        :
	  qt_cppflags=$qt_cppflags" -DQT_"${module^^}"_LIB -I"$qt_Prefix"/"$qt_Headers"/Qt"$module
	  qt_libs=$qt_libs" -lQt6"$module
        done
        AC_ARG_VAR([$1][_CFLAGS],[C++ compiler flags for $1, overriding autodetected values])
        AC_ARG_VAR([$1][_LIBS],[linker flags for $1, overriding autodetected values])
        AC_ARG_VAR([$1][_MOC],[path to the moc(1) binary, overriding autodetected values])
        AC_SUBST([$1][_CFLAGS],$qt_cppflags)
        AC_SUBST([$1][_LIBS],$qt_libs)
        AC_SUBST([$1][_MOC],$qt_moc)
        AC_MSG_RESULT([found])
        $3
      else
        AC_MSG_RESULT([moc(1) not found])
	$4
      fi
  else
      AC_MSG_RESULT([not found])
      $4
  fi
  ]
)
