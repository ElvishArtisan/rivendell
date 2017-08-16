/* rlm_urlwrite.c
 *
 *   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
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
 * to write to one or more file(s) on the local system specified in the
 * configuration file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_urlwrite.rlm rlm_urlwrite.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_urlwrite_devs;
char *rlm_urlwrite_urls;
char *rlm_urlwrite_usernames;
char *rlm_urlwrite_passwords;
char *rlm_urlwrite_formats;
int *rlm_urlwrite_encodings;
int *rlm_urlwrite_masters;
int *rlm_urlwrite_aux1s;
int *rlm_urlwrite_aux2s;

void rlm_urlwrite_ReplaceChar(char c,char *str,int pos)
{
  int i;

  str[pos]=c;
  for(i=pos+1;i<strlen(str);i++) {
    str[i]=str[i+1];
  }
}


void rlm_urlwrite_ProcessString(char *str)
{
  int i;

  for(i=0;i<(strlen(str)-1);i++) {
    if(str[i]=='\\') {
      if(str[i+1]=='r') {
	rlm_urlwrite_ReplaceChar(13,str,i);
      }
      else {
	if(str[i+1]=='n') {
	  rlm_urlwrite_ReplaceChar(10,str,i);
	}
      }
    }
  }
}


int rlm_urlwrite_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_urlwrite_RLMStart(void *ptr,const char *arg)
{
  char url[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_urlwrite_devs=0;
  rlm_urlwrite_urls=NULL;
  rlm_urlwrite_usernames=NULL;
  rlm_urlwrite_passwords=NULL;
  rlm_urlwrite_formats=NULL;
  rlm_urlwrite_encodings=NULL;
  rlm_urlwrite_masters=NULL;
  rlm_urlwrite_aux1s=NULL;
  rlm_urlwrite_aux2s=NULL;

  sprintf(section,"Url%d",i++);
  strncpy(url,RLMGetStringValue(ptr,arg,section,"Url",""),255);
  url[255]=0;
  if(strlen(url)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_urlwrite: no URLs specified");
    return;
  }
  while(strlen(url)>0) {
    rlm_urlwrite_urls=realloc(rlm_urlwrite_urls,
		      (rlm_urlwrite_devs+1)*(rlm_urlwrite_devs+1)*256);
    strcpy(rlm_urlwrite_urls+256*rlm_urlwrite_devs,url);
    rlm_urlwrite_usernames=realloc(rlm_urlwrite_usernames,(rlm_urlwrite_devs+1)*256);
    strncpy(rlm_urlwrite_usernames+256*rlm_urlwrite_devs,
	    RLMGetStringValue(ptr,arg,section,"Username",""),256);
    rlm_urlwrite_passwords=realloc(rlm_urlwrite_passwords,(rlm_urlwrite_devs+1)*256);
    strncpy(rlm_urlwrite_passwords+256*rlm_urlwrite_devs,
	    RLMGetStringValue(ptr,arg,section,"Password",""),256);
    rlm_urlwrite_formats=realloc(rlm_urlwrite_formats,(rlm_urlwrite_devs+1)*8192);
    strncpy(rlm_urlwrite_formats+8192*rlm_urlwrite_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),8192);
    rlm_urlwrite_masters=realloc(rlm_urlwrite_masters,
			    (rlm_urlwrite_devs+1)*sizeof(int));
    rlm_urlwrite_masters[rlm_urlwrite_devs]=
      rlm_urlwrite_GetLogStatus(ptr,arg,section,"MasterLog");

    rlm_urlwrite_encodings=realloc(rlm_urlwrite_encodings,
			    (rlm_urlwrite_devs+1)*sizeof(int));
    rlm_urlwrite_encodings[rlm_urlwrite_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Encoding",RLM_ENCODE_NONE);
    rlm_urlwrite_aux1s=realloc(rlm_urlwrite_aux1s,
			  (rlm_urlwrite_devs+1)*sizeof(int));
    rlm_urlwrite_aux1s[rlm_urlwrite_devs]=
      rlm_urlwrite_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_urlwrite_aux2s=realloc(rlm_urlwrite_aux2s,
			  (rlm_urlwrite_devs+1)*sizeof(int));
    rlm_urlwrite_aux2s[rlm_urlwrite_devs]=
      rlm_urlwrite_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_urlwrite: configured URL \"%s\"",
	    rlm_urlwrite_urls+256*rlm_urlwrite_devs);
    rlm_urlwrite_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Url%d",i++);
    strncpy(url,RLMGetStringValue(ptr,arg,section,"Url",""),255);
    url[255]=0;
  }
}


void rlm_urlwrite_RLMFree(void *ptr)
{
  free(rlm_urlwrite_urls);
  free(rlm_urlwrite_usernames);
  free(rlm_urlwrite_passwords);
  free(rlm_urlwrite_formats);
  free(rlm_urlwrite_encodings);
  free(rlm_urlwrite_masters);
  free(rlm_urlwrite_aux1s);
  free(rlm_urlwrite_aux2s);
}


void rlm_urlwrite_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[8192];
  char msg[1500];
  char account[1024];
  char tempfile[1024];
  FILE *f;

  for(i=0;i<rlm_urlwrite_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_urlwrite_masters[i];
	break;

      case 1:
	flag=rlm_urlwrite_aux1s[i];
	break;

      case 2:
	flag=rlm_urlwrite_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNextEncoded(ptr,now,next,
					   rlm_urlwrite_formats+8192*i,
					   rlm_urlwrite_encodings[i]),8192);
      rlm_urlwrite_ProcessString(str);
      snprintf(account,1024,"%s:%s",rlm_urlwrite_usernames+256*i,
	       rlm_urlwrite_passwords+256*i);
      snprintf(tempfile,1024,"/tmp/rlm_urlwrite_%d",i);
      f=fopen(tempfile,"w");
      if(f!=NULL) {
	snprintf(msg,1500,"rlm_urlwrite: sending pad update: \"%s\" to \"%s\"",
		 str,rlm_urlwrite_urls+256*i);
	fprintf(f,"%s",str);
	fclose(f);
	if(fork()==0) {
	  execlp("curl","curl","-s","-k","-u",account,"-T",tempfile,
		 rlm_urlwrite_urls+256*i,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_urlwrite: unable to execute curl(1)");
	  exit(0);
	}
	RLMLog(ptr,LOG_INFO,msg);
      }
      else {
	snprintf(msg,1500,"rlm_urlwrite: unable to open file \"%s\"",
		 rlm_urlwrite_urls+256*i);
	RLMLog(ptr,LOG_WARNING,msg);
      }
    }
  }
}
