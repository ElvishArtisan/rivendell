/* rlm_spottrap.c
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
 * This is a Rivendell Loadable Module.  It processes Now&Next PAD data
 * on the basis of Group and Length as specified in the configuration file 
 * pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_spottrap.rlm rlm_spottrap.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

int rlm_spottrap_devs;
char *rlm_spottrap_groups;
char *rlm_spottrap_addresses;
int *rlm_spottrap_minimums;
int *rlm_spottrap_maximums;
uint16_t *rlm_spottrap_ports;
char *rlm_spottrap_formats;
char *rlm_spottrap_default_formats;
int *rlm_spottrap_masters;
int *rlm_spottrap_aux1s;
int *rlm_spottrap_aux2s;

int rlm_spottrap_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_spottrap_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_spottrap_devs=0;
  rlm_spottrap_groups=NULL;
  rlm_spottrap_addresses=NULL;
  rlm_spottrap_minimums=NULL;
  rlm_spottrap_maximums=NULL;
  rlm_spottrap_ports=NULL;
  rlm_spottrap_formats=NULL;
  rlm_spottrap_default_formats=NULL;
  rlm_spottrap_masters=NULL;
  rlm_spottrap_aux1s=NULL;
  rlm_spottrap_aux2s=NULL;

  sprintf(section,"Rule%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_spottrap: no rules specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_spottrap_addresses=
      realloc(rlm_spottrap_addresses,(rlm_spottrap_devs+1)*(rlm_spottrap_devs+1)*16);
    strcpy(rlm_spottrap_addresses+16*rlm_spottrap_devs,address);
    rlm_spottrap_groups=realloc(rlm_spottrap_groups,(rlm_spottrap_devs+1)*16);
    strncpy(rlm_spottrap_groups+16*rlm_spottrap_devs,
	    RLMGetStringValue(ptr,arg,section,"GroupName",""),10);
    rlm_spottrap_minimums=realloc(rlm_spottrap_minimums,(rlm_spottrap_devs+1)*sizeof(int));
    rlm_spottrap_minimums[rlm_spottrap_devs]=
      RLMGetIntegerValue(ptr,arg,section,"MinimumLength",0);
    rlm_spottrap_maximums=realloc(rlm_spottrap_maximums,(rlm_spottrap_devs+1)*sizeof(int));
    rlm_spottrap_maximums[rlm_spottrap_devs]=
      RLMGetIntegerValue(ptr,arg,section,"MaximumLength",0);
    rlm_spottrap_ports=realloc(rlm_spottrap_ports,(rlm_spottrap_devs+1)*sizeof(uint16_t));
    rlm_spottrap_ports[rlm_spottrap_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_spottrap_formats=realloc(rlm_spottrap_formats,(rlm_spottrap_devs+1)*256);
    strncpy(rlm_spottrap_formats+256*rlm_spottrap_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_spottrap_default_formats=realloc(rlm_spottrap_default_formats,
					 (rlm_spottrap_devs+1)*256);
    strncpy(rlm_spottrap_default_formats+256*rlm_spottrap_devs,
	    RLMGetStringValue(ptr,arg,section,"DefaultFormatString",""),256);
    rlm_spottrap_masters=realloc(rlm_spottrap_masters,
			    (rlm_spottrap_devs+1)*sizeof(int));
    rlm_spottrap_masters[rlm_spottrap_devs]=
      rlm_spottrap_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_spottrap_aux1s=realloc(rlm_spottrap_aux1s,
			  (rlm_spottrap_devs+1)*sizeof(int));
    rlm_spottrap_aux1s[rlm_spottrap_devs]=
      rlm_spottrap_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_spottrap_aux2s=realloc(rlm_spottrap_aux2s,
			  (rlm_spottrap_devs+1)*sizeof(int));
    rlm_spottrap_aux2s[rlm_spottrap_devs]=
      rlm_spottrap_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_spottrap: configured rule for \"%s:%d\"",address,
	    rlm_spottrap_ports[rlm_spottrap_devs]);
    rlm_spottrap_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Rule%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_spottrap_RLMFree(void *ptr)
{
  free(rlm_spottrap_addresses);
  free(rlm_spottrap_groups);
  free(rlm_spottrap_minimums);
  free(rlm_spottrap_maximums);
  free(rlm_spottrap_ports);
  free(rlm_spottrap_formats);
  free(rlm_spottrap_default_formats);
  free(rlm_spottrap_masters);
  free(rlm_spottrap_aux1s);
  free(rlm_spottrap_aux2s);
}


void rlm_spottrap_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char msg[1500];

  for(i=0;i<rlm_spottrap_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_spottrap_masters[i];
	break;

      case 1:
	flag=rlm_spottrap_aux1s[i];
	break;

      case 2:
	flag=rlm_spottrap_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      if((strcasecmp(now->rlm_group,rlm_spottrap_groups+16*i)==0)&&
	 (now->rlm_len>=rlm_spottrap_minimums[i])&&
	 (now->rlm_len<=rlm_spottrap_maximums[i])) {
	const char *str=
	  RLMResolveNowNext(ptr,now,next,rlm_spottrap_formats+256*i);
	RLMSendUdp(ptr,rlm_spottrap_addresses+i*16,rlm_spottrap_ports[i],
		   str,strlen(str));
	snprintf(msg,1500,"rlm_spottrap: sending pad update: \"%s\"",
		 (const char *)str);
	RLMLog(ptr,LOG_INFO,msg);
      }
      else {
	const char *str=
	  RLMResolveNowNext(ptr,now,next,rlm_spottrap_default_formats+256*i);
	RLMSendUdp(ptr,rlm_spottrap_addresses+i*16,rlm_spottrap_ports[i],
		   str,strlen(str));
	snprintf(msg,1500,"rlm_spottrap: sending default pad update: \"%s\"",
		 (const char *)str);
	RLMLog(ptr,LOG_INFO,msg);
      }
    }
  }
}
