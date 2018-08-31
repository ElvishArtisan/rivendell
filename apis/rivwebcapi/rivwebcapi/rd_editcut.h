/* rd_editcut.h
 *
 * Header for the Edit Cut  Rivendell Access Library
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

#ifndef RD_EDITCUT_H
#define RD_EDITCUT_H

#include <rivwebcapi/rd_common.h>

#include <curl/curl.h>

_MYRIVLIB_INIT_DECL

#include <rivwebcapi/rd_cut.h>

struct edit_cut_values {
  int cut_evergreen;
  int use_cut_evergreen;
  char cut_description[65];
  int use_cut_description;
  char cut_outcue[65];
  int use_cut_outcue;
  char cut_isrc[13];
  int use_cut_isrc;
  char cut_isci[33];
  int use_cut_isci;
  struct tm cut_start_datetime;
  int use_cut_start_datetime;
  struct tm cut_end_datetime;
  int use_cut_end_datetime;
  int cut_sun;
  int use_cut_sun;
  int cut_mon;
  int use_cut_mon;
  int cut_tue;
  int use_cut_tue;
  int cut_wed;
  int use_cut_wed;
  int cut_thu;
  int use_cut_thu;
  int cut_fri;
  int use_cut_fri;
  int cut_sat;
  int use_cut_sat;
  char cut_start_daypart[15];
  int use_cut_start_daypart;
  char cut_end_daypart[15];
  int use_cut_end_daypart;
  unsigned cut_weight;
  int use_cut_weight;
  unsigned cut_validity;
  int use_cut_validity;
  unsigned cut_coding_format;
  int use_cut_coding_format;
  unsigned cut_sample_rate;
  int use_cut_sample_rate;
  unsigned cut_bit_rate;
  int use_cut_bit_rate;
  unsigned cut_channels;
  int use_cut_channels;
  int cut_play_gain;
  int use_cut_play_gain;
  int cut_start_point;
  int use_cut_start_point;
  int cut_end_point;
  int use_cut_end_point;
  int cut_fadeup_point;
  int use_cut_fadeup_point;
  int cut_fadedown_point;
  int use_cut_fadedown_point;
  int cut_segue_start_point;
  int use_cut_segue_start_point;
  int cut_segue_end_point;
  int use_cut_segue_end_point;
  int cut_segue_gain;
  int use_cut_segue_gain;
  int cut_hook_start_point;
  int use_cut_hook_start_point;
  int cut_hook_end_point;
  int use_cut_hook_end_point;
  int cut_talk_start_point;
  int use_cut_talk_start_point;
  int cut_talk_end_point;
  int use_cut_talk_end_point;
};


int RD_EditCut(struct rd_cut *cut[],
			struct edit_cut_values edit_cut_values,
                        const char      hostname[],
                        const char      username[],
                        const char      passwd[],
			const char      ticket[],
                        const unsigned  cartnum,
                        const unsigned  cutnum,
                        const char    user_agent[],
                        unsigned        *numrecs);

void Build_Post_Cut_Fields(struct curl_httppost **first,
			   struct curl_httppost **last,
			   struct edit_cut_values edit_values);
//void Build_Post_Cut_Fields(char *post, CURL * curl, struct edit_cut_values edit_values);

_MYRIVLIB_FINI_DECL


#endif  // RD_EDITCUT_H
