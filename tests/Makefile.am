## Makefile.am
##
## (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

noinst_PROGRAMS = audio_convert_test\
                  audio_export_test\
                  audio_import_test\
                  audio_metadata_test\
                  audio_peaks_test\
                  cmdline_parser_test\
                  datedecode_test\
                  dateparse_test\
                  db_charset_test\
                  delete_test\
                  download_test\
                  feed_image_test\
                  getpids_test\
                  log_unlink_test\
                  mcast_recv_test\
                  metadata_wildcard_test\
                  notification_test\
                  rdxml_parse_test\
                  readcd_test\
                  reserve_carts_test\
                  sendmail_test\
                  stringcode_test\
                  test_hash\
                  test_pam\
                  timer_test\
                  upload_test\
                  wav_chunk_test

dist_audio_convert_test_SOURCES = audio_convert_test.cpp audio_convert_test.h
audio_convert_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_audio_export_test_SOURCES = audio_export_test.cpp audio_export_test.h
audio_export_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_audio_import_test_SOURCES = audio_import_test.cpp audio_import_test.h
audio_import_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_audio_metadata_test_SOURCES = audio_metadata_test.cpp audio_metadata_test.h
audio_metadata_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_audio_peaks_test_SOURCES = audio_peaks_test.cpp audio_peaks_test.h
audio_peaks_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_cmdline_parser_test_SOURCES = cmdline_parser_test.cpp cmdline_parser_test.h
cmdline_parser_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_datedecode_test_SOURCES = datedecode_test.cpp datedecode_test.h
datedecode_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_dateparse_test_SOURCES = dateparse_test.cpp dateparse_test.h
dateparse_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_db_charset_test_SOURCES = db_charset_test.cpp db_charset_test.h
db_charset_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_delete_test_SOURCES = delete_test.cpp delete_test.h
delete_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_download_test_SOURCES = download_test.cpp download_test.h
download_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_feed_image_test_SOURCES = feed_image_test.cpp feed_image_test.h
feed_image_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_getpids_test_SOURCES = getpids_test.cpp getpids_test.h
getpids_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_log_unlink_test_SOURCES = log_unlink_test.cpp log_unlink_test.h
nodist_log_unlink_test_SOURCES = moc_log_unlink_test.cpp
log_unlink_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_metadata_wildcard_test_SOURCES = metadata_wildcard_test.cpp metadata_wildcard_test.h
nodist_metadata_wildcard_test_SOURCES = moc_metadata_wildcard_test.cpp
metadata_wildcard_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_mcast_recv_test_SOURCES = mcast_recv_test.cpp mcast_recv_test.h
nodist_mcast_recv_test_SOURCES = moc_mcast_recv_test.cpp
mcast_recv_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_notification_test_SOURCES = notification_test.cpp notification_test.h
nodist_notification_test_SOURCES = moc_notification_test.cpp
notification_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_rdxml_parse_test_SOURCES = rdxml_parse_test.cpp rdxml_parse_test.h
rdxml_parse_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_readcd_test_SOURCES = readcd_test.cpp readcd_test.h
readcd_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_reserve_carts_test_SOURCES = reserve_carts_test.cpp reserve_carts_test.h
reserve_carts_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_sendmail_test_SOURCES = sendmail_test.cpp sendmail_test.h
sendmail_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_stringcode_test_SOURCES = stringcode_test.cpp stringcode_test.h
stringcode_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_test_hash_SOURCES = test_hash.cpp test_hash.h
test_hash_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_test_pam_SOURCES = test_pam.cpp test_pam.h
test_pam_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_timer_test_SOURCES = timer_test.cpp timer_test.h
nodist_timer_test_SOURCES = moc_timer_test.cpp
timer_test_LDADD =  @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_upload_test_SOURCES = upload_test.cpp upload_test.h
upload_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

dist_wav_chunk_test_SOURCES = wav_chunk_test.cpp wav_chunk_test.h
wav_chunk_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rivendell_standard.txt\
             visualtraffic.txt

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
