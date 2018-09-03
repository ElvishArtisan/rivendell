/* rlm_xmpad.c
 *
 *   (C) Copyright 2008-2018 Fred Gleason <fredg@paravelsystems.com>
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
 * to an XM radio channel, using XM's in-house format.
 * plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_xmpad.rlm rlm_xmpad.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

#define RLM_XMPAD_DELIMITER 0x02
//#define RLM_XMPAD_DELIMITER 0x7C

int rlm_xmpad_devs;
int *rlm_xmpad_ids;
int *rlm_xmpad_handles;
int *rlm_xmpad_recordings;
char *rlm_xmpad_format1s;
char *rlm_xmpad_format2s;
int *rlm_xmpad_display_size1s;
int *rlm_xmpad_display_size2s;
int *rlm_xmpad_masters;
int *rlm_xmpad_aux1s;
int *rlm_xmpad_aux2s;
int *rlm_xmpad_vlog101s;
int *rlm_xmpad_vlog102s;
int *rlm_xmpad_vlog103s;
int *rlm_xmpad_vlog104s;
int *rlm_xmpad_vlog105s;
int *rlm_xmpad_vlog106s;
int *rlm_xmpad_vlog107s;
int *rlm_xmpad_vlog108s;
int *rlm_xmpad_vlog109s;
int *rlm_xmpad_vlog110s;
int *rlm_xmpad_vlog111s;
int *rlm_xmpad_vlog112s;
int *rlm_xmpad_vlog113s;
int *rlm_xmpad_vlog114s;
int *rlm_xmpad_vlog115s;
int *rlm_xmpad_vlog116s;
int *rlm_xmpad_vlog117s;
int *rlm_xmpad_vlog118s;
int *rlm_xmpad_vlog119s;
int *rlm_xmpad_vlog120s;

int rlm_xmpad_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_xmpad_SendPad(void *ptr,int config,const char *line1,
		       const char *line2,int len)
{
  char str[1024];
  char sline1[17];
  char sline2[17];
  char flag1[1024];
  char flag2[1024];
  int i;

  //
  // Log it
  //
  sprintf(str,"rlm_xmpad: sending pad update: |%s|%s|%d|",line1,line2,len);
  RLMLog(ptr,LOG_INFO,str);

  //
  // Generate Display Strings and Flags
  //
  strncpy(sline1,line1,16);
  sline1[16]=0;
  flag1[0]=0;
  for(i=0;i<rlm_xmpad_display_size1s[config];i++) {
    strcat(flag1,"1");
  }
  strncpy(sline2,line2,16);
  sline2[16]=0;
  flag2[0]=0;
  for(i=0;i<rlm_xmpad_display_size2s[config];i++) {
    strcat(flag2,"1");
  }
  
  //
  // Generate PAD Data
  // The B-4 packet
  //
  sprintf(str,"B-4%c%d%c%d%c%d%c%d%c%d%c%d%c%s%c%s%c%s%c%s%c%s%c%s%c%u\n",
	  RLM_XMPAD_DELIMITER,!(rlm_xmpad_recordings[config]),
	  RLM_XMPAD_DELIMITER,1,
	  RLM_XMPAD_DELIMITER,len/432,
	  RLM_XMPAD_DELIMITER,0,
	  RLM_XMPAD_DELIMITER,rlm_xmpad_display_size1s[config],
	  RLM_XMPAD_DELIMITER,rlm_xmpad_display_size2s[config],
	  RLM_XMPAD_DELIMITER,flag1,
	  RLM_XMPAD_DELIMITER,flag2,
	  RLM_XMPAD_DELIMITER,sline2,
	  RLM_XMPAD_DELIMITER,flag1,
	  RLM_XMPAD_DELIMITER,flag2,
	  RLM_XMPAD_DELIMITER,sline1,
	  RLM_XMPAD_DELIMITER,rlm_xmpad_ids[config]);
  RLMSendSerial(ptr,rlm_xmpad_handles[config],str,strlen(str));
  RLMSendSerial(ptr,rlm_xmpad_handles[config],str,strlen(str));
  RLMSendSerial(ptr,rlm_xmpad_handles[config],str,strlen(str));

  //
  // The A4 Packet
  //
  sprintf(str,"A4%c%d%c%s\n",RLM_XMPAD_DELIMITER,1,RLM_XMPAD_DELIMITER,line1);
  RLMSendSerial(ptr,rlm_xmpad_handles[config],str,strlen(str));
  
  //
  // The A5 Packet
  //
  sprintf(str,"A5%c%d%c%s\n",RLM_XMPAD_DELIMITER,1,RLM_XMPAD_DELIMITER,line2);
  RLMSendSerial(ptr,rlm_xmpad_handles[config],str,strlen(str));
}


void rlm_xmpad_RLMStart(void *ptr,const char *arg)
{
  int handle;
  char device[256];
  int speed;
  int parity;
  int wsize;
  char section[256];
  char errtext[256];
  int i=1;
  int interval;

  rlm_xmpad_devs=0;
  rlm_xmpad_ids=NULL;
  rlm_xmpad_handles=NULL;
  rlm_xmpad_recordings=NULL;
  rlm_xmpad_format1s=NULL;
  rlm_xmpad_format2s=NULL;
  rlm_xmpad_display_size1s=NULL;
  rlm_xmpad_display_size2s=NULL;
  rlm_xmpad_masters=NULL;
  rlm_xmpad_aux1s=NULL;
  rlm_xmpad_aux2s=NULL;
  rlm_xmpad_vlog101s=NULL;
  rlm_xmpad_vlog102s=NULL;
  rlm_xmpad_vlog103s=NULL;
  rlm_xmpad_vlog104s=NULL;
  rlm_xmpad_vlog105s=NULL;
  rlm_xmpad_vlog106s=NULL;
  rlm_xmpad_vlog107s=NULL;
  rlm_xmpad_vlog108s=NULL;
  rlm_xmpad_vlog109s=NULL;
  rlm_xmpad_vlog110s=NULL;
  rlm_xmpad_vlog111s=NULL;
  rlm_xmpad_vlog112s=NULL;
  rlm_xmpad_vlog113s=NULL;
  rlm_xmpad_vlog114s=NULL;
  rlm_xmpad_vlog115s=NULL;
  rlm_xmpad_vlog116s=NULL;
  rlm_xmpad_vlog117s=NULL;
  rlm_xmpad_vlog118s=NULL;
  rlm_xmpad_vlog119s=NULL;
  rlm_xmpad_vlog120s=NULL;

  sprintf(section,"Serial%d",i++);
  strncpy(device,RLMGetStringValue(ptr,arg,section,"Device",""),256);
  if(strlen(device)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_xmpad: no serial devices specified");
    return;
  }
  while(strlen(device)>0) {
    speed=RLMGetIntegerValue(ptr,arg,section,"Speed",9600);
    parity=RLMGetIntegerValue(ptr,arg,section,"Parity",0);
    wsize=RLMGetIntegerValue(ptr,arg,section,"WordSize",8);
    if((handle=RLMOpenSerial(ptr,device,speed,parity,wsize))>=0) {
      rlm_xmpad_handles=realloc(rlm_xmpad_handles,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_handles[rlm_xmpad_devs]=handle;
      rlm_xmpad_ids=realloc(rlm_xmpad_ids,(rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_ids[rlm_xmpad_devs]=
	RLMGetIntegerValue(ptr,arg,section,"ProgramID",0);
      rlm_xmpad_format1s=realloc(rlm_xmpad_format1s,(rlm_xmpad_devs+1)*256);
      strncpy(rlm_xmpad_format1s+256*rlm_xmpad_devs,
	      RLMGetStringValue(ptr,arg,section,"FormatString1",""),256);
      rlm_xmpad_format2s=realloc(rlm_xmpad_format2s,(rlm_xmpad_devs+1)*256);
      strncpy(rlm_xmpad_format2s+256*rlm_xmpad_devs,
	      RLMGetStringValue(ptr,arg,section,"FormatString2",""),256);
      rlm_xmpad_display_size1s=realloc(rlm_xmpad_display_size1s,
				       (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_display_size1s[rlm_xmpad_devs]=
	RLMGetIntegerValue(ptr,arg,section,"DisplaySize1",8);
      rlm_xmpad_display_size2s=realloc(rlm_xmpad_display_size2s,
				       (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_display_size2s[rlm_xmpad_devs]=
	RLMGetIntegerValue(ptr,arg,section,"DisplaySize2",10);
      rlm_xmpad_recordings=realloc(rlm_xmpad_recordings,
				   (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_recordings[rlm_xmpad_devs]=
	RLMGetBooleanValue(ptr,arg,section,"Recording",1);
      rlm_xmpad_masters=realloc(rlm_xmpad_masters,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_masters[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"MasterLog");
      rlm_xmpad_aux1s=realloc(rlm_xmpad_aux1s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_aux1s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"Aux1Log");
      rlm_xmpad_aux2s=realloc(rlm_xmpad_aux2s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_aux2s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"Aux2Log");

      rlm_xmpad_vlog101s=realloc(rlm_xmpad_vlog101s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog101s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog101");

      rlm_xmpad_vlog102s=realloc(rlm_xmpad_vlog102s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog102s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog102");

      rlm_xmpad_vlog103s=realloc(rlm_xmpad_vlog103s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog103s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog103");

      rlm_xmpad_vlog104s=realloc(rlm_xmpad_vlog104s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog104s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog104");

      rlm_xmpad_vlog105s=realloc(rlm_xmpad_vlog105s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog105s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog105");

      rlm_xmpad_vlog106s=realloc(rlm_xmpad_vlog106s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog106s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog106");

      rlm_xmpad_vlog107s=realloc(rlm_xmpad_vlog107s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog107s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog107");

      rlm_xmpad_vlog108s=realloc(rlm_xmpad_vlog108s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog108s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog108");

      rlm_xmpad_vlog109s=realloc(rlm_xmpad_vlog109s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog109s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog109");

      rlm_xmpad_vlog110s=realloc(rlm_xmpad_vlog110s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog110s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog110");

      rlm_xmpad_vlog111s=realloc(rlm_xmpad_vlog111s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog111s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog111");

      rlm_xmpad_vlog112s=realloc(rlm_xmpad_vlog112s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog112s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog112");

      rlm_xmpad_vlog113s=realloc(rlm_xmpad_vlog113s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog113s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog113");

      rlm_xmpad_vlog114s=realloc(rlm_xmpad_vlog114s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog114s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog114");

      rlm_xmpad_vlog115s=realloc(rlm_xmpad_vlog115s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog115s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog115");

      rlm_xmpad_vlog116s=realloc(rlm_xmpad_vlog116s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog116s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog116");

      rlm_xmpad_vlog117s=realloc(rlm_xmpad_vlog117s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog117s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog117");

      rlm_xmpad_vlog118s=realloc(rlm_xmpad_vlog118s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog118s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog118");

      rlm_xmpad_vlog119s=realloc(rlm_xmpad_vlog119s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog119s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog119");

      rlm_xmpad_vlog120s=realloc(rlm_xmpad_vlog120s,
				 (rlm_xmpad_devs+1)*sizeof(int));
      rlm_xmpad_vlog120s[rlm_xmpad_devs]=
	rlm_xmpad_GetLogStatus(ptr,arg,section,"VLog120");

      interval=RLMGetIntegerValue(ptr,arg,section,"HeartbeatInterval",30);
      if(interval>0) {
	RLMStartTimer(ptr,rlm_xmpad_devs,1000*interval,RLM_TIMER_REPEATING);
      }
      rlm_xmpad_devs++;
      sprintf(errtext,"rlm_xmpad: opened device \"%s\"",device);
      RLMLog(ptr,LOG_INFO,errtext);
    }
    else {
      sprintf(errtext,"unable to open tty \"%s\"",device);
      RLMLog(ptr,LOG_WARNING,errtext);
    }
    sprintf(section,"Serial%d",i++);
    strncpy(device,RLMGetStringValue(ptr,arg,section,"Device",""),256);
  }
}


void rlm_xmpad_RLMFree(void *ptr)
{
  int i;

  for(i=0;i<rlm_xmpad_devs;i++) {
    RLMCloseSerial(ptr,rlm_xmpad_handles[i]);
  }
  free(rlm_xmpad_handles);
  free(rlm_xmpad_ids);
  free(rlm_xmpad_format1s);
  free(rlm_xmpad_format2s);
  free(rlm_xmpad_display_size1s);
  free(rlm_xmpad_display_size2s);
  free(rlm_xmpad_recordings);
  free(rlm_xmpad_masters);
  free(rlm_xmpad_aux1s);
  free(rlm_xmpad_aux2s);
  free(rlm_xmpad_vlog101s);
  free(rlm_xmpad_vlog102s);
  free(rlm_xmpad_vlog103s);
  free(rlm_xmpad_vlog104s);
  free(rlm_xmpad_vlog105s);
  free(rlm_xmpad_vlog106s);
  free(rlm_xmpad_vlog107s);
  free(rlm_xmpad_vlog108s);
  free(rlm_xmpad_vlog109s);
  free(rlm_xmpad_vlog110s);
  free(rlm_xmpad_vlog111s);
  free(rlm_xmpad_vlog112s);
  free(rlm_xmpad_vlog113s);
  free(rlm_xmpad_vlog114s);
  free(rlm_xmpad_vlog115s);
  free(rlm_xmpad_vlog116s);
  free(rlm_xmpad_vlog117s);
  free(rlm_xmpad_vlog118s);
  free(rlm_xmpad_vlog119s);
  free(rlm_xmpad_vlog120s);
}

/*
void rlm_xmpad_RLMPadDataSent(void *ptr,const char *svcname,int onair,
			      int lognum,const struct rlm_pad *now,
			      const struct rlm_pad *next)
*/
void rlm_xmpad_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			      const struct rlm_log *log,
			      const struct rlm_pad *now,
			      const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str1[33];
  char str2[33];
  char msg[1500];

  if(now->rlm_cartnum==0) {  // Suppress null PAD frames
    return;
  }
  for(i=0;i<rlm_xmpad_devs;i++) {
    switch(log->log_mach) {
    case 0:
      flag=rlm_xmpad_masters[i];
      break;

    case 1:
      flag=rlm_xmpad_aux1s[i];
      break;

    case 2:
      flag=rlm_xmpad_aux2s[i];
      break;

    case 100:
      flag=rlm_xmpad_vlog101s[i];
      break;

    case 101:
      flag=rlm_xmpad_vlog102s[i];
      break;

    case 102:
      flag=rlm_xmpad_vlog103s[i];
      break;

    case 103:
      flag=rlm_xmpad_vlog104s[i];
      break;

    case 104:
      flag=rlm_xmpad_vlog105s[i];
      break;

    case 105:
      flag=rlm_xmpad_vlog106s[i];
      break;

    case 106:
      flag=rlm_xmpad_vlog107s[i];
      break;

    case 107:
      flag=rlm_xmpad_vlog108s[i];
      break;

    case 108:
      flag=rlm_xmpad_vlog109s[i];
      break;

    case 109:
      flag=rlm_xmpad_vlog110s[i];
      break;

    case 110:
      flag=rlm_xmpad_vlog111s[i];
      break;

    case 111:
      flag=rlm_xmpad_vlog112s[i];
      break;

    case 112:
      flag=rlm_xmpad_vlog113s[i];
      break;

    case 113:
      flag=rlm_xmpad_vlog114s[i];
      break;

    case 114:
      flag=rlm_xmpad_vlog115s[i];
      break;

    case 115:
      flag=rlm_xmpad_vlog116s[i];
      break;

    case 116:
      flag=rlm_xmpad_vlog117s[i];
      break;

    case 117:
      flag=rlm_xmpad_vlog118s[i];
      break;

    case 118:
      flag=rlm_xmpad_vlog119s[i];
      break;

    case 119:
      flag=rlm_xmpad_vlog120s[i];
      break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(str1,RLMResolveNowNext(ptr,now,next,rlm_xmpad_format1s+256*i),32);
      str1[32]=0;
      strncpy(str2,RLMResolveNowNext(ptr,now,next,rlm_xmpad_format2s+256*i),32);
      str2[32]=0;
      rlm_xmpad_SendPad(ptr,i,str1,str2,now->rlm_len);
      snprintf(msg,1500,"rlm_xmpad: sending pad update: \"%s | %s\"",
	       (const char *)str1,(const char *)str2);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}


void rlm_xmpad_RLMTimerExpired(void *ptr,int timernum)
{
  RLMSendSerial(ptr,rlm_xmpad_handles[timernum],"H0\n",3);
}
