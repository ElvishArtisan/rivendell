/* rd_listcuts.c
 *
 * Implementation of the LastCuts Rivendell Access Library
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
#include "rd_listcut.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_cut *cuts;
};


static void XMLCALL __ListCutElementStart(void *data, const char *el, 
					     const char **attr)
{
  unsigned i;
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"cut")==0) {    // Allocate a new cut entry
    xml_data->cuts=realloc(xml_data->cuts,
			   sizeof(struct rd_cut));
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}


static void XMLCALL __ListCutElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __ListCutElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_cut *cuts=xml_data->cuts;
  char hold_datetime[25];

  if(strcasecmp(el,"cutName")==0) {
    strlcpy(cuts->cut_name,xml_data->strbuf,11);
  }
  if(strcasecmp(el,"cartNumber")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_cart_number);
  }
  if(strcasecmp(el,"cutNumber")==0){
    sscanf(xml_data->strbuf,"%u",&cuts->cut_cut_number);
  }
  if(strcasecmp(el,"evergreen")==0) {
    cuts->cut_evergreen=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"description")==0) {
    strlcpy(cuts->cut_description,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"outcue")==0) {
    strlcpy(cuts->cut_outcue,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"isrc")==0) {
    strlcpy(cuts->cut_isrc,xml_data->strbuf,13);
  }
  if(strcasecmp(el,"isci")==0) {
    strlcpy(cuts->cut_isci,xml_data->strbuf,33);
  }
  if(strcasecmp(el,"length")==0){
    sscanf(xml_data->strbuf,"%u",&cuts->cut_length);
  }
  if(strcasecmp(el,"originDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cuts->cut_origin_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"startDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cuts->cut_start_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"endDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cuts->cut_end_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"sun")==0) {
    cuts->cut_sun=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"mon")==0) {
    cuts->cut_mon=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"tue")==0) {
    cuts->cut_tue=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"wed")==0) {
    cuts->cut_wed=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"thu")==0) {
    cuts->cut_thu=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"fri")==0) {
    cuts->cut_fri=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"sat")==0) {
    cuts->cut_sat=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"startDaypart")==0) {
    strlcpy(cuts->cut_start_daypart,xml_data->strbuf,15);
  }
  if(strcasecmp(el,"endDaypart")==0) {
    strlcpy(cuts->cut_end_daypart,xml_data->strbuf,15);
  }
  if(strcasecmp(el,"originName")==0) {
    strlcpy(cuts->cut_origin_name,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"originLoginName")==0) {
    strlcpy(cuts->cut_origin_login_name,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"sourceHostname")==0) {
    strlcpy(cuts->cut_source_hostname,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"weight")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_weight);
  }
  if(strcasecmp(el,"lastPlayDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cuts->cut_last_play_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"playCounter")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_play_counter);
  }
  if(strcasecmp(el,"localCounter")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_local_counter);
  }
  if(strcasecmp(el,"validity")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_validity);
  }
  if(strcasecmp(el,"codingFormat")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_coding_format);
  }
  if(strcasecmp(el,"sampleRate")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_sample_rate);
  }
  if(strcasecmp(el,"bitRate")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_bit_rate);
  }
  if(strcasecmp(el,"channels")==0) {
    sscanf(xml_data->strbuf,"%u",&cuts->cut_channels);
  }
  if(strcasecmp(el,"playGain")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_play_gain);
  }
  if(strcasecmp(el,"startPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_start_point);
  }
  if(strcasecmp(el,"endPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_end_point);
  }
  if(strcasecmp(el,"fadeupPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_fadeup_point);
  }
  if(strcasecmp(el,"fadedownPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_fadedown_point);
  }
  if(strcasecmp(el,"segueStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_segue_start_point);
  }
  if(strcasecmp(el,"segueEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_segue_end_point);
  }
  if(strcasecmp(el,"segueGain")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_segue_gain);
  }
  if(strcasecmp(el,"hookStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_hook_start_point);
  }
  if(strcasecmp(el,"hookEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_hook_end_point);
  }
  if(strcasecmp(el,"talkStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_talk_start_point);
  }
  if(strcasecmp(el,"talkEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cuts->cut_talk_end_point);
  }
}


size_t __ListCutCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_ListCut(struct rd_cut *cuts[],
		  	const char hostname[],
                  	const char username[],
                  	const char passwd[],
			const char ticket[],
                  	const unsigned cartnumber,
			const unsigned cutnumber,
                        const char user_agent[],
                  	unsigned *numrecs)
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];

   /* Set number of recs so if fail already set */
  *numrecs = 0;

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
  XML_SetElementHandler(parser,__ListCutElementStart,
			__ListCutElementEnd);
  XML_SetCharacterDataHandler(parser,__ListCutElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=8&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&CART_NUMBER=%u&CUT_NUMBER=%u",
	curl_easy_escape(curl,username,0),
	curl_easy_escape(curl,passwd,0),
	curl_easy_escape(curl,ticket,0),
	cartnumber,
	cutnumber);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__ListCutCallback);
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
    *cuts=xml_data.cuts;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_listcut Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}
