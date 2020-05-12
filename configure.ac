dnl configure.ac
dnl
dnl Autoconf configuration for Rivendell.
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
AM_INIT_AUTOMAKE([1.9 tar-pax])
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

#
# API Library Versioning
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
AC_PROG_CC
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
AC_ARG_ENABLE(docbook,[  --disable-docbook       disable building of documentation],[DOCBOOK_DISABLED=yes],[])

AC_ARG_ENABLE(gpio,[  --disable-gpio          disable General Purpose Input/Output GPIO support],
		      [GPIO_DISABLED=yes],[])
AC_ARG_ENABLE(mad,[  --disable-mad           disable MPEG decode support],
		      [MAD_DISABLED=yes],[])
AC_ARG_ENABLE(twolame,[  --disable-twolame       disable MPEG Layer 2 encode support],
		      [TWOLAME_DISABLED=yes],[])
AC_ARG_ENABLE(lame,[  --disable-lame          disable MPEG Layer 3 encode support],
		      [LAME_DISABLED=yes],[])
AC_ARG_ENABLE(flac,[  --disable-flac          disable FLAC encode/decode support],
		      [FLAC_DISABLED=yes],[])
AC_ARG_ENABLE(mp4v2,[  --disable-mp4           disable M4A decode support],
		      [MP4V2_DISABLED=yes],[])
AC_ARG_ENABLE(rdxport-debug,[  --enable-rdxport-debug  enable DEBUG support for RDXport services],
		      [RDXPORT_DEBUG_ENABLED=yes],[])


#
# Check for Qt4 (Mandatory)
#
PKG_CHECK_MODULES(QT4,QtCore QtGui QtNetwork QtSql QtXml Qt3Support,,[AC_MSG_ERROR([*** Qt4 not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt4,[moc-qt4],[moc])
AC_SUBST(QT_MOC,$MOC_NAME)


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
# Determine number of available CPU cores for building packages
#
AX_COUNT_CPUS([],[])
if test $CPU_COUNT -lt 2 ; then
   AC_SUBST(CPUS_AVAIL,1)
else
   if test $CPU_COUNT -gt 16 ; then
      AC_SUBST(CPUS_AVAIL,16)
   else
      AC_SUBST(CPUS_AVAIL,$(($CPU_COUNT-1)))
   fi
fi

#
# Determine Distro
#
AR_GET_DISTRO()
AC_SUBST(DISTRO,$ar_gcc_distro)
if test $ar_gcc_distro = suse ; then
  AC_SUBST(APACHE_PKG,"apache2")
  AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
#  AC_SUBST(CONSOLEHELPER_RDALSACONFIG,"")
  AC_SUBST(USERMODE_PKG,"")
  AC_SUBST(MYSQL_PKG,"mysql")
  AC_SUBST(QT4_MYSQL_PKG,"qt-mysql")
  AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
else
  AC_SUBST(APACHE_PKG,"httpd")
  AC_SUBST(APACHE_CONFIG_DIR,"/etc/httpd/conf.d")
  AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell-$PACKAGE_VERSION")
  if test $ar_distro_major -ge 7 ; then
    AC_SUBST(USERMODE_PKG,"usermode usermode-gtk")
    AC_SUBST(MYSQL_PKG,"mariadb")
    AC_SUBST(QT4_MYSQL_PKG,"qt-mysql")
  else
    if test $ar_distro_major -ge 6 ; then
    AC_SUBST(USERMODE_PKG,"usermode")
      AC_SUBST(MYSQL_PKG,"mysql")
      AC_SUBST(QT4_MYSQL_PKG,"qt4-mysql")
    else
      AC_SUBST(USERMODE_PKG,"usermode")
      AC_SUBST(MYSQL_PKG,"mysql")
      AC_SUBST(QT4_MYSQL_PKG,"qt-MySQL")
    fi
  fi
fi

#
# Check for Expat
#
AC_CHECK_HEADER(expat.h,[],[AC_MSG_ERROR([*** Expat not found ***])])

#
# Check for OpenSSL
#
AC_CHECK_HEADER(openssl/sha.h,[],[AC_MSG_ERROR([*** OpenSSL not found ***])])

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
# Check for MusicBrainz support libraries
# (libdiscid, libmusicbrainz and libcoverart)
#
PKG_CHECK_MODULES(MUSICBRAINZ,libdiscid libmusicbrainz5 libcoverart,,[AC_MSG_ERROR([*** One or more MusicBrainz libraries not found ***])])

#
# Check for Id3Lib
#
AC_CHECK_HEADER(id3/tag.h,[],[AC_MSG_ERROR([*** Id3Lib not found ***])])

#
# Check for TagLib
#
AC_CHECK_HEADER(taglib/taglib.h,[],[AC_MSG_ERROR([*** TagLib not found ***])])

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
# Check for Python
#
AM_PATH_PYTHON([3])
AC_SUBST(PYTHON_BASE_DEP,"python"`echo $PYTHON_VERSION | sed -e s/3./3/`)

#
# Check for runuser(1)
#
AC_PATH_PROG(RUNUSER_PATH,runuser)
if test -z $RUNUSER_PATH ; then
   AC_MSG_ERROR([*** runuser(1) not found ***])
fi

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
# Check for MP4V2
#
if test -z $MP4V2_DISABLED ; then
  AC_CHECK_HEADER(mp4v2/mp4v2.h,[MP4V2_HEADER_FOUND=yes],[])
  if test $MP4V2_HEADER_FOUND ; then
    AC_CHECK_HEADER(neaacdec.h,[MP4V2_FOUND=yes],[])
    if test $MP4V2_FOUND ; then
       AC_DEFINE(HAVE_MP4_LIBS)
    fi
  fi  
fi

#
# DocBook
#
if test -z $DOCBOOK_DISABLED ; then
  USING_DOCBOOK=yes
fi
AM_CONDITIONAL([DOCBOOK_AM], [test "$USING_DOCBOOK" = yes])
if test $DOCBOOK_STYLESHEETS ; then
   ln -s $DOCBOOK_STYLESHEETS helpers/docbook
fi

#AC_SUBST(DOCBOOK_STYLESHEETS,$DOCBOOK_STYLESHEETS)

#
# RDXport Debug
#
if test $RDXPORT_DEBUG_ENABLED ; then
  AC_DEFINE(RDXPORT_DEBUG)
  AC_SUBST(RDXPORTDEBUG,--enable-rdxport-debug)
else
  AC_SUBST(RDXPORTDEBUG,"")
fi

#
# Set Hard Library Dependencies
#
#AC_SUBST(LIB_RDLIBS,"-lm -lpthread -lrd -lcurl -lid3 $FLAC_LIBS -lsndfile -lsamplerate -lcdda_interface -lcdda_paranoia -lcrypt -ldl -lpam -lSoundTouch -lcrypto")
AC_SUBST(LIB_RDLIBS,"-lm -lpthread -lrd -lcurl -lid3 -ltag $FLAC_LIBS -lsndfile -lsamplerate -lcdda_interface -lcdda_paranoia -lcrypt -ldl -lpam -lSoundTouch -lcrypto")

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
  else 
    AC_SUBST(LIBHPI,"")
    AC_SUBST(HPI_FILE1,"")
    AC_SUBST(HPI_FILE2,"")
    AC_SUBST(HPI_FILE3,"")
    AC_SUBST(HPI_FILE4,"")
  fi
else
  AC_SUBST(LIBHPI,"")
  AC_SUBST(HPI_FILE1,"")
  AC_SUBST(HPI_FILE2,"")
  AC_SUBST(HPI_FILE3,"")
  AC_SUBST(HPI_FILE4,"")
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
# Configure RPM Build
#
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

#
# Create icon symlinks for Operations Guide
#
rm -f docs/opsguide/chain.png
ln -s ../../icons/chain.png docs/opsguide/chain.png

rm -f docs/opsguide/mic16.png
ln -s ../../icons/mic16.png docs/opsguide/mic16.png

rm -f docs/opsguide/music.png
ln -s ../../icons/music.png docs/opsguide/music.png

rm -f docs/opsguide/notemarker.png
ln -s ../../icons/notemarker.png docs/opsguide/notemarker.png

rm -f docs/opsguide/play.png
ln -s ../../icons/play.png docs/opsguide/play.png

rm -f docs/opsguide/rml5.png
ln -s ../../icons/rml5.png docs/opsguide/rml5.png

rm -f docs/opsguide/track_cart.png
ln -s ../../icons/track_cart.png docs/opsguide/track_cart.png

rm -f docs/opsguide/traffic.png
ln -s ../../icons/traffic.png docs/opsguide/traffic.png

rm -f docs/opsguide/trashcan-32x32.png
ln -s ../../icons/trashcan-32x32.png docs/opsguide/trashcan-32x32.png

AC_CONFIG_FILES([rivendell.spec \
    Makefile \
    conf/rd-bin.conf \
    icons/Makefile \
    helpers/Makefile \
    helpers/install_python.sh \
    apis/Makefile \
    apis/pypad/Makefile \
    apis/pypad/api/Makefile \
    apis/pypad/scripts/Makefile \
    apis/pypad/tests/Makefile \
    apis/rivwebcapi/Makefile \
    apis/rivwebcapi/rivwebcapi.pc \
    apis/rivwebcapi/rivwebcapi/Makefile \
    apis/rivwebcapi/tests/Makefile \
    lib/rdpaths.h \
    lib/Makefile \
    rdhpi/Makefile \
    cae/Makefile \
    web/Makefile \
    web/rdfeed/Makefile \
    web/rdcastmanager/Makefile \
    web/rdxport/Makefile \
    web/tests/Makefile \
    web/webget/Makefile \
    conf/Makefile \
    docs/Makefile \
    docs/apis/Makefile \
    docs/examples/Makefile \
    docs/manpages/Makefile \
    docs/misc/Makefile \
    docs/opsguide/frontspiece.xml \
    docs/opsguide/Makefile \
    docs/rivwebcapi/Makefile \
    docs/stylesheets/Makefile \
    docs/tables/Makefile \
    importers/Makefile \
    ios/Makefile \
    ios/rmlsend/Makefile \
    ios/rmlsend/Classes/Makefile \
    ios/rmlsend/RMLSend.xcodeproj/Makefile \
    rdadmin/Makefile \
    ripcd/Makefile \
    rdairplay/Makefile \
    rdcartslots/Makefile \
    rdcastmanager/Makefile \
    rdcatch/Makefile \
    rdcatchd/Makefile \
    rdlibrary/Makefile \
    rdlogin/Makefile \
    rdlogedit/Makefile \
    rdlogmanager/Makefile \
    rdmonitor/Makefile \
    rdpanel/Makefile \
    rdrepld/Makefile \
    rdpadd/Makefile \
    rdpadengined/Makefile \
    rdselect/Makefile \
    rdservice/Makefile \
    rdvairplayd/Makefile \
    scripts/Makefile \
    systemd/Makefile \
    systemd/rivendell.service \
    tests/Makefile \
    utils/Makefile \
    utils/rdalsaconfig/Makefile \
    utils/rdcheckcuts/Makefile \
    utils/rdcleandirs/Makefile \
    utils/rdclilogedit/Makefile \
    utils/rdcollect/Makefile \
    utils/rdconvert/Makefile \
    utils/rddbconfig/Makefile \
    utils/rddbmgr/Makefile \
    utils/rddelete/Makefile \
    utils/rddgimport/Makefile \
    utils/rddiscimport/Makefile \
    utils/rdexport/Makefile \
    utils/rdgen/Makefile \
    utils/rdgpimon/Makefile \
    utils/rdimport/Makefile \
    utils/rdmaint/Makefile \
    utils/rdmarkerset/Makefile \
    utils/rdmetadata/Makefile \
    utils/rdpopup/Makefile \
    utils/rdpurgecasts/Makefile \
    utils/rdrender/Makefile \
    utils/rdselect_helper/Makefile \
    utils/rdsoftkeys/Makefile \
    utils/rmlsend/Makefile \
    xdg/Makefile \
    xdg/rdalsaconfig-root-consolehelper \
    xdg/rddbconfig-root-consolehelper \
    ])
AC_OUTPUT()

chmod 755 helpers/install_python.sh

#
# Create symlinks in 'utils/rdselect_helper/'
#
rm -f utils/rdselect_helper/rd.h
ln -s ../../lib/rd.h utils/rdselect_helper/rd.h

rm -f utils/rdselect_helper/rdconfig.cpp
ln -s ../../lib/rdconfig.cpp utils/rdselect_helper/rdconfig.cpp
rm -f utils/rdselect_helper/rdconfig.h
ln -s ../../lib/rdconfig.h utils/rdselect_helper/rdconfig.h

rm -f utils/rdselect_helper/rdprofile.cpp
ln -s ../../lib/rdprofile.cpp utils/rdselect_helper/rdprofile.cpp
rm -f utils/rdselect_helper/rdprofile.h
ln -s ../../lib/rdprofile.h utils/rdselect_helper/rdprofile.h

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
if test -z $MP4V2_FOUND ; then
AC_MSG_NOTICE("|                      M4A Decoding Support ... No    |")
else
AC_MSG_NOTICE("|                      M4A Decoding Support ... Yes   |")
fi
AC_MSG_NOTICE("|-----------------------------------------------------|")
AC_MSG_NOTICE()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
