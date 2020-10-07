## automake.am
##
## (C) Copyright 2015 Todd Baker <bakert@rfa.org>
## (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CFLAGS = -Wall -I$ ..
LIBS = -L$ ../rivwebcapi
noinst_PROGRAMS = addcart_test \
		  addcut_test \
		  addlog_test \
		  assignschedcode_test \
		  audioinfo_test \
		  audiostore_test \
		  copyaudio_test \
		  createticket_test \
		  deleteaudio_test \
		  deletelog_test \
		  deletepodcast_test \
		  editcart_test \
		  editcut_test \
		  exportcart_test \
		  exportpeaks_test \
		  getuseragent_test \
		  getversion_test \
		  importcart_test \
		  listcut_test \
		  listcuts_test \
		  listcart_test \
		  listcartcuts_test \
		  listcarts_test \
		  listcartscuts_test \
		  listcartschedcodes_test \
		  listgroup_test \
                  listgroups_test \
		  listlog_test \
		  listlogs_test \
		  listschedcodes_test \
		  listservices_test \
		  listsystemsettings_test \
                  postimage_test \
                  postpodcast_test \
                  postrss_test \
		  removecart_test \
		  removecut_test \
                  removeimage_test \
                  removepodcast_test \
                  removerss_test \
		  savelog_test \
		  savepodcast_test \
		  trimaudio_test \
		  unassignschedcode_test

dist_addcart_test_SOURCES = addcart_test.c 
addcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_addcut_test_SOURCES = addcut_test.c 
addcut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_addlog_test_SOURCES = addlog_test.c common.c common.h 
addlog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_assignschedcode_test_SOURCES = assignschedcode_test.c 
assignschedcode_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_audioinfo_test_SOURCES = audioinfo_test.c 
audioinfo_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_audiostore_test_SOURCES = audiostore_test.c 
audiostore_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_copyaudio_test_SOURCES = copyaudio_test.c 
copyaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_createticket_test_SOURCES = createticket_test.c 
createticket_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_deleteaudio_test_SOURCES = deleteaudio_test.c 
deleteaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_deletelog_test_SOURCES = deletelog_test.c common.c common.h 
deletelog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_deletepodcast_test_SOURCES = deletepodcast_test.c common.c common.h 
deletepodcast_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_editcart_test_SOURCES = editcart_test.c 
editcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_editcut_test_SOURCES = editcut_test.c 
editcut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_exportcart_test_SOURCES = exportcart_test.c 
exportcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_exportpeaks_test_SOURCES = exportpeaks_test.c 
exportpeaks_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_getversion_test_SOURCES = getversion_test.c 
getversion_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_getuseragent_test_SOURCES = getuseragent_test.c 
getuseragent_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_importcart_test_SOURCES = importcart_test.c 
importcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcart_test_SOURCES = listcart_test.c 
listcart_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcartcuts_test_SOURCES = listcartcuts_test.c 
listcartcuts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcarts_test_SOURCES = listcarts_test.c 
listcarts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcartscuts_test_SOURCES = listcartscuts_test.c 
listcartscuts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcartschedcodes_test_SOURCES = listcartschedcodes_test.c 
listcartschedcodes_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcut_test_SOURCES = listcut_test.c 
listcut_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listcuts_test_SOURCES = listcuts_test.c 
listcuts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listgroup_test_SOURCES = listgroup_test.c 
listgroup_test_LDADD = -lrivwebcapi -lexpat -lcurl -lm

dist_listgroups_test_SOURCES = listgroups_test.c 
listgroups_test_LDADD = -lrivwebcapi -lexpat -lcurl -lm

dist_listlog_test_SOURCES = listlog_test.c 
listlog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_listlogs_test_SOURCES = listlogs_test.c 
listlogs_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_listschedcodes_test_SOURCES = listschedcodes_test.c 
listschedcodes_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_listservices_test_SOURCES = listservices_test.c 
listservices_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_listsystemsettings_test_SOURCES = listsystemsettings_test.c 
listsystemsettings_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_postimage_test_SOURCES = postimage_test.c 
postimage_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_postpodcast_test_SOURCES = postpodcast_test.c 
postpodcast_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_postrss_test_SOURCES = postrss_test.c 
postrss_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

dist_removecart_test_SOURCES = removecart_test.c 
removecart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_removecut_test_SOURCES = removecut_test.c 
removecut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_removeimage_test_SOURCES = removeimage_test.c 
removeimage_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_removepodcast_test_SOURCES = removepodcast_test.c 
removepodcast_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_removerss_test_SOURCES = removerss_test.c 
removerss_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_savelog_test_SOURCES = savelog_test.c common.c common.h
savelog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_savepodcast_test_SOURCES = savepodcast_test.c common.c common.h
savepodcast_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_trimaudio_test_SOURCES = trimaudio_test.c 
trimaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

dist_unassignschedcode_test_SOURCES = unassignschedcode_test.c 
unassignschedcode_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
