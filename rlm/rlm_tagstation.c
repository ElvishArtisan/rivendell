/* rlm_tagstation.c
 *
 *   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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
 * This is a sample Rivendell Loadable Module.  All it does is print
 * Now & Next data to standard output for each event transition.
 *
 * To compile this module, just do:
 * 
 *   gcc -shared -o rlm_tagstation.rlm rlm_tagstation.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#include <rlm/rlm.h>

int rlm_tagstation_devs;
char *rlm_tagstation_clientids;
char *rlm_tagstation_passwords;
char *rlm_tagstation_titles;
char *rlm_tagstation_artists;
char *rlm_tagstation_albums;
char *rlm_tagstation_adps;
char *rlm_tagstation_etos;
char *rlm_tagstation_muss;
int *rlm_tagstation_masters;
int *rlm_tagstation_aux1s;
int *rlm_tagstation_aux2s;

int rlm_tagstation_ContainsString(const char *str,const char *substr)
{
  int ptr=0;

  do {
    if(strspn(str+ptr,substr)==strlen(substr)) {
      return 1;
    }
    while((str[ptr]!=',')&&(ptr<strlen(str))) {
      ptr++;
    }
  } while(++ptr<strlen(str));
  return 0;
}


char *rlm_tagstation_GetCategory(const char *grp_name,int num)
{
  static char ret[3];

  strncpy(ret,"PRO",3);
  if(strlen(grp_name)!=0) {
    if(rlm_tagstation_ContainsString(rlm_tagstation_adps+256*num,grp_name)>0) {
      strncpy(ret,"ADP",3);
    }
    if(rlm_tagstation_ContainsString(rlm_tagstation_etos+256*num,grp_name)>0) {
      strncpy(ret,"ETO",3);
    }
    if(rlm_tagstation_ContainsString(rlm_tagstation_muss+256*num,grp_name)>0) {
      strncpy(ret,"MUS",3);
    }
  }
  return ret;
}


int rlm_tagstation_GetLogStatus(void *ptr,const char *arg,const char *section,
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


void rlm_tagstation_RLMStart(void *ptr,const char *arg)
{
  char username[256];
  char section[256];
  char errtext[256];
  int i=1;

  rlm_tagstation_devs=0;
  rlm_tagstation_clientids=NULL;
  rlm_tagstation_passwords=NULL;
  rlm_tagstation_titles=NULL;
  rlm_tagstation_artists=NULL;
  rlm_tagstation_albums=NULL;
  rlm_tagstation_adps=NULL;
  rlm_tagstation_etos=NULL;
  rlm_tagstation_muss=NULL;
  rlm_tagstation_masters=NULL;
  rlm_tagstation_aux1s=NULL;
  rlm_tagstation_aux2s=NULL;

  sprintf(section,"TagStation%d",i++);
  strncpy(username,RLMGetStringValue(ptr,arg,section,"ClientId",""),255);
  username[255]=0;
  if(strlen(username)==0) {
    RLMLog(ptr,LOG_WARNING,"rlm_tagstation: no tagstation instances specified");
    return;
  }
  while(strlen(username)>0) {
    rlm_tagstation_clientids=realloc(rlm_tagstation_clientids,
			   (rlm_tagstation_devs+1)*(rlm_tagstation_devs+1)*256);
    strcpy(rlm_tagstation_clientids+256*rlm_tagstation_devs,username);

    rlm_tagstation_passwords=realloc(rlm_tagstation_passwords,
			   (rlm_tagstation_devs+1)*(rlm_tagstation_devs+1)*256);
    strcpy(rlm_tagstation_passwords+256*rlm_tagstation_devs,
	   RLMGetStringValue(ptr,arg,section,"Password",""));

    rlm_tagstation_titles=realloc(rlm_tagstation_titles,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_titles+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"TitleString",""),256);

    rlm_tagstation_artists=realloc(rlm_tagstation_artists,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_artists+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"ArtistString",""),256);

    rlm_tagstation_albums=realloc(rlm_tagstation_albums,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_albums+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"AlbumString",""),256);

    rlm_tagstation_adps=realloc(rlm_tagstation_adps,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_adps+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"CategoryAdvertisement",""),256);

    rlm_tagstation_etos=realloc(rlm_tagstation_etos,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_etos+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"CategoryGapLookup",""),256);

    rlm_tagstation_muss=realloc(rlm_tagstation_muss,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_muss+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"CategoryMusic",""),256);

    rlm_tagstation_masters=realloc(rlm_tagstation_masters,
			    (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_masters[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"MasterLog");
    rlm_tagstation_aux1s=realloc(rlm_tagstation_aux1s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_aux1s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"Aux1Log");
    rlm_tagstation_aux2s=realloc(rlm_tagstation_aux2s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_aux2s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"Aux2Log");
    sprintf(errtext,
	"rlm_tagstation: configured TagStation instance for Client ID \"%s\"",
	    rlm_tagstation_clientids+256*rlm_tagstation_devs);
    rlm_tagstation_devs++;
    RLMLog(ptr,LOG_INFO,errtext);
    sprintf(section,"TagStation%d",i++);
    strncpy(username,RLMGetStringValue(ptr,arg,section,"Username",""),255);
    username[255]=0;
  }
}


void rlm_tagstation_RLMFree(void *ptr)
{
  free(rlm_tagstation_clientids);
  free(rlm_tagstation_passwords);
  free(rlm_tagstation_titles);
  free(rlm_tagstation_artists);
  free(rlm_tagstation_albums);
  free(rlm_tagstation_adps);
  free(rlm_tagstation_etos);
  free(rlm_tagstation_muss);
  free(rlm_tagstation_masters);
  free(rlm_tagstation_aux1s);
  free(rlm_tagstation_aux2s);
}


void rlm_tagstation_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
				const struct rlm_log *log,
				const struct rlm_pad *now,
				const struct rlm_pad *next)
{
  int i;
  int flag=0;
  char account[1024];
  char fmt[1024];
  char url[4096];
  char msg[1500];

  for(i=0;i<rlm_tagstation_devs;i++) {
    switch(log->log_mach) {
      case 0:
	flag=rlm_tagstation_masters[i];
	break;

      case 1:
	flag=rlm_tagstation_aux1s[i];
	break;

      case 2:
	flag=rlm_tagstation_aux2s[i];
	break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      snprintf(fmt,1024,"https://tsl.tagstation.com/tsl.ashx?CID=%s&Title=%s&Artist=%s&Album=%s&EventCategory=%s&EventID=%%n&Duration=%%h&LookAhead=False",
	       rlm_tagstation_clientids+256*i,
	       rlm_tagstation_titles+256*i,
	       rlm_tagstation_artists+256*i,
	       rlm_tagstation_albums+256*i,
	       rlm_tagstation_GetCategory(now->rlm_group,i));
	      
      strncpy(url,RLMResolveNowNextEncoded(ptr,now,next,fmt,RLM_ENCODE_URL),
	      4096);
      snprintf(account,1024,"%s:%s",rlm_tagstation_clientids+256*i,
	       rlm_tagstation_passwords+256*i);
      if(strlen(now->rlm_title)!=0) {
	if(fork()==0) {
	  execlp("curl","curl","-u",account,"-o","/dev/null","-s",
		 url,(char *)NULL);
	  RLMLog(ptr,LOG_WARNING,"rlm_tagstation: unable to execute curl(1)");
	  exit(0);
	}
      }
      snprintf(msg,1500,"rlm_tagstation: sending pad update: \"%s\"",
	       (const char *)url);
      RLMLog(ptr,LOG_INFO,msg);
    }
  }
}
