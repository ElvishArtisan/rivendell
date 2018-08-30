/* rd_addcart.c
 *
 * Implementation of the Add Cart Rivendell Access Library
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

#include "rd_addcart.h"
#include "rd_common.h"
#include "rd_getuseragent.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_cart *cart;
};

static void XMLCALL __AddCartElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"cart")==0) {    // Allocate a new cart entry
    xml_data->cart=realloc(xml_data->cart,
			   sizeof(struct rd_cart));
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}

static void XMLCALL __AddCartElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __AddCartElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_cart *cart=xml_data->cart;
  char hold_datetime[25];

  if(strcasecmp(el,"number")==0) {
    sscanf(xml_data->strbuf,"%u",&cart->cart_number);
  }
  if(strcasecmp(el,"type")==0) {
    if(strcasecmp(xml_data->strbuf,"audio")==0) {
      cart->cart_type=TYPE_AUDIO;
    }
    else {
      if(strcasecmp(xml_data->strbuf,"macro")==0) {
        cart->cart_type=TYPE_MACRO;
      }
      else
      {
        /*  This is ALL type */
        cart->cart_type=TYPE_ALL;
      }
    }
  }

  if(strcasecmp(el,"groupName")==0) {
    strlcpy(cart->cart_grp_name,xml_data->strbuf,11);
  }
  if(strcasecmp(el,"title")==0) {
    strlcpy(cart->cart_title,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"artist")==0) {
    strlcpy(cart->cart_artist,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"album")==0) {
    strlcpy(cart->cart_album,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"year")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_year);
  }
  if(strcasecmp(el,"label")==0) {
    strlcpy(cart->cart_label,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"client")==0) {
    strlcpy(cart->cart_client,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"agency")==0) {
    strlcpy(cart->cart_agency,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"publisher")==0) {
    strlcpy(cart->cart_publisher,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"composer")==0) {
    strlcpy(cart->cart_composer,xml_data->strbuf,65);
  }
  if(strcasecmp(el,"userDefined")==0) {
    strlcpy(cart->cart_user_defined,xml_data->strbuf,256);
  }
  if(strcasecmp(el,"usageCode")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_usage_code);
  }
  if(strcasecmp(el,"forcedLength")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_forced_length);
  }
  if(strcasecmp(el,"averageLength")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_average_length);
  }
  if(strcasecmp(el,"lengthDeviation")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_length_deviation);
  }
  if(strcasecmp(el,"averageSegueLength")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_average_segue_length);
  }
  if(strcasecmp(el,"averageHookLength")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_average_hook_length);
  }
  if(strcasecmp(el,"cutQuantity")==0) {
    sscanf(xml_data->strbuf,"%u",&cart->cart_cut_quantity);
  }
  if(strcasecmp(el,"lastCutPlayed")==0) {
    sscanf(xml_data->strbuf,"%u",&cart->cart_last_cut_played);
  }
  if(strcasecmp(el,"validity")==0) {
    sscanf(xml_data->strbuf,"%u",&cart->cart_validity);
  }
  if(strcasecmp(el,"enforceLength")==0) {
    cart->cart_enforce_length=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"asyncronous")==0) {
    cart->cart_asyncronous=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"owner")==0) {
    strlcpy(cart->cart_owner,xml_data->strbuf,66);
  }
  if(strcasecmp(el,"metadataDatetime")==0) {
    strlcpy(hold_datetime,xml_data->strbuf,26);
    cart->cart_metadata_datetime = RD_Cnv_DTString_to_tm(hold_datetime);
  }
}


size_t __AddCartCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_AddCart(struct rd_cart *cart[],
		  	const char hostname[],
			const char username[],
			const char passwd[],
			const char ticket[],
			const char group[],
			const char type[],
                  	const unsigned cartnumber,
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
  XML_SetElementHandler(parser,__AddCartElementStart,
			__AddCartElementEnd);
  XML_SetCharacterDataHandler(parser,__AddCartElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=12&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&GROUP_NAME=%s&TYPE=%s&CART_NUMBER=%u",
	curl_easy_escape(curl,username,0),
	curl_easy_escape(curl,passwd,0),
	curl_easy_escape(curl,ticket,0), 
	curl_easy_escape(curl,group,0),
	type,
	cartnumber);

  // Check if User Agent Present otherwise set to default
  if (strlen(user_agent)> 0){
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent);
  }
  else
  {
    strcpy(user_agent_string,RD_GetUserAgent());
    strcat(user_agent_string,VERSION);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,user_agent_string);
  }

  curl_easy_setopt(curl,CURLOPT_WRITEDATA,parser);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__AddCartCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
  //  curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    size_t len = strlen(errbuf);
    #ifdef RIVC_DEBUG_OUT
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
    *cart=xml_data.cart;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr,"rd_addcart Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}
