/* rlm_inno713.c
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD
 * to the Innovonics model 713 RDS encoder, as specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_inno713.rlm rlm_inno713.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <rlm/rlm.h>

int rlm_inno713_devs;
char *rlm_inno713_addresses;
uint16_t *rlm_inno713_ports;
int *rlm_inno713_handles;
char *rlm_inno713_dynamicpss;
char *rlm_inno713_pss;
char *rlm_inno713_radiotexts;
int *rlm_inno713_masters;
int *rlm_inno713_aux1s;
int *rlm_inno713_aux2s;

void rlm_inno713_Upcase(char *str)
{
  int i;

  for(i=0;i<strlen(str);i++) {
    str[i]=toupper(str[i]);
  }
}


void rlm_inno713_SendString(void *ptr,const struct rlm_pad *now,
			    const struct rlm_pad *next,const char *tag,
			    const char *fmt,const char *addr,uint16_t port,
			    int handle)
{
  char pkt[1024];

  if(strlen(fmt)>0) {
    const char *str=RLMResolveNowNext(ptr,now,next,fmt);
    if(strlen(str)>0) {
      sprintf(pkt,"%s=%s\r\n",tag,str);
      rlm_inno713_Upcase(pkt);
      if(strlen(addr)>0) {
	RLMSendUdp(ptr,addr,port,pkt,strlen(pkt));
      }
      if(handle>=0) {
	RLMSendSerial(ptr,handle,pkt,strlen(pkt));
      }
    }
  }
}
			    

int rlm_inno713_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_inno713_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  char device[256];
  int speed;
  int parity;
  int wsize;
  int i=1;

  rlm_inno713_devs=0;
  rlm_inno713_addresses=NULL;
  rlm_inno713_ports=NULL;
  rlm_inno713_handles=NULL;
  rlm_inno713_dynamicpss=NULL;
  rlm_inno713_masters=NULL;
  rlm_inno713_aux1s=NULL;
  rlm_inno713_aux2s=NULL;

  sprintf(section,"Rds%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  strncpy(device,RLMGetStringValue(ptr,arg,section,"Device",""),256);
  if((strlen(address)==0)&&(strlen(device)==0)) {
    RLMLog(ptr,LOG_WARNING,"rlm_inno713: no RDS connections specified");
    return;
  }
  while((strlen(address)>0)||(strlen(device)>0)) {
    rlm_inno713_addresses=
      realloc(rlm_inno713_addresses,
	      (rlm_inno713_devs+1)*(rlm_inno713_devs+1)*16);
    strcpy(rlm_inno713_addresses+16*rlm_inno713_devs,address);
    rlm_inno713_ports=realloc(rlm_inno713_ports,
			      (rlm_inno713_devs+1)*sizeof(uint16_t));
    rlm_inno713_ports[rlm_inno713_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    speed=RLMGetIntegerValue(ptr,arg,section,"Speed",9600);
    parity=RLMGetIntegerValue(ptr,arg,section,"Parity",0);
    wsize=RLMGetIntegerValue(ptr,arg,section,"WordSize",8);
    rlm_inno713_handles=realloc(rlm_inno713_handles,
				(rlm_inno713_devs+1)*sizeof(int));
    rlm_inno713_handles[rlm_inno713_devs]=
      RLMOpenSerial(ptr,device,speed,parity,wsize);
    rlm_inno713_dynamicpss=realloc(rlm_inno713_dynamicpss,(rlm_inno713_devs+1)*256);
    strncpy(rlm_inno713_dynamicpss+256*rlm_inno713_devs,
	    RLMGetStringValue(ptr,arg,section,"DynamicPsString",""),256);

    rlm_inno713_pss=realloc(rlm_inno713_pss,(rlm_inno713_devs+1)*256);
    strncpy(rlm_inno713_pss+256*rlm_inno713_devs,
	    RLMGetStringValue(ptr,arg,section,"PsString",""),256);
    rlm_inno713_radiotexts=realloc(rlm_inno713_radiotexts,
				   (rlm_inno713_devs+1)*256);
    strncpy(rlm_inno713_radiotexts+256*rlm_inno713_devs,
	    RLMGetStringValue(ptr,arg,section,"RadiotextString",""),256);
    rlm_inno713_masters=realloc(rlm_inno713_masters,
			    (rlm_inno713_devs+1)*sizeof(int));
    rlm_inno713_masters[rlm_inno713_devs]=
      rlm_inno713_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_inno713_aux1s=realloc(rlm_inno713_aux1s,
			  (rlm_inno713_devs+1)*sizeof(int));
    rlm_inno713_aux1s[rlm_inno713_devs]=
      rlm_inno713_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_inno713_aux2s=realloc(rlm_inno713_aux2s,
			  (rlm_inno713_devs+1)*sizeof(int));
    rlm_inno713_aux2s[rlm_inno713_devs]=
      rlm_inno713_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_inno713: configured destination \"%s:%d\"",address,
	    rlm_inno713_ports[rlm_inno713_devs]);
    RLMLog(ptr,LOG_INFO,errtext);
    if(strlen(device)>0) {
      if(rlm_inno713_handles[rlm_inno713_devs]<0) {
	sprintf(errtext,"rlm_inno713: unable to open serial device \"%s\"",device);
      }
      else {
	sprintf(errtext,"rlm_inno713: configured serial device \"%s\"",device);
      }
      RLMLog(ptr,LOG_INFO,errtext);
    }
    rlm_inno713_devs++;
    sprintf(section,"Rds%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
    strncpy(device,RLMGetStringValue(ptr,arg,section,"Device",""),256);
  }
}


void rlm_inno713_RLMFree(void *ptr)
{
  int i;

  free(rlm_inno713_addresses);
  free(rlm_inno713_ports);
  for(i=0;i<rlm_inno713_devs;i++) {
    if(rlm_inno713_handles[i]>=0) {
      RLMCloseSerial(ptr,rlm_inno713_handles[i]);
    }
  }
  free(rlm_inno713_handles);
  free(rlm_inno713_dynamicpss);
  free(rlm_inno713_pss);
  free(rlm_inno713_radiotexts);
  free(rlm_inno713_masters);
  free(rlm_inno713_aux1s);
  free(rlm_inno713_aux2s);
}


void rlm_inno713_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			     const struct rlm_log *log,
			     const struct rlm_pad *now,
			     const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char msg[1500];

  for(i=0;i<rlm_inno713_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_inno713_masters[i];
	break;

      case 1:
	flag=rlm_inno713_aux1s[i];
	break;

      case 2:
	flag=rlm_inno713_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      rlm_inno713_SendString(ptr,now,next,"DPS",rlm_inno713_dynamicpss+256*i,
			     rlm_inno713_addresses+i*16,rlm_inno713_ports[i],
			     rlm_inno713_handles[i]);
      rlm_inno713_SendString(ptr,now,next,"PS",rlm_inno713_pss+256*i,
			     rlm_inno713_addresses+i*16,rlm_inno713_ports[i],
			     rlm_inno713_handles[i]);
      rlm_inno713_SendString(ptr,now,next,"TEXT",rlm_inno713_radiotexts+256*i,
			     rlm_inno713_addresses+i*16,rlm_inno713_ports[i],
			     rlm_inno713_handles[i]);

      const char *str=RLMResolveNowNext(ptr,now,next,rlm_inno713_pss+256*i);
      snprintf(msg,1500,"rlm_inno713: sending pad update: \"%s\"",str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
