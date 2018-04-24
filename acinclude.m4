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

dnl BNV_HAVE_QT [--with-Qt-dir=DIR] [--with-Qt-lib-dir=DIR] [with-Qt-lib=LIB]
dnl BNV_HAVE_QT [--with-Qt-include-dir=DIR] [--with-Qt-bin-dir=DIR] 
dnl             [--with-Qt-lib-dir=DIR] [--with-Qt-lib=LIB]
dnl
dnl THANKS! This code includes bug fixes and contributions made by:
dnl Tim McClarren,
dnl Dennis R. Weilert,
dnl Qingning Huo,
dnl Brian Mingus,
dnl Jens Hannemann,
dnl Pavel Roskin,
dnl Scott J. Bertin.

dnl ChangeLog
dnl 2006-03-12  * Hide output of ls and fix an m4 quoting problem (due to Scott J. Bertin).
dnl 2006-02-13  * Check compiler return value instead of parsing the error stream,
dnl               which detected warnings as false negatives (due to Jens Hannemann).
dnl 2006-02-02  * Spelling of "Success".
dnl             * Fixed unsave test for $bnv_qt_lib without quotes.
dnl             * Put dnl in front of all comments.
dnl             * Changed -l$bnv_qt_lib_dir into -L$bnv_qt_lib_dir (all due to Pavel Roskin).
dnl 2006-01-19  * Support for 64bit architectures.
dnl             * Updated documentation.
dnl 2006-01-18: * Fix "cat: bnv_qt_test.c: No such file or directory" (due to Jens Hannemann).
dnl             * Hide output of failing ls.
dnl 2006-01-11: * Check in /Developer on Mac OS X; Check in $QTDIR (due to Brian Mingus).

dnl Calls BNV_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([BNV_HAVE_QT],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING(for Qt)

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to $QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin. The library is in DIR/lib, unless
                          --with-Qt-lib-dir is also set.])
  AC_ARG_WITH([Qt-include-dir],
    [  --with-Qt-include-dir=DIR
                          Qt header files are in DIR])
  AC_ARG_WITH([Qt-bin-dir],
    [  --with-Qt-bin-dir=DIR   Qt utilities such as moc and uic are in DIR])
  AC_ARG_WITH([Qt-lib-dir],
    [  --with-Qt-lib-dir=DIR   The Qt library is in DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include-dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      bnv_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      bnv_qt_dir="$with_Qt_dir"
      bnv_qt_include_dir="$with_Qt_dir/include"
      bnv_qt_bin_dir="$with_Qt_dir/bin"
      bnv_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$bnv_qt_lib" = x; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(bnv_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        bnv_qt_dir=NO
        bnv_qt_include_dir=NO
        bnv_qt_lib_dir=NO
        if test x"$bnv_qt_lib" = x; then
          bnv_qt_lib=NO
        fi
        BNV_PATH_QT_DIRECT
        if test "$bnv_qt_dir" = NO ||
           test "$bnv_qt_include_dir" = NO ||
           test "$bnv_qt_lib_dir" = NO ||
           test "$bnv_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          bnv_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          bnv_cv_have_qt="have_qt=yes                  \
                       bnv_qt_dir=$bnv_qt_dir          \
               bnv_qt_include_dir=$bnv_qt_include_dir  \
                   bnv_qt_bin_dir=$bnv_qt_bin_dir      \
                      bnv_qt_LIBS=\"$bnv_qt_LIBS\""
        fi
      ])dnl
      eval "$bnv_cv_have_qt"
    fi # all $bnv_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    QT_CXXFLAGS="-I$bnv_qt_include_dir"
    if test x"$bnv_qt_lib" = xqt-mt; then
        QT_CXXFLAGS="$QT_CXXFLAGS -DQT_THREAD_SUPPORT"
    fi
    QT_DIR="$bnv_qt_dir"
    QT_LIBS="$bnv_qt_LIBS"
    # If bnv_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$bnv_qt_dir" != x; then
        QT_BIN="$bnv_qt_dir/bin"
        if test -x "$bnv_qt_dir/bin/uic"; then
          QT_UIC="$bnv_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$bnv_qt_dir/bin/moc"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$bnv_qt_bin_dir" != x; then
        QT_BIN="$bnv_qt_bin_dir"
        if test -x "$bnv_qt_bin_dir/uic"; then
          QT_UIC="$bnv_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$bnv_qt_bin_dir/moc"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
        QT_BIN="`dirname $QT_MOC`"
      fi
    fi
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt:
    QT_CXXFLAGS=$QT_CXXFLAGS
    QT_DIR=$QT_DIR
    QT_LIBS=$QT_LIBS
    QT_BIN=$QT_BIN
    QT_UIC=$QT_UIC
    QT_MOC=$QT_MOC])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_BIN=
    QT_UIC=
    QT_MOC=
    AC_MSG_RESULT($have_qt)
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_BIN)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)

  #### Being paranoid:
  if test x"$have_qt" = xyes; then
    AC_MSG_CHECKING(correct functioning of Qt installation)
    AC_CACHE_VAL(bnv_cv_qt_test_result,
    [
      cat > bnv_qt_test.h << EOF
#include <qobject.h>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > bnv_qt_main.$ac_ext << EOF
#include "bnv_qt_test.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      bnv_cv_qt_test_result="failure"
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
      AC_TRY_EVAL(bnv_try_1)
      if test x"$ac_status" != x0; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>/dev/null"
        AC_TRY_EVAL(bnv_try_2)
        if test x"$ac_status" != x0; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat moc_bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>/dev/null"
          AC_TRY_EVAL(bnv_try_3)
          if test x"$ac_status" != x0; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o $QT_LIBS $LIBS >/dev/null 2>/dev/null"
            AC_TRY_EVAL(bnv_try_4)
            if test x"$ac_status" != x0; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="success"
            fi
          fi
        fi
      fi
    ])dnl AC_CACHE_VAL bnv_cv_qt_test_result
    AC_MSG_RESULT([$bnv_cv_qt_test_result]);
    if test x"$bnv_cv_qt_test_result" = "xfailure"; then
      AC_MSG_ERROR([Failed to find matching components of a complete
                  Qt installation. Try using more options,
                  see ./configure --help.])
    fi

    rm -f bnv_qt_test.h moc_bnv_qt_test.$ac_ext moc_bnv_qt_test.o \
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main
  fi
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
AC_DEFUN([BNV_PATH_QT_DIRECT],
[
  ## Binary utilities ##
  if test x"$with_Qt_bin_dir" != x; then
    bnv_qt_bin_dir=$with_Qt_bin_dir
  fi
  ## Look for header files ##
  if test x"$with_Qt_include_dir" != x; then
    bnv_qt_include_dir="$with_Qt_include_dir"
  else
    # The following header file is expected to define QT_VERSION.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    bnv_include_path_list="
      /usr/include
      `ls -dr ${QTDIR}/include 2>/dev/null`
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
      `ls -dr /Developer/qt*/include 2>/dev/null`
    "
    for bnv_dir in $bnv_include_path_list; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
        bnv_dirs="$bnv_dirs $bnv_dir"
      fi
    done
    # Now look for the newest in this list
    bnv_prev_ver=0
    for bnv_dir in $bnv_dirs; do
      bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
      if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
        bnv_qt_include_dir=$bnv_dir
        bnv_prev_ver=$bnv_this_ver
      fi
    done
  fi dnl Found header files.

  # Are these headers located in a traditional Trolltech installation?
  # That would be $bnv_qt_include_dir stripped from its last element:
  bnv_possible_qt_dir=`dirname $bnv_qt_include_dir`
  if (test -x $bnv_possible_qt_dir/bin/moc) &&
     ((ls $bnv_possible_qt_dir/lib/libqt* > /dev/null 2>/dev/null) ||
      (ls $bnv_possible_qt_dir/lib64/libqt* > /dev/null 2>/dev/null)); then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
    else
      if (test -d $bnv_qt_dir/lib64); then
        bnv_qt_lib_dir="$bnv_qt_dir/lib64"
      else
        bnv_qt_lib_dir="$bnv_qt_dir/lib"
      fi
    fi
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
  else
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    bnv_qt_dir=
    ## Look for Qt library ##
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
      # Only look for lib if the user did not supply it already
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
    else
      # Normally, when there is no traditional Trolltech installation,
      # the library is installed in a place where the linker finds it
      # automatically.
      # If the user did not define the library name, try with qt
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib=qt
      fi
      qt_direct_test_header=qapplication.h
      qt_direct_test_main="
        int argc;
        char ** argv;
        QApplication app(argc,argv);
      "
      # See if we find the library without any special options.
      # Don't add top $LIBS permanently yet
      bnv_save_LIBS="$LIBS"
      LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
      bnv_qt_LIBS="$LIBS"
      bnv_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="-I$bnv_qt_include_dir"
      AC_TRY_LINK([#include <$qt_direct_test_header>],
        $qt_direct_test_main,
      [
        # Success.
        # We can link with no special library directory.
        bnv_qt_lib_dir=
      ], [
        # That did not work. Try the multi-threaded version
        echo "Non-critical error, please neglect the above." >&AC_FD_CC
        bnv_qt_lib=qt-mt
        LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
        AC_TRY_LINK([#include <$qt_direct_test_header>],
          $qt_direct_test_main,
        [
          # Success.
          # We can link with no special library directory.
          bnv_qt_lib_dir=
        ], [
          # That did not work. Try the OpenGL version
          echo "Non-critical error, please neglect the above." >&AC_FD_CC
          bnv_qt_lib=qt-gl
          LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
          AC_TRY_LINK([#include <$qt_direct_test_header>],
            $qt_direct_test_main,
          [
            # Success.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
              /usr/lib64
              /usr/lib
              /usr/local/lib64
              /usr/local/lib
              /opt/lib64
              /opt/lib
              `ls -dr /usr/lib64/qt* 2>/dev/null`
              `ls -dr /usr/lib64/qt*/lib64 2>/dev/null`
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt* >/dev/null 2>/dev/null; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* | sed -n 1p |
                            sed s@$bnv_dir/lib@@ | sed s/[[.]].*//`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXt $X_EXTRA_LIBS"
            AC_TRY_LINK([#include <$qt_direct_test_header>],
              $qt_direct_test_main,
            [
              # Success.
              # We can link with no special library directory.
              bnv_qt_lib_dir=
            ], [
              # Leave bnv_qt_lib_dir defined
	      bnv_qt_lib_dir=bnv_qt_lib_dir
            ])
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-L$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"
    fi dnl $with_Qt_lib_dir was not given
  fi dnl Done setting up for non-traditional Trolltech installation
])
