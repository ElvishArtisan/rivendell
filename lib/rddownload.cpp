// rddownload.cpp
//
// Download a File
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddownload.cpp,v 1.5.4.2 2013/01/30 15:03:53 cvs Exp $
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

#include <curl/curl.h>

#include <qapplication.h>
#include <qfileinfo.h>

#include <rd.h>
#include <rdsystemuser.h>
#include <rddownload.h>

//
// CURL Progress Callback
//
int DownloadProgressCallback(void *clientp,double dltotal,double dlnow,
			  double ultotal,double ulnow)
{
  RDDownload *conv=(RDDownload *)clientp;
  conv->UpdateProgress(ulnow);
  qApp->processEvents();
  if(conv->aborting()) {
    return 1;
  }
  return 0;
}


int DownloadErrorCallback(CURL *curl,curl_infotype type,char *msg,size_t size,
			void *clientp)
{
  char str[1000];

  if(type!=CURLINFO_TEXT) {
    return 0;
  }
  if(size>999) {
    size=999;
  }
  memset(&str,0,size+1);
  memcpy(str,msg,size);
  syslog(LOG_DEBUG,"CURL MSG: %s",str);
  return 0;
}


RDDownload::RDDownload(const QString &station_name,
		   QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_aborting=false;
}


void RDDownload::setSourceUrl(const QString &url)
{
  conv_src_url=url;
}


void RDDownload::setDestinationFile(const QString &filename)
{
  conv_dst_filename=filename;
  QFileInfo fi(filename);
  conv_dst_size=fi.size();
}


int RDDownload::totalSteps() const
{
  return conv_dst_size;
}


RDDownload::ErrorCode RDDownload::runDownload(const QString &username,
					      const QString &password,
					      bool log_debug)
{
  CURL *curl=NULL;
  CURLcode curl_err;
  FILE *f;
  long response_code=0;
  RDDownload::ErrorCode ret=RDDownload::ErrorOk;
  RDSystemUser *user=NULL;
  char url[1024];
  char userpwd[256];

  //
  // Validate User for file: transfers
  //
  if((getuid()==0)&&(conv_src_url.protocol().lower()=="file")) {
    user=new RDSystemUser(username);
    if(!user->validatePassword(password)) {
      delete user;
      return RDDownload::ErrorInvalidUser;
    }
  }

  if((curl=curl_easy_init())==NULL) {
    return RDDownload::ErrorInternal;
  }
  if((f=fopen(conv_dst_filename,"w"))==NULL) {
    curl_easy_cleanup(curl);
    return RDDownload::ErrorNoDestination;
  }
  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_src_url.toString(conv_src_url.protocol().lower()=="http"),
	  1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,f);
  strncpy(userpwd,username+":"+password,256);
  curl_easy_setopt(curl,CURLOPT_USERPWD,userpwd);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1);
  curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,DownloadProgressCallback);
  curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,this);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0);
  if(log_debug) {
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,DownloadErrorCallback);
  }

  if(user!=NULL) {
    setegid(user->gid());
    seteuid(user->uid());
  }
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
    if(conv_src_url.protocol().lower()=="http") {
      curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&response_code);
      if(response_code!=200) {
	ret=RDDownload::ErrorUrlInvalid;
      }
    }
    break;

  case CURLE_UNSUPPORTED_PROTOCOL:
    ret=RDDownload::ErrorUnsupportedProtocol;
    break;

  case CURLE_URL_MALFORMAT:
    ret=RDDownload::ErrorUrlInvalid;
    break;

  case CURLE_COULDNT_RESOLVE_HOST:
    ret=RDDownload::ErrorInvalidHostname;
    break;

  case CURLE_LOGIN_DENIED:
    ret=RDDownload::ErrorInvalidLogin;
    break;

  case CURLE_COULDNT_CONNECT:
    ret=RDDownload::ErrorRemoteConnection;
    break;

  case 9:   // CURLE_REMOTE_ACCESS_DENIED
    ret=RDDownload::ErrorRemoteAccess;
    break;

  default:
    syslog(LOG_ERR,"Unknown CURL Error [%d]: %s",
	   curl_err,curl_easy_strerror(curl_err));
    ret=RDDownload::ErrorUnspecified;
  }
  if(user!=NULL) {
    seteuid(getuid());
    setegid(getgid());
    delete user;
  }
  if((curl_err!=CURLE_OK)&&log_debug) {
    syslog(LOG_WARNING,"CURL download failed: url: %s  username: %s",
	   (const char *)conv_src_url.toString(),
	   (const char *)username);
  }
  curl_easy_cleanup(curl);
  fclose(f);

  return ret;
}


bool RDDownload::aborting() const
{
  return conv_aborting;
}


QString RDDownload::errorText(RDDownload::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDDownload::ErrorOk:
    ret=tr("OK");
    break;

  case RDDownload::ErrorUnsupportedProtocol:
    ret=tr("Unsupported protocol");
    break;

  case RDDownload::ErrorNoSource:
    ret=tr("Unable to access source file");
    break;

  case RDDownload::ErrorNoDestination:
    ret=tr("Unable to create destination file");
    break;

  case RDDownload::ErrorInvalidHostname:
    ret=tr("Unable to resolve hostname");
    break;

  case RDDownload::ErrorRemoteServer:
    ret=tr("Remote server error");
    break;

  case RDDownload::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDDownload::ErrorUnspecified:
    ret=tr("Unspecified error");
    break;

  case RDDownload::ErrorInvalidUser:
    ret=tr("Invalid User");
    break;

  case RDDownload::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDDownload::ErrorAborted:
    ret=tr("Download aborted");
    break;

  case RDDownload::ErrorInvalidLogin:
    ret=tr("Invalid username or password");
    break;

  case RDDownload::ErrorRemoteAccess:
    ret=tr("Remote access denied");
    break;

  case RDDownload::ErrorRemoteConnection:
    ret=tr("Couldn't connect to server");
    break;
  }
  return ret;
}


void RDDownload::abort()
{
  conv_aborting=true;
}


void RDDownload::UpdateProgress(int step)
{
  emit progressChanged(step);
}
