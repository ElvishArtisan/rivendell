/* rlm_live365.c
 *
 *   (C) Copyright 2014-2018 Fred Gleason <fredg@paravelsystems.com>
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
 * to update the metadata on an Icecast2 mountpoint specified in the 
 * configuration file pointed to by the plugin argument.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_live365.rlm rlm_live365.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_live365_devs;
char *rlm_live365_stations;
char *rlm_live365_passwords;
char *rlm_live365_titles;
char *rlm_live365_artists;
char *rlm_live365_albums;
int *rlm_live365_masters;
int *rlm_live365_aux1s;
int *rlm_live365_aux2s;
int *rlm_live365_vlog101s;
int *rlm_live365_vlog102s;
int *rlm_live365_vlog103s;
int *rlm_live365_vlog104s;
int *rlm_live365_vlog105s;
int *rlm_live365_vlog106s;
int *rlm_live365_vlog107s;
int *rlm_live365_vlog108s;
int *rlm_live365_vlog109s;
int *rlm_live365_vlog110s;
int *rlm_live365_vlog111s;
int *rlm_live365_vlog112s;
int *rlm_live365_vlog113s;
int *rlm_live365_vlog114s;
int *rlm_live365_vlog115s;
int *rlm_live365_vlog116s;
int *rlm_live365_vlog117s;
int *rlm_live365_vlog118s;
int *rlm_live365_vlog119s;
int *rlm_live365_vlog120s;


int rlm_live365_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
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


int rlm_live365_EncodeString(char *sString,int dMaxSize)
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
      if(rlm_live365_BufferDiff(sString,i,2,dMaxSize)<0) {
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


int rlm_live365_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_live365_RLMStart(void *ptr,const char *arg)
{
  char password[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_live365_devs=0;
  rlm_live365_stations=NULL;
  rlm_live365_passwords=NULL;
  rlm_live365_titles=NULL;
  rlm_live365_artists=NULL;
  rlm_live365_albums=NULL;
  rlm_live365_masters=NULL;
  rlm_live365_aux1s=NULL;
  rlm_live365_aux2s=NULL;
  rlm_live365_vlog101s=NULL;
  rlm_live365_vlog102s=NULL;
  rlm_live365_vlog103s=NULL;
  rlm_live365_vlog104s=NULL;
  rlm_live365_vlog105s=NULL;
  rlm_live365_vlog106s=NULL;
  rlm_live365_vlog107s=NULL;
  rlm_live365_vlog108s=NULL;
  rlm_live365_vlog109s=NULL;
  rlm_live365_vlog110s=NULL;
  rlm_live365_vlog111s=NULL;
  rlm_live365_vlog112s=NULL;
  rlm_live365_vlog113s=NULL;
  rlm_live365_vlog114s=NULL;
  rlm_live365_vlog115s=NULL;
  rlm_live365_vlog116s=NULL;
  rlm_live365_vlog117s=NULL;
  rlm_live365_vlog118s=NULL;
  rlm_live365_vlog119s=NULL;
  rlm_live365_vlog120s=NULL;

  sprintf(section,"Station%d",i++);
  strncpy(password,RLMGetStringValue(ptr,arg,section,"Password",""),255);
  password[255]=0;
  if(strlen(password)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_live365: no Live365 stations specified");
    return;
  }
  while(strlen(password)>0) {
    rlm_live365_passwords=realloc(rlm_live365_passwords,
		      (rlm_live365_devs+1)*(rlm_live365_devs+1)*256);
    strcpy(rlm_live365_passwords+256*rlm_live365_devs,password);
    rlm_live365_stations=realloc(rlm_live365_stations,(rlm_live365_devs+1)*256);
    strncpy(rlm_live365_stations+256*rlm_live365_devs,
	    RLMGetStringValue(ptr,arg,section,"MemberName",""),256);

    rlm_live365_titles=realloc(rlm_live365_titles,(rlm_live365_devs+1)*256);
    strncpy(rlm_live365_titles+256*rlm_live365_devs,
	    RLMGetStringValue(ptr,arg,section,"TitleString",""),256);

    rlm_live365_artists=realloc(rlm_live365_artists,(rlm_live365_devs+1)*256);
    strncpy(rlm_live365_artists+256*rlm_live365_devs,
	    RLMGetStringValue(ptr,arg,section,"ArtistString",""),256);

    rlm_live365_albums=realloc(rlm_live365_albums,(rlm_live365_devs+1)*256);
    strncpy(rlm_live365_albums+256*rlm_live365_devs,
	    RLMGetStringValue(ptr,arg,section,"AlbumString",""),256);


    rlm_live365_masters=realloc(rlm_live365_masters,
			    (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_masters[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_live365_aux1s=realloc(rlm_live365_aux1s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_aux1s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_live365_aux2s=realloc(rlm_live365_aux2s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_aux2s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"Aux2Log");

    rlm_live365_vlog101s=realloc(rlm_live365_vlog101s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog101s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog101");

    rlm_live365_vlog102s=realloc(rlm_live365_vlog102s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog102s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog102");

    rlm_live365_vlog103s=realloc(rlm_live365_vlog103s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog103s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog103");

    rlm_live365_vlog104s=realloc(rlm_live365_vlog104s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog104s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog104");

    rlm_live365_vlog105s=realloc(rlm_live365_vlog105s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog105s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog105");

    rlm_live365_vlog106s=realloc(rlm_live365_vlog106s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog106s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog106");

    rlm_live365_vlog107s=realloc(rlm_live365_vlog107s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog107s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog107");

    rlm_live365_vlog108s=realloc(rlm_live365_vlog108s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog108s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog108");

    rlm_live365_vlog109s=realloc(rlm_live365_vlog109s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog109s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog109");

    rlm_live365_vlog110s=realloc(rlm_live365_vlog110s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog110s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog110");

    rlm_live365_vlog111s=realloc(rlm_live365_vlog111s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog111s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog111");

    rlm_live365_vlog112s=realloc(rlm_live365_vlog112s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog112s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog112");

    rlm_live365_vlog113s=realloc(rlm_live365_vlog113s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog113s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog113");

    rlm_live365_vlog114s=realloc(rlm_live365_vlog114s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog114s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog114");

    rlm_live365_vlog115s=realloc(rlm_live365_vlog115s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog115s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog115");

    rlm_live365_vlog116s=realloc(rlm_live365_vlog116s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog116s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog116");

    rlm_live365_vlog117s=realloc(rlm_live365_vlog117s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog117s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog117");

    rlm_live365_vlog118s=realloc(rlm_live365_vlog118s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog118s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog118");

    rlm_live365_vlog119s=realloc(rlm_live365_vlog119s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog119s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog119");

    rlm_live365_vlog120s=realloc(rlm_live365_vlog120s,
			  (rlm_live365_devs+1)*sizeof(int));
    rlm_live365_vlog120s[rlm_live365_devs]=
      rlm_live365_GetLogStatus(ptr,arg,section,"VLog120");

    sprintf(errtext,"rlm_live365: configured station \"%s\"",
	    rlm_live365_stations+256*rlm_live365_devs);
    rlm_live365_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Station%d",i++);
    strncpy(password,RLMGetStringValue(ptr,arg,section,"Password",""),255);
    password[255]=0;
  }
}


void rlm_live365_RLMFree(void *ptr)
{
  free(rlm_live365_stations);
  free(rlm_live365_passwords);
  free(rlm_live365_titles);
  free(rlm_live365_artists);
  free(rlm_live365_albums);
  free(rlm_live365_masters);
  free(rlm_live365_aux1s);
  free(rlm_live365_aux2s);
  free(rlm_live365_vlog101s);
  free(rlm_live365_vlog102s);
  free(rlm_live365_vlog103s);
  free(rlm_live365_vlog104s);
  free(rlm_live365_vlog105s);
  free(rlm_live365_vlog106s);
  free(rlm_live365_vlog107s);
  free(rlm_live365_vlog108s);
  free(rlm_live365_vlog109s);
  free(rlm_live365_vlog110s);
  free(rlm_live365_vlog111s);
  free(rlm_live365_vlog112s);
  free(rlm_live365_vlog113s);
  free(rlm_live365_vlog114s);
  free(rlm_live365_vlog115s);
  free(rlm_live365_vlog116s);
  free(rlm_live365_vlog117s);
  free(rlm_live365_vlog118s);
  free(rlm_live365_vlog119s);
  free(rlm_live365_vlog120s);
}


void rlm_live365_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char station[1024];
  char password[1024];
  char title[1024];
  char artist[1024];
  char album[1024];
  char url[8192];
  char msg[1500];

  for(i=0;i<rlm_live365_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_live365_masters[i];
	break;

      case 1:
	flag=rlm_live365_aux1s[i];
	break;

      case 2:
	flag=rlm_live365_aux2s[i];
	break;

    case 100:
      flag=rlm_live365_vlog101s[i];
      break;

    case 101:
      flag=rlm_live365_vlog102s[i];
      break;

    case 102:
      flag=rlm_live365_vlog103s[i];
      break;

    case 103:
      flag=rlm_live365_vlog104s[i];
      break;

    case 104:
      flag=rlm_live365_vlog105s[i];
      break;

    case 105:
      flag=rlm_live365_vlog106s[i];
      break;

    case 106:
      flag=rlm_live365_vlog107s[i];
      break;

    case 107:
      flag=rlm_live365_vlog108s[i];
      break;

    case 108:
      flag=rlm_live365_vlog109s[i];
      break;

    case 109:
      flag=rlm_live365_vlog110s[i];
      break;

    case 110:
      flag=rlm_live365_vlog111s[i];
      break;

    case 111:
      flag=rlm_live365_vlog112s[i];
      break;

    case 112:
      flag=rlm_live365_vlog113s[i];
      break;

    case 113:
      flag=rlm_live365_vlog114s[i];
      break;

    case 114:
      flag=rlm_live365_vlog115s[i];
      break;

    case 115:
      flag=rlm_live365_vlog116s[i];
      break;

    case 116:
      flag=rlm_live365_vlog117s[i];
      break;

    case 117:
      flag=rlm_live365_vlog118s[i];
      break;

    case 118:
      flag=rlm_live365_vlog119s[i];
      break;

    case 119:
      flag=rlm_live365_vlog120s[i];
      break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(station,RLMResolveNowNext(ptr,now,next,
					rlm_live365_stations+256*i),256);
      rlm_live365_EncodeString(station,1023);

      strncpy(password,RLMResolveNowNext(ptr,now,next,
					 rlm_live365_passwords+256*i),256);
      rlm_live365_EncodeString(title,1023);

      strncpy(title,RLMResolveNowNext(ptr,now,next,
				      rlm_live365_titles+256*i),256);
      rlm_live365_EncodeString(title,1023);
      strncpy(artist,RLMResolveNowNext(ptr,now,next,
				      rlm_live365_artists+256*i),256);
      rlm_live365_EncodeString(artist,1023);
      strncpy(album,RLMResolveNowNext(ptr,now,next,
				      rlm_live365_albums+256*i),256);
      rlm_live365_EncodeString(album,1023);
      snprintf(url,8192,"http://www.live365.com/cgi-bin/add_song.cgi?member_name=%s&password=%s&version=2&filename=Rivendell&seconds=%u&title=%s&artist=%s&album=%s",
	       station,
	       password,
	       now->rlm_len/1000,
	       title,
	       artist,
	       album);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-o","/dev/null","-s",url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_live365: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_live365: sending pad update: \"%s\"",
	       (const char *)url);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
