// rdaudiostore.cpp
//
// Get information about the audio store.
//
//   (C) Copyright 2014-2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdaudiostore.h>

size_t RDAudioStoreCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  QString *xml=(QString *)userdata;
  for(unsigned i=0;i<(size*nmemb);i++) {
    *xml+=((const char *)ptr)[i];
  }
  return size*nmemb;
}


RDAudioStore::RDAudioStore(RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  conv_station=station;
  conv_config=config;
  conv_free_bytes=0;
  conv_total_bytes=0;
}


uint64_t RDAudioStore::freeBytes() const
{
  return conv_free_bytes;
}


uint64_t RDAudioStore::totalBytes() const
{
  return conv_total_bytes;
}


RDAudioStore::ErrorCode RDAudioStore::runStore(const QString &username,
					       const QString &password)
{
  long response_code;
  CURL *curl=NULL;
  char url[1024];
  CURLcode curl_err;

  //
  // Generate POST Data
  //
  QString post=QString().sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s",
				 RDXPORT_COMMAND_AUDIOSTORE,
				 (const char *)RDFormPost::urlEncode(username),
				 (const char *)RDFormPost::urlEncode(password));
  if((curl=curl_easy_init())==NULL) {
    return RDAudioStore::ErrorInternal;
  }

  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(conv_config),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,RDAudioStoreCallback);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,&conv_xml);
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
    return RDAudioStore::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:   // CURLE_REMOTE_ACCESS_DENIED
    curl_easy_cleanup(curl);
    return RDAudioStore::ErrorUrlInvalid;

  default:
    curl_easy_cleanup(curl);
    return RDAudioStore::ErrorService;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);

  switch(response_code) {
  case 200:
    break;

  case 404:
    return RDAudioStore::ErrorNoAudio;

  default:
    return RDAudioStore::ErrorService;
  }
  conv_free_bytes=ParseInt("freeBytes",conv_xml);
  conv_total_bytes=ParseInt("totalBytes",conv_xml);

  return RDAudioStore::ErrorOk;
}


QString RDAudioStore::errorText(RDAudioStore::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDAudioStore::ErrorOk:
    ret=tr("OK");
    break;

  case RDAudioStore::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDAudioStore::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDAudioStore::ErrorService:
    ret=tr("RDXport service returned an error");
    break;

  case RDAudioStore::ErrorInvalidUser:
    ret=tr("Invalid user or password");
    break;

  case RDAudioStore::ErrorNoAudio:
    ret=tr("Audio does not exist");
    break;
  }
  return ret;
}


uint64_t RDAudioStore::ParseInt(const QString &tag,const QString &xml)
{
  //
  // FIXME: This is totally ad-hoc, but should work until we settle on
  //        a proper XML parser.
  //
  QStringList list=list.split("\n",xml);
  for(int i=0;i<list.size();i++) {
    if(list[i].contains(tag)) {
      QStringList list2=list.split("<",list[i]);
      if(list2.size()>=2) {
	list2=list2.split(">",list2[1]);
	if(list2.size()>=2) {
	  return list2[1].toLongLong();
	}
      }
    }
  }
  return -1;
}
