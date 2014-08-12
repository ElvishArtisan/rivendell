/* rlm_udp.c
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data via
 * UDP packets to the destination(s) specified in the configuration file 
 * pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_udp.rlm rlm_udp.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

int rlm_udp_devs;
char *rlm_udp_addresses;
uint16_t *rlm_udp_ports;
char *rlm_udp_formats;
int *rlm_udp_encodings;
int *rlm_udp_null_updates;
int *rlm_udp_masters;
int *rlm_udp_aux1s;
int *rlm_udp_aux2s;

int rlm_udp_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_udp_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_udp_devs=0;
  rlm_udp_addresses=NULL;
  rlm_udp_ports=NULL;
  rlm_udp_formats=NULL;
  rlm_udp_encodings=NULL;
  rlm_udp_null_updates=NULL;
  rlm_udp_masters=NULL;
  rlm_udp_aux1s=NULL;
  rlm_udp_aux2s=NULL;

  sprintf(section,"Udp%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_udp: no udp destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_udp_addresses=
      realloc(rlm_udp_addresses,(rlm_udp_devs+1)*(rlm_udp_devs+1)*16);
    strcpy(rlm_udp_addresses+16*rlm_udp_devs,address);
    rlm_udp_ports=realloc(rlm_udp_ports,(rlm_udp_devs+1)*sizeof(uint16_t));
    rlm_udp_ports[rlm_udp_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_udp_formats=realloc(rlm_udp_formats,(rlm_udp_devs+1)*256);
    strncpy(rlm_udp_formats+256*rlm_udp_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),256);
    rlm_udp_encodings=realloc(rlm_udp_encodings,
			    (rlm_udp_devs+1)*sizeof(int));
    rlm_udp_encodings[rlm_udp_devs]=
      RLMGetIntegerValue(ptr,arg,section,"Encoding",RLM_ENCODE_NONE);
    rlm_udp_null_updates=realloc(rlm_udp_null_updates,
			    (rlm_udp_devs+1)*sizeof(int));
    rlm_udp_null_updates[rlm_udp_devs]=
      RLMGetIntegerValue(ptr,arg,section,"ProcessNullUpdates",0);
    rlm_udp_masters=realloc(rlm_udp_masters,
			    (rlm_udp_devs+1)*sizeof(int));
    rlm_udp_masters[rlm_udp_devs]=
      rlm_udp_GetLogStatus(ptr,arg,section,"MasterLog");

    rlm_udp_aux1s=realloc(rlm_udp_aux1s,
			  (rlm_udp_devs+1)*sizeof(int));
    rlm_udp_aux1s[rlm_udp_devs]=
      rlm_udp_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_udp_aux2s=realloc(rlm_udp_aux2s,
			  (rlm_udp_devs+1)*sizeof(int));
    rlm_udp_aux2s[rlm_udp_devs]=
      rlm_udp_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_udp: configured destination \"%s:%d\"",address,
	    rlm_udp_ports[rlm_udp_devs]);
    rlm_udp_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Udp%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_udp_RLMFree(void *ptr)
{
  free(rlm_udp_addresses);
  free(rlm_udp_ports);
  free(rlm_udp_formats);
  free(rlm_udp_encodings);
  free(rlm_udp_null_updates);
  free(rlm_udp_masters);
  free(rlm_udp_aux1s);
  free(rlm_udp_aux2s);
}


void rlm_udp_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char msg[1500];

  for(i=0;i<rlm_udp_devs;i++) {
    switch(rlm_udp_null_updates[i]) {
    case 0:  /* Process all updates */
      break;

    case 1:  /* Process only non-null NOW updates */
      if(now->rlm_cartnum==0) {
	return;
      }
      break;

    case 2:  /* Process only non-null NEXT updates */
      if(next->rlm_cartnum==0) {
	return;
      }
      break;

    case 3:  /* Process only non-null NOW and NEXT updates */
      if((now->rlm_cartnum==0)||(next->rlm_cartnum==0)) {
	return;
      }
      break;
    }

    switch(log->log_mach) {
      case 0:
	flag=rlm_udp_masters[i];
	break;

      case 1:
	flag=rlm_udp_aux1s[i];
	break;

      case 2:
	flag=rlm_udp_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      const char *str=
	RLMResolveNowNextEncoded(ptr,now,next,rlm_udp_formats+256*i,
				 rlm_udp_encodings[i]);
      RLMSendUdp(ptr,rlm_udp_addresses+i*16,rlm_udp_ports[i],str,strlen(str));
      snprintf(msg,1500,"rlm_udp: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
