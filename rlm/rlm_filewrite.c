/* rlm_filewrite.c
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
 *   gcc -shared -o rlm_filewrite.rlm rlm_filewrite.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_filewrite_devs;
char *rlm_filewrite_filenames;
int *rlm_filewrite_appends;
char *rlm_filewrite_formats;
int *rlm_filewrite_encodings;
int *rlm_filewrite_masters;
int *rlm_filewrite_aux1s;
int *rlm_filewrite_aux2s;

void rlm_filewrite_ReplaceChar(char c,char *str,int pos)
{
  int i;

  str[pos]=c;
  for(i=pos+1;i<strlen(str);i++) {
    str[i]=str[i+1];
  }
}


void rlm_filewrite_ProcessString(char *str)
{
  int i;

  for(i=0;i<(strlen(str)-1);i++) {
    if(str[i]=='\\') {
      if(str[i+1]=='r') {
	rlm_filewrite_ReplaceChar(13,str,i);
      }
      else {
	if(str[i+1]=='n') {
	  rlm_filewrite_ReplaceChar(10,str,i);
	}
      }
    }
  }
}


int rlm_filewrite_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_filewrite_RLMStart(void *ptr,const char *arg)
{
  char filename[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_filewrite_devs=0;
  rlm_filewrite_filenames=NULL;
  rlm_filewrite_appends=NULL;
  rlm_filewrite_formats=NULL;
  rlm_filewrite_encodings=NULL;
  rlm_filewrite_masters=NULL;
  rlm_filewrite_aux1s=NULL;
  rlm_filewrite_aux2s=NULL;

  sprintf(section,"File%d",i++);
  strncpy(filename,RLMGetStringValue(ptr,arg,section,"Filename",""),255);
  filename[255]=0;
  if(strlen(filename)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_filewrite: no files specified");
    return;
  }
  while(strlen(filename)>0) {
    rlm_filewrite_filenames=realloc(rlm_filewrite_filenames,
		      (rlm_filewrite_devs+1)*(rlm_filewrite_devs+1)*256);
    strcpy(rlm_filewrite_filenames+256*rlm_filewrite_devs,filename);
    rlm_filewrite_appends=realloc(rlm_filewrite_appends,
			    (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_appends[rlm_filewrite_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Append",0);
    rlm_filewrite_formats=realloc(rlm_filewrite_formats,(rlm_filewrite_devs+1)*256);
    strncpy(rlm_filewrite_formats+256*rlm_filewrite_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_filewrite_masters=realloc(rlm_filewrite_masters,
			    (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_masters[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"MasterLog");

    rlm_filewrite_encodings=realloc(rlm_filewrite_encodings,
			    (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_encodings[rlm_filewrite_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Encoding",RLM_ENCODE_NONE);
    rlm_filewrite_aux1s=realloc(rlm_filewrite_aux1s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_aux1s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_filewrite_aux2s=realloc(rlm_filewrite_aux2s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_aux2s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_filewrite: configured file \"%s\"",
	    rlm_filewrite_filenames+256*rlm_filewrite_devs);
    rlm_filewrite_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"File%d",i++);
    strncpy(filename,RLMGetStringValue(ptr,arg,section,"Filename",""),255);
    filename[255]=0;
  }
}


void rlm_filewrite_RLMFree(void *ptr)
{
  free(rlm_filewrite_filenames);
  free(rlm_filewrite_appends);
  free(rlm_filewrite_formats);
  free(rlm_filewrite_encodings);
  free(rlm_filewrite_masters);
  free(rlm_filewrite_aux1s);
  free(rlm_filewrite_aux2s);
}


void rlm_filewrite_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[8192];
  char msg[1500];
  FILE *f;

  for(i=0;i<rlm_filewrite_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_filewrite_masters[i];
	break;

      case 1:
	flag=rlm_filewrite_aux1s[i];
	break;

      case 2:
	flag=rlm_filewrite_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNextEncoded(ptr,now,next,
					   rlm_filewrite_formats+256*i,
					   rlm_filewrite_encodings[i]),8192);
      rlm_filewrite_ProcessString(str);
      if(rlm_filewrite_appends[i]==0) {
	f=fopen(rlm_filewrite_filenames+256*i,"w");
      }
      else {
	f=fopen(rlm_filewrite_filenames+256*i,"a");
      }
      if(f!=NULL) {
	snprintf(msg,1500,"rlm_filewrite: sending pad update: \"%s\" to \"%s\"",
		 str,rlm_filewrite_filenames+256*i);
	fprintf(f,"%s",str);
	fclose(f);
	RLMLog(ptr,LOG_INFO,msg);
      }
      else {
	snprintf(msg,1500,"rlm_filewrite: unable to open file \"%s\"",
		 rlm_filewrite_filenames+256*i);
	RLMLog(ptr,LOG_WARNING,msg);
      }
    }
  }
}
