/* rlm_tunein.c
 *
 *   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It uses Now&Next PAD data
 * to update the metadata on TuneIn specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_tunein.rlm rlm_tunein.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_tunein_devs;
char *rlm_tunein_stationids;
char *rlm_tunein_partnerids;
char *rlm_tunein_partnerkeys;
char *rlm_tunein_titles;
char *rlm_tunein_artists;
char *rlm_tunein_albums;
int *rlm_tunein_masters;
int *rlm_tunein_aux1s;
int *rlm_tunein_aux2s;


int rlm_tunein_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
{
  int dOldSize,dNewSize;
  int i;

  /*
   * Will it fit?
   */
  dOldSize=strlen(sString);
  if((dOldSize+dDiff)>=dMaxSize) {
    return -1;
  }
  dNewSize=dOldSize+dDiff;

  /* 
   * Adding characters 
   */
  if(dDiff>0) {
    for(i=dOldSize;i>dOrigin;i--) {
      sString[i+dDiff]=sString[i];
    }
    return dNewSize;
  }

  /* 
   * No Change
   */
  if(dDiff==0) {
    return dNewSize;
  }

  /*
   * Deleting Characters
   */
  if(dDiff<0) {
    for(i=dOrigin;i<dOldSize;i++) {
      sString[i]=sString[i-dDiff];
    }
    return dNewSize;
  }
  return -1; 
}


int rlm_tunein_EncodeString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if(((sString[i]!='*') && (sString[i]!='-') &&
	(sString[i]!='_') && (sString[i]!='.')) && 
       ((sString[i]<'0') ||
       ((sString[i]>'9') && (sString[i]<'A')) ||
       ((sString[i]>'Z') && (sString[i]<'a')) ||
       (sString[i]>'z'))) {
      if(rlm_tunein_BufferDiff(sString,i,2,dMaxSize)<0) {
	return -1;
      }
      sprintf(sAccum,"%%%2x",sString[i]);
      sString[i++]=sAccum[0];
      sString[i++]=sAccum[1];
      sString[i]=sAccum[2];
    }
    if(sString[i]==' ') {
     sString[i]='+';
    }
    i++;
    if(i>=dMaxSize) {
      return -1;
    }
  }
  return strlen(sString);
}


int rlm_tunein_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_tunein_RLMStart(void *ptr,const char *arg)
{
  char stationid[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_tunein_devs=0;
  rlm_tunein_stationids=NULL;
  rlm_tunein_partnerids=NULL;
  rlm_tunein_partnerkeys=NULL;
  rlm_tunein_titles=NULL;
  rlm_tunein_artists=NULL;
  rlm_tunein_albums=NULL;
  rlm_tunein_masters=NULL;
  rlm_tunein_aux1s=NULL;
  rlm_tunein_aux2s=NULL;

  sprintf(section,"Station%d",i++);
  strncpy(stationid,RLMGetStringValue(ptr,arg,section,"StationID",""),255);
  stationid[255]=0;
  if(strlen(stationid)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_tunein: no Live365 stations specified");
    return;
  }
  while(strlen(stationid)>0) {
    rlm_tunein_stationids=realloc(rlm_tunein_stationids,
		      (rlm_tunein_devs+1)*(rlm_tunein_devs+1)*256);
    strcpy(rlm_tunein_stationids+256*rlm_tunein_devs,stationid);
    rlm_tunein_partnerids=realloc(rlm_tunein_partnerids,(rlm_tunein_devs+1)*256);
    strncpy(rlm_tunein_partnerids+256*rlm_tunein_devs,
	    RLMGetStringValue(ptr,arg,section,"PartnerID",""),256);
    rlm_tunein_partnerkeys=realloc(rlm_tunein_partnerkeys,(rlm_tunein_devs+1)*256);
    strncpy(rlm_tunein_partnerkeys+256*rlm_tunein_devs,
	    RLMGetStringValue(ptr,arg,section,"PartnerKey",""),256);

    rlm_tunein_titles=realloc(rlm_tunein_titles,(rlm_tunein_devs+1)*256);
    strncpy(rlm_tunein_titles+256*rlm_tunein_devs,
	    RLMGetStringValue(ptr,arg,section,"TitleString",""),256);

    rlm_tunein_artists=realloc(rlm_tunein_artists,(rlm_tunein_devs+1)*256);
    strncpy(rlm_tunein_artists+256*rlm_tunein_devs,
	    RLMGetStringValue(ptr,arg,section,"ArtistString",""),256);

    rlm_tunein_albums=realloc(rlm_tunein_albums,(rlm_tunein_devs+1)*256);
    strncpy(rlm_tunein_albums+256*rlm_tunein_devs,
	    RLMGetStringValue(ptr,arg,section,"AlbumString",""),256);


    rlm_tunein_masters=realloc(rlm_tunein_masters,
			    (rlm_tunein_devs+1)*sizeof(int));
    rlm_tunein_masters[rlm_tunein_devs]=
      rlm_tunein_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_tunein_aux1s=realloc(rlm_tunein_aux1s,
			  (rlm_tunein_devs+1)*sizeof(int));
    rlm_tunein_aux1s[rlm_tunein_devs]=
      rlm_tunein_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_tunein_aux2s=realloc(rlm_tunein_aux2s,
			  (rlm_tunein_devs+1)*sizeof(int));
    rlm_tunein_aux2s[rlm_tunein_devs]=
      rlm_tunein_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_tunein: configured station \"%s\"",
	    rlm_tunein_stationids+256*rlm_tunein_devs);
    rlm_tunein_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Station%d",i++);
    strncpy(stationid,RLMGetStringValue(ptr,arg,section,"StationID",""),255);
    stationid[255]=0;
  }
}


void rlm_tunein_RLMFree(void *ptr)
{
  free(rlm_tunein_stationids);
  free(rlm_tunein_partnerids);
  free(rlm_tunein_partnerkeys);
  free(rlm_tunein_titles);
  free(rlm_tunein_artists);
  free(rlm_tunein_albums);
  free(rlm_tunein_masters);
  free(rlm_tunein_aux1s);
  free(rlm_tunein_aux2s);
}


void rlm_tunein_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char stationid[1024];
  char partnerid[1024];
  char partnerkey[1024];
  char title[1024];
  char artist[1024];
  char album[1024];
  char url[8192];
  char msg[1500];

  for(i=0;i<rlm_tunein_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_tunein_masters[i];
	break;

      case 1:
	flag=rlm_tunein_aux1s[i];
	break;

      case 2:
	flag=rlm_tunein_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(stationid,RLMResolveNowNext(ptr,now,next,
					rlm_tunein_stationids+256*i),256);
      rlm_tunein_EncodeString(stationid,1023);

      strncpy(partnerid,RLMResolveNowNext(ptr,now,next,
					 rlm_tunein_partnerids+256*i),256);
      rlm_tunein_EncodeString(partnerid,1023);

      strncpy(partnerkey,RLMResolveNowNext(ptr,now,next,
					 rlm_tunein_partnerkeys+256*i),256);
      rlm_tunein_EncodeString(partnerkey,1023);

      strncpy(title,RLMResolveNowNext(ptr,now,next,
				      rlm_tunein_titles+256*i),256);
      rlm_tunein_EncodeString(title,1023);
      strncpy(artist,RLMResolveNowNext(ptr,now,next,
				      rlm_tunein_artists+256*i),256);
      rlm_tunein_EncodeString(artist,1023);
      strncpy(album,RLMResolveNowNext(ptr,now,next,
				      rlm_tunein_albums+256*i),256);
      rlm_tunein_EncodeString(album,1023);
      snprintf(url,8192,"http://air.radiotime.com/Playing.ashx?partnerId=%s&partnerKey=%s&id=%s&title=%s&artist=%s",
	       partnerid,
	       partnerkey,
	       stationid,
	       title,
	       artist);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-o","/dev/null","-s",url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_tunein: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_tunein: sending pad update: \"%s\"",
	       (const char *)url);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
