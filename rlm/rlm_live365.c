/* rlm_live365.c
 *
 *   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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
    //    sprintf(errtext,"rlm_live365: configured server \"%s:%d\"",rlm_live365_hostnames+256*rlm_live365_devs,rlm_live365_tcpports[rlm_live365_devs]);
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
      snprintf(url,8192,"http://www.live365.com/cgi-bin/add_song.cgi?member_name=%s&password=%s&version=2&filename=Rivendell&seconds=%u&title=%s&artist=%s&album=%s",
	       station,
	       password,
	       now->rlm_len/1000,
	       title,
	       artist,
	       album);
      printf("URL: %s\n",url);
      /*
       * D.N.A.S v1.9.8 refuses to process updates with the default CURL
       * user-agent value, hence we lie to it.
       */
      /*
      strncpy(user_agent,"User-agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2) Gecko/20070219 Firefox/2.0.0.2",255);
      */
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-o","/dev/null","-s",url,(char *)NULL);
	  /*
	  execlp("curl","curl","-o","/dev/null","-s","--header",user_agent,
		 url,(char *)NULL);
	  */
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
