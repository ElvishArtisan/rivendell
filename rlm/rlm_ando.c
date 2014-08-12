/* rlm_ando.c
 *
 *   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data to an
 * ANDO Media Streaming system.  Options are specified in the configuration 
 * file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_ando.rlm rlm_ando.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

int rlm_ando_devs;
char *rlm_ando_addresses;
uint16_t *rlm_ando_ports;
char *rlm_ando_titles;
char *rlm_ando_artists;
char *rlm_ando_albums;
char *rlm_ando_labels;
int *rlm_ando_masters;
int *rlm_ando_aux1s;
int *rlm_ando_aux2s;

int rlm_ando_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_ando_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_ando_devs=0;
  rlm_ando_addresses=NULL;
  rlm_ando_ports=NULL;
  rlm_ando_masters=NULL;
  rlm_ando_aux1s=NULL;
  rlm_ando_aux2s=NULL;

  sprintf(section,"System%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_ando: no ando destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_ando_addresses=
      realloc(rlm_ando_addresses,(rlm_ando_devs+1)*(rlm_ando_devs+1)*16);
    strcpy(rlm_ando_addresses+16*rlm_ando_devs,address);
    rlm_ando_ports=realloc(rlm_ando_ports,(rlm_ando_devs+1)*sizeof(uint16_t));
    rlm_ando_ports[rlm_ando_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_ando_titles=realloc(rlm_ando_titles,(rlm_ando_devs+1)*256);
    strncpy(rlm_ando_titles+256*rlm_ando_devs,
	    RLMGetStringValue(ptr,arg,section,"Title",""),256);
    rlm_ando_artists=realloc(rlm_ando_artists,(rlm_ando_devs+1)*256);
    strncpy(rlm_ando_artists+256*rlm_ando_devs,
	    RLMGetStringValue(ptr,arg,section,"Artist",""),256);
    rlm_ando_albums=realloc(rlm_ando_albums,(rlm_ando_devs+1)*256);
    strncpy(rlm_ando_albums+256*rlm_ando_devs,
	    RLMGetStringValue(ptr,arg,section,"Album",""),256);
    rlm_ando_labels=realloc(rlm_ando_labels,(rlm_ando_devs+1)*256);
    strncpy(rlm_ando_labels+256*rlm_ando_devs,
	    RLMGetStringValue(ptr,arg,section,"Label",""),256);
    rlm_ando_masters=realloc(rlm_ando_masters,
			    (rlm_ando_devs+1)*sizeof(int));
    rlm_ando_masters[rlm_ando_devs]=
      rlm_ando_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_ando_aux1s=realloc(rlm_ando_aux1s,
			  (rlm_ando_devs+1)*sizeof(int));
    rlm_ando_aux1s[rlm_ando_devs]=
      rlm_ando_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_ando_aux2s=realloc(rlm_ando_aux2s,
			  (rlm_ando_devs+1)*sizeof(int));
    rlm_ando_aux2s[rlm_ando_devs]=
      rlm_ando_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_ando: configured destination \"%s:%d\"",address,
	    rlm_ando_ports[rlm_ando_devs]);
    rlm_ando_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"System%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
  RLMStartTimer(ptr,0,30000,RLM_TIMER_REPEATING);
}


void rlm_ando_RLMFree(void *ptr)
{
  RLMStopTimer(ptr,0);
  free(rlm_ando_addresses);
  free(rlm_ando_ports);
  free(rlm_ando_titles);
  free(rlm_ando_artists);
  free(rlm_ando_albums);
  free(rlm_ando_labels);
  free(rlm_ando_masters);
  free(rlm_ando_aux1s);
  free(rlm_ando_aux2s);
}


void rlm_ando_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			     const struct rlm_log *log,
			     const struct rlm_pad *now,
			     const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char fmt[1024];
  char msg[1500];
  int hours;
  int minutes;
  int seconds;

  for(i=0;i<rlm_ando_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_ando_masters[i];
	break;

      case 1:
	flag=rlm_ando_aux1s[i];
	break;

      case 2:
	flag=rlm_ando_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      if(strlen(rlm_ando_labels+256*i)==0) {  // Original format
	snprintf(fmt,1024,"^%s~%s~%02d:%02d~%%g~%s~%%n|",
		 rlm_ando_artists+256*i,
		 rlm_ando_titles+256*i,
		 now->rlm_len/60000,(now->rlm_len%60000)/1000,
		 rlm_ando_albums+256*i);
      }
      else {  // Enhanced format
	hours=now->rlm_len/3600000;
	minutes=(now->rlm_len-hours*3600000)/60000;
	seconds=(now->rlm_len-hours*3600000-minutes*60000)/1000;
	snprintf(fmt,1024,"^%s~%s~%02d:%02d:%02d~%%g~%%n~%s~%s|",
		 rlm_ando_artists+256*i,
		 rlm_ando_titles+256*i,
		 hours,minutes,seconds,
		 rlm_ando_albums+256*i,
		 rlm_ando_labels+256*i);
      }
      const char *str=RLMResolveNowNext(ptr,now,next,fmt);
      RLMSendUdp(ptr,rlm_ando_addresses+i*16,rlm_ando_ports[i],str,strlen(str));
      snprintf(msg,1500,"rlm_ando: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}


void rlm_ando_RLMTimerExpired(void *ptr,int timernum)
{
  int i;

  switch(timernum) {
    case 0:  // Heartbeat
      for(i=0;i<rlm_ando_devs;i++) {
	RLMSendUdp(ptr,rlm_ando_addresses+i*16,rlm_ando_ports[i],"HB",2);
      }
      break;
  }
}
