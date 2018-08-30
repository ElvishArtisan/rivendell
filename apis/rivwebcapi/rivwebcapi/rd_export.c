/* rd_export.c
 *
 * Implementation of the Export Cart Rivendell Access Library
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
#include "rd_common.h"
#include "rd_getuseragent.h"
#include "rd_export.h"

size_t write_data( void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  size_t written;
  written = fwrite(ptr,size,nmemb,stream);
  return written;
}



int RD_ExportCart( const char hostname[],
			const char         username[],
			const char           passwd[],
			const char           ticket[],
			const unsigned        cartnum,
			const unsigned         cutnum,
			const int              format,
			const int	     channels,
			const int         sample_rate,
			const int            bit_rate,
			const int             quality,
			const int         start_point,
			const int           end_point,
                        const int normalization_level,
			const int     enable_metadata,
                        const char         filename[],
                        const char user_agent[])

{
  char post[1500];
  char url[1500];
  CURL *curl=NULL;
  FILE *fp;
  long response_code;
  char *fnameptr;
  char checked_fname[BUFSIZ];
  int i;
  char errbuf[CURL_ERROR_SIZE];
  CURLcode res;
  char user_agent_string[255];

  /*   Check File name */
  memset(checked_fname,'\0',sizeof(checked_fname));
  fnameptr=&checked_fname[0];

  for (i = 0 ; i < strlen(filename) ; i++) {
    if (filename[i]>32) {
      strncpy(fnameptr,&filename[i],1);
      fnameptr++;
    }
  }
  
//
// Generate POST Data
//

  if((curl=curl_easy_init())==NULL) {
    curl_easy_cleanup(curl);
    return -1;
  }

  snprintf(url,1500,"http://%s/rd-bin/rdxport.cgi",hostname);
  snprintf(post,1500,"COMMAND=1&LOGIN_NAME=%s&PASSWORD=%s&TICKET=%s&CART_NUMBER=%u&CUT_NUMBER=%u&FORMAT=%d&CHANNELS=%d&SAMPLE_RATE=%d&BIT_RATE=%d&QUALITY=%d&START_POINT=%d&END_POINT=%d&NORMALIZATION_LEVEL=%d&ENABLE_METADATA=%d",
        curl_easy_escape(curl,username,0),
	curl_easy_escape(curl,passwd,0),
	curl_easy_escape(curl,ticket,0),
	cartnum, 
	cutnum,
	format,
	channels, 
	sample_rate, 
	bit_rate,
	quality, 
	start_point, 
	end_point,
	normalization_level,
	enable_metadata);

  /*
   * Setup the CURL call
   */

  fp = fopen(checked_fname,"wb");
  if (!fp)
  {
    #ifdef RIVC_DEBUG_OUT
        fprintf(stderr,"Error Opening Destination File\n");
    #endif
    curl_easy_cleanup(curl);
    return -1;
  }

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

  curl_easy_setopt(curl,CURLOPT_URL, url);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,post);
  curl_easy_setopt(curl,CURLOPT_VERBOSE,0);
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
    fclose(fp);
    return -1;
  }
/* The response OK - so figure out if we got what we wanted.. */

  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  
  if (response_code > 199 && response_code < 300) {  //Success
    return 0;
  }
  else 
    return (int)response_code;
}
