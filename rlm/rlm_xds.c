/* rlm_xds.c
 *
 *   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It sends generates enhanced netcue
 * (aka CIC) packets for the Citadel/X-Digital Systems copy-splitting system,
 * using a configuration file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_xds.rlm rlm_xds.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <rlm/rlm.h>

int rlm_xds_devs;
char *rlm_xds_isci_prefixes;
char *rlm_xds_addresses;
uint16_t *rlm_xds_ports;
int *rlm_xds_handles;
char *rlm_xds_tty_buffers;
int *rlm_xds_masters;
int *rlm_xds_aux1s;
int *rlm_xds_aux2s;

unsigned rlm_xds_GetChecksum(const char *pack)
{
  unsigned ret=0;
  int i;

  for(i=0;i<strlen(pack);i++) {
    ret+=pack[i];
  }

  return ret;
}


const char *rlm_xds_FilterCharacters(const char *str)
{
  static char ret[32];
  int i;

  strncpy(ret,str,31);
  for(i=0;i<strlen(ret);i++) {
    switch(ret[i]) {
    case ' ':
    case ',':
    case '.':
      ret[i]='_';
      break;

    case '"':
      ret[i]=39;
      break;

    case '%':
    case '*':
    case '+':
    case '/':
    case ':':
    case ';':
    case '<':
    case '=':
    case '>':
    case '?':
    case '@':
    case '[':
    case '\\':
    case ']':
    case '^':
    case '{':
    case '|':
    case '}':
      ret[i]='-';
      break;
    }
  }
  return ret;
}

int rlm_xds_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_xds_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int tty_speed;
  int i=1;

  rlm_xds_devs=0;
  rlm_xds_addresses=NULL;
  rlm_xds_ports=NULL;
  rlm_xds_handles=NULL;
  rlm_xds_tty_buffers=NULL;
  rlm_xds_masters=NULL;
  rlm_xds_aux1s=NULL;
  rlm_xds_aux2s=NULL;

  sprintf(section,"Udp%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_xds: no xds destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_xds_addresses=
      realloc(rlm_xds_addresses,(rlm_xds_devs+1)*(rlm_xds_devs+1)*16);
    strcpy(rlm_xds_addresses+16*rlm_xds_devs,address);
    rlm_xds_isci_prefixes=realloc(rlm_xds_isci_prefixes,(rlm_xds_devs+1)*32);
    strncpy(rlm_xds_isci_prefixes+32*rlm_xds_devs,
	   RLMGetStringValue(ptr,arg,section,"IsciPrefix",""),31);
    rlm_xds_ports=realloc(rlm_xds_ports,(rlm_xds_devs+1)*sizeof(uint16_t));
    rlm_xds_ports[rlm_xds_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_xds_handles=realloc(rlm_xds_handles,(rlm_xds_devs+1)*sizeof(int));
    rlm_xds_handles[rlm_xds_devs]=-1;
    tty_speed=RLMGetIntegerValue(ptr,arg,section,"TtySpeed",9600);
    printf("speed: %d\n",tty_speed);
    rlm_xds_handles[rlm_xds_devs]=
      RLMOpenSerial(ptr,RLMGetStringValue(ptr,arg,section,"TtyDevice",""),
		    tty_speed,RLM_PARITY_NONE,1);
    rlm_xds_tty_buffers=realloc(rlm_xds_tty_buffers,(rlm_xds_devs+1)*256);
    memset(rlm_xds_tty_buffers+rlm_xds_devs,0,256);
    rlm_xds_masters=realloc(rlm_xds_masters,
			    (rlm_xds_devs+1)*sizeof(int));
    rlm_xds_masters[rlm_xds_devs]=
      rlm_xds_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_xds_aux1s=realloc(rlm_xds_aux1s,
			  (rlm_xds_devs+1)*sizeof(int));
    rlm_xds_aux1s[rlm_xds_devs]=
      rlm_xds_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_xds_aux2s=realloc(rlm_xds_aux2s,
			  (rlm_xds_devs+1)*sizeof(int));
    rlm_xds_aux2s[rlm_xds_devs]=
      rlm_xds_GetLogStatus(ptr,arg,section,"Aux2Log");
    if(rlm_xds_handles[rlm_xds_devs]<0) {
      sprintf(errtext,"rlm_xds: configured destination \"%s:%d\"",address,
	      rlm_xds_ports[rlm_xds_devs]);
    }
    else {
      sprintf(errtext,"rlm_xds: configured destination \"%s\"",
	      (const char *)RLMGetStringValue(ptr,arg,section,"TtyDevice",""));
    }
    rlm_xds_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Udp%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_xds_RLMFree(void *ptr)
{
  int i;

  for(i=0;i<rlm_xds_devs;i++) {
    if(rlm_xds_handles[i]>=0) {
      RLMCloseSerial(ptr,rlm_xds_handles[i]);
      rlm_xds_handles[i]=-1;
    }
  }
  free(rlm_xds_addresses);
  free(rlm_xds_ports);
  free(rlm_xds_handles);
  free(rlm_xds_tty_buffers);
  free(rlm_xds_isci_prefixes);
  free(rlm_xds_masters);
  free(rlm_xds_aux1s);
  free(rlm_xds_aux2s);
}


void rlm_xds_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char packet[64];
  char tty_packet[256];
  char msg[1500];

  if(strlen((now->rlm_ext_eventid))==0) {
    return;
  }
  for(i=0;i<rlm_xds_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_xds_masters[i];
	break;

      case 1:
	flag=rlm_xds_aux1s[i];
	break;

      case 2:
	flag=rlm_xds_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      packet[0]=0;
      snprintf(packet,64,"0:%s:%s%s:*",svc->svc_pgmcode,
	       rlm_xds_isci_prefixes+i*32,
	       rlm_xds_FilterCharacters(now->rlm_ext_eventid));
      if(rlm_xds_handles[i]<0) {
	RLMSendUdp(ptr,rlm_xds_addresses+i*16,rlm_xds_ports[i],
		   packet,strlen(packet));
	snprintf(msg,1500,"rlm_xds: sending CIC NetCue: \"%s\" to %s:%d",
		 (const char *)packet,rlm_xds_addresses+i*16,rlm_xds_ports[i]);
      }
      else {
	snprintf(tty_packet,256,"NC:%u:%s\r\n",
		 rlm_xds_GetChecksum(packet),packet);
	RLMSendSerial(ptr,rlm_xds_handles[i],tty_packet,strlen(tty_packet));
	RLMSendSerial(ptr,rlm_xds_handles[i],tty_packet,strlen(tty_packet));
	snprintf(msg,1500,"rlm_xds: sending CIC NetCue: \"%s\" to tty",
		 (const char *)packet);
      }
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}


void rlm_xds_RLMSerialDataReceived(void *ptr,int handle,
				   const char *data,int len)
{
  int dev;
  int i;
  char *buffer;

  for(dev=0;dev<rlm_xds_devs;dev++) {
    if(handle==rlm_xds_handles[dev]) {
      buffer=rlm_xds_tty_buffers+256*dev;
      for(i=0;i<len;i++) {
	if(data[i]==13) {
	  if(strcmp(buffer,"PINGRECV")==0) {
	    RLMSendSerial(ptr,handle,"PONGRECV\r\n",10);
	  }
	  buffer[0]=0;
	}
	else {
	  if(data[i]!=10) {
	    buffer[strlen(buffer)+1]=0;
	    buffer[strlen(buffer)]=data[i];
	  }
	}
      }
      return;
    }
  }
  fprintf(stderr,"rlm_xds: unknown tty handle %d\n",handle);
}
