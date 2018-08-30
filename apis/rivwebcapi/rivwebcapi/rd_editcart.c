/* rd_editcart.c
 *
 * Implementation of the Edit Cart Rivendell Access Library
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
#include "rd_editcart.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_cart *cart;
};


static void XMLCALL __EditCartElementStart(void *data, const char *el, 
					     const char **attr)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"cart")==0) {    // Allocate a new cart entry
    xml_data->cart=realloc(xml_data->cart,
			   sizeof(struct rd_cart));
    memset(xml_data->cart,0,sizeof(struct rd_cart)); 
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}

static void XMLCALL __EditCartElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __EditCartElementEnd(void *data, const char *el)
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
    strlcpy(cart->cart_grp_name,xml_data->strbuf,10);
  }
  if(strcasecmp(el,"title")==0) {
    strlcpy(cart->cart_title,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"artist")==0) {
    strlcpy(cart->cart_artist,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"album")==0) {
    strlcpy(cart->cart_album,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"year")==0) {
    sscanf(xml_data->strbuf,"%d",&cart->cart_year);
  }
  if(strcasecmp(el,"label")==0) {
    strlcpy(cart->cart_label,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"client")==0) {
    strlcpy(cart->cart_client,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"agency")==0) {
    strlcpy(cart->cart_agency,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"publisher")==0) {
    strlcpy(cart->cart_publisher,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"composer")==0) {
    strlcpy(cart->cart_composer,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"conductor")==0) {
    strlcpy(cart->cart_conductor,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"userDefined")==0) {
    strlcpy(cart->cart_user_defined,xml_data->strbuf,255);
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
    strlcpy(cart->cart_owner,xml_data->strbuf,64);
  }
  if(strcasecmp(el,"notes")==0 ){
    /* handle multiple NOTE Lines */
    strlcpy(cart->cart_notes,xml_data->strbuf,1024);
  }
}


size_t __EditCartCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_EditCart(struct rd_cart *cart[],
			struct edit_cart_values edit_c_values,
		  	const char hostname[],
			const char username[],
			const char passwd[],
			const char ticket[],
			const unsigned cartnum,
                        const char user_agent[],
			unsigned *numrecs)
{
  char post[3350];
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
  XML_SetElementHandler(parser,__EditCartElementStart,
			__EditCartElementEnd);
  XML_SetCharacterDataHandler(parser,__EditCartElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,3350,"COMMAND=14&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&CART_NUMBER=%u",
	   	curl_easy_escape(curl,username,0),
		curl_easy_escape(curl,passwd,0),
	        curl_easy_escape(curl,ticket,0),
		cartnum);
  Build_Post_Cart_Fields(post,curl,edit_c_values);
  
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
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__EditCartCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,errbuf);
 
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
    *cart=xml_data.cart;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_editcart Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}

void Build_Post_Cart_Fields(char *post, CURL *curl, struct edit_cart_values edit_values)
{ 
  char buffer[1024];
  /*  Copy all of the applicable values into the post string */

  if (edit_values.use_cart_grp_name)
  {
    snprintf(buffer,1024,"&GROUP_NAME=%s",curl_easy_escape(curl,edit_values.cart_grp_name,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_title)
  {
    snprintf(buffer,1024,"&TITLE=%s",curl_easy_escape(curl,edit_values.cart_title,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_artist)
  {
    snprintf(buffer,1024,"&ARTIST=%s",curl_easy_escape(curl,edit_values.cart_artist,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_album)
  {
    snprintf(buffer,1024,"&ALBUM=%s",curl_easy_escape(curl,edit_values.cart_album,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_year)
  {
    snprintf(buffer,1024,"&YEAR=%d",edit_values.cart_year);
    strcat(post,buffer);
  }
  if (edit_values.use_cart_label)
  {
    snprintf(buffer,1024,"&LABEL=%s",curl_easy_escape(curl,edit_values.cart_label,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_client)
  {
    snprintf(buffer,1024,"&CLIENT=%s",curl_easy_escape(curl,edit_values.cart_client,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_agency)
  {
    snprintf(buffer,1024,"&AGENCY=%s",curl_easy_escape(curl,edit_values.cart_agency,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_publisher)
  {
    snprintf(buffer,1024,"&PUBLISHER=%s",curl_easy_escape(curl,edit_values.cart_publisher,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_composer)
  {
    snprintf(buffer,1024,"&COMPOSER=%s",curl_easy_escape(curl,edit_values.cart_composer,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_conductor)
  {
    snprintf(buffer,1024,"&CONDUCTOR=%s",curl_easy_escape(curl,edit_values.cart_conductor,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_user_defined)
  {
    snprintf(buffer,1024,"&USER_DEFINED=%s",curl_easy_escape(curl,edit_values.cart_user_defined,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_usage_code)
  {
    snprintf(buffer,1024,"&USAGE_CODE=%d",edit_values.cart_usage_code);
    strcat(post,buffer);
  }
  if (edit_values.use_cart_forced_length)
  {
    snprintf(buffer,1024,"&FORCED_LENGTH=%d",edit_values.cart_forced_length);
    strcat(post,buffer);
  }
  if (edit_values.use_cart_enforce_length)
  {
    snprintf(buffer,1024,"&ENFORCE_LENGTH=%d",edit_values.cart_enforce_length);
    strcat(post,buffer);
  }
  if (edit_values.use_cart_asyncronous)
  {
    snprintf(buffer,1024,"&ASYNCRONOUS=%d",edit_values.cart_asyncronous);
    strcat(post,buffer);
  }
  if (edit_values.use_cart_owner)
  {
    snprintf(buffer,1024,"&OWNER=%s",curl_easy_escape(curl,edit_values.cart_owner,0));
    strcat(post,buffer);
  }
  if (edit_values.use_cart_notes)
  {
    snprintf(buffer,1024,"&NOTES=%s",curl_easy_escape(curl,edit_values.cart_notes,0));
    strcat(post,buffer);
  }

  return;
}
