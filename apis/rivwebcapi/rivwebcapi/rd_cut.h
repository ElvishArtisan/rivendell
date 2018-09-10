/* rd_cut.h
 *
 * Header for the RDCUT  Rivendell Access Library
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

#ifndef RD_CUT_H
#define RD_CUT_H

#include <time.h>

struct rd_cut {
  char cut_name[11];
  unsigned cut_cart_number;
  unsigned cut_cut_number;
  int cut_evergreen;
  char cut_description[257];
  char cut_outcue[257];
  char cut_isrc[49];
  char cut_isci[129];
  unsigned  cut_length;
  struct tm cut_origin_datetime;
  struct tm cut_start_datetime;
  struct tm cut_end_datetime;
  int cut_sun;
  int cut_mon;
  int cut_tue;
  int cut_wed;
  int cut_thu;
  int cut_fri;
  int cut_sat;
  char cut_start_daypart[15];
  char cut_end_daypart[15];
  char cut_origin_name[257];
  char cut_origin_login_name[1021];
  char cut_source_hostname[1021];
  unsigned cut_weight;
  struct tm cut_last_play_datetime;
  unsigned cut_play_counter;
  unsigned cut_local_counter;
  unsigned cut_validity;
  unsigned cut_coding_format;
  unsigned cut_sample_rate;
  unsigned cut_bit_rate;
  unsigned cut_channels;
  int cut_play_gain; 
  int cut_start_point; 
  int cut_end_point; 
  int cut_fadeup_point; 
  int cut_fadedown_point; 
  int cut_segue_start_point; 
  int cut_segue_end_point; 
  int cut_segue_gain; 
  int cut_hook_start_point; 
  int cut_hook_end_point; 
  int cut_talk_start_point;
  int cut_talk_end_point;
};


#endif  // RD_CUT_H
