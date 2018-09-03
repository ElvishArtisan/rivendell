/* rd_import.c
 *
 * Implementation of the Import Cart Rivendell Access Library
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

#include "rd_import.h"
#include "rd_getuseragent.h"
#include "rd_common.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_cartimport *cartimport;
};


static void XMLCALL __ImportCartElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  // Always allocate - because even if error we want to get error string
  // if(strcasecmp(el,"RDWebResult")==0) {    // Allocate a new cart entry
  //
  xml_data->cartimport=realloc(xml_data->cartimport, sizeof(struct rd_cartimport));
  //}
  strncpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}

static void XMLCALL __ImportCartElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}

static void XMLCALL __ImportCartElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_cartimport *cartimport=xml_data->cartimport;

  if(strcasecmp(el,"CartNumber")==0) {
    sscanf(xml_data->strbuf,"%u",&cartimport->cart_number);
  }
  if(strcasecmp(el,"CutNumber")==0) {
    sscanf(xml_data->strbuf,"%u",&cartimport->cut_number);
  }

  if(strcasecmp(el,"ErrorString")==0) {
    strlcpy(cartimport->error_string,xml_data->strbuf,256);
  }

}


size_t __ImportCartCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_ImportCart(struct rd_cartimport *cartimport[],
                        const char hostname[],
			const char         username[],
			const char           passwd[],
			const char           ticket[],
			const unsigned        cartnum,
			const unsigned         cutnum,
			const unsigned       channels,
			const int normalization_level,
			const int autotrim_level,
                        const int  use_metadata,
                        const int  create,
                        const char group[],
                        const char title[],
                        const char filename[],
                        const char user_agent[],
			unsigned *numrecs)
{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  XML_Parser parser;
  struct xml_data xml_data;
  long response_code;
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;
  char *arrayptr;
  char checked_fname[BUFSIZ];
  int i;
  char cart_buffer[50];
  char cut_buffer[50];
  char channels_buffer[50];
  char normalization_buffer[50];
  char autotrim_buffer[50];
  char use_metadata_buffer[50];
  char create_flag[50];
  char checked_group_name[50];
  long userlen = strlen(username);
  long passwdlen = strlen(passwd);
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];

  /*   Check File name */
  memset(checked_fname,'\0',sizeof(checked_fname));
  arrayptr=&checked_fname[0];

  for (i = 0 ; i < strlen(filename) ; i++) {
    if (filename[i]>32) {
      strncpy(arrayptr,&filename[i],1);
      arrayptr++;
    }
  }
  
  /*   Check Group Name */
  memset(checked_group_name,'\0',sizeof(checked_group_name));
  arrayptr=&checked_group_name[0];

  for (i = 0 ; i < strlen(group) ; i++) {
    if (group[i]>32) {
      strncpy(arrayptr,&group[i],1);
      arrayptr++;
    }
  }

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

//
// Generate POST Data
//
// We have to use multipart here because we have a file to send.
//
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"COMMAND",
        CURLFORM_COPYCONTENTS,
        "2",
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

  sprintf(cart_buffer,"%u",cartnum);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CART_NUMBER",
        CURLFORM_COPYCONTENTS, 
	cart_buffer,
	CURLFORM_END);

  sprintf(cut_buffer,"%u",cutnum);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CUT_NUMBER",
        CURLFORM_COPYCONTENTS,
	cut_buffer,
	CURLFORM_END);

  sprintf(channels_buffer,"%u",channels);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CHANNELS",
        CURLFORM_COPYCONTENTS, 
	channels_buffer,
	CURLFORM_END);

  sprintf(normalization_buffer,"%d",normalization_level);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"NORMALIZATION_LEVEL",
        CURLFORM_COPYCONTENTS, 
	normalization_buffer, 
	CURLFORM_END);

  sprintf(autotrim_buffer,"%d",autotrim_level);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"AUTOTRIM_LEVEL",
        CURLFORM_COPYCONTENTS,  
	autotrim_buffer,
	CURLFORM_END);

  sprintf(use_metadata_buffer,"%d",use_metadata);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"USE_METADATA",
        CURLFORM_COPYCONTENTS, 
	use_metadata_buffer, 
	CURLFORM_END);

  sprintf(create_flag,"%d",create);
  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"CREATE",
        CURLFORM_COPYCONTENTS, 
	create_flag, 
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"GROUP_NAME",
        CURLFORM_COPYCONTENTS,
	checked_group_name,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"TITLE",
        CURLFORM_COPYCONTENTS,
        title,
	CURLFORM_END);

  curl_formadd(&first,
	&last,
	CURLFORM_PTRNAME,
	"FILENAME",
        CURLFORM_FILE,
	checked_fname,
	CURLFORM_END);
  

  /*  Set number of recs so if fail already set */
  *numrecs = 0;

  /*
   * Setup the CURL call
   */
  memset(&xml_data,0,sizeof(xml_data));
  parser=XML_ParserCreate(NULL);
  XML_SetUserData(parser,&xml_data);
  XML_SetElementHandler(parser,__ImportCartElementStart,
			__ImportCartElementEnd);
  XML_SetCharacterDataHandler(parser,__ImportCartElementData);

  //curl_easy_setopt(curl, CURLOPT_WRITEDATA, stderr);  Debug try
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,1200);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__ImportCartCallback);

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
    *cartimport=xml_data.cartimport;
    *numrecs = 1;
  return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_import Call Returned Error: %s\n",xml_data.strbuf);		
    #endif
    *cartimport=xml_data.cartimport;
    *numrecs = 0;
    return (int)response_code;
  }
}
