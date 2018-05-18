/* rlm_tagstation.c
 *
 *   (C) Copyright 2017-2018 Fred Gleason <fredg@paravelsystems.com>
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
char *rlm_tagstation_pros;
char *rlm_tagstation_muss;
int *rlm_tagstation_masters;
int *rlm_tagstation_aux1s;
int *rlm_tagstation_aux2s;
int *rlm_tagstation_vlog101s;
int *rlm_tagstation_vlog102s;
int *rlm_tagstation_vlog103s;
int *rlm_tagstation_vlog104s;
int *rlm_tagstation_vlog105s;
int *rlm_tagstation_vlog106s;
int *rlm_tagstation_vlog107s;
int *rlm_tagstation_vlog108s;
int *rlm_tagstation_vlog109s;
int *rlm_tagstation_vlog110s;
int *rlm_tagstation_vlog111s;
int *rlm_tagstation_vlog112s;
int *rlm_tagstation_vlog113s;
int *rlm_tagstation_vlog114s;
int *rlm_tagstation_vlog115s;
int *rlm_tagstation_vlog116s;
int *rlm_tagstation_vlog117s;
int *rlm_tagstation_vlog118s;
int *rlm_tagstation_vlog119s;
int *rlm_tagstation_vlog120s;

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

  memset(ret,0,3);
  if(strlen(grp_name)!=0) {
    if(rlm_tagstation_ContainsString(rlm_tagstation_adps+256*num,grp_name)>0) {
      strncpy(ret,"ADP",3);
    }
    if(rlm_tagstation_ContainsString(rlm_tagstation_pros+256*num,grp_name)>0) {
      strncpy(ret,"PRO",3);
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
  rlm_tagstation_pros=NULL;
  rlm_tagstation_muss=NULL;
  rlm_tagstation_masters=NULL;
  rlm_tagstation_aux1s=NULL;
  rlm_tagstation_aux2s=NULL;
  rlm_tagstation_vlog101s=NULL;
  rlm_tagstation_vlog102s=NULL;
  rlm_tagstation_vlog103s=NULL;
  rlm_tagstation_vlog104s=NULL;
  rlm_tagstation_vlog105s=NULL;
  rlm_tagstation_vlog106s=NULL;
  rlm_tagstation_vlog107s=NULL;
  rlm_tagstation_vlog108s=NULL;
  rlm_tagstation_vlog109s=NULL;
  rlm_tagstation_vlog110s=NULL;
  rlm_tagstation_vlog111s=NULL;
  rlm_tagstation_vlog112s=NULL;
  rlm_tagstation_vlog113s=NULL;
  rlm_tagstation_vlog114s=NULL;
  rlm_tagstation_vlog115s=NULL;
  rlm_tagstation_vlog116s=NULL;
  rlm_tagstation_vlog117s=NULL;
  rlm_tagstation_vlog118s=NULL;
  rlm_tagstation_vlog119s=NULL;
  rlm_tagstation_vlog120s=NULL;

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
	    RLMGetStringValue(ptr,arg,section,"CategoryAdvertisements",""),256);

    rlm_tagstation_pros=realloc(rlm_tagstation_pros,(rlm_tagstation_devs+1)*256);
    strncpy(rlm_tagstation_pros+256*rlm_tagstation_devs,
	    RLMGetStringValue(ptr,arg,section,"CategoryPromotions",""),256);

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

    rlm_tagstation_vlog101s=realloc(rlm_tagstation_vlog101s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog101s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog101");

    rlm_tagstation_vlog102s=realloc(rlm_tagstation_vlog102s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog102s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog102");

    rlm_tagstation_vlog103s=realloc(rlm_tagstation_vlog103s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog103s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog103");

    rlm_tagstation_vlog104s=realloc(rlm_tagstation_vlog104s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog104s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog104");

    rlm_tagstation_vlog105s=realloc(rlm_tagstation_vlog105s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog105s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog105");

    rlm_tagstation_vlog106s=realloc(rlm_tagstation_vlog106s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog106s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog106");

    rlm_tagstation_vlog107s=realloc(rlm_tagstation_vlog107s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog107s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog107");

    rlm_tagstation_vlog108s=realloc(rlm_tagstation_vlog108s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog108s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog108");

    rlm_tagstation_vlog109s=realloc(rlm_tagstation_vlog109s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog109s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog109");

    rlm_tagstation_vlog110s=realloc(rlm_tagstation_vlog110s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog110s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog110");

    rlm_tagstation_vlog111s=realloc(rlm_tagstation_vlog111s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog111s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog111");

    rlm_tagstation_vlog112s=realloc(rlm_tagstation_vlog112s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog112s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog112");

    rlm_tagstation_vlog113s=realloc(rlm_tagstation_vlog113s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog113s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog113");

    rlm_tagstation_vlog114s=realloc(rlm_tagstation_vlog114s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog114s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog114");

    rlm_tagstation_vlog115s=realloc(rlm_tagstation_vlog115s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog115s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog115");

    rlm_tagstation_vlog116s=realloc(rlm_tagstation_vlog116s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog116s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog116");

    rlm_tagstation_vlog117s=realloc(rlm_tagstation_vlog117s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog117s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog117");

    rlm_tagstation_vlog118s=realloc(rlm_tagstation_vlog118s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog118s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog118");

    rlm_tagstation_vlog119s=realloc(rlm_tagstation_vlog119s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog119s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog119");

    rlm_tagstation_vlog120s=realloc(rlm_tagstation_vlog120s,
			  (rlm_tagstation_devs+1)*sizeof(int));
    rlm_tagstation_vlog120s[rlm_tagstation_devs]=
      rlm_tagstation_GetLogStatus(ptr,arg,section,"VLog120");

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
  free(rlm_tagstation_pros);
  free(rlm_tagstation_muss);
  free(rlm_tagstation_masters);
  free(rlm_tagstation_aux1s);
  free(rlm_tagstation_aux2s);
  free(rlm_tagstation_vlog101s);
  free(rlm_tagstation_vlog102s);
  free(rlm_tagstation_vlog103s);
  free(rlm_tagstation_vlog104s);
  free(rlm_tagstation_vlog105s);
  free(rlm_tagstation_vlog106s);
  free(rlm_tagstation_vlog107s);
  free(rlm_tagstation_vlog108s);
  free(rlm_tagstation_vlog109s);
  free(rlm_tagstation_vlog110s);
  free(rlm_tagstation_vlog111s);
  free(rlm_tagstation_vlog112s);
  free(rlm_tagstation_vlog113s);
  free(rlm_tagstation_vlog114s);
  free(rlm_tagstation_vlog115s);
  free(rlm_tagstation_vlog116s);
  free(rlm_tagstation_vlog117s);
  free(rlm_tagstation_vlog118s);
  free(rlm_tagstation_vlog119s);
  free(rlm_tagstation_vlog120s);
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
  char category[4]={0};

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

    case 100:
      flag=rlm_tagstation_vlog101s[i];
      break;

    case 101:
      flag=rlm_tagstation_vlog102s[i];
      break;

    case 102:
      flag=rlm_tagstation_vlog103s[i];
      break;

    case 103:
      flag=rlm_tagstation_vlog104s[i];
      break;

    case 104:
      flag=rlm_tagstation_vlog105s[i];
      break;

    case 105:
      flag=rlm_tagstation_vlog106s[i];
      break;

    case 106:
      flag=rlm_tagstation_vlog107s[i];
      break;

    case 107:
      flag=rlm_tagstation_vlog108s[i];
      break;

    case 108:
      flag=rlm_tagstation_vlog109s[i];
      break;

    case 109:
      flag=rlm_tagstation_vlog110s[i];
      break;

    case 110:
      flag=rlm_tagstation_vlog111s[i];
      break;

    case 111:
      flag=rlm_tagstation_vlog112s[i];
      break;

    case 112:
      flag=rlm_tagstation_vlog113s[i];
      break;

    case 113:
      flag=rlm_tagstation_vlog114s[i];
      break;

    case 114:
      flag=rlm_tagstation_vlog115s[i];
      break;

    case 115:
      flag=rlm_tagstation_vlog116s[i];
      break;

    case 116:
      flag=rlm_tagstation_vlog117s[i];
      break;

    case 117:
      flag=rlm_tagstation_vlog118s[i];
      break;

    case 118:
      flag=rlm_tagstation_vlog119s[i];
      break;

    case 119:
      flag=rlm_tagstation_vlog120s[i];
      break;
    }
    if((flag==1)||((flag==2)&&(log->log_onair!=0))) {
      
      strncpy(category,rlm_tagstation_GetCategory(now->rlm_group,i),3);
      if(category[0]!=0) {
	snprintf(fmt,1024,"https://tsl.tagstation.com/tsl.ashx?CID=%s&Title=%s&Artist=%s&Album=%s&EventCategory=%s&EventID=%%n&Duration=%%h&LookAhead=False",
		 rlm_tagstation_clientids+256*i,
		 rlm_tagstation_titles+256*i,
		 rlm_tagstation_artists+256*i,
		 rlm_tagstation_albums+256*i,
		 category);
	      
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
}
