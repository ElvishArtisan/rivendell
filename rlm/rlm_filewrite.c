/* rlm_filewrite.c
 *
 *   (C) Copyright 2012-2018 Fred Gleason <fredg@paravelsystems.com>
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
int *rlm_filewrite_vlog101s;
int *rlm_filewrite_vlog102s;
int *rlm_filewrite_vlog103s;
int *rlm_filewrite_vlog104s;
int *rlm_filewrite_vlog105s;
int *rlm_filewrite_vlog106s;
int *rlm_filewrite_vlog107s;
int *rlm_filewrite_vlog108s;
int *rlm_filewrite_vlog109s;
int *rlm_filewrite_vlog110s;
int *rlm_filewrite_vlog111s;
int *rlm_filewrite_vlog112s;
int *rlm_filewrite_vlog113s;
int *rlm_filewrite_vlog114s;
int *rlm_filewrite_vlog115s;
int *rlm_filewrite_vlog116s;
int *rlm_filewrite_vlog117s;
int *rlm_filewrite_vlog118s;
int *rlm_filewrite_vlog119s;
int *rlm_filewrite_vlog120s;

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
  rlm_filewrite_vlog101s=NULL;
  rlm_filewrite_vlog102s=NULL;
  rlm_filewrite_vlog103s=NULL;
  rlm_filewrite_vlog104s=NULL;
  rlm_filewrite_vlog105s=NULL;
  rlm_filewrite_vlog106s=NULL;
  rlm_filewrite_vlog107s=NULL;
  rlm_filewrite_vlog108s=NULL;
  rlm_filewrite_vlog109s=NULL;
  rlm_filewrite_vlog110s=NULL;
  rlm_filewrite_vlog111s=NULL;
  rlm_filewrite_vlog112s=NULL;
  rlm_filewrite_vlog113s=NULL;
  rlm_filewrite_vlog114s=NULL;
  rlm_filewrite_vlog115s=NULL;
  rlm_filewrite_vlog116s=NULL;
  rlm_filewrite_vlog117s=NULL;
  rlm_filewrite_vlog118s=NULL;
  rlm_filewrite_vlog119s=NULL;
  rlm_filewrite_vlog120s=NULL;

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
    rlm_filewrite_formats=realloc(rlm_filewrite_formats,(rlm_filewrite_devs+1)*8192);
    strncpy(rlm_filewrite_formats+8192*rlm_filewrite_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),8192);
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

    rlm_filewrite_vlog101s=realloc(rlm_filewrite_vlog101s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog101s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog101");

    rlm_filewrite_vlog102s=realloc(rlm_filewrite_vlog102s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog102s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog102");

    rlm_filewrite_vlog103s=realloc(rlm_filewrite_vlog103s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog103s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog103");

    rlm_filewrite_vlog104s=realloc(rlm_filewrite_vlog104s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog104s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog104");

    rlm_filewrite_vlog105s=realloc(rlm_filewrite_vlog105s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog105s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog105");

    rlm_filewrite_vlog106s=realloc(rlm_filewrite_vlog106s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog106s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog106");

    rlm_filewrite_vlog107s=realloc(rlm_filewrite_vlog107s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog107s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog107");

    rlm_filewrite_vlog108s=realloc(rlm_filewrite_vlog108s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog108s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog108");

    rlm_filewrite_vlog109s=realloc(rlm_filewrite_vlog109s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog109s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog109");

    rlm_filewrite_vlog110s=realloc(rlm_filewrite_vlog110s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog110s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog110");

    rlm_filewrite_vlog111s=realloc(rlm_filewrite_vlog111s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog111s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog111");

    rlm_filewrite_vlog112s=realloc(rlm_filewrite_vlog112s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog112s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog112");

    rlm_filewrite_vlog113s=realloc(rlm_filewrite_vlog113s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog113s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog113");

    rlm_filewrite_vlog114s=realloc(rlm_filewrite_vlog114s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog114s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog114");

    rlm_filewrite_vlog115s=realloc(rlm_filewrite_vlog115s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog115s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog115");

    rlm_filewrite_vlog116s=realloc(rlm_filewrite_vlog116s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog116s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog116");

    rlm_filewrite_vlog117s=realloc(rlm_filewrite_vlog117s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog117s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog117");

    rlm_filewrite_vlog118s=realloc(rlm_filewrite_vlog118s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog118s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog118");

    rlm_filewrite_vlog119s=realloc(rlm_filewrite_vlog119s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog119s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog119");

    rlm_filewrite_vlog120s=realloc(rlm_filewrite_vlog120s,
			  (rlm_filewrite_devs+1)*sizeof(int));
    rlm_filewrite_vlog120s[rlm_filewrite_devs]=
      rlm_filewrite_GetLogStatus(ptr,arg,section,"VLog120");

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
  free(rlm_filewrite_vlog101s);
  free(rlm_filewrite_vlog102s);
  free(rlm_filewrite_vlog103s);
  free(rlm_filewrite_vlog104s);
  free(rlm_filewrite_vlog105s);
  free(rlm_filewrite_vlog106s);
  free(rlm_filewrite_vlog107s);
  free(rlm_filewrite_vlog108s);
  free(rlm_filewrite_vlog109s);
  free(rlm_filewrite_vlog110s);
  free(rlm_filewrite_vlog111s);
  free(rlm_filewrite_vlog112s);
  free(rlm_filewrite_vlog113s);
  free(rlm_filewrite_vlog114s);
  free(rlm_filewrite_vlog115s);
  free(rlm_filewrite_vlog116s);
  free(rlm_filewrite_vlog117s);
  free(rlm_filewrite_vlog118s);
  free(rlm_filewrite_vlog119s);
  free(rlm_filewrite_vlog120s);
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

    case 100:
      flag=rlm_filewrite_vlog101s[i];
      break;

    case 101:
      flag=rlm_filewrite_vlog102s[i];
      break;

    case 102:
      flag=rlm_filewrite_vlog103s[i];
      break;

    case 103:
      flag=rlm_filewrite_vlog104s[i];
      break;

    case 104:
      flag=rlm_filewrite_vlog105s[i];
      break;

    case 105:
      flag=rlm_filewrite_vlog106s[i];
      break;

    case 106:
      flag=rlm_filewrite_vlog107s[i];
      break;

    case 107:
      flag=rlm_filewrite_vlog108s[i];
      break;

    case 108:
      flag=rlm_filewrite_vlog109s[i];
      break;

    case 109:
      flag=rlm_filewrite_vlog110s[i];
      break;

    case 110:
      flag=rlm_filewrite_vlog111s[i];
      break;

    case 111:
      flag=rlm_filewrite_vlog112s[i];
      break;

    case 112:
      flag=rlm_filewrite_vlog113s[i];
      break;

    case 113:
      flag=rlm_filewrite_vlog114s[i];
      break;

    case 114:
      flag=rlm_filewrite_vlog115s[i];
      break;

    case 115:
      flag=rlm_filewrite_vlog116s[i];
      break;

    case 116:
      flag=rlm_filewrite_vlog117s[i];
      break;

    case 117:
      flag=rlm_filewrite_vlog118s[i];
      break;

    case 118:
      flag=rlm_filewrite_vlog119s[i];
      break;

    case 119:
      flag=rlm_filewrite_vlog120s[i];
      break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str,RLMResolveNowNextEncoded(ptr,now,next,
					   rlm_filewrite_formats+8192*i,
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
