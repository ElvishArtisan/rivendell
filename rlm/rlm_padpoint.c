/* rlm_padpoint.c
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data via
 * UDP packets to one or more PadPoint PAD processors as specified in the
 * configuration file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_padpoint.rlm rlm_padpoint.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

int rlm_padpoint_devs;
char *rlm_padpoint_addresses;
uint16_t *rlm_padpoint_ports;
int *rlm_padpoint_masters;
int *rlm_padpoint_aux1s;
int *rlm_padpoint_aux2s;

int rlm_padpoint_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_padpoint_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_padpoint_devs=0;
  rlm_padpoint_addresses=NULL;
  rlm_padpoint_ports=NULL;
  rlm_padpoint_masters=NULL;
  rlm_padpoint_aux1s=NULL;
  rlm_padpoint_aux2s=NULL;

  sprintf(section,"PadPoint%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_padpoint: no padpoint destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_padpoint_addresses=
      realloc(rlm_padpoint_addresses,(rlm_padpoint_devs+1)*(rlm_padpoint_devs+1)*16);
    strcpy(rlm_padpoint_addresses+16*rlm_padpoint_devs,address);
    rlm_padpoint_ports=realloc(rlm_padpoint_ports,(rlm_padpoint_devs+1)*sizeof(uint16_t));
    rlm_padpoint_ports[rlm_padpoint_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_padpoint_masters=realloc(rlm_padpoint_masters,
			    (rlm_padpoint_devs+1)*sizeof(int));
    rlm_padpoint_masters[rlm_padpoint_devs]=
      rlm_padpoint_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_padpoint_aux1s=realloc(rlm_padpoint_aux1s,
			  (rlm_padpoint_devs+1)*sizeof(int));
    rlm_padpoint_aux1s[rlm_padpoint_devs]=
      rlm_padpoint_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_padpoint_aux2s=realloc(rlm_padpoint_aux2s,
			  (rlm_padpoint_devs+1)*sizeof(int));
    rlm_padpoint_aux2s[rlm_padpoint_devs]=
      rlm_padpoint_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_padpoint: configured destination \"%s:%d\"",address,
	    rlm_padpoint_ports[rlm_padpoint_devs]);
    rlm_padpoint_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"PadPoint%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_padpoint_RLMFree(void *ptr)
{
  free(rlm_padpoint_addresses);
  free(rlm_padpoint_ports);
  free(rlm_padpoint_masters);
  free(rlm_padpoint_aux1s);
  free(rlm_padpoint_aux2s);
}


void rlm_padpoint_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char str[1501];
  char msg[2048];

  for(i=0;i<rlm_padpoint_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_padpoint_masters[i];
	break;

      case 1:
	flag=rlm_padpoint_aux1s[i];
	break;

      case 2:
	flag=rlm_padpoint_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      snprintf(str,1501,"%06u~%u~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s~%s",
	       now->rlm_cartnum,
	       now->rlm_len,
	       now->rlm_year,
	       now->rlm_group,
	       now->rlm_title,
	       now->rlm_artist,
	       now->rlm_album,
	       now->rlm_label,
	       now->rlm_client,
	       now->rlm_agency,
	       now->rlm_comp,
	       now->rlm_pub,
	       now->rlm_userdef,
	       now->rlm_isrc,
	       now->rlm_isci);
      RLMSendUdp(ptr,rlm_padpoint_addresses+i*16,rlm_padpoint_ports[i],
		 str,strlen(str));
      snprintf(msg,1500,"rlm_padpoint: sending pad update: \"%s\"",str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
