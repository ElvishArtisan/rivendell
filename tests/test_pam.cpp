// test_pam.cpp
//
// Test PAM Authentication Service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: test_pam.cpp,v 1.2 2010/07/29 19:32:39 cvs Exp $
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

#include <unistd.h>

#include <security/pam_appl.h>

#include <qapplication.h>

#include <rdcmd_switch.h>

#include <test_pam.h>

//
// NOTE: Since this is just a test harness, we make no attempt to
//       free() the response data structures after using them!
//

int ConversationResponseCallback(int num_msg,const struct pam_message **msg,
				 struct pam_response **resp,
				 void *appdata_ptr)
{
  int i;
  char *pw;

  *resp=new struct pam_response[num_msg];
  for(i=0;i<num_msg;i++) {
    resp[i]->resp=new char[256];
    switch(msg[i]->msg_style) {
    case PAM_PROMPT_ECHO_OFF:
      pw=getpass(msg[i]->msg);
      strncpy(resp[i]->resp,pw,256);
      resp[i]->resp_retcode=0;
      break;

    case PAM_PROMPT_ECHO_ON:
      printf("%s ",msg[i]->msg);
      fflush(NULL);
      fgets(resp[i]->resp,256,stdin);
      resp[i]->resp[strlen(resp[i]->resp)-1]=0;
      resp[i]->resp_retcode=0;
      break;

    case PAM_ERROR_MSG:
    case PAM_TEXT_INFO:
      printf("%s\n",msg[i]->msg);
      break;
    }
  }
  return 0;
}


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  int err;
  struct pam_conv conv;
  pam_handle_t *pamh=NULL;
  QString service_name="";

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"test_pam",TEST_PAM_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--service-name") {
      service_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
  }
  if(service_name.isEmpty()) {
    fprintf(stderr,"test_pam: missing service-name\n");
    exit(256);
  }

  memset(&conv,0,sizeof(conv));
  conv.conv=ConversationResponseCallback;
  if((err=pam_start(service_name,NULL,&conv,&pamh))!=PAM_SUCCESS) {
    perror(pam_strerror(pamh,err));
    exit(256);
  }
  err=pam_authenticate(pamh,0);
  if(err==PAM_SUCCESS) {
    printf("Success!\n");
  }
  else {
    perror(pam_strerror(pamh,err));
  }
  pam_end(pamh,err);
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
