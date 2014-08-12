/* rlm_spinitron_plus.c
 *
 *   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a Rivendell Loadable Module.  It sends Now&Next PAD data
 * to the Spinitron account(s) specified in the configuration file pointed
 * to by the plugin argument.  For information about Spinitron, see
 * http://www.spinitron.com/.
 *
 * Inspired by the original spinitron RLM by Eric Berg, Benjamin Yu
 * and Max Goldstein.
 *
 * This module requires the curl(1) network transfer tool, included with
 * most Linux distros.  It is also available at http://curl.haxx.se/.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_spinitron_plus.rlm rlm_spinitron_plus.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

#define RLM_SPINITRON_PLUS_LOGGER_NAME "Rivendell"

int rlm_spinitron_plus_devs;
char *rlm_spinitron_plus_stations;
char *rlm_spinitron_plus_usernames;
char *rlm_spinitron_plus_passwords;
int *rlm_spinitron_plus_playlist_modes;
char *rlm_spinitron_plus_titles;
char *rlm_spinitron_plus_artists;
char *rlm_spinitron_plus_albums;
char *rlm_spinitron_plus_labels;
char *rlm_spinitron_plus_composers;
char *rlm_spinitron_plus_notes;
int *rlm_spinitron_plus_masters;
int *rlm_spinitron_plus_aux1s;
int *rlm_spinitron_plus_aux2s;


int rlm_spinitron_plus_BufferDiff(char *sString,int dOrigin,int dDiff,int dMaxSize)
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


int rlm_spinitron_plus_EncodeString(char *sString,int dMaxSize)
{
  int i;                  /* General Purpose Counter */
  char sAccum[4];          /* General String Buffer */

  i=0;
  while(sString[i]!=0) {
    if(((sString[i]!=' ') && (sString[i]!='*') && (sString[i]!='-') &&
	(sString[i]!='_') && (sString[i]!='.')) && 
       ((sString[i]<'0') ||
       ((sString[i]>'9') && (sString[i]<'A')) ||
       ((sString[i]>'Z') && (sString[i]<'a')) ||
       (sString[i]>'z'))) {
      if(rlm_spinitron_plus_BufferDiff(sString,i,2,dMaxSize)<0) {
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
  }
  return strlen(sString);
}


int rlm_spinitron_plus_GetLogStatus(void *ptr,const char *arg,
				    const char *section,const char *logname)
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


void rlm_spinitron_plus_RLMStart(void *ptr,const char *arg)
{
  char station[256];
  char section[256];
  char errtext[256];
  char mode[256];
  int pmode;
  int i=1;

  rlm_spinitron_plus_devs=0;
  rlm_spinitron_plus_stations=NULL;
  rlm_spinitron_plus_usernames=NULL;
  rlm_spinitron_plus_passwords=NULL;
  rlm_spinitron_plus_playlist_modes=NULL;
  rlm_spinitron_plus_titles=NULL;
  rlm_spinitron_plus_artists=NULL;
  rlm_spinitron_plus_albums=NULL;
  rlm_spinitron_plus_labels=NULL;
  rlm_spinitron_plus_composers=NULL;
  rlm_spinitron_plus_notes=NULL;
  rlm_spinitron_plus_masters=NULL;
  rlm_spinitron_plus_aux1s=NULL;
  rlm_spinitron_plus_aux2s=NULL;

  sprintf(section,"Spinitron%d",i++);
  strncpy(station,RLMGetStringValue(ptr,arg,section,"Station",""),255);
  station[255]=0;
  if(strlen(station)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_spinitron_plus: no spinitron accounts specified");
    return;
  }
  while(strlen(station)>0) {
    rlm_spinitron_plus_stations=realloc(rlm_spinitron_plus_stations,
			   (rlm_spinitron_plus_devs+1)*(rlm_spinitron_plus_devs+1)*256);
    strcpy(rlm_spinitron_plus_stations+256*rlm_spinitron_plus_devs,station);
    rlm_spinitron_plus_EncodeString(rlm_spinitron_plus_stations+256*rlm_spinitron_plus_devs,255);

    rlm_spinitron_plus_usernames=realloc(rlm_spinitron_plus_usernames,
		  (rlm_spinitron_plus_devs+1)*(rlm_spinitron_plus_devs+1)*256);
    strcpy(rlm_spinitron_plus_usernames+256*rlm_spinitron_plus_devs,
	   RLMGetStringValue(ptr,arg,section,"Username",""));
    rlm_spinitron_plus_EncodeString(rlm_spinitron_plus_usernames+256*rlm_spinitron_plus_devs,255);

    rlm_spinitron_plus_passwords=realloc(rlm_spinitron_plus_passwords,
			   (rlm_spinitron_plus_devs+1)*(rlm_spinitron_plus_devs+1)*256);
    strcpy(rlm_spinitron_plus_passwords+256*rlm_spinitron_plus_devs,
	   RLMGetStringValue(ptr,arg,section,"Password",""));
    rlm_spinitron_plus_EncodeString(rlm_spinitron_plus_passwords+256*rlm_spinitron_plus_devs,255);

    pmode=3;
    strcpy(mode,RLMGetStringValue(ptr,arg,section,"PlaylistMode",""));
    if(strcasecmp(mode,"full")==0) {
      pmode=0;
    }
    if(strcasecmp(mode,"assist")==0) {
      pmode=2;
    }
    rlm_spinitron_plus_playlist_modes=
      realloc(rlm_spinitron_plus_playlist_modes,
	      (rlm_spinitron_plus_devs+1)*sizeof(int));
    rlm_spinitron_plus_playlist_modes[rlm_spinitron_plus_devs]=pmode;

    rlm_spinitron_plus_titles=realloc(rlm_spinitron_plus_titles,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_titles+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Title",""),256);

    rlm_spinitron_plus_artists=realloc(rlm_spinitron_plus_artists,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_artists+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Artist",""),256);

    rlm_spinitron_plus_albums=realloc(rlm_spinitron_plus_albums,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_albums+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Album",""),256);

    rlm_spinitron_plus_labels=realloc(rlm_spinitron_plus_labels,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_labels+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Label",""),256);

    rlm_spinitron_plus_composers=realloc(rlm_spinitron_plus_composers,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_composers+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Composer",""),256);

    rlm_spinitron_plus_notes=realloc(rlm_spinitron_plus_notes,(rlm_spinitron_plus_devs+1)*256);
    strncpy(rlm_spinitron_plus_notes+256*rlm_spinitron_plus_devs,
	    RLMGetStringValue(ptr,arg,section,"Notes",""),256);


    rlm_spinitron_plus_masters=realloc(rlm_spinitron_plus_masters,
			    (rlm_spinitron_plus_devs+1)*sizeof(int));
    rlm_spinitron_plus_masters[rlm_spinitron_plus_devs]=
      rlm_spinitron_plus_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_spinitron_plus_aux1s=realloc(rlm_spinitron_plus_aux1s,
			  (rlm_spinitron_plus_devs+1)*sizeof(int));
    rlm_spinitron_plus_aux1s[rlm_spinitron_plus_devs]=
      rlm_spinitron_plus_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_spinitron_plus_aux2s=realloc(rlm_spinitron_plus_aux2s,
			  (rlm_spinitron_plus_devs+1)*sizeof(int));
    rlm_spinitron_plus_aux2s[rlm_spinitron_plus_devs]=
      rlm_spinitron_plus_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,"rlm_spinitron_plus: configured account for station \"%s\"",
	    station);

    rlm_spinitron_plus_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"Spinitron%d",i++);
    strncpy(station,RLMGetStringValue(ptr,arg,section,"Station",""),255);
    station[255]=0;
  }
}


void rlm_spinitron_plus_RLMFree(void *ptr)
{
  free(rlm_spinitron_plus_stations);
  free(rlm_spinitron_plus_usernames);
  free(rlm_spinitron_plus_passwords);
  free(rlm_spinitron_plus_playlist_modes);
  free(rlm_spinitron_plus_titles);
  free(rlm_spinitron_plus_artists);
  free(rlm_spinitron_plus_albums);
  free(rlm_spinitron_plus_labels);
  free(rlm_spinitron_plus_composers);
  free(rlm_spinitron_plus_notes);
  free(rlm_spinitron_plus_masters);
  free(rlm_spinitron_plus_aux1s);
  free(rlm_spinitron_plus_aux2s);
}


void rlm_spinitron_plus_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				 const struct rlm_log *log,
				 const struct rlm_pad *now,
				 const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char title[1024];
  char artist[1024];
  char album[1024];
  char label[1024];
  char composer[1024];
  char notes[1024];
  char msg[8192];
  char sysmsg[8192];
  int pm;

  for(i=0;i<rlm_spinitron_plus_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_spinitron_plus_masters[i];
	break;

      case 1:
	flag=rlm_spinitron_plus_aux1s[i];
	break;

      case 2:
	flag=rlm_spinitron_plus_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      strncpy(title,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_titles+256*i),256);
      rlm_spinitron_plus_EncodeString(title,1023);
      strncpy(artist,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_artists+256*i),256);
      rlm_spinitron_plus_EncodeString(artist,1023);
      strncpy(album,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_albums+256*i),256);
      rlm_spinitron_plus_EncodeString(album,1023);
      strncpy(label,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_labels+256*i),256);
      rlm_spinitron_plus_EncodeString(label,1023);
      strncpy(composer,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_composers+256*i),256);
      rlm_spinitron_plus_EncodeString(composer,1023);
      strncpy(notes,RLMResolveNowNext(ptr,now,next,
				      rlm_spinitron_plus_notes+256*i),256);
      rlm_spinitron_plus_EncodeString(notes,1023);
      if(rlm_spinitron_plus_playlist_modes[i]==3) {
	switch(log->log_mode) {
	case RLM_LOGMODE_AUTOMATIC:
	  pm=0;
	  break;

	case RLM_LOGMODE_LIVEASSIST:
	case RLM_LOGMODE_MANUAL:
	default:
	  pm=2;
	  break;
	}
      }
      else {
	pm=rlm_spinitron_plus_playlist_modes[i];
      }

      if(strlen(now->rlm_title)!=0) {
	snprintf(msg,8192,
		 "https://spinitron.com/member/logthis.php?un=%s&pw=%s&sn=%s&aw=%s&dn=%s&ln=%s&sc=%s&se=%s&df=%s&st=%s&sd=%d&pm=%d",
		 rlm_spinitron_plus_usernames+256*i,
		 rlm_spinitron_plus_passwords+256*i,
		 title,
		 artist,
		 album,
		 label,
		 composer,
		 notes,
		 RLM_SPINITRON_PLUS_LOGGER_NAME,
		 rlm_spinitron_plus_stations+256*i,
		 now->rlm_len/1000,
		 pm);
	if(fork()==0) {
	  execlp("curl","curl",msg,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_spinitron_plus: unable to execute curl(1)");
	  exit(0);
	}
	snprintf(sysmsg,8192,"rlm_spinitron_plus: sending pad update: \"%s\"",
		 (const char *)msg);
	RLMLog(ptr,LOG_DEBUG,sysmsg);
      }
    }
  }
}
