/* rd_savelog.c
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
  char *post=malloc(1);
  CURL *curl=NULL;
  CURLcode res;
  char errbuf[CURL_ERROR_SIZE];
  long response_code;
  unsigned i=0;
  post[0]=0;
  char user_agent_string[255];

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

  //
  // Log Header
  //
  post=AppendString(post,"COMMAND=28&");

  snprintf(str,1024,"LOGIN_NAME=%s&",curl_easy_escape(curl,username,0));
  post=AppendString(post,str);

  snprintf(str,1024,"PASSWORD=%s&",curl_easy_escape(curl,passwd,0));
  post=AppendString(post,str);

  snprintf(str,1024,"TICKET=%s&",curl_easy_escape(curl,ticket,0));
  post=AppendString(post,str);

  snprintf(str,1024,"LOG_NAME=%s&",curl_easy_escape(curl,logname,0));
  post=AppendString(post,str);

  snprintf(str,1024,"SERVICE_NAME=%s&",curl_easy_escape(curl,hdrvals->loghdr_service,0));
  post=AppendString(post,str);

  snprintf(str,1024,"DESCRIPTION=%s&",curl_easy_escape(curl,hdrvals->loghdr_description,0));
  post=AppendString(post,str);

  snprintf(str,1024,"AUTO_REFRESH=%d&",hdrvals->loghdr_autorefresh);
  post=AppendString(post,str);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_purge_date,dtstr);
  snprintf(str,1024,"PURGE_DATE=%s&",dtstr);
  post=AppendString(post,str);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_start_date,dtstr);
  snprintf(str,1024,"START_DATE=%s&",dtstr);
  post=AppendString(post,str);

  RD_Cnv_tm_to_DTString(&hdrvals->loghdr_end_date,dtstr);
  snprintf(str,1024,"END_DATE=%s&",dtstr);
  post=AppendString(post,str);

  snprintf(str,1024,"LINE_QUANTITY=%d",linevals_quan);
  post=AppendString(post,str);

  //
  // Log Lines
  //
  for(i=0;i<linevals_quan;i++) {
    snprintf(str,1024,"&LINE%u_ID=%u",i,linevals[i].logline_id);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_TYPE=%u",i,linevals[i].logline_type);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_CART_NUMBER=%u",i,
	     linevals[i].logline_cart_number);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_START_TIME=%u",i,linevals[i].logline_starttime);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_GRACE_TIME=%u",i,linevals[i].logline_gracetime);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_TIME_TYPE=%u",i,linevals[i].logline_time_type);
    post=AppendString(post,str);

    switch(linevals[i].logline_transition_type) {
    case 0:
      snprintf(str,1024,"&LINE%u_TRANS_TYPE=PLAY",i);
      break;

    case 1:
      snprintf(str,1024,"&LINE%u_TRANS_TYPE=SEGUE",i);
      break;

    case 2:
      snprintf(str,1024,"&LINE%u_TRANS_TYPE=STOP",i);
      break;
    }
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_START_POINT=%d",i,
	     linevals[i].logline_start_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_END_POINT=%d",i,
	     linevals[i].logline_end_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_SEGUE_START_POINT=%d",i,
	     linevals[i].logline_segue_start_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_SEGUE_END_POINT=%d",i,
	     linevals[i].logline_segue_end_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_FADEUP_POINT=%d",i,
	     linevals[i].logline_fadeup_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_FADEUP_GAIN=%d",i,
	     linevals[i].logline_fadeup_gain);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_FADEDOWN_POINT=%d",i,
	     linevals[i].logline_fadedown_point_log);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_FADEDOWN_GAIN=%d",i,
	     linevals[i].logline_fadedown_gain);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_DUCK_UP_GAIN=%d",i,
	     linevals[i].logline_duckup_gain);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_DUCK_DOWN_GAIN=%d",i,
	     linevals[i].logline_duckdown_gain);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_COMMENT=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_marker_comment,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LABEL=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_marker_label,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_ORIGIN_USER=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_origin_user,0));
    post=AppendString(post,str);

    RD_Cnv_tm_to_DTString(&linevals[i].logline_origin_datetime,dtstr);
    snprintf(str,1024,"&LINE%u_ORIGIN_DATETIME=%s",i,dtstr);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_ORIGIN_USER=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_origin_user,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EVENT_LENGTH=%u",i,
	     linevals[i].logline_event_length);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_EVENT_NAME=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_link_event_name,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_LENGTH=%u",i,
	     linevals[i].logline_link_length);
    post=AppendString(post,str);

    RD_Cnv_tm_to_DTString(&linevals[i].logline_link_starttime,dtstr);
    snprintf(str,1024,"&LINE%u_LINK_START_TIME=%s",i,dtstr);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_START_SLOP=%u",i,
	     linevals[i].logline_link_start_slop);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_END_SLOP=%u",i,
	     linevals[i].logline_link_end_slop);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_ID=%u",i,
	     linevals[i].logline_link_id);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_LINK_EMBEDDED=%u",i,
	     linevals[i].logline_link_embedded);
    post=AppendString(post,str);

    RD_Cnv_tm_to_DTString(&linevals[i].logline_ext_starttime,dtstr);
    snprintf(str,1024,"&LINE%u_EXT_START_TIME=%s",i,dtstr);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EXT_LENGTH=%u",i,
	     linevals[i].logline_ext_length);
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EXT_CART_NAME=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_ext_cart_name,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EXT_DATA=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_ext_data,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EXT_EVENT_ID=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_ext_event_id,0));
    post=AppendString(post,str);

    snprintf(str,1024,"&LINE%u_EXT_ANNC_TYPE=%s",i,
	     curl_easy_escape(curl,linevals[i].logline_ext_annc_type,0));
    post=AppendString(post,str);
  }

  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
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
    curl_easy_cleanup(curl);
    return -1;
  }
/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {  //Success
    return 0;
  }
  return (int)response_code;
}
