/* rd_editcut.c
 *
 * Implementation of the Edit Cut Rivendell Access Library
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
#include "rd_editcut.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_cut *cut;
};


static void XMLCALL __EditCutElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"cut")==0) {    // Allocate a new cut entry
    xml_data->cut=realloc(xml_data->cut,
			   sizeof(struct rd_cut));
  }
  strncpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}

static void XMLCALL __EditCutElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __EditCutElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_cut *cut=xml_data->cut;
  char hold_datetime[25];

  if(strcasecmp(el,"cutName")==0) {
    strncpy(cut->cut_name,xml_data->strbuf,11);
  }
  if(strcasecmp(el,"cartNumber")==0) {
    sscanf(xml_data->strbuf,"%u",&cut->cut_cart_number);
  }
  if(strcasecmp(el,"cutNumber")==0){
    sscanf(xml_data->strbuf,"%u",&cut->cut_cut_number);
  }
  if(strcasecmp(el,"evergreen")==0) {
    cut->cut_evergreen=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"description")==0) {
    strncpy(cut->cut_description,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"outcue")==0) {
    strncpy(cut->cut_outcue,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"isrc")==0) {
    strncpy(cut->cut_isrc,xml_data->strbuf,13);
  }
  if(strcasecmp(el,"isci")==0) {
    strncpy(cut->cut_isci,xml_data->strbuf,33);
  }
  if(strcasecmp(el,"originDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cut->cut_origin_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"startDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cut->cut_start_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"endDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cut->cut_end_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
  if(strcasecmp(el,"sun")==0) {
    cut->cut_sun=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"mon")==0) {
    cut->cut_mon=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"tue")==0) {
    cut->cut_tue=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"wed")==0) {
    cut->cut_wed=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"thu")==0) {
    cut->cut_thu=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"fri")==0) {
    cut->cut_fri=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"sat")==0) {
    cut->cut_sat=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"startDaypart")==0) {
    strncpy(cut->cut_start_daypart,xml_data->strbuf,15);
  }
  if(strcasecmp(el,"endDaypart")==0) {
    strncpy(cut->cut_end_daypart,xml_data->strbuf,15);
  }
  if(strcasecmp(el,"originName")==0) {
    strncpy(cut->cut_origin_name,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"weight")==0) {
    sscanf(xml_data->strbuf,"%u",&cut->cut_weight);
  }
  if(strcasecmp(el,"startPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_start_point);
  }
  if(strcasecmp(el,"endPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_end_point);
  }
  if(strcasecmp(el,"fadeupPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_fadeup_point);
  }
  if(strcasecmp(el,"fadedownPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_fadedown_point);
  }
  if(strcasecmp(el,"segueStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_segue_start_point);
  }
  if(strcasecmp(el,"segueEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_segue_end_point);
  }
  if(strcasecmp(el,"segueGain")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_segue_gain);
  }
  if(strcasecmp(el,"hookStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_hook_start_point);
  }
  if(strcasecmp(el,"hookEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_hook_end_point);
  }
  if(strcasecmp(el,"talkStartPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_talk_start_point);
  }
  if(strcasecmp(el,"talkEndPoint")==0) {
    sscanf(xml_data->strbuf,"%d",&cut->cut_talk_end_point);
  }
}


size_t __EditCutCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_EditCut(struct rd_cut *cut[],
			struct edit_cut_values edit_cut_values,
		  	const char 	hostname[],
			const char 	username[],
			const char 	passwd[],
			const char      ticket[],
			const unsigned  cartnum,
			const unsigned  cutnum,
                        const char      user_agent[],
			unsigned 	*numrecs)
{
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];
  char cart_buffer[7];
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

  /*  Set number of recs so if fail already set */
  *numrecs = 0;
  /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__EditCutElementStart,
			__EditCutElementEnd);
  XML_SetCharacterDataHandler(parser,__EditCutElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"COMMAND",
        CURLFORM_COPYCONTENTS,
        "15",
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

  snprintf(cart_buffer,7,"%u",cartnum);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CART_NUMBER",
        CURLFORM_COPYCONTENTS, 
	cart_buffer,
	CURLFORM_END);

  snprintf(cart_buffer,7,"%u",cutnum);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CUT_NUMBER",
        CURLFORM_COPYCONTENTS, 
	cart_buffer,
	CURLFORM_END);
  Build_Post_Cut_Fields(&first,&last,edit_cut_values);

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
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__EditCutCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);

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
  
  if (response_code > 199 && response_code < 300) {  //Success
    *cut=xml_data.cut;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr,"rd_editcut  Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}


void Build_Post_Cut_Fields(struct curl_httppost **first,
			   struct curl_httppost **last,
			   struct edit_cut_values edit_values)
{
  char buffer[255]={0};
  char hold_datetime[34];  // take URL Encoding into account
  int  retlen=0;

  if (edit_values.use_cut_evergreen) {
    snprintf(buffer,255,"%d",edit_values.cut_evergreen);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "EVERGREEN",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_description)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "DESCRIPTION",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_description,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_outcue)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "OUTCUE",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_outcue,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_isrc)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "ISRC",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_isrc,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_isci)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "ISCI",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_isci,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_start_datetime)
  {
    retlen = RD_Cnv_tm_to_DTString( &edit_values.cut_start_datetime,hold_datetime);
    if (retlen > 0) 
    {
      curl_formadd(first,
		   last,
		   CURLFORM_PTRNAME,
		   "START_DATETIME",
		   CURLFORM_COPYCONTENTS,
		   hold_datetime,
		   CURLFORM_END); 
    }
  }

  if (edit_values.use_cut_end_datetime)
  {
    retlen = RD_Cnv_tm_to_DTString( &edit_values.cut_end_datetime,hold_datetime);
    if (retlen > 0) 
    {
      curl_formadd(first,
		   last,
		   CURLFORM_PTRNAME,
		   "END_DATETIME",
		   CURLFORM_COPYCONTENTS,
		   hold_datetime,
		   CURLFORM_END); 
    }
  }

  if (edit_values.use_cut_sun)
  {
    snprintf(buffer,255,"%d",edit_values.cut_sun);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "SUN",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_mon)
  {
    snprintf(buffer,255,"%d",edit_values.cut_mon);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "MON",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_tue)
  {
    snprintf(buffer,255,"%d",edit_values.cut_tue);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "TUE",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_wed)
  {
    snprintf(buffer,255,"%d",edit_values.cut_wed);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "WED",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_thu)
  {
    snprintf(buffer,255,"%d",edit_values.cut_thu);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "THU",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_fri)
  {
    snprintf(buffer,255,"%d",edit_values.cut_fri);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "FRI",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_sat)
  {
    snprintf(buffer,255,"%d",edit_values.cut_sat);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "SAT",
		 CURLFORM_COPYCONTENTS,
		 hold_datetime,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_start_daypart)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "START_DAYPART",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_start_daypart,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_end_daypart)
  {
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "END_DAYPART",
		 CURLFORM_COPYCONTENTS,
		 edit_values.cut_end_daypart,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_weight)
  {
    snprintf(buffer,255,"%d",edit_values.cut_weight);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "WEIGHT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_validity)
  {
    snprintf(buffer,255,"%u",edit_values.cut_validity);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "VALIDITY",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_coding_format)
  {
    snprintf(buffer,255,"%u",edit_values.cut_coding_format);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "CODING_FORMAT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_bit_rate)
  {
    snprintf(buffer,255,"%u",edit_values.cut_bit_rate);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "BIT_RATE",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_channels)
  {
    snprintf(buffer,255,"%u",edit_values.cut_channels);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "CHANNELS",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_play_gain)
  {
    snprintf(buffer,255,"%d",edit_values.cut_play_gain);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "PLAY_GAIN",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_start_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_start_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "START_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_end_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_end_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "END_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_fadeup_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_fadeup_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "FADEUP_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_fadedown_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_fadedown_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "FADEDOWN_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_segue_start_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_segue_start_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "SEGUE_START_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_segue_end_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_segue_end_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "SEGUE_END_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END);
  }

  if (edit_values.use_cut_segue_gain)
  {
    snprintf(buffer,255,"%d",edit_values.cut_segue_gain);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "SEGUE_GAIN",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_hook_start_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_hook_start_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "HOOK_START_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }

  if (edit_values.use_cut_hook_end_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_hook_end_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "HOOK_END_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }


  if (edit_values.use_cut_talk_start_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_talk_start_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "TALK_START_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END);
  }

  if (edit_values.use_cut_talk_end_point)
  {
    snprintf(buffer,255,"%d",edit_values.cut_talk_end_point);
    curl_formadd(first,
		 last,
		 CURLFORM_PTRNAME,
		 "FALK_END_POINT",
		 CURLFORM_COPYCONTENTS,
		 buffer,
		 CURLFORM_END); 
  }
}
