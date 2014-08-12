// rdupload.cpp
//
// Upload a File
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdupload.cpp,v 1.5.4.2 2012/05/10 23:12:42 cvs Exp $
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

#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <pwd.h>

#include <curl/curl.h>

#include <qapplication.h>
#include <qfileinfo.h>

#include <rd.h>
#include <rdsystemuser.h>
#include <rdupload.h>

//
// CURL Progress Callback
//
int UploadProgressCallback(void *clientp,double dltotal,double dlnow,
			  double ultotal,double ulnow)
{
  RDUpload *conv=(RDUpload *)clientp;
  conv->UpdateProgress(ulnow);
  qApp->processEvents();
  if(conv->aborting()) {
    return 1;
  }
  return 0;
}


int UploadErrorCallback(CURL *curl,curl_infotype type,char *msg,size_t size,
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


RDUpload::RDUpload(const QString &station_name,
		   QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_aborting=false;
}


void RDUpload::setSourceFile(const QString &filename)
{
  conv_src_filename=filename;
  QFileInfo fi(filename);
  conv_src_size=fi.size();
}


void RDUpload::setDestinationUrl(const QString &url)
{
  conv_dst_url=url;
}


int RDUpload::totalSteps() const
{
  return conv_src_size;
}


RDUpload::ErrorCode RDUpload::runUpload(const QString &username,
					const QString &password,
					bool log_debug)
{
  CURL *curl=NULL;
  CURLcode curl_err;
  FILE *f;
  RDUpload::ErrorCode ret=RDUpload::ErrorOk;
  RDSystemUser *user=NULL;
  char url[1024];
  char userpwd[256];

  //
  // Validate User for file: transfers
  //
  if((getuid()==0)&&(conv_dst_url.protocol().lower()=="file")) {
    user=new RDSystemUser(username);
    if(!user->validatePassword(password)) {
      delete user;
      return RDUpload::ErrorInvalidUser;
    }
  }

  if((curl=curl_easy_init())==NULL) {
    return RDUpload::ErrorInternal;
  }
  if((f=fopen(conv_src_filename,"r"))==NULL) {
    curl_easy_cleanup(curl);
    return RDUpload::ErrorNoSource;
  }

  //
  // Write out URL as a C string before passing to curl_easy_setopt(), 
  // otherwise some versions of LibCurl will throw a 'bad/illegal format' 
  // error.
  //
  strncpy(url,conv_dst_url.toString(conv_dst_url.protocol().lower()=="http"),
	  1024);
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_UPLOAD,1);
  curl_easy_setopt(curl,CURLOPT_READDATA,f);
  curl_easy_setopt(curl,CURLOPT_INFILESIZE,(long)conv_src_size);
  strncpy(userpwd,(username+":"+password),256);
  curl_easy_setopt(curl,CURLOPT_USERPWD,userpwd);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,RD_CURL_TIMEOUT);
  curl_easy_setopt(curl,CURLOPT_PROGRESSFUNCTION,UploadProgressCallback);
  curl_easy_setopt(curl,CURLOPT_PROGRESSDATA,this);
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,0);
  if(log_debug) {
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,UploadErrorCallback);
  }
  if(user!=NULL) {
    setegid(user->gid());
    seteuid(user->uid());
  }
  switch((curl_err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case CURLE_PARTIAL_FILE:
    break;

  case CURLE_UNSUPPORTED_PROTOCOL:
    ret=RDUpload::ErrorUnsupportedProtocol;
    break;

  case CURLE_URL_MALFORMAT:
    ret=RDUpload::ErrorUrlInvalid;
    break;

  case CURLE_COULDNT_RESOLVE_HOST:
    ret=RDUpload::ErrorInvalidHostname;
    break;

  case CURLE_LOGIN_DENIED:
    ret=RDUpload::ErrorInvalidLogin;
    break;

  case CURLE_COULDNT_CONNECT:
    ret=RDUpload::ErrorRemoteConnection;
    break;

  case 9:   // CURLE_REMOTE_ACCESS_DENIED
    ret=RDUpload::ErrorRemoteAccess;
    break;

  default:
    syslog(LOG_ERR,"Unknown CURL Error [%d]: %s",
	   curl_err,curl_easy_strerror(curl_err));
    ret=RDUpload::ErrorUnspecified;
    break;
  }
  if(user!=NULL) {
    seteuid(getuid());
    setegid(getgid());
    delete user;
  }
  if((curl_err!=CURLE_OK)&&log_debug) {
    syslog(LOG_WARNING,"CURL upload failed: url: %s  username: %s",
	   (const char *)conv_dst_url.toString(),
	   (const char *)username);
  }
  curl_easy_cleanup(curl);
  fclose(f);

  return ret;
}


bool RDUpload::aborting() const
{
  return conv_aborting;
}


QString RDUpload::errorText(RDUpload::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDUpload::ErrorOk:
    ret=tr("OK");
    break;

  case RDUpload::ErrorUnsupportedProtocol:
    ret=tr("Unsupported protocol");
    break;

  case RDUpload::ErrorNoSource:
    ret=tr("Unable to access source file");
    break;

  case RDUpload::ErrorNoDestination:
    ret=tr("Unable to create destination file");
    break;

  case RDUpload::ErrorInvalidHostname:
    ret=tr("Unable to resolve hostname");
    break;

  case RDUpload::ErrorRemoteServer:
    ret=tr("Remote server error");
    break;

  case RDUpload::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDUpload::ErrorUnspecified:
    ret=tr("Unspecified error");
    break;

  case RDUpload::ErrorInvalidUser:
    ret=tr("Invalid User");
    break;

  case RDUpload::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDUpload::ErrorAborted:
    ret=tr("Upload aborted");
    break;

  case RDUpload::ErrorInvalidLogin:
    ret=tr("Invalid username or password");
    break;

  case RDUpload::ErrorRemoteAccess:
    ret=tr("Remote access denied");
    break;

  case RDUpload::ErrorRemoteConnection:
    ret=tr("Couldn't connect to server");
    break;
  }
  return ret;
}


void RDUpload::abort()
{
  conv_aborting=true;
}


void RDUpload::UpdateProgress(int step)
{
  emit progressChanged(step);
}
