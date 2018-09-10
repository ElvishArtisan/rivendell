/* rd_listlogs.h
 *
 * Header for the ListLogs  Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef RD_LISTLOGS_H
#define RD_LISTLOGS_H

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

struct rd_log {
  char  log_name[257];
  char  log_service[41];
  char  log_description[257];
  char  log_origin_username[1021];
  struct tm log_origin_datetime;
  struct tm  log_purge_date;
  struct tm  log_link_datetime;
  struct tm  log_modified_datetime;
  int   log_autorefresh;
  struct tm  log_startdate;
  struct tm  log_enddate;
  int   log_scheduled_tracks;
  int   log_completed_tracks;
  int   log_music_links;
  int   log_music_linked;
  int   log_traffic_links;
  int   log_traffic_linked;
};

int RD_ListLogs(struct rd_log *logs[],
		const char hostname[],
		const char username[],
		const char passwd[],
		const char ticket[],
		const char servicename[],
		const char logname[],
		const int  trackable,
		const char filter[],
		const int recent,
		const char user_agent[],
		unsigned *numrecs);

_MYRIVLIB_FINI_DECL


#endif  // RD_LISTLOGS_H
