/* rd_listlogs.c
 *
 * Implementation of the ListLogs Rivendell Access Library
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

#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <expat.h>

#include "rd_common.h"
#include "rd_getuseragent.h"
#include "rd_listlogs.h"

struct xml_data {
  unsigned logs_quan;
  char elem_name[256];
  char strbuf[1024];
  struct rd_log *logs;
};


static void XMLCALL __ListLogsElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"log")==0) {    // Allocate a new log entry
    xml_data->logs=realloc(xml_data->logs,
			   (xml_data->logs_quan+1)*sizeof(struct rd_log));
    (xml_data->logs_quan)++;
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}


static void XMLCALL __ListLogsElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __ListLogsElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_log *logs=xml_data->logs+(xml_data->logs_quan-1);
  char hold_datetime[25];

  if(strcasecmp(el,"name")==0) {
    strlcpy(logs->log_name,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"serviceName")==0) {
    strlcpy(logs->log_service,xml_data->strbuf,10);
  }
  if(strcasecmp(el,"description")==0) {
    strlcpy(logs->log_description,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"originUserName")==0) {
    strlcpy(logs->log_origin_username,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"originDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_origin_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"purgeDate")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_purge_date = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"linkDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_link_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"modifiedDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_modified_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"autoRefresh")==0) {
    logs->log_autorefresh=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"startDate")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_startdate = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"endDate")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    logs->log_enddate = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"scheduledTracks")==0) {
    sscanf(xml_data->strbuf,"%d",&logs->log_scheduled_tracks);
  }
  if(strcasecmp(el,"completedTracks")==0) {
    sscanf(xml_data->strbuf,"%d",&logs->log_completed_tracks);
  }
  if(strcasecmp(el,"musicLinks")==0) {
    sscanf(xml_data->strbuf,"%d",&logs->log_music_links);
  }
  if(strcasecmp(el,"musicLinked")==0) {
    logs->log_music_linked=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"trafficLinks")==0) {
    sscanf(xml_data->strbuf,"%d",&logs->log_traffic_links);
  }
  if(strcasecmp(el,"trafficLinked")==0) {
    logs->log_traffic_linked=RD_ReadBool(xml_data->strbuf);
  }
}


size_t __ListLogsCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


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
		unsigned *numrecs)
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  char checked_service[11]={0};
  char *check_svc = &checked_service[0];
  char checked_logname[65]={0};
  char *check_logname = &checked_logname[0];
  int checked_trackable = 0;
  int checked_recent = 0;
  struct xml_data xml_data;
  long response_code;
  int i;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];

  /*  Set number of recs so if fail already set */
  *numrecs = 0;

  if (trackable == 1) {
    checked_trackable = 1;
  }
  if (recent == 1) {
    checked_recent = 1;
  }
  memset(checked_service,'\0',sizeof(checked_service)); 
  if ((strlen(servicename) > 0) && 
      (strlen(servicename) < 11))  {
    for (i = 0; i<strlen(servicename);i++) {
      if (servicename[i]>32) {
        strncpy(check_svc,&servicename[i],1);
        check_svc++;
      }
    }
  }
  if ((strlen(logname) > 0) && 
      (strlen(logname) < 65))  {
    for (i = 0; i<strlen(logname);i++) {
      if (logname[i]>32) {
        strncpy(check_logname,&logname[i],1);
        check_logname++;
      }
    }
  }

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }
    
   /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__ListLogsElementStart,
			__ListLogsElementEnd);
  XML_SetCharacterDataHandler(parser,__ListLogsElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=20&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&SERVICE_NAME=%s&LOG_NAME=%s&TRACKABLE=%d&FILTER=%s&RECENT=%d",
	   curl_easy_escape(curl,username,0),
	   curl_easy_escape(curl,passwd,0),
	   curl_easy_escape(curl,ticket,0),
	   curl_easy_escape(curl,checked_service,0),
	   curl_easy_escape(curl,checked_logname,0),
	   checked_trackable,
	   curl_easy_escape(curl,filter,0),
	   checked_recent);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__ListLogsCallback);
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
  
  if (response_code > 199 && response_code < 300) {
    *logs=xml_data.logs;
    *numrecs = xml_data.logs_quan;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_listlogs Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}
