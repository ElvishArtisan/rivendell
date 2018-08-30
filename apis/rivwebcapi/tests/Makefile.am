## automake.am
##
## (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

AM_CFLAGS = -Wall -I$(top_srcdir)/apis/rivwebcapi
LIBS = -L$(top_srcdir)/apis/rivwebcapi/rivwebcapi
noinst_PROGRAMS = listgroups_test \
		listgroup_test \
		listcarts_test \
		listcart_test \
		listcuts_test \
		listcut_test \
		listsystemsettings_test \
		listschedcodes_test \
		listcartschedcodes_test \
		assignschedcode_test \
		unassignschedcode_test \
		addcart_test \
		addcut_test \
		removecart_test \
		removecut_test \
		audioinfo_test \
		createticket_test \
		audiostore_test \
		trimaudio_test \
		listlogs_test \
		listservices_test \
		addlog_test \
		deletelog_test \
		listlog_test \
		savelog_test \
		editcart_test \
		editcut_test \
		importcart_test \
		exportcart_test \
		exportpeaks_test \
		deleteaudio_test \
		copyaudio_test \
		getversion_test \
		getuseragent_test

dist_listgroups_test_SOURCES = listgroups_test.c 
dist_listgroup_test_SOURCES = listgroup_test.c 
dist_listcarts_test_SOURCES = listcarts_test.c 
dist_listcart_test_SOURCES = listcart_test.c 
dist_listcuts_test_SOURCES = listcuts_test.c 
dist_listcut_test_SOURCES = listcut_test.c 
dist_listschedcodes_test_SOURCES = listschedcodes_test.c 
dist_listsystemsettings_test_SOURCES = listsystemsettings_test.c 
dist_assignschedcode_test_SOURCES = assignschedcode_test.c 
dist_unassignschedcode_test_SOURCES = unassignschedcode_test.c 
dist_addcart_test_SOURCES = addcart_test.c 
dist_addcut_test_SOURCES = addcut_test.c 
dist_addlog_test_SOURCES = addlog_test.c\
                           common.c common.h 
dist_deletelog_test_SOURCES = deletelog_test.c\
                              common.c common.h 
dist_removecart_test_SOURCES = removecart_test.c 
dist_removecut_test_SOURCES = removecut_test.c 
dist_audioinfo_test_SOURCES = audioinfo_test.c 
dist_createticket_test_SOURCES = createticket_test.c 
dist_audiostore_test_SOURCES = audiostore_test.c 
dist_trimaudio_test_SOURCES = trimaudio_test.c 
dist_listlog_test_SOURCES = listlog_test.c 
dist_listlogs_test_SOURCES = listlogs_test.c 
dist_savelog_test_SOURCES = savelog_test.c \
                            common.c common.h
dist_listservices_test_SOURCES = listservices_test.c 
dist_editcart_test_SOURCES = editcart_test.c 
dist_editcut_test_SOURCES = editcut_test.c 
dist_importcart_test_SOURCES = importcart_test.c 
dist_exportcart_test_SOURCES = exportcart_test.c 
dist_exportpeaks_test_SOURCES = exportpeaks_test.c 
dist_deleteaudio_test_SOURCES = deleteaudio_test.c 
dist_copyaudio_test_SOURCES = copyaudio_test.c 
dist_getversion_test_SOURCES = getversion_test.c 
dist_getuseragent_test_SOURCES = getuseragent_test.c 

listgroups_test_LDADD = -lrivwebcapi -lexpat -lcurl -lm
listgroup_test_LDADD = -lrivwebcapi -lexpat -lcurl -lm
listcarts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listcart_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listcuts_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listcut_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listschedcodes_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listsystemsettings_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
assignschedcode_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
listcartschedcodes_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
unassignschedcode_test_LDADD =  -lrivwebcapi -lexpat -lcurl -lm
addcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
addcut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
addlog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
deletelog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
removecart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
removecut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
audioinfo_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
createticket_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
audiostore_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
trimaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
listlog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
listlogs_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
savelog_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
listservices_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
editcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
editcut_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
importcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
exportcart_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
exportpeaks_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
deleteaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
copyaudio_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
getversion_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm
getuseragent_test_LDADD  =  -lrivwebcapi -lexpat -lcurl -lm

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
