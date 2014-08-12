// rddelete.cpp
//
// Delete a file from the audio store via the Rivendell Web Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddelete.cpp,v 1.3 2011/12/23 23:07:00 cvs Exp $
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
#include <rddelete.h>

//
// CURL Callbackd
//
size_t DeleteWriteCallback(void *ptr,size_t size,size_t nmemb,void *userdata)
{
  QString *xml=(QString *)userdata;
  for(unsigned i=0;i<(size*nmemb);i++) {
    *xml+=((const char *)ptr)[i];
  }
  return size*nmemb;
}


int DeleteErrorCallback(CURL *curl,curl_infotype type,char *msg,size_t size,
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


RDDelete::RDDelete(QObject *parent,const char *name)
  : QObject(parent,name)
{
}


void RDDelete::setTargetUrl(const QString &url)
{
  conv_target_url=url;
}


RDDelete::ErrorCode RDDelete::runDelete(const QString &username,
					const QString &password,
					bool log_debug)
{
  CURL *curl=NULL;
  struct curl_slist *cmds=NULL;
  CURLcode err;
  RDDelete::ErrorCode ret=RDDelete::ErrorOk;
  QString currentdir;
  char urlstr[1024];
  char userpwd[256];
  QString xml="";

  if((curl=curl_easy_init())==NULL) {
    syslog(LOG_ERR,"unable to initialize curl library\n");
    return RDDelete::ErrorInternal;
  }
  strncpy(urlstr,(const char *)(conv_target_url.protocol()+"://"+
				conv_target_url.host()+"/"),1024);
  curl_easy_setopt(curl,CURLOPT_URL,urlstr);
  strncpy(userpwd,(const char *)QString().
		   sprintf("%s:%s",(const char *)username,
			   (const char *)password),256);
  curl_easy_setopt(curl,CURLOPT_USERPWD,userpwd);
  curl_easy_setopt(curl,CURLOPT_HTTPAUTH,CURLAUTH_ANY);
  printf("HERE\n");
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,DeleteWriteCallback);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,&xml);
  if(log_debug) {
    curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
    curl_easy_setopt(curl,CURLOPT_DEBUGFUNCTION,DeleteErrorCallback);
  }
  currentdir="";
  if(!conv_target_url.dirPath().right(conv_target_url.dirPath().length()-1).
     isEmpty()) {
    currentdir=conv_target_url.dirPath().
      right(conv_target_url.dirPath().length()-1)+"/";
  }
  if(!currentdir.isEmpty()) {
    cmds=curl_slist_append(cmds,QString().sprintf("cwd %s",
						  (const char *)currentdir));
  }
  cmds=curl_slist_append(cmds, QString().sprintf("dele %s",
			 (const char *)conv_target_url.fileName()));
  curl_easy_setopt(curl,CURLOPT_QUOTE,cmds);
  switch((err=curl_easy_perform(curl))) {
  case CURLE_OK:
  case 21:   // CURLE_QUOTE_ERROR -- In case the file is already gone
    ret=RDDelete::ErrorOk;
    break;

  case CURLE_UNSUPPORTED_PROTOCOL:
    ret=RDDelete::ErrorUnsupportedProtocol;
    break;

  case CURLE_URL_MALFORMAT:
    ret=RDDelete::ErrorUrlInvalid;
    break;

  case CURLE_COULDNT_RESOLVE_HOST:
    ret=RDDelete::ErrorInvalidHostname;
    break;

  case CURLE_LOGIN_DENIED:
    ret=RDDelete::ErrorInvalidLogin;
    break;

  case CURLE_COULDNT_CONNECT:
    ret=RDDelete::ErrorRemoteConnection;
    break;

  case 9:   // CURLE_REMOTE_ACCESS_DENIED:
    ret=RDDelete::ErrorRemoteAccess;
    break;

  default:
    ret=RDDelete::ErrorUnknown;
    printf("CURL error: %d\n",err);
    break;
  }
  if(log_debug) {
    syslog(LOG_NOTICE,curl_easy_strerror(err));
  }
  curl_slist_free_all(cmds);
  curl_easy_cleanup(curl);
    
  return ret;
}


QString RDDelete::errorText(RDDelete::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDDelete::ErrorOk:
    ret=tr("OK");
    break;

  case RDDelete::ErrorUnsupportedProtocol:
    ret=tr("Unsupported protocol");
    break;

  case RDDelete::ErrorInvalidHostname:
    ret=tr("Unable to resolve hostname");
    break;

  case RDDelete::ErrorRemoteServer:
    ret=tr("Remote server error");
    break;

  case RDDelete::ErrorUrlInvalid:
    ret=tr("Invalid URL");
    break;

  case RDDelete::ErrorUnspecified:
    ret=tr("Unspecified error");
    break;

  case RDDelete::ErrorInvalidUser:
    ret=tr("Invalid User");
    break;

  case RDDelete::ErrorInvalidLogin:
    ret=tr("Invalid Login");
    break;

  case RDDelete::ErrorRemoteAccess:
    ret=tr("Remote Access Denied");
    break;

  case RDDelete::ErrorRemoteConnection:
    ret=tr("Counldn't Connect");
    break;

  case RDDelete::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDDelete::ErrorUnknown:
    ret=tr("Unknown Error");
    break;
  }
  return ret;
}
