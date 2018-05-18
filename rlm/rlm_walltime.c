/* rlm_walltime.c
 *
 *   (C) Copyright 2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data to
 * to one or more Walltime clock displays as specified in the configuration
 * file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_walltime.rlm rlm_walltime.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_walltime_devs;
char *rlm_walltime_addresses;
char *rlm_walltime_passwords;
char *rlm_walltime_formats;
int *rlm_walltime_masters;
int *rlm_walltime_aux1s;
int *rlm_walltime_aux2s;
int *rlm_walltime_vlog101s;
int *rlm_walltime_vlog102s;
int *rlm_walltime_vlog103s;
int *rlm_walltime_vlog104s;
int *rlm_walltime_vlog105s;
int *rlm_walltime_vlog106s;
int *rlm_walltime_vlog107s;
int *rlm_walltime_vlog108s;
int *rlm_walltime_vlog109s;
int *rlm_walltime_vlog110s;
int *rlm_walltime_vlog111s;
int *rlm_walltime_vlog112s;
int *rlm_walltime_vlog113s;
int *rlm_walltime_vlog114s;
int *rlm_walltime_vlog115s;
int *rlm_walltime_vlog116s;
int *rlm_walltime_vlog117s;
int *rlm_walltime_vlog118s;
int *rlm_walltime_vlog119s;
int *rlm_walltime_vlog120s;

int rlm_walltime_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_walltime_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_walltime_devs=0;
  rlm_walltime_addresses=NULL;
  rlm_walltime_passwords=NULL;
  rlm_walltime_formats=NULL;
  rlm_walltime_masters=NULL;
  rlm_walltime_aux1s=NULL;
  rlm_walltime_aux2s=NULL;
  rlm_walltime_vlog101s=NULL;
  rlm_walltime_vlog102s=NULL;
  rlm_walltime_vlog103s=NULL;
  rlm_walltime_vlog104s=NULL;
  rlm_walltime_vlog105s=NULL;
  rlm_walltime_vlog106s=NULL;
  rlm_walltime_vlog107s=NULL;
  rlm_walltime_vlog108s=NULL;
  rlm_walltime_vlog109s=NULL;
  rlm_walltime_vlog110s=NULL;
  rlm_walltime_vlog111s=NULL;
  rlm_walltime_vlog112s=NULL;
  rlm_walltime_vlog113s=NULL;
  rlm_walltime_vlog114s=NULL;
  rlm_walltime_vlog115s=NULL;
  rlm_walltime_vlog116s=NULL;
  rlm_walltime_vlog117s=NULL;
  rlm_walltime_vlog118s=NULL;
  rlm_walltime_vlog119s=NULL;
  rlm_walltime_vlog120s=NULL;

  sprintf(section,"Walltime%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_walltime: no walltime destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_walltime_addresses=
      realloc(rlm_walltime_addresses,(rlm_walltime_devs+1)*(rlm_walltime_devs+1)*16);
    strcpy(rlm_walltime_addresses+16*rlm_walltime_devs,address);
    rlm_walltime_passwords=realloc(rlm_walltime_passwords,(rlm_walltime_devs+1)*1024);
    strncpy(rlm_walltime_passwords+1024*rlm_walltime_devs,
	    RLMGetStringValue(ptr,arg,section,"Password",""),1023);
    rlm_walltime_formats=realloc(rlm_walltime_formats,(rlm_walltime_devs+1)*8192);
    strncpy(rlm_walltime_formats+8192*rlm_walltime_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),8192);
    rlm_walltime_masters=realloc(rlm_walltime_masters,
			    (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_masters[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"MasterLog");

    rlm_walltime_aux1s=realloc(rlm_walltime_aux1s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_aux1s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_walltime_aux2s=realloc(rlm_walltime_aux2s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_aux2s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"Aux2Log");

    rlm_walltime_vlog101s=realloc(rlm_walltime_vlog101s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog101s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog101");

    rlm_walltime_vlog102s=realloc(rlm_walltime_vlog102s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog102s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog102");

    rlm_walltime_vlog103s=realloc(rlm_walltime_vlog103s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog103s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog103");

    rlm_walltime_vlog104s=realloc(rlm_walltime_vlog104s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog104s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog104");

    rlm_walltime_vlog105s=realloc(rlm_walltime_vlog105s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog105s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog105");

    rlm_walltime_vlog106s=realloc(rlm_walltime_vlog106s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog106s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog106");

    rlm_walltime_vlog107s=realloc(rlm_walltime_vlog107s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog107s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog107");

    rlm_walltime_vlog108s=realloc(rlm_walltime_vlog108s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog108s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog108");

    rlm_walltime_vlog109s=realloc(rlm_walltime_vlog109s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog109s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog109");

    rlm_walltime_vlog110s=realloc(rlm_walltime_vlog110s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog110s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog110");

    rlm_walltime_vlog111s=realloc(rlm_walltime_vlog111s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog111s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog111");

    rlm_walltime_vlog112s=realloc(rlm_walltime_vlog112s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog112s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog112");

    rlm_walltime_vlog113s=realloc(rlm_walltime_vlog113s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog113s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog113");

    rlm_walltime_vlog114s=realloc(rlm_walltime_vlog114s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog114s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog114");

    rlm_walltime_vlog115s=realloc(rlm_walltime_vlog115s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog115s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog115");

    rlm_walltime_vlog116s=realloc(rlm_walltime_vlog116s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog116s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog116");

    rlm_walltime_vlog117s=realloc(rlm_walltime_vlog117s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog117s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog117");

    rlm_walltime_vlog118s=realloc(rlm_walltime_vlog118s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog118s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog118");

    rlm_walltime_vlog119s=realloc(rlm_walltime_vlog119s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog119s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog119");

    rlm_walltime_vlog120s=realloc(rlm_walltime_vlog120s,
			  (rlm_walltime_devs+1)*sizeof(int));
    rlm_walltime_vlog120s[rlm_walltime_devs]=
      rlm_walltime_GetLogStatus(ptr,arg,section,"VLog120");

    sprintf(errtext,"rlm_walltime: configured destination \"%s\"",address);
    rlm_walltime_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Walltime%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_walltime_RLMFree(void *ptr)
{
  free(rlm_walltime_addresses);
  free(rlm_walltime_passwords);
  free(rlm_walltime_formats);
  free(rlm_walltime_masters);
  free(rlm_walltime_aux1s);
  free(rlm_walltime_aux2s);
  free(rlm_walltime_vlog101s);
  free(rlm_walltime_vlog102s);
  free(rlm_walltime_vlog103s);
  free(rlm_walltime_vlog104s);
  free(rlm_walltime_vlog105s);
  free(rlm_walltime_vlog106s);
  free(rlm_walltime_vlog107s);
  free(rlm_walltime_vlog108s);
  free(rlm_walltime_vlog109s);
  free(rlm_walltime_vlog110s);
  free(rlm_walltime_vlog111s);
  free(rlm_walltime_vlog112s);
  free(rlm_walltime_vlog113s);
  free(rlm_walltime_vlog114s);
  free(rlm_walltime_vlog115s);
  free(rlm_walltime_vlog116s);
  free(rlm_walltime_vlog117s);
  free(rlm_walltime_vlog118s);
  free(rlm_walltime_vlog119s);
  free(rlm_walltime_vlog120s);
}


void rlm_walltime_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  FILE *f;
  char tempfile[256];
  int fd=-1;
  char url[1024];
  char password[1024];

  for(i=0;i<rlm_walltime_devs;i++) {
    switch(log->log_mach) {
    case 0:
      flag=rlm_walltime_masters[i];
      break;

    case 1:
      flag=rlm_walltime_aux1s[i];
      break;

    case 2:
      flag=rlm_walltime_aux2s[i];
      break;

    case 100:
      flag=rlm_walltime_vlog101s[i];
      break;

    case 101:
      flag=rlm_walltime_vlog102s[i];
      break;

    case 102:
      flag=rlm_walltime_vlog103s[i];
      break;

    case 103:
      flag=rlm_walltime_vlog104s[i];
      break;

    case 104:
      flag=rlm_walltime_vlog105s[i];
      break;

    case 105:
      flag=rlm_walltime_vlog106s[i];
      break;

    case 106:
      flag=rlm_walltime_vlog107s[i];
      break;

    case 107:
      flag=rlm_walltime_vlog108s[i];
      break;

    case 108:
      flag=rlm_walltime_vlog109s[i];
      break;

    case 109:
      flag=rlm_walltime_vlog110s[i];
      break;

    case 110:
      flag=rlm_walltime_vlog111s[i];
      break;

    case 111:
      flag=rlm_walltime_vlog112s[i];
      break;

    case 112:
      flag=rlm_walltime_vlog113s[i];
      break;

    case 113:
      flag=rlm_walltime_vlog114s[i];
      break;

    case 114:
      flag=rlm_walltime_vlog115s[i];
      break;

    case 115:
      flag=rlm_walltime_vlog116s[i];
      break;

    case 116:
      flag=rlm_walltime_vlog117s[i];
      break;

    case 117:
      flag=rlm_walltime_vlog118s[i];
      break;

    case 118:
      flag=rlm_walltime_vlog119s[i];
      break;

    case 119:
      flag=rlm_walltime_vlog120s[i];
      break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(tempfile,"/tmp/walltimeXXXXXX",256);
      if((fd=mkstemp(tempfile))>0) {
	f=fdopen(fd,"w");
	if(now->rlm_len==0) {
	  fprintf(f,"<body bgcolor=\"#000000\">&nbsp;</body>");
	}
	else {
	  fprintf(f,"%s\n",
	    RLMResolveNowNextEncoded(ptr,now,next,rlm_walltime_formats+8192*i,
					   RLM_ENCODE_XML));
	}
	fclose(f);
	snprintf(url,1024,"http://%s/webwidget",
		 rlm_walltime_addresses+16*i);
	snprintf(password,1024,"user:%s",rlm_walltime_passwords+1024*i);
	if(fork()==0) {
	  execlp("curl","curl","-u",password,"-o","/dev/null","-s","-T",
		 tempfile,url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_walltime: unable to execute curl(1)");
	  exit(0);
	}
	RLMLog(ptr,LOG_INFO,"rlm_walltime: sending pad update!");
      }
    }
    else {
      RLMLog(ptr,LOG_WARNING,"rlm_walltime: unable to create temp file");
    }
  }
}
