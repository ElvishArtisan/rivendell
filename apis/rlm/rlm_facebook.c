/* rlm_facebook.c
 *
 *   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2
 *   as published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data
 * to the Facebook account(s) specified in the configuration file pointed
 * to by the plugin argument.  For information about the Facebook service,
 * see http://www.facebook.com/.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_facebook.rlm rlm_facebook.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_facebook_devs;
char *rlm_facebook_addresses;
char *rlm_facebook_passwords;
char *rlm_facebook_formats;
int *rlm_facebook_masters;
int *rlm_facebook_aux1s;
int *rlm_facebook_aux2s;
char *rlm_facebook_cookies;


int rlm_facebook_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
{
  int dOldSize,dNewSize;
  int i;

  /*
   * Will it fit?
   */
  dOldSize=strlen(sString);
  if((dOldSize+dDiff)>=dMaxSize) {
    return -1;
  }
  dNewSize=dOldSize+dDiff;

  /* 
   * Adding characters 
   */
  if(dDiff>0) {
    for(i=dOldSize;i>dOrigin;i--) {
      sString[i+dDiff]=sString[i];
    }
    return dNewSize;
  }

  /* 
   * No Change
   */
  if(dDiff==0) {
    return dNewSize;
  }

  /*
   * Deleting Characters
   */
  if(dDiff<0) {
    for(i=dOrigin;i<dOldSize;i++) {
      sString[i]=sString[i-dDiff];
    }
    return dNewSize;
  }
  return -1; 
}


int rlm_facebook_EncodeString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if(((sString[i]!=' ') && (sString[i]!='*') && (sString[i]!='-') &&
	(sString[i]!='_') && (sString[i]!='.')) && 
       ((sString[i]<'0') ||
       ((sString[i]>'9') && (sString[i]<'A')) ||
       ((sString[i]>'Z') && (sString[i]<'a')) ||
       (sString[i]>'z'))) {
      if(rlm_facebook_BufferDiff(sString,i,2,dMaxSize)<0) {
	return -1;
      }
      sprintf(sAccum,"%%%2x",sString[i]);
      sString[i++]=sAccum[0];
      sString[i++]=sAccum[1];
      sString[i]=sAccum[2];
    }
    if(sString[i]==' ') {
      sString[i]='+';
    }
    i++;
  }
  return strlen(sString);
}


void rlm_facebook_Login(int config)
{
  char cmd[1024];

  unlink(rlm_facebook_cookies+256*config);  /* Cleanup from past session */
  snprintf(cmd,1024,
	   "curl -o /dev/null -s -A Mozilla/4.0 -c %s http://www.facebook.com/",
	   rlm_facebook_cookies+256*config);
  system(cmd);
  snprintf(cmd,1024,"curl -o /dev/null -s --insecure -A Mozilla/4.0 -c %s -b %s -d persistent=1 -d email=%s -d pass=%s https://login.facebook.com/login.php",
	   rlm_facebook_cookies+256*config,
	   rlm_facebook_cookies+256*config,
	   rlm_facebook_addresses+256*config,
	   rlm_facebook_passwords+256*config);
  system(cmd);
}


void rlm_facebook_Logout(int config)
{
  if(fork()==0) {
    execlp("curl","curl","-A","Mozilla/4.0","-b",
	   rlm_facebook_cookies+256*config,
	   "-o","/dev/null","-s",
	   "http://www.facebook.com/logout.php",
	   (char *)NULL);
    exit(0);
  }
}


int rlm_facebook_GetLogStatus(void *ptr,const char *arg,const char *section,
			    const char *logname)
{
  const char *tag=RLMGetStringValue(ptr,arg,section,logname,"");
  if(strcasecmp(tag,"yes")==0) {
    return 1;
  }
  if(strcasecmp(tag,"on")==0) {
    return 1;
  }
  if(strcasecmp(tag,"true")==0) {
    return 1;
  }
  if(strcasecmp(tag,"no")==0) {
    return 0;
  }
  if(strcasecmp(tag,"off")==0) {
    return 0;
  }
  if(strcasecmp(tag,"false")==0) {
    return 0;
  }
  if(strcasecmp(tag,"onair")==0) {
    return 2;
  }  
  return 0;
}


void rlm_facebook_RLMStart(void *ptr,const char *arg)
{
  char address[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_facebook_devs=0;
  rlm_facebook_addresses=NULL;
  rlm_facebook_passwords=NULL;
  rlm_facebook_formats=NULL;
  rlm_facebook_masters=NULL;
  rlm_facebook_aux1s=NULL;
  rlm_facebook_aux2s=NULL;
  rlm_facebook_cookies=NULL;

  sprintf(section,"Facebook%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"EmailAddress",""),255);
  address[255]=0;
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_facebook: no facebook accounts specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_facebook_addresses=realloc(rlm_facebook_addresses,
			   (rlm_facebook_devs+1)*(rlm_facebook_devs+1)*256);
    strcpy(rlm_facebook_addresses+256*rlm_facebook_devs,address);
    rlm_facebook_EncodeString(rlm_facebook_addresses+256*rlm_facebook_devs,255);
    rlm_facebook_passwords=realloc(rlm_facebook_passwords,
			   (rlm_facebook_devs+1)*(rlm_facebook_devs+1)*256);
    strcpy(rlm_facebook_passwords+256*rlm_facebook_devs,
	   RLMGetStringValue(ptr,arg,section,"Password",""));
    rlm_facebook_EncodeString(rlm_facebook_passwords+256*rlm_facebook_devs,255);
    rlm_facebook_formats=realloc(rlm_facebook_formats,(rlm_facebook_devs+1)*256);
    strncpy(rlm_facebook_formats+256*rlm_facebook_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_facebook_masters=realloc(rlm_facebook_masters,
			    (rlm_facebook_devs+1)*sizeof(int));
    rlm_facebook_masters[rlm_facebook_devs]=
      rlm_facebook_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_facebook_aux1s=realloc(rlm_facebook_aux1s,
			  (rlm_facebook_devs+1)*sizeof(int));
    rlm_facebook_aux1s[rlm_facebook_devs]=
      rlm_facebook_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_facebook_aux2s=realloc(rlm_facebook_aux2s,
			  (rlm_facebook_devs+1)*sizeof(int));
    rlm_facebook_aux2s[rlm_facebook_devs]=
      rlm_facebook_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_facebook: configured account \"%s\"",address);
    rlm_facebook_cookies=
      realloc(rlm_facebook_cookies,(rlm_facebook_devs+1)*256);
    snprintf(rlm_facebook_cookies+rlm_facebook_devs*256,256,
	     "/tmp/rlm_facebook%d_cookies.txt",rlm_facebook_devs+1);
    rlm_facebook_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Facebook%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"EmailAddress",""),255);
    address[255]=0;
  }

  for(i=0;i<(rlm_facebook_devs);i++) {
    rlm_facebook_Login(i);
  }
}


void rlm_facebook_RLMFree(void *ptr)
{
  int i;

  for(i=0;i<(rlm_facebook_devs);i++) {
    rlm_facebook_Logout(i);
  }
  free(rlm_facebook_addresses);
  free(rlm_facebook_passwords);
  free(rlm_facebook_formats);
  free(rlm_facebook_masters);
  free(rlm_facebook_aux1s);
  free(rlm_facebook_aux2s);
  free(rlm_facebook_cookies);
}


void rlm_facebook_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				 const struct rlm_log *log,
				 const struct rlm_pad *now,
				 const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[1024];
  char pad[1024];
  char msg[1500];

  for(i=0;i<rlm_facebook_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_facebook_masters[i];
	break;

      case 1:
	flag=rlm_facebook_aux1s[i];
	break;

      case 2:
	flag=rlm_facebook_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNext(ptr,now,next,
				    rlm_facebook_formats+256*i),256);
      rlm_facebook_EncodeString(str,1023);
      snprintf(pad,1024,"status=%s",str);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-A","Mozilla/4.0","-b",
		 rlm_facebook_cookies+256*i,
		 "-d",pad,
		 "-d","test_name=INLINE_STATUS_EDITOR",
		 "-d","action=OTHER_UPDATE",
		 "-d","post_form_id=aae2d1af1c8ed0cd36ade54bc8f48427",
		 "-o","/dev/null","-s",
		 "http://www.facebook.com/updatestatus.php",(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_facebook: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_facebook: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
