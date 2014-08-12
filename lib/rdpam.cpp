// rdpam.cpp
//
// Authenticate a PAM name.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpam.cpp,v 1.2.8.1 2012/12/13 22:33:44 cvs Exp $
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

#include <syslog.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

#include <security/pam_appl.h>

#include <qstringlist.h>

#include <rdpam.h>

int RDPamCallback(int num_msg, const struct pam_message **msg,
		struct pam_response **resp, void *appdata_ptr)
{
  RDPam *pam=(RDPam *)appdata_ptr;

  pam->CleanupPam();
  *resp=new struct pam_response[num_msg];
  for(int i=0;i<num_msg;i++) {
    resp[i]->resp=new char[256];
    memset(resp[i]->resp,0,256);
    switch(msg[i]->msg_style) {
    case PAM_PROMPT_ECHO_OFF:
      strncpy(resp[i]->resp,pam->system_token,255);
      break;

    case PAM_PROMPT_ECHO_ON:
      syslog(LOG_ERR,"unhandled PAM request: %s",msg[i]->msg);
      break;

    case PAM_ERROR_MSG:
    case PAM_TEXT_INFO:
      syslog(LOG_NOTICE,"PAM message: %s",msg[i]->msg);
      break;
    }
  }
  return 0;
}


RDPam::RDPam(const QString &pam_service)
{
  system_pam_response=NULL;
  system_pam_response_count=0;
  system_pam_service=pam_service;
}


bool RDPam::authenticate(const QString &username,const QString &token)
{
  pam_handle_t *pamh=NULL;
  struct pam_conv conv;
  int err;

  system_token=token;
  memset(&conv,0,sizeof(conv));
  conv.conv=RDPamCallback;
  conv.appdata_ptr=(RDPam *)this;
  if((err=pam_start(system_pam_service,username,&conv,&pamh))!=PAM_SUCCESS) {
    syslog(LOG_NOTICE,"PAM Error: %s",pam_strerror(pamh,err));
    pam_end(pamh,err);
    CleanupPam();
    return false;
  }
  if((err=pam_authenticate(pamh,0))!=PAM_SUCCESS) {
    pam_end(pamh,err);
    CleanupPam();
    return false;
  }
  pam_end(pamh,err);
  CleanupPam();
  return true;
}


void RDPam::CleanupPam()
{
  if(system_pam_response==NULL) {
    return;
  }
  for(int i=0;i<system_pam_response_count;i++) {
    delete system_pam_response[i].resp;
  }
  delete system_pam_response;
  system_pam_response=NULL;
}
