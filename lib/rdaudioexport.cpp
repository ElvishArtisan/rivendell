// rdaudioexport.cpp
//
// Export an Audio File using the RdXport Web Service
//
//   (C) Copyright 2010,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdapplication.h>
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


RDAudioExport::RDAudioExport(QObject *parent)
  : QObject(parent)
{
  conv_cart_number=0;
  conv_cut_number=0;
  conv_start_point=-1;
  conv_end_point=-1;
  conv_enable_metadata=false;
  conv_settings=NULL;
  conv_aborting=false;
}


unsigned RDAudioExport::cartNumber() const
{
  return conv_cart_number;
}


void RDAudioExport::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


unsigned RDAudioExport::cutNumber() const
{
  return conv_cut_number;
}


void RDAudioExport::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


QString RDAudioExport::destinationFile() const
{
  return conv_dst_filename;
}


void RDAudioExport::setDestinationFile(const QString &filename)
{
  conv_dst_filename=filename;
}


RDSettings *RDAudioExport::destinationSettings() const
{
  return conv_settings;
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
  struct curl_httppost *first=NULL;
  struct curl_httppost *last=NULL;
  RDAudioExport::ErrorCode ret;
  RDWebResult web_result;

  //
  // Generate POST Data
  //
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"COMMAND",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",RDXPORT_COMMAND_EXPORT).toUtf8().
	       constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"LOGIN_NAME",
	       CURLFORM_COPYCONTENTS,username.toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"PASSWORD",
	       CURLFORM_COPYCONTENTS,password.toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CART_NUMBER",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_cart_number).toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CUT_NUMBER",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_cut_number).toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"FORMAT",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_settings->format()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"CHANNELS",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_settings->channels()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"SAMPLE_RATE",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_settings->sampleRate()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"BIT_RATE",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_settings->bitRate()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"QUALITY",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_settings->quality()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"START_POINT",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%d",conv_start_point).toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"END_POINT",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%d",conv_end_point).toUtf8().constData(),
	       CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"NORMALIZATION_LEVEL",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%d",conv_settings->normalizationLevel()).
	       toUtf8().constData(),CURLFORM_END);
  curl_formadd(&first,&last,CURLFORM_PTRNAME,"ENABLE_METADATA",
	       CURLFORM_COPYCONTENTS,
	       QString().sprintf("%u",conv_enable_metadata).
	       toUtf8().constData(),CURLFORM_END);
  if((curl=curl_easy_init())==NULL) {
    curl_formfree(first);
    return RDAudioExport::ErrorInternal;
  }
  if((f=fopen(conv_dst_filename.toUtf8(),"w"))==NULL) {
    curl_easy_cleanup(curl);
    curl_formfree(first);
    return RDAudioExport::ErrorNoDestination;
  }
  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  //  strncpy(url,rda->station()->webServiceUrl(rda->config()),1024);
  curl_easy_setopt(curl,CURLOPT_URL,rda->station()->webServiceUrl(rda->config()).toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_HTTPPOST,first);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,f);
  curl_easy_setopt(curl,CURLOPT_USERAGENT,
		   rda->config()->userAgent().toUtf8().constData());
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,ExportProgressCallback);
  curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,this);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0);

  switch(curl_easy_perform(curl)) {
  case CURLE_OK:
    break;

  case CURLE_ABORTED_BY_CALLBACK:
    curl_easy_cleanup(curl);
    curl_formfree(first);
    unlink(conv_dst_filename.toUtf8());
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
    curl_formfree(first);
    return RDAudioExport::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:  // CURLE_REMOTE_ACCESS_DENIED:
    curl_easy_cleanup(curl);
    curl_formfree(first);
    return RDAudioExport::ErrorUrlInvalid;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);
  curl_formfree(first);
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
  unlink(conv_dst_filename.toUtf8());

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
