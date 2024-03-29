/* rd_import.c
 *
 * Implementation of the save RSS item audio Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>
 * (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include "rd_savepodcast.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
};

static void XMLCALL __SavePodcastElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}

static void XMLCALL __SavePodcastElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}

static void XMLCALL __SavePodcastElementEnd(void *data, const char *el)
{
  //  struct xml_data *xml_data=(struct xml_data *)data;

}


size_t __SavePodcastCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}



int RD_SavePodcast(const char hostname[],
		   const char username[],
		   const char passwd[],
		   const char ticket[],
		   const unsigned cast_id,
		   const char filename[],
		   const char user_agent[])
{
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;
  char id_buffer[21];
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

//
// Generate POST Data
//
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"COMMAND",
        CURLFORM_COPYCONTENTS,
        "38",
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

  sprintf(id_buffer,"%u",cast_id);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"ID",
        CURLFORM_COPYCONTENTS, 
	id_buffer,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"FILENAME",
        CURLFORM_FILE,
	filename,
	CURLFORM_END);
  
  /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__SavePodcastElementStart,
			__SavePodcastElementEnd);
  XML_SetCharacterDataHandler(parser,__SavePodcastElementData);

  //curl_easy_setopt(curl, CURLOPT_WRITEDATA, stderr);  Debug try
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,1200);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__SavePodcastCallback);

  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);

  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_VERBOSE,0);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);

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
  curl_formfree(first);
  curl_easy_cleanup(curl);
  if (response_code > 199 && response_code < 300) {
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_savepodcast Call Returned Error: %s\n",xml_data.strbuf);
    #endif

    return (int)response_code;
  }
}
