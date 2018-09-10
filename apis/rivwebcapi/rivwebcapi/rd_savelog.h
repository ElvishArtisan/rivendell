/* rd_savelog.h
 *
 * Header for the Save Log Rivendell Access Library
 *
 * (C) Copyright 2017 Todd Baker  <bakert@rfa.org>
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

#ifndef RD_SAVELOG_H
#define RD_SAVELOG_H

#include <rivwebcapi/rd_common.h>

_MYRIVLIB_INIT_DECL

struct save_loghdr_values {
  char loghdr_service[41];
  char loghdr_description[257];
  int loghdr_autorefresh;
  struct tm loghdr_purge_date;
  struct tm loghdr_start_date;
  struct tm loghdr_end_date;
};

struct save_logline_values {
  int  logline_id;
  int  logline_type;
  int  logline_source;
  unsigned logline_cart_number;
  int  logline_starttime;
  int  logline_gracetime;
  int  logline_time_type;
  int  logline_transition_type;
  int  logline_start_point_log;
  int  logline_end_point_log;
  int  logline_segue_start_point_log;
  int  logline_segue_end_point_log;
  int  logline_fadeup_point_log;
  int  logline_fadeup_gain;
  int  logline_fadedown_point_log;
  int  logline_fadedown_gain;
  int  logline_duckup_gain;
  int  logline_duckdown_gain;
  char logline_marker_comment[1021];
  char logline_marker_label[257];
  char logline_origin_user[1021];
  struct tm logline_origin_datetime;
  int  logline_event_length;
  char logline_link_event_name[257];
  struct tm logline_link_starttime;
  int  logline_link_length;
  int  logline_link_start_slop;
  int  logline_link_end_slop;
  int  logline_link_id;
  int  logline_link_embedded;
  struct tm logline_ext_starttime;
  int  logline_ext_length;
  char logline_ext_cart_name[129];
  char logline_ext_data[129];
  char logline_ext_event_id[129];
  char logline_ext_annc_type[33];
};

int RD_SaveLog(struct save_loghdr_values *hdrvals,
	       struct save_logline_values *linevals,
               unsigned linevals_quan,
	       const char hostname[],
	       const char username[],
	       const char passwd[],
	       const char ticket[],
	       const char logname[],
	       const char user_agent[]);

_MYRIVLIB_FINI_DECL


#endif  // RD_SAVELOG_H
