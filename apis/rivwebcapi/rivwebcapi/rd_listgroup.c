/* rd_listgroup.c
 *
 * Implementation of the ListGroup Rivendell Access Library
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
#include "rd_listgroup.h"

struct xml_data {
  char elem_name[256];
  char strbuf[1024];
  struct rd_group *group;
};


static void XMLCALL __ListGroupElementStart(void *data, const char *el, 
					     const char **attr)
{
  unsigned i;
  struct xml_data *xml_data=(struct xml_data *)data;
  if(strcasecmp(el,"group")==0) {    // Allocate a new group entry
    xml_data->group=realloc(xml_data->group, sizeof(struct rd_group));
  }
  strlcpy(xml_data->elem_name,el,256);
  memset(xml_data->strbuf,0,1024);
}


static void XMLCALL __ListGroupElementData(void *data,const XML_Char *s,
					    int len)
{
  struct xml_data *xml_data=(struct xml_data *)data;

  memcpy(xml_data->strbuf+strlen(xml_data->strbuf),s,len);
}


static void XMLCALL __ListGroupElementEnd(void *data, const char *el)
{
  struct xml_data *xml_data=(struct xml_data *)data;
  struct rd_group *grp=xml_data->group;

  if(strcasecmp(el,"name")==0) {
    strlcpy(grp->grp_name,xml_data->strbuf,10);
  }
  if(strcasecmp(el,"description")==0) {
    strlcpy(grp->grp_desc,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"defaultcarttype")==0) {
    if(strcasecmp(xml_data->strbuf,"audio")==0) {
      grp->grp_default_cart_type=0;
    }
    if(strcasecmp(xml_data->strbuf,"macro")==0) {
      grp->grp_default_cart_type=1;
    }
  }
  if(strcasecmp(el,"defaultlowcart")==0) {
    sscanf(xml_data->strbuf,"%u",&grp->grp_lo_limit);
  }
  if(strcasecmp(el,"defaulthighcart")==0) {
    sscanf(xml_data->strbuf,"%u",&grp->grp_hi_limit);
  }
  if(strcasecmp(el,"cutshelflife")==0) {
    sscanf(xml_data->strbuf,"%d",&grp->grp_shelf_life);
  }
  if(strcasecmp(el,"defaulttitle")==0) {
    strlcpy(grp->grp_default_title,xml_data->strbuf,255);
  }
  if(strcasecmp(el,"enforcecartrange")==0) {
    grp->grp_enforce_range=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"reporttfc")==0) {
    grp->grp_report_tfc=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"reportmus")==0) {
    grp->grp_report_mus=RD_ReadBool(xml_data->strbuf);
  }
  if(strcasecmp(el,"color")==0) {
    strlcpy(grp->grp_color,xml_data->strbuf,8);
  }
}


size_t __ListGroupCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  XML_Parser p=(XML_Parser)userdata;

  XML_Parse(p,ptr,size*nmemb,0);
  
  return size*nmemb;
}


int RD_ListGroup(struct rd_group *grp[],
		  	const char hostname[],
			const char username[],
			const char passwd[],
			const char ticket[],
			const char groupname[],
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

   /* set number of recs so if fail already set */
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
  XML_SetElementHandler(parser,__ListGroupElementStart,
			__ListGroupElementEnd);
  XML_SetCharacterDataHandler(parser,__ListGroupElementData);
  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=5&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&GROUP_NAME=%s",
	curl_easy_escape(curl,username,0),
	curl_easy_escape(curl,passwd,0),
	curl_easy_escape(curl,ticket,0),
	curl_easy_escape(curl,groupname,0));
  //
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
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__ListGroupCallback);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
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
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {
    *grp=xml_data.group;
    *numrecs = 1;
    return 0;
  }
  else {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr," rd_listgroup Call Returned Error: %s\n",xml_data.strbuf);
    #endif
    return (int)response_code;
  }
}
