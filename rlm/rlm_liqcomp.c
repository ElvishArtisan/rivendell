/* rlm_liqcomp.c
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data to a
 * Liquid Compass Internet streaming encoder.  Options are specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_liqcomp.rlm rlm_liqcomp.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <rlm/rlm.h>

int rlm_liqcomp_devs;
char *rlm_liqcomp_addresses;
uint16_t *rlm_liqcomp_ports;
char *rlm_liqcomp_titles;
char *rlm_liqcomp_artists;
char *rlm_liqcomp_albums;
char *rlm_liqcomp_labels;
int *rlm_liqcomp_masters;
int *rlm_liqcomp_aux1s;
int *rlm_liqcomp_aux2s;

int rlm_liqcomp_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_liqcomp_RLMStart(void *ptr,const char *arg)
{
  char address[17];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_liqcomp_devs=0;
  rlm_liqcomp_addresses=NULL;
  rlm_liqcomp_ports=NULL;
  rlm_liqcomp_masters=NULL;
  rlm_liqcomp_aux1s=NULL;
  rlm_liqcomp_aux2s=NULL;

  sprintf(section,"System%d",i++);
  strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  if(strlen(address)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_liqcomp: no encoder destinations specified");
    return;
  }
  while(strlen(address)>0) {
    rlm_liqcomp_addresses=
      realloc(rlm_liqcomp_addresses,(rlm_liqcomp_devs+1)*(rlm_liqcomp_devs+1)*16);
    strcpy(rlm_liqcomp_addresses+16*rlm_liqcomp_devs,address);
    rlm_liqcomp_ports=realloc(rlm_liqcomp_ports,(rlm_liqcomp_devs+1)*sizeof(uint16_t));
    rlm_liqcomp_ports[rlm_liqcomp_devs]=
      RLMGetIntegerValue(ptr,arg,section,"UdpPort",0);
    rlm_liqcomp_titles=realloc(rlm_liqcomp_titles,(rlm_liqcomp_devs+1)*256);
    strncpy(rlm_liqcomp_titles+256*rlm_liqcomp_devs,
	    RLMGetStringValue(ptr,arg,section,"Title",""),256);
    rlm_liqcomp_artists=realloc(rlm_liqcomp_artists,(rlm_liqcomp_devs+1)*256);
    strncpy(rlm_liqcomp_artists+256*rlm_liqcomp_devs,
	    RLMGetStringValue(ptr,arg,section,"Artist",""),256);
    rlm_liqcomp_albums=realloc(rlm_liqcomp_albums,(rlm_liqcomp_devs+1)*256);
    strncpy(rlm_liqcomp_albums+256*rlm_liqcomp_devs,
	    RLMGetStringValue(ptr,arg,section,"Album",""),256);
    rlm_liqcomp_labels=realloc(rlm_liqcomp_labels,(rlm_liqcomp_devs+1)*256);
    strncpy(rlm_liqcomp_labels+256*rlm_liqcomp_devs,
	    RLMGetStringValue(ptr,arg,section,"Label",""),256);
    rlm_liqcomp_masters=realloc(rlm_liqcomp_masters,
			    (rlm_liqcomp_devs+1)*sizeof(int));
    rlm_liqcomp_masters[rlm_liqcomp_devs]=
      rlm_liqcomp_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_liqcomp_aux1s=realloc(rlm_liqcomp_aux1s,
			  (rlm_liqcomp_devs+1)*sizeof(int));
    rlm_liqcomp_aux1s[rlm_liqcomp_devs]=
      rlm_liqcomp_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_liqcomp_aux2s=realloc(rlm_liqcomp_aux2s,
			  (rlm_liqcomp_devs+1)*sizeof(int));
    rlm_liqcomp_aux2s[rlm_liqcomp_devs]=
      rlm_liqcomp_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_liqcomp: configured destination \"%s:%d\"",address,
	    rlm_liqcomp_ports[rlm_liqcomp_devs]);
    rlm_liqcomp_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"System%d",i++);
    strncpy(address,RLMGetStringValue(ptr,arg,section,"IpAddress",""),15);
  }
}


void rlm_liqcomp_RLMFree(void *ptr)
{
  free(rlm_liqcomp_addresses);
  free(rlm_liqcomp_ports);
  free(rlm_liqcomp_titles);
  free(rlm_liqcomp_artists);
  free(rlm_liqcomp_albums);
  free(rlm_liqcomp_labels);
  free(rlm_liqcomp_masters);
  free(rlm_liqcomp_aux1s);
  free(rlm_liqcomp_aux2s);
}


void rlm_liqcomp_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			     const struct rlm_log *log,
			     const struct rlm_pad *now,
			     const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char fmt[1500];
  char msg[1500];

  for(i=0;i<rlm_liqcomp_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_liqcomp_masters[i];
	break;

      case 1:
	flag=rlm_liqcomp_aux1s[i];
	break;

      case 2:
	flag=rlm_liqcomp_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      snprintf(fmt,1500,"|%s|%s|%06u|%u|%s|%s|%s|\n",
	      rlm_liqcomp_titles+256*i,
	      rlm_liqcomp_artists+256*i,
	      now->rlm_cartnum,
	      now->rlm_len,
	      now->rlm_group,
	      rlm_liqcomp_albums+256*i,
	      rlm_liqcomp_labels+256*i);
      const char *str=RLMResolveNowNext(ptr,now,next,fmt);
      RLMSendUdp(ptr,rlm_liqcomp_addresses+i*16,rlm_liqcomp_ports[i],str,
		 strlen(str));
      snprintf(msg,1500,"rlm_liqcomp: sending pad update: \"%s\"",
	       (const char *)str);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
