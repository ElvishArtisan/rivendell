// rdpeaksexport.cpp
//
// Export peak data using the RdXport Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpeaksexport.cpp,v 1.6.4.5 2013/11/13 23:36:33 cvs Exp $
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
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <curl/curl.h>

#include <qapplication.h>
#include <qobject.h>

#include <rd.h>
#include <rdxport_interface.h>
#include <rdformpost.h>
#include <rdpeaksexport.h>

//
// LibCURL Write Callback
//
size_t RDPeaksExportWrite(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  RDPeaksExport *peaks=(RDPeaksExport *)userdata;
  int bytes=size*nmemb;
  peaks->conv_energy_data=
    (unsigned short *)realloc(peaks->conv_energy_data,peaks->conv_write_ptr+bytes);
  for(int i=0;i<bytes;i++) {
    ((uint8_t *)peaks->conv_energy_data)[i+peaks->conv_write_ptr]=
      ((uint8_t *)ptr)[i];
  }
  peaks->conv_write_ptr+=bytes;
  return size*nmemb;
}


RDPeaksExport::~RDPeaksExport()
{
  if(conv_energy_data!=NULL) {
    free(conv_energy_data);
  }
}


RDPeaksExport::RDPeaksExport(RDStation *station,RDConfig *config,
			     QObject *parent,const char *name)
{
  conv_station=station;
  conv_config=config;
  conv_cart_number=0;
  conv_cut_number=0;
  conv_energy_data=NULL;
  conv_write_ptr=0;
}


void RDPeaksExport::setCartNumber(unsigned cartnum)
{
  conv_cart_number=cartnum;
}


void RDPeaksExport::setCutNumber(unsigned cutnum)
{
  conv_cut_number=cutnum;
}


RDPeaksExport::ErrorCode RDPeaksExport::runExport(const QString &username,
						  const QString &password)
{
  long response_code;
  CURL *curl=NULL;
  CURLcode curl_err;
  char url[1024];

  //
  // Generate POST Data
  //
  QString post=QString().
    sprintf("COMMAND=%d&LOGIN_NAME=%s&PASSWORD=%s&CART_NUMBER=%u&CUT_NUMBER=%u",
	    RDXPORT_COMMAND_EXPORT_PEAKS,
	    (const char *)RDFormPost::urlEncode(username),
	    (const char *)RDFormPost::urlEncode(password),
	    conv_cart_number,
	    conv_cut_number);

  if((curl=curl_easy_init())==NULL) {
    return RDPeaksExport::ErrorInternal;
  }
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,this);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,RDPeaksExportWrite);

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
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1);
  //curl_easy_setopt(curl,CURLOPT_VERBOSE,1);

  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
    break;

  case CURLE_ABORTED_BY_CALLBACK:
    curl_easy_cleanup(curl);
    return RDPeaksExport::ErrorAborted;

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
    return RDPeaksExport::ErrorInternal;

  case CURLE_URL_MALFORMAT:
  case CURLE_COULDNT_RESOLVE_HOST:
  case CURLE_COULDNT_CONNECT:
  case 9:  // CURLE_REMOTE_ACCESS_DENIED
    curl_easy_cleanup(curl);
    return RDPeaksExport::ErrorUrlInvalid;
  }
  curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
  curl_easy_cleanup(curl);

  switch(response_code) {
  case 200:
    break;

  case 403:
    return RDPeaksExport::ErrorInvalidUser;

  default:
    return RDPeaksExport::ErrorService;
  }
  return RDPeaksExport::ErrorOk;
}


QString RDPeaksExport::errorText(RDPeaksExport::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDPeaksExport::ErrorOk:
    ret=QObject::tr("OK");
    break;

  case RDPeaksExport::ErrorNoSource:
    ret=QObject::tr("No such cart/cut");
    break;

  case RDPeaksExport::ErrorInternal:
    ret=QObject::tr("Internal Error");
    break;

  case RDPeaksExport::ErrorUrlInvalid:
    ret=QObject::tr("Invalid URL");
    break;

  case RDPeaksExport::ErrorService:
    ret=QObject::tr("RDXport service returned an error");
    break;

  case RDPeaksExport::ErrorInvalidUser:
    ret=QObject::tr("Invalid user or password");
    break;

  case RDPeaksExport::ErrorAborted:
    ret=QObject::tr("Aborted");
    break;
  }
  return ret;
}


unsigned RDPeaksExport::energySize()
{
  return conv_write_ptr/sizeof(unsigned short);
}


unsigned short RDPeaksExport::energy(unsigned frame)
{
  return conv_energy_data[frame];
}


int RDPeaksExport::readEnergy(unsigned short buf[],int count)
{
  for(int i=0;i<count;i++) {
    buf[i]=conv_energy_data[i];
  }
  return count;
}
