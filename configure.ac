dnl configure.ac
dnl
dnl Autoconf configuration for Rivendell.
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2002-2007,2014 Fred Gleason <fredg@paravelsystems.com>
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

AC_INIT([rivendell],m4_esyscmd(cat PACKAGE_VERSION))
AM_INIT_AUTOMAKE
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

#
# Some Fully Resolved Install Paths
# (for the packaging system and friends)
#
if test ${prefix} = "NONE" ; then
  PREFIX=/usr/local
  AC_SUBST(LOCAL_PREFIX,/usr/local)
else
  PREFIX=${prefix}
  AC_SUBST(LOCAL_PREFIX,${prefix})
fi

#
# Basic Compiler Checks
#
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_LANG(C++)

# to avoid rpath usage :
# http://wiki.debian.net/index.cgi?RpathIssue
case ${host} in
  *-pc-linux-gnu)
    AC_MSG_RESULT([Fixing libtool for -rpath problems.])
    sed < libtool > libtool-2 \
    's/^hardcode_libdir_flag_spec.*$/hardcode_libdir_flag_spec=" -D__LIBTOOL_IS_A_FOOL__ "/'
    mv libtool-2 libtool
    chmod 755 libtool
  ;;
esac


#
# Option Switches
#
AC_ARG_ENABLE(hpi,[  --disable-hpi           disable AudioScience HPI sound support],
		      [HPI_DISABLED=yes],[])
AC_ARG_ENABLE(jack,[  --disable-jack          disable JACK sound support],
		      [JACK_DISABLED=yes],[])
AC_ARG_ENABLE(alsa,[  --disable-alsa          disable direct ALSA sound support],
		      [ALSA_DISABLED=yes],[])
AC_ARG_ENABLE(gpio,[  --disable-gpio          disable General Purpose Input/Output GPIO support],
		      [GPIO_DISABLED=yes],[])
AC_ARG_ENABLE(pam,[  --disable-pam           disable Rivendell PAM pam_rd.so support],
		      [PAM_DISABLED=yes],[])
AC_ARG_ENABLE(mad,[  --disable-mad           disable MPEG decode support],
		      [MAD_DISABLED=yes],[])
AC_ARG_ENABLE(twolame,[  --disable-twolame       disable MPEG Layer 2 encode support],
		      [TWOLAME_DISABLED=yes],[])
AC_ARG_ENABLE(lame,[  --disable-lame          disable MPEG Layer 3 encode support],
		      [LAME_DISABLED=yes],[])
AC_ARG_ENABLE(flac,[  --disable-flac          disable FLAC encode/decode support],
		      [FLAC_DISABLED=yes],[])

#
# Check for Qt
#
BNV_HAVE_QT
if test $have_qt = "no" ; then
  AC_MSG_ERROR([*** Qt toolkit not found, unable to continue ***])
fi

#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)
if test $ar_gcc_arch = x86_64 ; then
  LIB_PATH=$PREFIX/lib64
  AC_SUBST(RD_LIB_PATH,lib64)
else
  LIB_PATH=$PREFIX/lib
  AC_SUBST(RD_LIB_PATH,lib)
fi

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)
if test $ar_gcc_distro = suse ; then
  AC_SUBST(APACHE_PKG,"apache2")
  AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
  AC_SUBST(CONSOLEHELPER_RDALSACONFIG,"")
  AC_SUBST(USERMODE_PKG,"")
  AC_SUBST(QT3_MYSQL_PKG,"qt3-mysql")
  AC_SUBST(QT3_DEVEL_PKG,"qt3-devel")
  AC_SUBST(MYSQL_PKG,"mysql")
else
  AC_SUBST(APACHE_PKG,"httpd")
  AC_SUBST(APACHE_CONFIG_DIR,"/etc/httpd/conf.d")
  AC_SUBST(CONSOLEHELPER_RDALSACONFIG,"/usr/bin/rdalsaconfig-root")
  AC_SUBST(USERMODE_PKG,"usermode")
  if test $ar_distro_major -ge 7 ; then
    AC_SUBST(QT3_MYSQL_PKG,"qt3-MySQL")
    AC_SUBST(QT3_DEVEL_PKG,"qt3-devel")
    AC_SUBST(MYSQL_PKG,"mariadb")
  else
    if test $ar_distro_major -ge 6 ; then
      AC_SUBST(QT3_MYSQL_PKG,"qt-MySQL")
      AC_SUBST(QT3_DEVEL_PKG,"qt3-devel")
      AC_SUBST(MYSQL_PKG,"mysqld")
    else
      AC_SUBST(QT3_MYSQL_PKG,"qt-MySQL")
      AC_SUBST(QT3_DEVEL_PKG,"qt-devel")
      AC_SUBST(MYSQL_PKG,"mysqld")
    fi
  fi
fi

#
# Check for OggVorbis
#
AC_CHECK_HEADER(vorbis/vorbisfile.h,[VORBIS_FOUND=yes],[])
if test $VORBIS_FOUND = yes ; then
  AC_DEFINE(HAVE_VORBIS)
  AC_SUBST(LIBVORBIS,"-lvorbisfile -lvorbisenc")
else
  AC_SUBST(LIBVORBIS,"")
fi

#
# Check for Secret Rabbit Code
#
AC_CHECK_HEADER(samplerate.h,[SRC_FOUND=yes],[AC_MSG_ERROR([*** LibSampleRate not found ***])])
AC_CHECK_FUNC(src_short_to_float_array,[SRC_CONV_FOUND=yes],[])

#
# Check for Libsndfile
#
AC_CHECK_HEADER(sndfile.h,[],[AC_MSG_ERROR([*** Libsndfile not found ***])])

#
# Check for LibParanoia
#
AC_CHECK_HEADER(cdda_interface.h,[PARANOIA_FOUND=yes],[])
if test $PARANOIA_FOUND ; then
  AC_SUBST(PARANOIA_INCLUDES,"<cdda_interface.h>")
else
  # RedHat/CentOS puts this in a subdirectory
  AC_CHECK_HEADER(cdda/cdda_interface.h,[PARANOIA_FOUND=yes],[])
  if test $PARANOIA_FOUND ; then
    AC_SUBST(PARANOIA_INCLUDES,"<cdda/cdda_interface.h>")
  else
    AC_MSG_ERROR([*** LibParanoia not found ***])
  fi
fi

#
# Check for Id3Lib
#
AC_CHECK_HEADER(id3/tag.h,[],[AC_MSG_ERROR([*** Id3Lib not found ***])])

#
# Check for LibCurl
#
AC_CHECK_HEADER(curl/curl.h,[],[AC_MSG_ERROR([*** LibCurl not found ***])])

#
# Check for PAM
#
AC_CHECK_HEADER(security/pam_appl.h,[],[AC_MSG_ERROR([*** PAM not found ***])])

#
# Check for SoundTouch
#
AC_CHECK_HEADER(soundtouch/SoundTouch.h,[],[AC_MSG_ERROR([*** SoundTouch not found ***])])

#
# Check for FLAC
#
if test -z $FLAC_DISABLED ; then
  AC_CHECK_HEADER(FLAC++/encoder.h,[FLAC_FOUND=yes],[])
  if test $FLAC_FOUND ; then
    FLAC_LIBS="-lFLAC -lFLAC++"
    AC_DEFINE(HAVE_FLAC)
  fi
  AC_CHECK_LIB(FLAC,FLAC__metadata_get_tags,[FLAC_METADATA_FOUND=yes],[])
fi

#
# Set Hard Library Dependencies
#
AC_SUBST(LIB_RDLIBS,"-lm -lpthread -lqui -lrd -lcurl -lid3 $FLAC_LIBS -lsndfile -lsamplerate -lcdda_interface -lcdda_paranoia -lcrypt -ldl -lpam -lSoundTouch")

#
# Setup MPEG Dependencies
#
AC_CHECK_HEADER(mad.h,[MAD_FOUND=yes],[])
AC_CHECK_HEADER(twolame.h,[TWOLAME_FOUND=yes],[])
AC_CHECK_HEADER(lame/lame.h,[LAME_FOUND=yes],[])
if test $MAD_FOUND ; then
  if test -z $MAD_DISABLED ; then
    AC_DEFINE(HAVE_MAD)
    USING_MAD=yes
  fi
fi
if test $TWOLAME_FOUND ; then
  if test -z $TWOLAME_DISABLED ; then
    AC_DEFINE(HAVE_TWOLAME)
    USING_TWOLAME=yes
  fi
fi
if test $LAME_FOUND ; then
  if test -z $LAME_DISABLED ; then
    AC_DEFINE(HAVE_LAME)
    USING_LAME=yes
  fi
fi

#
# Setup Sound API Dependencies
#
AC_CHECK_HEADER(asihpi/hpi.h,[HPI_FOUND=yes],[])
AC_CHECK_HEADER(jack/jack.h,[JACK_FOUND=yes],[])
AC_CHECK_HEADER(alsa/asoundlib.h,[ALSA_FOUND=yes],[])
if test $HPI_FOUND ; then
  if test -z $HPI_DISABLED ; then
    USING_HPI=yes
    AC_DEFINE(HPI,yes)
    AC_SUBST(LIBHPI,"-lrdhpi -lhpi")
    AC_SUBST(HPI_FILE1,$LIB_PATH/librdhpi-$VERSION.so)
    AC_SUBST(HPI_FILE2,$LIB_PATH/librdhpi.a)
    AC_SUBST(HPI_FILE3,$LIB_PATH/librdhpi.la)
    AC_SUBST(HPI_FILE4,$LIB_PATH/librdhpi.so)
    AC_SUBST(HPI_FILE5,$PREFIX/bin/rdhpiinfo)
    if test $ar_gcc_distro = suse ; then
      AC_SUBST(HPI_FILE6,"")
    else
      AC_SUBST(HPI_FILE6,$PREFIX/bin/rdhpiinfo-root)
    fi
    AC_SUBST(HPI_FILE7,$PREFIX/share/rivendell/rdhpi_es.qm)
    AC_SUBST(HPI_FILE8,$PREFIX/share/rivendell/rdhpi_cs.qm)
    AC_SUBST(HPI_FILE9,$PREFIX/share/rivendell/rdhpi_de.qm)
    AC_SUBST(HPI_FILE10,$PREFIX/share/rivendell/rdhpi_nn.qm)
    AC_SUBST(HPI_FILE11,$PREFIX/share/rivendell/rdhpi_nb.qm)
    AC_SUBST(HPI_FILE12,$PREFIX/share/rivendell/rdhpi_pt_BR.qm)
    AC_SUBST(HPI_FILE13,$PREFIX/share/applications/rivendell-rdhpiinfo-root.desktop)
    AC_SUBST(HPI_FILE14,/etc/pam.d/rdhpiinfo-root)
    AC_SUBST(HPI_FILE15,/etc/security/console.apps/rdhpiinfo-root)
  else 
    AC_SUBST(LIBHPI,"")
    AC_SUBST(HPI_FILE1,"")
    AC_SUBST(HPI_FILE2,"")
    AC_SUBST(HPI_FILE3,"")
    AC_SUBST(HPI_FILE4,"")
    AC_SUBST(HPI_FILE5,"")
    AC_SUBST(HPI_FILE6,"")
    AC_SUBST(HPI_FILE7,"")
    AC_SUBST(HPI_FILE8,"")
    AC_SUBST(HPI_FILE9,"")
    AC_SUBST(HPI_FILE10,"")
    AC_SUBST(HPI_FILE11,"")
    AC_SUBST(HPI_FILE12,"")
    AC_SUBST(HPI_FILE13,"")
    AC_SUBST(HPI_FILE14,"")
    AC_SUBST(HPI_FILE15,"")
  fi
else
  AC_SUBST(LIBHPI,"")
  AC_SUBST(HPI_FILE1,"")
  AC_SUBST(HPI_FILE2,"")
  AC_SUBST(HPI_FILE3,"")
  AC_SUBST(HPI_FILE4,"")
  AC_SUBST(HPI_FILE5,"")
  AC_SUBST(HPI_FILE6,"")
  AC_SUBST(HPI_FILE7,"")
  AC_SUBST(HPI_FILE8,"")
  AC_SUBST(HPI_FILE9,"")
  AC_SUBST(HPI_FILE10,"")
  AC_SUBST(HPI_FILE11,"")
  AC_SUBST(HPI_FILE12,"")
  AC_SUBST(HPI_FILE13,"")
  AC_SUBST(HPI_FILE14,"")
  AC_SUBST(HPI_FILE15,"")
fi
AM_CONDITIONAL([HPI_RD_AM], [test "$USING_HPI" = yes])

if test $JACK_FOUND ; then
  if test -z $JACK_DISABLED ; then
    if test -z $SRC_FOUND ; then
      AC_MSG_ERROR([*** libsamplerate not found, but is needed for JACK support ***])
    fi
    AC_DEFINE(JACK,yes)
    AC_SUBST(LIBJACK,-ljack)
    SRC_NEEDED=yes
    USING_JACK=yes
  else 
    AC_SUBST(LIBJACK,"")
  fi
else 
  AC_SUBST(LIBJACK,"")
fi

if test $ALSA_FOUND ; then
  if test -z $ALSA_DISABLED ; then
    if test -z $SRC_FOUND ; then
      AC_MSG_ERROR([*** libsamplerate not found, but is needed for ALSA support ***])
    fi
    AC_DEFINE(ALSA,yes)
    AC_SUBST(LIBALSA,-lasound)
    SRC_NEEDED=yes
    USING_ALSA=yes
  else 
    AC_SUBST(LIBALSA,"")
  fi
else 
  AC_SUBST(LIBALSA,"")
fi
AM_CONDITIONAL([ALSA_RD_AM], [test "$USING_ALSA" = yes])

if test -z $SRC_NEEDED ; then
  AC_SUBST(LIBSRC,"")
else
  AC_DEFINE(SRC,yes)
  AC_SUBST(LIBSRC,-lsamplerate)
fi
if test -z $SRC_CONV_FOUND ; then
  AC_MSG_NOTICE([Using local format converters])
else
  AC_MSG_NOTICE([Using SRC format converters])
  AC_DEFINE(HAVE_SRC_CONV,yes)
fi
if test -z $FLAC_METADATA_FOUND ; then
  AC_MSG_NOTICE([Ogg Metadata tags will not be supported])
else
  AC_DEFINE(HAVE_FLAC_METADATA,yes)
fi

#
# pam_rd hooks
#
if test -z $PAM_DISABLED ; then
  AC_CHECK_HEADERS([security/pam_modules.h security/_pam_macros.h],[PAM_FOUND=yes],[PAM_FOUND=no;break])
  if test $PAM_FOUND = yes ; then
    USING_PAM=yes
  fi
fi
AM_CONDITIONAL([PAM_RD_AM], [test "$USING_PAM" = yes])

#
# Distro-Specific Stuff
#
if test -f /etc/gentoo-release ; then
  rm -f rivendell
  ln -s rivendell-gentoo rivendell
  AC_MSG_NOTICE([Configured to install Gentoo-specific init script])
else
  rm -f rivendell
  ln -s rivendell-suse rivendell
  AC_MSG_NOTICE([Configured to install generic init script])
fi

#
# Configure RPM Build
#
AC_MSG_CHECKING([for $WIN32_SETUPS/rivendell-$VERSION-$RPM_RELEASE.exe])
if test -f $WIN32_SETUPS/rivendell-$VERSION-$RPM_RELEASE.exe ; then
  AC_MSG_RESULT([yes])
  AC_SUBST(WIN32_SOURCE,"$WIN32_SETUPS/rivendell-$VERSION-$RPM_RELEASE.exe")
  AC_SUBST(WIN32_PATH,"/var/win32/rivendell-$VERSION-$RPM_RELEASE.exe")
else
  AC_MSG_RESULT([no])
  AC_SUBST(WIN32_SOURCE,"")
  AC_SUBST(WIN32_PATH,"")
fi
AC_CHECK_PROG(RPMBUILD_FOUND,rpmbuild,[yes],[])
if test -z $RPMBUILD_FOUND ; then
  AC_SUBST(RPMBUILD,rpm)
else
  AC_SUBST(RPMBUILD,rpmbuild)
fi
if test -d /usr/src/redhat ; then
dnl  AC_SUBST(RPM_ROOT,/usr/src/redhat)
  AC_SUBST(VENDOR,redhat)
else
dnl  AC_SUBST(RPM_ROOT,/usr/src/packages)
  AC_SUBST(VENDOR,suse)
fi 
AC_SUBST(RPM_DATESTAMP,`date +%a\ %b\ %d\ %Y`)

AC_CONFIG_FILES([rivendell.spec \
    Makefile \
    make_slack \
    slack-desc \
    rivendell-suse \
    rdrepld-suse \
    conf/rd-bin.conf \
    icons/Makefile \
    helpers/Makefile \
    lib/rdpaths.h \
    lib/Makefile \
    rdhpi/Makefile \
    rlm/Makefile \
    cae/Makefile \
    utils/Makefile \
    utils/rdalsaconfig/Makefile \
    utils/rdcheckcuts/Makefile \
    utils/rdchunk/Makefile \
    utils/rdcollect/Makefile \
    utils/rddbcheck/Makefile \
    utils/rddelete/Makefile \
    utils/rddgimport/Makefile \
    utils/rddiscimport/Makefile \
    utils/rdgen/Makefile \
    utils/rdhpiinfo/Makefile \
    utils/rdgpimon/Makefile \
    utils/rdimport/Makefile \
    utils/rdmaint/Makefile \
    utils/rdmarkerset/Makefile \
    utils/rdpopup/Makefile \
    utils/rdpurgecasts/Makefile \
    utils/rdsoftkeys/Makefile \
    utils/rmlsend/Makefile \
    utils/sas_shim/Makefile \
    web/Makefile \
    web/rdfeed/Makefile \
    web/rdcastmanager/Makefile \
    web/rdxport/Makefile \
    web/tests/Makefile \
    conf/Makefile \
    docs/Makefile \
    docs/tables/Makefile \
    docs/examples/Makefile \
    debian/Makefile \
    debian/patches/Makefile \
    xdg/Makefile \
    rdadmin/Makefile \
    scripts/Makefile \
    ripcd/Makefile \
    rdlogin/Makefile \
    rdlibrary/Makefile \
    rdcatch/Makefile \
    rdcatchd/Makefile \
    rdlogedit/Makefile \
    rdlogmanager/Makefile \
    rdairplay/Makefile \
    rdpanel/Makefile \
    rdcartslots/Makefile \
    rdcastmanager/Makefile \
    rdselect/Makefile \
    rdmonitor/Makefile \
    rdrepld/Makefile \
    tests/Makefile \
    importers/Makefile \
    ios/Makefile \
    ios/rmlsend/Makefile \
    ios/rmlsend/Classes/Makefile \
    ios/rmlsend/RMLSend.xcodeproj/Makefile \
    pam_rd/Makefile ])
AC_OUTPUT()
chmod 755 make_slack
chmod 755 rivendell-suse
chmod 755 rdrepld-suse

#
# Configuration Results
#
AC_MSG_NOTICE()
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE("|       *** RIVENDELL CONFIGURATION SUMMARY ***       |")
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE("| Configured Audio Drivers:                           |")
if test -z $USING_ALSA ; then
AC_MSG_NOTICE("|  Advanced Linux Sound Architecture (ALSA) ... No    |")
else
AC_MSG_NOTICE("|  Advanced Linux Sound Architecture (ALSA) ... Yes   |")
fi
if test -z $USING_HPI ; then
AC_MSG_NOTICE("|                          AudioScience HPI ... No    |")
else
AC_MSG_NOTICE("|                          AudioScience HPI ... Yes   |")
fi
if test -z $USING_JACK ; then
AC_MSG_NOTICE("|                 JACK Audio Connection Kit ... No    |")
else
AC_MSG_NOTICE("|                 JACK Audio Connection Kit ... Yes   |")
fi
AC_MSG_NOTICE("|                                                     |")
AC_MSG_NOTICE("| Audio Codecs:                                       |")
if test -z $FLAC_FOUND ; then
AC_MSG_NOTICE("|            FLAC Encoding/Decoding Support ... No    |")
else
AC_MSG_NOTICE("|            FLAC Encoding/Decoding Support ... Yes   |")
fi
if test -z $USING_MAD ; then
AC_MSG_NOTICE("|                     MPEG Decoding Support ... No    |")
else
AC_MSG_NOTICE("|                     MPEG Decoding Support ... Yes   |")
fi
if test -z $USING_TWOLAME ; then
AC_MSG_NOTICE("|             MPEG Layer 2 Encoding Support ... No    |")
else
AC_MSG_NOTICE("|             MPEG Layer 2 Encoding Support ... Yes   |")
fi
if test -z $USING_LAME ; then
AC_MSG_NOTICE("|             MPEG Layer 3 Encoding Support ... No    |")
else
AC_MSG_NOTICE("|             MPEG Layer 3 Encoding Support ... Yes   |")
fi
if test -z $VORBIS_FOUND ; then
AC_MSG_NOTICE("|       OggVorbis Encoding/Decoding Support ... No    |")
else
AC_MSG_NOTICE("|       OggVorbis Encoding/Decoding Support ... Yes   |")
fi
AC_MSG_NOTICE("|                                                     |")
AC_MSG_NOTICE("| Optional Components:                                |")
if test -z $USING_PAM ; then
AC_MSG_NOTICE("|           Rivendell PAM pam_rd.so Support ... No    |")
else
AC_MSG_NOTICE("|           Rivendell PAM pam_rd.so Support ... Yes   |")
fi
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE()
if test $WIN32_SOURCE ; then
AC_MSG_NOTICE([Including RPM win32 setup from:])
AC_MSG_NOTICE([  $WIN32_SOURCE])
AC_MSG_NOTICE()
fi
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
