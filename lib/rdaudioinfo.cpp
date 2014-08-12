// rdaudioinfo.cpp
//
// Get information about a cut in the audio store.
//
//   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioinfo.cpp,v 1.3.4.3 2013/11/13 23:36:30 cvs Exp $
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
#include <rdaudioinfo.h>

size_t RDAudioInfoCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  QString *xml=(QString *)userdata;
  for(unsigned i=0;i<(size*nmemb);i++) {
    *xml+=((const char *)ptr)[i];
  }
  return size*nmemb;
}


RDAudioInfo::RDAudioInfo(RDStation *station,RDConfig *config,
			     QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_station=station;
  conv_config=config;
  conv_cart_number=0;
  conv_cut_number=0;
  conv_format=RDWaveFile::Pcm16;
  conv_channels=0;
  conv_sample_rate=0;
  conv_frames=0;
  conv_length=0;
}


RDWaveFile::Format RDAudioInfo::format() const
{
  return conv_format;
}


unsigned RDAudioInfo::channels() const
{
  return conv_channels;
}


unsigned RDAudioInfo::sampleRate() const
{
  return conv_sample_rate;
}


unsigned RDAudioInfo::frames() const
{
  return conv_frames;
}


unsigned RDAudioInfo::length() const
{
  return conv_length;
}


void RDAudioInfo::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


void RDAudioInfo::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


RDAudioInfo::ErrorCode RDAudioInfo::runInfo(const QString &username,
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
				 RDXPORT_COMMAND_AUDIOINFO,
				 (const char *)RDFormPost::urlEncode(username),
				 (const char *)RDFormPost::urlEncode(password),
				 conv_cart_number,
				 conv_cut_number);
  if((curl=curl_easy_init())==NULL) {
    return RDAudioInfo::ErrorInternal;
  }

  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(conv_config),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,RDAudioInfoCallback);
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
    return RDAudioInfo::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:   // CURLE_REMOTE_ACCESS_DENIED
    curl_easy_cleanup(curl);
    return RDAudioInfo::ErrorUrlInvalid;

  default:
    curl_easy_cleanup(curl);
    return RDAudioInfo::ErrorService;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);

  switch(response_code) {
  case 200:
    break;

  case 404:
    return RDAudioInfo::ErrorNoAudio;

  default:
    return RDAudioInfo::ErrorService;
  }
  conv_format=(RDWaveFile::Format)ParseInt("format",conv_xml);
  conv_channels=ParseInt("channels",conv_xml);
  conv_sample_rate=ParseInt("sampleRate",conv_xml);
  conv_frames=ParseInt("frames",conv_xml);
  conv_length=ParseInt("length",conv_xml);

  return RDAudioInfo::ErrorOk;
}


QString RDAudioInfo::errorText(RDAudioInfo::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDAudioInfo::ErrorOk:
    ret=tr("OK");
    break;

  case RDAudioInfo::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDAudioInfo::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDAudioInfo::ErrorService:
    ret=tr("RDXport service returned an error");
    break;

  case RDAudioInfo::ErrorInvalidUser:
    ret=tr("Invalid user or password");
    break;

  case RDAudioInfo::ErrorNoAudio:
    ret=tr("Audio does not exist");
    break;
  }
  return ret;
}


int RDAudioInfo::ParseInt(const QString &tag,const QString &xml)
{
  //
  // FIXME: This is totally ad-hoc, but should work until we settle on
  //        a proper XML parser.
  //
  QStringList list=list.split("\n",xml);
  for(unsigned i=0;i<list.size();i++) {
    if(list[i].contains(tag)) {
      QStringList list2=list.split("<",list[i]);
      if(list2.size()>=2) {
	list2=list2.split(">",list2[1]);
	if(list2.size()>=2) {
	  return list2[1].toInt();
	}
      }
    }
  }
  return -1;
}
