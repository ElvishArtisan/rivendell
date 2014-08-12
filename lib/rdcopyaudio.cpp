// rdcopyaudio.cpp
//
// Get the trim points for an audio cut.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcopyaudio.cpp,v 1.3.4.1 2013/11/13 23:36:32 cvs Exp $
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
#include <qobject.h>

#include <rd.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdcopyaudio.h>

RDCopyAudio::RDCopyAudio(RDStation *station,RDConfig *config)
{
  conv_station=station;
  conv_config=config;
  conv_source_cart_number=0;
  conv_source_cut_number=0;
  conv_destination_cart_number=0;
  conv_destination_cut_number=0;
}


void RDCopyAudio::setSourceCartNumber(unsigned cartnum)
{
  conv_source_cart_number=cartnum;
}


void RDCopyAudio::setSourceCutNumber(unsigned cutnum)
{
  conv_source_cut_number=cutnum;
}


void RDCopyAudio::setDestinationCartNumber(unsigned cartnum)
{
  conv_destination_cart_number=cartnum;
}


void RDCopyAudio::setDestinationCutNumber(unsigned cutnum)
{
  conv_destination_cut_number=cutnum;
}


RDCopyAudio::ErrorCode RDCopyAudio::runCopy(const QString &username,
					    const QString &password)
{
  long response_code;
  CURL *curl=NULL;
  char url[1024];

  //
  // Generate POST Data
  //
  QString post=QString().sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s&SOURCE_CART_NUMBER=%u&SOURCE_CUT_NUMBER=%u&DESTINATION_CART_NUMBER=%u&DESTINATION_CUT_NUMBER=%u",
				 RDXPORT_COMMAND_COPYAUDIO,
				 (const char *)RDFormPost::urlEncode(username),
				 (const char *)RDFormPost::urlEncode(password),
				 conv_source_cart_number,
				 conv_source_cut_number,
				 conv_destination_cart_number,
				 conv_destination_cut_number);
  if((curl=curl_easy_init())==NULL) {
    return RDCopyAudio::ErrorInternal;
  }

  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(conv_config),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,(const char *)post);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);

  switch(curl_easy_perform(curl)) {
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
    return RDCopyAudio::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:   // CURLE_REMOTE_ACCESS_DENIED:
    curl_easy_cleanup(curl);
    return RDCopyAudio::ErrorUrlInvalid;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);

  switch(response_code) {
  case 200:
    break;

  case 400:
    return RDCopyAudio::ErrorInternal;

  case 404:
    return RDCopyAudio::ErrorNoCart;

  default:
    return RDCopyAudio::ErrorService;
  }

  return RDCopyAudio::ErrorOk;
}


QString RDCopyAudio::errorText(RDCopyAudio::ErrorCode err)
{
  QString ret=QString().sprintf("Uknown Error [%u]",err);

  switch(err) {
  case RDCopyAudio::ErrorOk:
    ret=QObject::tr("OK");
    break;

  case RDCopyAudio::ErrorNoCart:
    ret=QObject::tr("No such cart");
    break;

  case RDCopyAudio::ErrorInternal:
    ret=QObject::tr("Internal Error");
    break;

  case RDCopyAudio::ErrorUrlInvalid:
    ret=QObject::tr("Invalid URL");
    break;

  case RDCopyAudio::ErrorService:
    ret=QObject::tr("RDXport service returned an error");
    break;

  case RDCopyAudio::ErrorInvalidUser:
    ret=QObject::tr("Invalid user or password");
    break;
  }
  return ret;
}
