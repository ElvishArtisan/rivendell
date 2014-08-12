## automake.am
##
## Scripts Directory Makefile.am for Rivendell
##
## (C) Copyright 2002-2012 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.20.6.5 2012/11/29 01:37:37 cvs Exp $
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

install-exec-local:	
	mkdir -p /etc/X11/xinit/xinitrc.d
	cp start-rdmonitor.sh /etc/X11/xinit/xinitrc.d/

uninstall-local:	
	rm -f /etc/X11/xinit/xinitrc.d/start-rdmonitor.sh

bin_SCRIPTS = crc-unity4k.sh\
              rd_audio_sync\
              rd_config\
              rd_backup\
              rdmemcheck.sh\
              sage_endec_rwt.sh

EXTRA_DIST = crc-unity4k.sh\
             kill_rd\
             rd_audio_sync\
             rd_backup\
             rd_backup_system.sh\
             rd_config\
             rd_create_db\
             rd_memmon.sh\
             rd_mysql_enable_host.sh\
             rd_restore_system.sh\
             rdmemcheck.sh\
             sage_endec_rwt.sh\
             start_rd\
             start_traverso.sh\
             start-rdmonitor.sh

CLEANFILES = *~

MAINTAINERCLEANFILES = *~\
                       Makefile.in
