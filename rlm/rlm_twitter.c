/* rlm_twitter.c
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
 * to the Twitter account(s) specified in the configuration file pointed
 * to by the plugin argument.  For information about the Twitter service,
 * see http://www.twitter.com/.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_twitter.rlm rlm_twitter.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_twitter_devs;
char *rlm_twitter_addresses;
char *rlm_twitter_passwords;
char *rlm_twitter_formats;
int *rlm_twitter_masters;
int *rlm_twitter_aux1s;
int *rlm_twitter_aux2s;


int rlm_twitter_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
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


int rlm_twitter_EncodeString(char *sString,int dMaxSize)
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
      if(rlm_twitter_BufferDiff(sString,i,2,dMaxSize)<0) {
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


int rlm_twitter_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_twitter_RLMStart(void *ptr,const char *arg)
{
  char address[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_twitter_devs=0;
  rlm_twitter_addresses=NULL;
  rlm_twitter_passwords=NULL;
  rlm_twitter_formats=NULL;
  rlm_twitter_masters=NULL;
  rlm_twitter_aux1s=NULL;
  rlm_twitter_aux2s=NULL;

  sprintf(section,"Twitter%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"EmailAddress",""),255);
  address[255]=0;
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_twitter: no twitter accounts specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_twitter_addresses=realloc(rlm_twitter_addresses,
			   (rlm_twitter_devs+1)*(rlm_twitter_devs+1)*256);
    strcpy(rlm_twitter_addresses+256*rlm_twitter_devs,address);
    rlm_twitter_passwords=realloc(rlm_twitter_passwords,
			   (rlm_twitter_devs+1)*(rlm_twitter_devs+1)*256);
    strcpy(rlm_twitter_passwords+256*rlm_twitter_devs,
	   RLMGetStringValue(ptr,arg,section,"Password",""));
    rlm_twitter_formats=realloc(rlm_twitter_formats,(rlm_twitter_devs+1)*256);
    strncpy(rlm_twitter_formats+256*rlm_twitter_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_twitter_masters=realloc(rlm_twitter_masters,
			    (rlm_twitter_devs+1)*sizeof(int));
    rlm_twitter_masters[rlm_twitter_devs]=
      rlm_twitter_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_twitter_aux1s=realloc(rlm_twitter_aux1s,
			  (rlm_twitter_devs+1)*sizeof(int));
    rlm_twitter_aux1s[rlm_twitter_devs]=
      rlm_twitter_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_twitter_aux2s=realloc(rlm_twitter_aux2s,
			  (rlm_twitter_devs+1)*sizeof(int));
    rlm_twitter_aux2s[rlm_twitter_devs]=
      rlm_twitter_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_twitter: configured account \"%s\"",address);
    rlm_twitter_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Twitter%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"EmailAddress",""),255);
    address[255]=0;
  }
}


void rlm_twitter_RLMFree(void *ptr)
{
  free(rlm_twitter_addresses);
  free(rlm_twitter_passwords);
  free(rlm_twitter_formats);
  free(rlm_twitter_masters);
  free(rlm_twitter_aux1s);
  free(rlm_twitter_aux2s);
}


void rlm_twitter_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[1024];
  char account[1024];
  char pad[1024];
  char msg[1500];

  for(i=0;i<rlm_twitter_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_twitter_masters[i];
	break;

      case 1:
	flag=rlm_twitter_aux1s[i];
	break;

      case 2:
	flag=rlm_twitter_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNext(ptr,now,next,
				    rlm_twitter_formats+256*i),256);
      rlm_twitter_EncodeString(str,1023);
      snprintf(account,1024,"%s:%s",rlm_twitter_addresses+256*i,
	       rlm_twitter_passwords+256*i);
      snprintf(pad,1024,"status=%s",str);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-u",account,"-d",pad,"-o","/dev/null","-s",
		 "http://twitter.com/statuses/update.xml",(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_twitter: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_twitter: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
