// rdaudioexport.cpp
//
// Export an Audio File using the RdXport Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioexport.cpp,v 1.8.4.1 2013/11/13 23:36:30 cvs Exp $
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

#include <qapplication.h>

#include <rd.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdaudioexport.h>
#include <rdwebresult.h>

//
// CURL Progress Callback
//
int ExportProgressCallback(void *clientp,double dltotal,double dlnow,
			  double ultotal,double ulnow)
{
  RDAudioExport *conv=(RDAudioExport *)clientp;
  qApp->processEvents();
  if(conv->aborting()) {
    return 1;
  }
  return 0;
}


RDAudioExport::RDAudioExport(RDStation *station,RDConfig *config,
			     QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_station=station;
  conv_config=config;
  conv_cart_number=0;
  conv_cut_number=0;
  conv_start_point=-1;
  conv_end_point=-1;
  conv_enable_metadata=false;
  conv_settings=NULL;
  conv_aborting=false;
}


void RDAudioExport::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


void RDAudioExport::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


void RDAudioExport::setDestinationFile(const QString &filename)
{
  conv_dst_filename=filename;
}


void RDAudioExport::setDestinationSettings(RDSettings *settings)
{
  conv_settings=settings;
}


void RDAudioExport::setRange(int start_pt,int end_pt)
{
  conv_start_point=start_pt;
  conv_end_point=end_pt;
}


void RDAudioExport::setEnableMetadata(bool state)
{
  conv_enable_metadata=state;
}


RDAudioExport::ErrorCode RDAudioExport::runExport(const QString &username,
						  const QString &password,
					 RDAudioConvert::ErrorCode *conv_err)
{
  long response_code;
  CURL *curl=NULL;
  FILE *f=NULL;
  char url[1024];
  RDAudioExport::ErrorCode ret;
  RDWebResult web_result;

  //
  // Generate POST Data
  //
  QString post=QString().sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s&CART_NUMBER=%u&CUT_NUMBER=%u&FORMAT=%d&CHANNELS=%d&SAMPLE_RATE=%d&BIT_RATE=%d&QUALITY=%d&START_POINT=%d&END_POINT=%d&NORMALIZATION_LEVEL=%d&ENABLE_METADATA=%d",
				 RDXPORT_COMMAND_EXPORT,
				 (const char *)RDFormPost::urlEncode(username),
				 (const char *)RDFormPost::urlEncode(password),
				 conv_cart_number,
				 conv_cut_number,
				 conv_settings->format(),
				 conv_settings->channels(),
				 conv_settings->sampleRate(),
				 conv_settings->bitRate(),
				 conv_settings->quality(),
				 conv_start_point,
				 conv_end_point,
				 conv_settings->normalizationLevel(),
				 conv_enable_metadata);

  if((curl=curl_easy_init())==NULL) {
    return RDAudioExport::ErrorInternal;
  }
  if((f=fopen(conv_dst_filename,"w"))==NULL) {
    curl_easy_cleanup(curl);
    return RDAudioExport::ErrorNoDestination;
  }
  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_station->webServiceUrl(conv_config),1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,f);
  curl_easy_setopt(curl,CURLOPT_POST,1);
  curl_easy_setopt(curl,CURLOPT_POSTFIELDS,(const char *)post);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,ExportProgressCallback);
  curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,this);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0);

  switch(curl_easy_perform(curl)) {
  case CURLE_OK:
    break;

  case CURLE_ABORTED_BY_CALLBACK:
    curl_easy_cleanup(curl);
    unlink(conv_dst_filename);
    return RDAudioExport::ErrorAborted;

  case CURLE_UNSUPPORTED_PROTOCOL:
  case CURLE_FAILED_INIT:
  case CURLE_COULDNT_RESOLVE_PROXY:
  case CURLE_PARTIAL_FILE:
  case CURLE_HTTP_RETURNED_ERROR:
  case CURLE_WRITE_ERROR:
  case CURLE_OUT_OF_MEMORY:
  case CURLE_OPERATION_TIMEDOUT:
  case CURLE_HTTP_POST_ERROR:
  default:
    curl_easy_cleanup(curl);
    return RDAudioExport::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:  // CURLE_REMOTE_ACCESS_DENIED:
    curl_easy_cleanup(curl);
    return RDAudioExport::ErrorUrlInvalid;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  fclose(f);

  if(response_code==200) {
    *conv_err=RDAudioConvert::ErrorOk;
    return RDAudioExport::ErrorOk;
  }

  if(web_result.readXmlFromFile(conv_dst_filename)) {
    *conv_err=web_result.converterErrorCode();
  }
  switch(response_code) {
  case 400:
    ret=RDAudioExport::ErrorService;

  case 401:
    ret=RDAudioExport::ErrorInvalidUser;

  case 404:
    ret=RDAudioExport::ErrorNoSource;

  default:
    ret=RDAudioExport::ErrorConverter;
  }
  unlink(conv_dst_filename);

  return ret;
}


bool RDAudioExport::aborting() const
{
  return conv_aborting;
}


QString RDAudioExport::errorText(RDAudioExport::ErrorCode err,
				 RDAudioConvert::ErrorCode conv_err)
{
  QString ret=QString().sprintf("Uknown Error [%u]",err);

  switch(err) {
  case RDAudioExport::ErrorOk:
    ret=tr("OK");
    break;

  case RDAudioExport::ErrorInvalidSettings:
    ret=tr("Invalid/unsupported audio parameters");
    break;

  case RDAudioExport::ErrorNoSource:
    ret=tr("No such cart/cut");
    break;

  case RDAudioExport::ErrorNoDestination:
    ret=tr("Unable to create destination file");
    break;

  case RDAudioExport::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDAudioExport::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDAudioExport::ErrorService:
    ret=tr("RDXport service returned an error");
    break;

  case RDAudioExport::ErrorInvalidUser:
    ret=tr("Invalid user or password");
    break;

  case RDAudioExport::ErrorAborted:
    ret=tr("Aborted");
    break;

  case RDAudioExport::ErrorConverter:
    ret=tr("Audio Converter Error: ")+RDAudioConvert::errorText(conv_err);
    break;
  }
  return ret;
}


void RDAudioExport::abort()
{
  conv_aborting=true;
}
