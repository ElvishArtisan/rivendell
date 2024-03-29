dnl configure.ac
dnl
dnl Autoconf configuration for Rivendell.
dnl Use autoconf to process this into a configure script
dnl
dnl   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

AC_INIT([rivendell],m4_esyscmd(cat versions/PACKAGE_VERSION))
AM_INIT_AUTOMAKE([1.9 tar-pax])
AC_SUBST(RPM_RELEASE,1)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AC_CONFIG_MACRO_DIR([m4])

#
# API Versioning
#
AC_SUBST(PYTHONAPI_VERSION,m4_esyscmd(cat versions/PYTHONAPI_VERSION))
RIVWEBCAPI_CURRENT=m4_esyscmd(cat versions/RIVWEBCAPI_CURRENT)
RIVWEBCAPI_REVISION=m4_esyscmd(cat versions/RIVWEBCAPI_REVISION)
RIVWEBCAPI_AGE=m4_esyscmd(cat versions/RIVWEBCAPI_AGE)
RIVWEBCAPI_POINT=$(($RIVWEBCAPI_CURRENT - $RIVWEBCAPI_AGE))
AC_SUBST(INTERFACE_RIVWEBCAPI_CURRENT,$RIVWEBCAPI_CURRENT)
AC_SUBST(INTERFACE_RIVWEBCAPI_REVISION,$RIVWEBCAPI_REVISION)
AC_SUBST(INTERFACE_RIVWEBCAPI_AGE,$RIVWEBCAPI_AGE)
AC_SUBST(INTERFACE_RIVWEBCAPI_MAJOR,$RIVWEBCAPI_POINT)
AC_SUBST(INTERFACE_RIVWEBCAPI_MINOR,$RIVWEBCAPI_AGE)
AC_SUBST(INTERFACE_RIVWEBCAPI_POINT,$RIVWEBCAPI_REVISION)

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
AC_ARG_ENABLE(i18n-updates,[  --enable-i18n-updates   enable I18N metadata updates],
		      [I18N_ENABLED=yes],[])


#
# Check for Qt5 (Mandatory)
#
PKG_CHECK_MODULES(QT5,Qt5Core Qt5Widgets Qt5Gui Qt5Network Qt5Sql Qt5Xml Qt5WebKitWidgets,,[AC_MSG_ERROR([*** Qt5 not found ***])])
PKG_CHECK_MODULES(QT5_CLI,Qt5Core Qt5Network Qt5Sql Qt5Xml,,[AC_MSG_ERROR([*** Qt5 not found ***])])
AC_CHECK_PROG(MOC_NAME,moc-qt5,[moc-qt5],[moc])
AC_SUBST(QT_MOC,$MOC_NAME)

AC_CHECK_PROG(LUPDATE_NAME,lupdate-qt5,[lupdate-qt5],[lupdate])
AC_SUBST(QT_LUPDATE,$LUPDATE_NAME)

AC_CHECK_PROG(LRELEASE_NAME,lrelease-qt5,[lrelease-qt5],[lrelease])
AC_SUBST(QT_LRELEASE,$LRELEASE_NAME)


#
# Determine the target architecture
#
AR_GCC_TARGET()
AC_SUBST(VENDOR,$ar_gcc_distro)
AC_SUBST(ARCH,$ar_gcc_arch)

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
AC_MSG_NOTICE($ar_distro_id)
DISTRO_NAME="|                         Distribution Name ... Unknown   |"
case "$ar_distro_id" in
     debian)
       AC_MSG_NOTICE([Distro is Debian-ish])
       DISTRO_FAMILY="|                       Distribution Family ... Debian    |"
       DISTRO_IS_DEBIANISH=yes
       DISTRO_NAME="|                         Distribution Name ... Debian    |"
       AC_SUBST(APACHE_PKG,"apache2")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
       AC_SUBST(USERMODE_PKG,"")
       AC_SUBST(MYSQL_PKG,"mariadb-client")
       AC_SUBST(QT_MYSQL_PKG,"libqt5sql5-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
       AC_SUBST(HAVE_USERMODE,"no")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig-sudo.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig-sudo.desktop")
       AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-ubuntu.pam")  
     ;;

     raspbian)
       AC_MSG_NOTICE([Distro is Debian-ish])
       DISTRO_FAMILY="|                       Distribution Family ... Debian    |"
       DISTRO_IS_DEBIANISH=yes
       DISTRO_NAME="|                  Distribution Name ... Raspberry Pi OS  |"
       AC_SUBST(APACHE_PKG,"apache2")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
       AC_SUBST(USERMODE_PKG,"")
       AC_SUBST(MYSQL_PKG,"mariadb-client")
       AC_SUBST(QT_MYSQL_PKG,"libqt5sql5-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
       AC_SUBST(HAVE_USERMODE,"no")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig-sudo.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig-sudo.desktop")
       AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-ubuntu.pam")  
     ;;

     linuxmint)
       AC_MSG_NOTICE([Distro is Debian-ish])
       DISTRO_FAMILY="|                       Distribution Family ... Debian    |"
       DISTRO_IS_DEBIANISH=yes
       DISTRO_NAME="|                         Distribution Name ... Linux Mint|"
       AC_SUBST(APACHE_PKG,"apache2")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
       AC_SUBST(USERMODE_PKG,"")
       AC_SUBST(MYSQL_PKG,"mariadb-client")
       AC_SUBST(QT_MYSQL_PKG,"libqt5sql5-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
       AC_SUBST(HAVE_USERMODE,"no")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig-sudo.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig-sudo.desktop")
       AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-ubuntu.pam")
     ;;

     ubuntu)
       AC_MSG_NOTICE([Distro is Debian-ish])
       DISTRO_FAMILY="|                       Distribution Family ... Debian    |"
       DISTRO_IS_DEBIANISH=yes
       DISTRO_NAME="|                         Distribution Name ... Ubuntu    |"
       AC_SUBST(APACHE_PKG,"apache2")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
       AC_SUBST(USERMODE_PKG,"")
       AC_SUBST(MYSQL_PKG,"mariadb-client")
       AC_SUBST(QT_MYSQL_PKG,"libqt5sql5-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
       AC_SUBST(HAVE_USERMODE,"no")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig-sudo.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig-sudo.desktop")
       AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-ubuntu.pam")  
     ;;

     centos|fedora|rhel|rocky)
       AC_MSG_NOTICE([Distro is RedHat-ish])
       DISTRO_FAMILY="|                       Distribution Family ... RedHat    |"
       if test $ar_distro_id = "centos" ; then
         DISTRO_NAME="|                         Distribution Name ... CentOS    |"
       fi
       if test $ar_distro_id = "fedora" ; then
         DISTRO_NAME="|                         Distribution Name ... Fedora    |"
       fi
       if test $ar_distro_id = "rhel" ; then
         DISTRO_NAME="|                         Distribution Name ... RHEL      |"
       fi
       if test $ar_distro_id = "rocky" ; then
         DISTRO_NAME="|                         Distribution Name ... Rocky     |"
       fi
       AC_SUBST(APACHE_PKG,"httpd")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/httpd/conf.d")
       AC_SUBST(USERMODE_PKG,"usermode usermode-gtk")
       AC_SUBST(MYSQL_PKG,"mariadb")
       AC_SUBST(QT_MYSQL_PKG,"qt5-qtbase-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell-$PACKAGE_VERSION")
       AC_SUBST(HAVE_USERMODE,"yes")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig-root.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig-root.desktop")
       if test $ar_distro_major -lt 8 ; then
       	  AC_SUBST(PYPAD_DEPS,"python36 python36-pycurl python36-requests python36-pyserial python36-mysql python36-urllib3")
       else
          if test $ar_distro_major -lt 9 ; then
	     AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
	  else
	     AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysqlclient python3-urllib3")
          fi
       fi
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-rhel.pam")  
       ;;

     *)
       AC_MSG_NOTICE([Distro is unknown])
       DISTRO_FAMILY="|                       Distribution Family ... Unknown   |"
       AC_SUBST(APACHE_PKG,"apache2")
       AC_SUBST(APACHE_CONFIG_DIR,"/etc/apache2/conf.d")
       AC_SUBST(USERMODE_PKG,"")
       AC_SUBST(MYSQL_PKG,"mysql")
       AC_SUBST(QT_MYSQL_PKG,"qt-mysql")
       AC_SUBST(DOC_PATH,"/usr/share/doc/rivendell")
       AC_SUBST(HAVE_USERMODE,"no")
       AC_SUBST(RDALSACONFIG_DESKTOP_FILE,"rivendell-rdalsaconfig.desktop")
       AC_SUBST(RDDBCONFIG_DESKTOP_FILE,"rivendell-rddbconfig.desktop")
       AC_SUBST(PYPAD_DEPS,"python3 python3-pycurl python3-requests python3-pyserial python3-mysql python3-urllib3")
       AC_SUBST(RIVENDELL_PAM_FILE,"rivendell-rhel.pam")  
     ;;
esac

#
# Internationalization
#
if test $I18N_ENABLED ; then
   AC_SUBST(I18N_ACTIVE,1)
else
   AC_SUBST(I18N_ACTIVE,0)
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
# Check for ImageMagick
#
PKG_CHECK_MODULES(IMAGEMAGICK,Magick++-6.Q16,[],[AC_MSG_ERROR([*** ImageMagick 6 Magick++ binding not found ***])])

#
# Check for Python
#
if test -z $DISTRO_IS_DEBIANISH ; then
   AM_PATH_PYTHON([3])
   #echo -n $pythondir | sed "s^\${prefix}^/usr^" > debian/pythondir
   AC_SUBST(PYTHON_BASE_DEP,"python"`echo $PYTHON_VERSION | sed -e s/3./3/`)
else
   #
   # FIXME: Horrible hack to make Python install correctly on Debianish setups
   #
   AC_MSG_NOTICE([Configuring Debian-style Python installation])
   PYTHON="/usr/bin/python3"
   AC_ARG_VAR(PYTHON,[the Python interpreter])
   pythondir="/usr/lib/python3/dist-packages"
   AC_ARG_VAR(pythondir,[the Python modules directory])
   pyexecdir="/usr/lib/python3/dist-packages"
   AC_ARG_VAR(pyexecdir,[the Python extension modules directory])
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
PKG_CHECK_MODULES(LIBJACK,jack,[JACK_FOUND=yes],[JACK_NOT_FOUND=yes])
PKG_CHECK_MODULES(ASOUNDLIB,alsa,[ALSA_FOUND=yes],[ALSA_NOT_FOUND=yes])
if test $HPI_FOUND ; then
  if test -z $HPI_DISABLED ; then
    USING_HPI=yes
    AC_DEFINE(HPI,yes)
    AC_SUBST(LIBHPI,"-lrdhpi -lhpi")
    AC_SUBST(HPI_FILE1,${libdir}/librdhpi-$VERSION.so)
    AC_SUBST(HPI_FILE2,${libdir}/librdhpi.a)
    AC_SUBST(HPI_FILE3,${libdir}/librdhpi.la)
    AC_SUBST(HPI_FILE4,${libdir}/librdhpi.so)
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
    SRC_NEEDED=yes
    USING_JACK=yes
  else
     AC_SUBST(JACK_CFLAGS,"")
     AC_SUBST(JACK_LIBS,"")
fi
else 
     AC_SUBST(JACK_CFLAGS,"")
     AC_SUBST(JACK_LIBS,"")
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
  AC_SUBST(VENDOR,redhat)
else
  AC_SUBST(VENDOR,suse)
fi 
AC_SUBST(RPM_DATESTAMP,`date +%a\ %b\ %d\ %Y`)

#
# Generated File Warnings
#
AC_SUBST_FILE(GENERATED_C_FILE_WARNING)
GENERATED_C_FILE_WARNING=$srcdir/helpers/c_file_warning

AC_SUBST_FILE(GENERATED_SCRIPT_FILE_WARNING)
GENERATED_SCRIPT_FILE_WARNING=$srcdir/helpers/script_file_warning

AC_SUBST_FILE(GENERATED_XML_FILE_WARNING)
GENERATED_XML_FILE_WARNING=$srcdir/helpers/xml_file_warning

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

rm -f docs/opsguide/blueball.png
ln -s ../../icons/blueball.png docs/opsguide/blueball.png

rm -f docs/opsguide/greenball.png
ln -s ../../icons/greenball.png docs/opsguide/greenball.png

rm -f docs/opsguide/redball.png
ln -s ../../icons/redball.png docs/opsguide/redball.png

rm -f docs/opsguide/whiteball.png
ln -s ../../icons/whiteball.png docs/opsguide/whiteball.png

rm -f docs/opsguide/greencheckmark.png
ln -s ../../icons/greencheckmark.png docs/opsguide/greencheckmark.png

rm -f docs/opsguide/redx.png
ln -s ../../icons/redx.png docs/opsguide/redx.png


AC_CONFIG_FILES([rivendell.spec \
    versions/Makefile \
    debian/Makefile \
    debian/source/Makefile \
    Makefile \
    conf/rd-bin.conf \
    icons/Makefile \
    helpers/Makefile \
    helpers/install_python.sh \
    helpers/rdi18n_helper.sh \
    LICENSES/Makefile \
    apis/setup.cfg \
    apis/Makefile \
    apis/cdn/scripts/Makefile \
    apis/cdn/Makefile \
    apis/pypad/Makefile \
    apis/pypad/api/Makefile \
    apis/pypad/scripts/Makefile \
    apis/pypad/tests/Makefile \
    apis/rivwebcapi/Makefile \
    apis/rivwebcapi/rivwebcapi.pc \
    apis/rivwebcapi/rivwebcapi/Makefile \
    apis/rivwebcapi/tests/Makefile \
    apis/rivwebpyapi/Makefile \
    apis/rivwebpyapi/api/Makefile \
    apis/rivwebpyapi/tests/Makefile \
    lib/rdpaths.h \
    lib/Makefile \
    rdhpi/Makefile \
    cae/Makefile \
    web/Makefile \
    web/common/Makefile \
    web/rdxport/Makefile \
    web/stylesheets/Makefile \
    web/tests/Makefile \
    web/webget/Makefile \
    conf/Makefile \
    docs/Makefile \
    docs/apis/Makefile \
    docs/dtds/Makefile \
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
    rdrssd/Makefile \
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
    utils/rdexport/Makefile \
    utils/rdgen/Makefile \
    utils/rdgpimon/Makefile \
    utils/rdimport/Makefile \
    utils/rdmaint/Makefile \
    utils/rdmarkerset/Makefile \
    utils/rdmetadata/Makefile \
    utils/rdpopup/Makefile \
    utils/rdrender/Makefile \
    utils/rdselect_helper/Makefile \
    utils/rdsinglestart/Makefile \
    utils/rdsoftkeys/Makefile \
    utils/rmlsend/Makefile \
    xdg/Makefile \
    xdg/install_usermode.sh \
    xdg/rdalsaconfig-root-consolehelper \
    xdg/rddbconfig-root-consolehelper \
    build_debs.sh \
    ])
AC_OUTPUT()

chmod 755 helpers/rdi18n_helper.sh
chmod 755 helpers/install_python.sh
chmod 755 xdg/install_usermode.sh
chmod 755 build_debs.sh

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
AC_MSG_NOTICE("|---------------------------------------------------------|")
AC_MSG_NOTICE("|         *** RIVENDELL CONFIGURATION SUMMARY ***         |")
AC_MSG_NOTICE("|---------------------------------------------------------|")
AC_MSG_NOTICE("| Platform Information:                                   |")
AC_MSG_NOTICE("$DISTRO_NAME")
AC_MSG_NOTICE("$DISTRO_FAMILY")
if test $I18N_ENABLED ; then
AC_MSG_NOTICE("|                          Update I18N Data ... Yes       |")
else
AC_MSG_NOTICE("|                          Update I18N Data ... No        |")
fi
AC_MSG_NOTICE("|                                                         |")
AC_MSG_NOTICE("| Configured Audio Drivers:                               |")
if test -z $USING_ALSA ; then
AC_MSG_NOTICE("|  Advanced Linux Sound Architecture (ALSA) ... No        |")
else
AC_MSG_NOTICE("|  Advanced Linux Sound Architecture (ALSA) ... Yes       |")
fi
if test -z $USING_HPI ; then
AC_MSG_NOTICE("|                          AudioScience HPI ... No        |")
else
AC_MSG_NOTICE("|                          AudioScience HPI ... Yes       |")
fi
if test -z $USING_JACK ; then
AC_MSG_NOTICE("|                 JACK Audio Connection Kit ... No        |")
else
AC_MSG_NOTICE("|                 JACK Audio Connection Kit ... Yes       |")
fi
AC_MSG_NOTICE("|                                                         |")
AC_MSG_NOTICE("| Audio Codecs:                                           |")
if test -z $FLAC_FOUND ; then
AC_MSG_NOTICE("|            FLAC Encoding/Decoding Support ... No        |")
else
AC_MSG_NOTICE("|            FLAC Encoding/Decoding Support ... Yes       |")
fi
if test -z $USING_MAD ; then
AC_MSG_NOTICE("|                     MPEG Decoding Support ... No        |")
else
AC_MSG_NOTICE("|                     MPEG Decoding Support ... Yes       |")
fi
if test -z $USING_TWOLAME ; then
AC_MSG_NOTICE("|             MPEG Layer 2 Encoding Support ... No        |")
else
AC_MSG_NOTICE("|             MPEG Layer 2 Encoding Support ... Yes       |")
fi
if test -z $USING_LAME ; then
AC_MSG_NOTICE("|             MPEG Layer 3 Encoding Support ... No        |")
else
AC_MSG_NOTICE("|             MPEG Layer 3 Encoding Support ... Yes       |")
fi
if test -z $VORBIS_FOUND ; then
AC_MSG_NOTICE("|       OggVorbis Encoding/Decoding Support ... No        |")
else
AC_MSG_NOTICE("|       OggVorbis Encoding/Decoding Support ... Yes       |")
fi
if test -z $MP4V2_FOUND ; then
AC_MSG_NOTICE("|                      M4A Decoding Support ... No        |")
else
AC_MSG_NOTICE("|                      M4A Decoding Support ... Yes       |")
fi
AC_MSG_NOTICE("|---------------------------------------------------------|")
AC_MSG_NOTICE()
AC_MSG_NOTICE()
AC_MSG_NOTICE(Now enter 'make' to build the software.)
AC_MSG_NOTICE()
