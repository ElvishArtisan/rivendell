/* rlm_shoutcast1.c
 *
 *   (C) Copyright 2011 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It uses Now&Next PAD data
 * to update the metadata on an Icecast2 mountpoint specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_shoutcast1.rlm rlm_shoutcast1.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_shoutcast1_devs;
char *rlm_shoutcast1_passwords;
char *rlm_shoutcast1_hostnames;
int *rlm_shoutcast1_tcpports;
char *rlm_shoutcast1_formats;
int *rlm_shoutcast1_masters;
int *rlm_shoutcast1_aux1s;
int *rlm_shoutcast1_aux2s;


int rlm_shoutcast1_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
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


int rlm_shoutcast1_EncodeString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if(((sString[i]!='*') && (sString[i]!='-') &&
	(sString[i]!='_') && (sString[i]!='.')) && 
       ((sString[i]<'0') ||
       ((sString[i]>'9') && (sString[i]<'A')) ||
       ((sString[i]>'Z') && (sString[i]<'a')) ||
       (sString[i]>'z'))) {
      if(rlm_shoutcast1_BufferDiff(sString,i,2,dMaxSize)<0) {
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
    if(i>=dMaxSize) {
      return -1;
    }
  }
  return strlen(sString);
}


int rlm_shoutcast1_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_shoutcast1_RLMStart(void *ptr,const char *arg)
{
  char password[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_shoutcast1_devs=0;
  rlm_shoutcast1_passwords=NULL;
  rlm_shoutcast1_formats=NULL;
  rlm_shoutcast1_masters=NULL;
  rlm_shoutcast1_aux1s=NULL;
  rlm_shoutcast1_aux2s=NULL;

  sprintf(section,"Shoutcast%d",i++);
  strncpy(password,RLMGetStringValue(ptr,arg,section,"Password",""),255);
  password[255]=0;
  if(strlen(password)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_shoutcast1: no shoutcast servers specified");
    return;
  }
  while(strlen(password)>0) {
    rlm_shoutcast1_passwords=realloc(rlm_shoutcast1_passwords,
		      (rlm_shoutcast1_devs+1)*(rlm_shoutcast1_devs+1)*256);
    strcpy(rlm_shoutcast1_passwords+256*rlm_shoutcast1_devs,password);
    rlm_shoutcast1_hostnames=realloc(rlm_shoutcast1_hostnames,
			   (rlm_shoutcast1_devs+1)*(rlm_shoutcast1_devs+1)*256);
    strcpy(rlm_shoutcast1_hostnames+256*rlm_shoutcast1_devs,
	   RLMGetStringValue(ptr,arg,section,"Hostname",""));
    rlm_shoutcast1_tcpports=realloc(rlm_shoutcast1_tcpports,
			    (rlm_shoutcast1_devs+1)*sizeof(int));
    rlm_shoutcast1_tcpports[rlm_shoutcast1_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Tcpport",0);
    rlm_shoutcast1_formats=realloc(rlm_shoutcast1_formats,(rlm_shoutcast1_devs+1)*256);
    strncpy(rlm_shoutcast1_formats+256*rlm_shoutcast1_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_shoutcast1_masters=realloc(rlm_shoutcast1_masters,
			    (rlm_shoutcast1_devs+1)*sizeof(int));
    rlm_shoutcast1_masters[rlm_shoutcast1_devs]=
      rlm_shoutcast1_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_shoutcast1_aux1s=realloc(rlm_shoutcast1_aux1s,
			  (rlm_shoutcast1_devs+1)*sizeof(int));
    rlm_shoutcast1_aux1s[rlm_shoutcast1_devs]=
      rlm_shoutcast1_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_shoutcast1_aux2s=realloc(rlm_shoutcast1_aux2s,
			  (rlm_shoutcast1_devs+1)*sizeof(int));
    rlm_shoutcast1_aux2s[rlm_shoutcast1_devs]=
      rlm_shoutcast1_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_shoutcast1: configured server \"%s:%d\"",rlm_shoutcast1_hostnames+256*rlm_shoutcast1_devs,rlm_shoutcast1_tcpports[rlm_shoutcast1_devs]);
    rlm_shoutcast1_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Shoutcast%d",i++);
    strncpy(password,RLMGetStringValue(ptr,arg,section,"Password",""),255);
    password[255]=0;
  }
}


void rlm_shoutcast1_RLMFree(void *ptr)
{
  free(rlm_shoutcast1_passwords);
  free(rlm_shoutcast1_hostnames);
  free(rlm_shoutcast1_tcpports);
  free(rlm_shoutcast1_formats);
  free(rlm_shoutcast1_masters);
  free(rlm_shoutcast1_aux1s);
  free(rlm_shoutcast1_aux2s);
}


void rlm_shoutcast1_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[1024];
  char account[1024];
  char url[1024];
  char msg[1500];
  char user_agent[255];

  for(i=0;i<rlm_shoutcast1_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_shoutcast1_masters[i];
	break;

      case 1:
	flag=rlm_shoutcast1_aux1s[i];
	break;

      case 2:
	flag=rlm_shoutcast1_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNext(ptr,now,next,
				    rlm_shoutcast1_formats+256*i),256);
      rlm_shoutcast1_EncodeString(str,1023);
      snprintf(account,1024,":%s",rlm_shoutcast1_passwords+256*i);
      snprintf(url,1024,"http://%s:%d/admin.cgi?pass=%s&mode=updinfo&song=%s",
	       rlm_shoutcast1_hostnames+256*i,
	       rlm_shoutcast1_tcpports[i],
	       rlm_shoutcast1_passwords+256*i,
	       str);
      /*
       * D.N.A.S v1.9.8 refuses to process updates with the default CURL
       * user-agent value, hence we lie to it.
       */
      strncpy(user_agent,"User-agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.2",255);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-o","/dev/null","-s","--header",user_agent,
		 url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_shoutcast1: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_shoutcast1: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
