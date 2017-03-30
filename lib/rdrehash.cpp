// rdrehash.cpp
//
// Generate a SHA-1 hash of an audio file and write it to the database.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <curl/curl.h>

#include <qstringlist.h>

#include <rd.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdrehash.h>

size_t __RDRehashCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  return size*nmemb;
}


RDRehash::RDRehash(RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  conv_station=station;
  conv_config=config;
  conv_cart_number=0;
  conv_cut_number=0;
}


void RDRehash::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


void RDRehash::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


RDRehash::ErrorCode RDRehash::runRehash(const QString &username,
					const QString &password)
{
  long response_code;
  CURL *curl=NULL;
  char url[1024];
  CURLcode curl_err;

  //
  // Generate POST Data
  //
  QString post=QString().sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s&CART_NUMBER=%u&CUT_NUMBER=%u",
				 RDXPORT_COMMAND_REHASH,
				 (const char *)RDFormPost::urlEncode(username),
				 (const char *)RDFormPost::urlEncode(password),
				 conv_cart_number,
				 conv_cut_number);
  if((curl=curl_easy_init())==NULL) {
    return RDRehash::ErrorInternal;
  }

  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(conv_config),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,__RDRehashCallback);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,(const char *)post);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);

  switch(curl_err=curl_easy_perform(curl)) {
  case CURLE_OK:
    break;

  case CURLE_UNSUPPORTED_PROTOCOL:
  case CURLE_FAILED_INIT:
  case CURLE_COULDNT_RESOLVE_PROXY:
  case CURLE_PARTIAL_FILE:
  case CURLE_HTTP_RETURNED_ERROR:
  case CURLE_WRITE_ERROR:
  case CURLE_OUT_OF_MEMORY:
  case CURLE_OPERATION_TIMEDOUT:
  case CURLE_HTTP_POST_ERROR:
    curl_easy_cleanup(curl);
    fprintf(stderr,"curl error: %d\n",curl_err);
    return RDRehash::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:   // CURLE_REMOTE_ACCESS_DENIED
    curl_easy_cleanup(curl);
    return RDRehash::ErrorUrlInvalid;

  default:
    curl_easy_cleanup(curl);
    return RDRehash::ErrorService;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);

  switch(response_code) {
  case 200:
    break;

  case 404:
    return RDRehash::ErrorNoAudio;

  default:
    return RDRehash::ErrorService;
  }
  return RDRehash::ErrorOk;
}


QString RDRehash::errorText(RDRehash::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDRehash::ErrorOk:
    ret=tr("OK");
    break;

  case RDRehash::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDRehash::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDRehash::ErrorService:
    ret=tr("RDXport service returned an error");
    break;

  case RDRehash::ErrorInvalidUser:
    ret=tr("Invalid user or password");
    break;

  case RDRehash::ErrorNoAudio:
    ret=tr("Audio does not exist");
    break;
  }
  return ret;
}


RDRehash::ErrorCode RDRehash::rehash(RDStation *station,RDUser *user,
				     RDConfig *config,
				     unsigned cartnum,int cutnum)
{
  RDRehash::ErrorCode err;
  RDRehash *rehash=new RDRehash(station,config);

  rehash->setCartNumber(cartnum);
  rehash->setCutNumber(cutnum);
  err=rehash->runRehash(user->name(),user->password());

  delete rehash;
  return err;
}
