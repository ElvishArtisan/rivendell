/* rd_savelog.c
 *
 * Header for the Save Log Rivendell Access Library
 *
 * (C) Copyright 2017 Todd Baker  <bakert@rfa.org>
 * (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <curl/curl.h>

#include "rd_savelog.h"
#include "rd_getuseragent.h"

char *AppendString(char *str,const char *added_str)
{
  str=realloc(str,strlen(str)+strlen(added_str)+1);
  strcat(str,added_str);
  return str;
}


int RD_SaveLog(struct save_loghdr_values *hdrvals,
	       struct save_logline_values *linevals,
               unsigned linevals_quan,
	       const char hostname[],
	       const char username[],
	       const char passwd[],
	       const char ticket[],
	       const char logname[],
               const char user_agent[])
{
  char url[1500];
  char str[1024];
  char dtstr[256];
  CURL *curl=NULL;
  CURLcode res;
  char errbuf[CURL_ERROR_SIZE];
  long response_code;
  unsigned i=0;
  char user_agent_string[255];
  char buffer[1024];
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

  //
  // Log Header
  //
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"COMMAND",
        CURLFORM_COPYCONTENTS,
        "28",
        CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"LOGIN_NAME",
	CURLFORM_COPYCONTENTS,
	username,
	CURLFORM_END); 

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"PASSWORD",
        CURLFORM_COPYCONTENTS,
	passwd,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"TICKET",
        CURLFORM_COPYCONTENTS,
        ticket,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"SERVICE_NAME",
        CURLFORM_COPYCONTENTS,
	hdrvals->loghdr_service,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"DESCRIPTION",
        CURLFORM_COPYCONTENTS,
        hdrvals->loghdr_description,
	CURLFORM_END);

  snprintf(buffer,1024,"%d",hdrvals->loghdr_autorefresh);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"AUTO_REFRESH",
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_purge_date,dtstr);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"PURGE_DATE",
        CURLFORM_COPYCONTENTS,
        dtstr,
	CURLFORM_END);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_start_date,dtstr);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"START_DATE",
        CURLFORM_COPYCONTENTS,
        dtstr,
	CURLFORM_END);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_end_date,dtstr);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"END_DATE",
        CURLFORM_COPYCONTENTS,
        dtstr,
	CURLFORM_END);

  snprintf(buffer,1024,"%d",linevals_quan);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"LINE_QUANTITY",
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

  //
  // Log Lines
  //
  for(i=0;i<linevals_quan;i++) {
    snprintf(str,1024,"LINE%u_ID",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_id);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_TYPE",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_type);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_CART_NUMBER",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_cart_number);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_START_TIME",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_starttime);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_GRACE_TIME",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_gracetime);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_TIME_TYPE",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_time_type);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_TRANS_TYPE",i);
    switch(linevals[i].logline_transition_type) {
    case 0:
      snprintf(buffer,1024,"PLAY");
      break;

    case 1:
      snprintf(buffer,1024,"SEGUE");
      break;

    case 2:
      snprintf(buffer,1024,"STOP");
      break;
    }
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_START_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_start_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_END_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_end_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_SEGUE_START_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_segue_start_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_SEGUE_END_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_segue_end_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_FADEUP_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_fadeup_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_FADEUP_GAIN",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_fadeup_gain);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_FADEDOWN_POINT",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_fadedown_point_log);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_FADEDOWN_GAIN",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_fadedown_gain);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_DUCK_UP_GAIN",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_duckup_gain);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_DUCK_DOWN_GAIN",i);
    snprintf(buffer,1024,"%d",linevals[i].logline_duckdown_gain);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_COMMENT",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_marker_comment,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LABEL",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_marker_label,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_ORIGIN_USER",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_origin_user,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_ORIGIN_DATETIME",i);
    RD_Cnv_tm_to_DTString(&linevals[i].logline_origin_datetime,dtstr);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
	dtstr,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EVENT_LENGTH",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_event_length);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_EVENT_NAME",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_link_event_name,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_LENGTH",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_link_length);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_START_TIME",i);
    RD_Cnv_tm_to_DTString(&linevals[i].logline_link_starttime,dtstr);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
	dtstr,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_START_SLOP",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_link_start_slop);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_END_SLOP",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_link_end_slop);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_ID",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_link_id);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_LINK_EMBEDDED",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_link_embedded);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_START_TIME",i);
    RD_Cnv_tm_to_DTString(&linevals[i].logline_ext_starttime,dtstr);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
	dtstr,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_LENGTH",i);
    snprintf(buffer,1024,"%u",linevals[i].logline_ext_length);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        buffer,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_CART_NAME",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_ext_cart_name,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_DATA",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_ext_data,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_EVENT_ID",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_ext_event_id,
	CURLFORM_END);

    snprintf(str,1024,"LINE%u_EXT_ANNC_TYPE",i);
    curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	str,
        CURLFORM_COPYCONTENTS,
        linevals[i].logline_ext_annc_type,
	CURLFORM_END);
  }
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);

  // Check if User Agent Present otherwise set to default
  if (strlen(user_agent)> 0){
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent);
  }
  else
  {
    strcpy(user_agent_string, RD_GetUserAgent());
    strcat(user_agent_string,VERSION);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent_string);
  }

  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    #ifdef RIVC_DEBUG_OUT
        size_t len = strlen(errbuf);
        fprintf(stderr, "\nlibcurl error: (%d)", res);
        if (len)
            fprintf(stderr, "%s%s", errbuf,
                ((errbuf[len-1] != '\n') ? "\n" : ""));
        else
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
    #endif
    curl_formfree(first);
    curl_easy_cleanup(curl);
    return -1;
  }
/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_formfree(first);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {  //Success
    return 0;
  }
  return (int)response_code;
}
