/* rlm_walltime.c
 *
 *   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <rlm/rlm.h>

int rlm_walltime_devs;
char *rlm_walltime_addresses;
uint16_t *rlm_walltime_ports;
char *rlm_walltime_formats;
char *rlm_walltime_host_addresses;
int *rlm_walltime_masters;
int *rlm_walltime_aux1s;
int *rlm_walltime_aux2s;

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
  rlm_walltime_ports=NULL;
  rlm_walltime_formats=NULL;
  rlm_walltime_host_addresses=NULL;
  rlm_walltime_masters=NULL;
  rlm_walltime_aux1s=NULL;
  rlm_walltime_aux2s=NULL;

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
    rlm_walltime_ports=realloc(rlm_walltime_ports,(rlm_walltime_devs+1)*sizeof(uint16_t));
    rlm_walltime_ports[rlm_walltime_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_walltime_formats=realloc(rlm_walltime_formats,(rlm_walltime_devs+1)*8192);
    strncpy(rlm_walltime_formats+8192*rlm_walltime_devs,
	    RLMGetStringValue(ptr,arg,section,"FormatString",""),8192);
    rlm_walltime_host_addresses=
      realloc(rlm_walltime_host_addresses,(rlm_walltime_devs+1)*(rlm_walltime_devs+1)*16);
    strncpy(rlm_walltime_host_addresses+16*rlm_walltime_devs,
	    RLMGetStringValue(ptr,arg,section,"HostAddress",""),15);
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
    sprintf(errtext,"rlm_walltime: configured destination \"%s:%d\"",address,
	    rlm_walltime_ports[rlm_walltime_devs]);
    rlm_walltime_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Walltime%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_walltime_RLMFree(void *ptr)
{
  free(rlm_walltime_addresses);
  free(rlm_walltime_ports);
  free(rlm_walltime_formats);
  free(rlm_walltime_host_addresses);
  free(rlm_walltime_masters);
  free(rlm_walltime_aux1s);
  free(rlm_walltime_aux2s);
}


void rlm_walltime_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			    const struct rlm_log *log,
			    const struct rlm_pad *now,
			    const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char timer[1500];
  FILE *f;

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
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      f=fopen("/var/www/html/walltime/walltime.html","w");
      if(now->rlm_len==0) {
	snprintf(timer,1500,
		 "SP!RS!WU http://%s/walltime/walltime.html!",
		 rlm_walltime_host_addresses+16*i);
	fprintf(f,"<body bgcolor=\"#000000\">&nbsp;</body>");
      }
      else {
	snprintf(timer,1500,
		 "SM D!PS %d!ST!WU http://%s/walltime/walltime.html!",
		 now->rlm_len/1000,
		 rlm_walltime_host_addresses+16*i);
	fprintf(f,"%s\n",
	     RLMResolveNowNextEncoded(ptr,now,next,rlm_walltime_formats+8192*i,
				      RLM_ENCODE_XML));
      }
      fclose(f);
      RLMSendUdp(ptr,rlm_walltime_addresses+i*16,rlm_walltime_ports[i],
		 timer,strlen(timer));
      RLMLog(ptr,LOG_INFO,"rlm_walltime: sending pad update!");
    }
  }
}
