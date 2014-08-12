/* rlm_icecast2.c
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
 * This is a Rivendell Loadable Module.  It uses Now&Next PAD data
 * to update the metadata on an Icecast2 mountpoint specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_icecast2.rlm rlm_icecast2.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_icecast2_devs;
char *rlm_icecast2_usernames;
char *rlm_icecast2_passwords;
char *rlm_icecast2_hostnames;
int *rlm_icecast2_tcpports;
char *rlm_icecast2_mountpoints;
char *rlm_icecast2_formats;
int *rlm_icecast2_masters;
int *rlm_icecast2_aux1s;
int *rlm_icecast2_aux2s;


int rlm_icecast2_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
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


int rlm_icecast2_EncodeString(char *sString,int dMaxSize)
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
      if(rlm_icecast2_BufferDiff(sString,i,2,dMaxSize)<0) {
	fprintf(stderr,"rlm_icecast2: BufferDiff() failed, maxsize: %d\n",
		dMaxSize);
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
      fprintf(stderr,"rlm_icecast2: offset exceeded limit, maxsize: %d\n",
	      dMaxSize);
      return -1;
    }
  }
  return strlen(sString);
}


int rlm_icecast2_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_icecast2_RLMStart(void *ptr,const char *arg)
{
  char username[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_icecast2_devs=0;
  rlm_icecast2_usernames=NULL;
  rlm_icecast2_passwords=NULL;
  rlm_icecast2_formats=NULL;
  rlm_icecast2_masters=NULL;
  rlm_icecast2_aux1s=NULL;
  rlm_icecast2_aux2s=NULL;

  sprintf(section,"Icecast%d",i++);
  strncpy(username,RLMGetStringValue(ptr,arg,section,"Username",""),255);
  username[255]=0;
  if(strlen(username)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_icecast2: no icecast mountpoints specified");
    return;
  }
  while(strlen(username)>0) {
    rlm_icecast2_usernames=realloc(rlm_icecast2_usernames,
			   (rlm_icecast2_devs+1)*(rlm_icecast2_devs+1)*256);
    strcpy(rlm_icecast2_usernames+256*rlm_icecast2_devs,username);

    rlm_icecast2_passwords=realloc(rlm_icecast2_passwords,
			   (rlm_icecast2_devs+1)*(rlm_icecast2_devs+1)*256);
    strcpy(rlm_icecast2_passwords+256*rlm_icecast2_devs,
	   RLMGetStringValue(ptr,arg,section,"Password",""));
    rlm_icecast2_hostnames=realloc(rlm_icecast2_hostnames,
			   (rlm_icecast2_devs+1)*(rlm_icecast2_devs+1)*256);
    strcpy(rlm_icecast2_hostnames+256*rlm_icecast2_devs,
	   RLMGetStringValue(ptr,arg,section,"Hostname",""));
    rlm_icecast2_tcpports=realloc(rlm_icecast2_tcpports,
			    (rlm_icecast2_devs+1)*sizeof(int));
    rlm_icecast2_tcpports[rlm_icecast2_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Tcpport",0);
    rlm_icecast2_mountpoints=realloc(rlm_icecast2_mountpoints,
			   (rlm_icecast2_devs+1)*(rlm_icecast2_devs+1)*256);
    strcpy(rlm_icecast2_mountpoints+256*rlm_icecast2_devs,
	   RLMGetStringValue(ptr,arg,section,"Mountpoint",""));
    rlm_icecast2_formats=realloc(rlm_icecast2_formats,(rlm_icecast2_devs+1)*256);
    strncpy(rlm_icecast2_formats+256*rlm_icecast2_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_icecast2_masters=realloc(rlm_icecast2_masters,
			    (rlm_icecast2_devs+1)*sizeof(int));
    rlm_icecast2_masters[rlm_icecast2_devs]=
      rlm_icecast2_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_icecast2_aux1s=realloc(rlm_icecast2_aux1s,
			  (rlm_icecast2_devs+1)*sizeof(int));
    rlm_icecast2_aux1s[rlm_icecast2_devs]=
      rlm_icecast2_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_icecast2_aux2s=realloc(rlm_icecast2_aux2s,
			  (rlm_icecast2_devs+1)*sizeof(int));
    rlm_icecast2_aux2s[rlm_icecast2_devs]=
      rlm_icecast2_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_icecast2: configured mountpoint \"http://%s:%u%s\"",
	    rlm_icecast2_hostnames+256*rlm_icecast2_devs,
	    rlm_icecast2_tcpports[rlm_icecast2_devs],
	    rlm_icecast2_mountpoints+256*rlm_icecast2_devs);
    rlm_icecast2_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Icecast%d",i++);
    strncpy(username,RLMGetStringValue(ptr,arg,section,"Username",""),255);
    username[255]=0;
  }
}


void rlm_icecast2_RLMFree(void *ptr)
{
  free(rlm_icecast2_usernames);
  free(rlm_icecast2_passwords);
  free(rlm_icecast2_hostnames);
  free(rlm_icecast2_tcpports);
  free(rlm_icecast2_mountpoints);
  free(rlm_icecast2_formats);
  free(rlm_icecast2_masters);
  free(rlm_icecast2_aux1s);
  free(rlm_icecast2_aux2s);
}


void rlm_icecast2_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
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

  for(i=0;i<rlm_icecast2_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_icecast2_masters[i];
	break;

      case 1:
	flag=rlm_icecast2_aux1s[i];
	break;

      case 2:
	flag=rlm_icecast2_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNext(ptr,now,next,
				    rlm_icecast2_formats+256*i),256);
      rlm_icecast2_EncodeString(str,1023);
      snprintf(account,1024,"%s:%s",rlm_icecast2_usernames+256*i,
	       rlm_icecast2_passwords+256*i);
      snprintf(url,1024,"http://%s:%d/admin/metadata?mount=%s&mode=updinfo&song=%s",
	       rlm_icecast2_hostnames+256*i,
	       rlm_icecast2_tcpports[i],
	       rlm_icecast2_mountpoints+256*i,str);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-u",account,"-o","/dev/null","-s",
		 url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_icecast2: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_icecast2: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
