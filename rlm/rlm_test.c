/* rlm_test.c
 *
 *   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
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
 *   gcc -shared -o rlm_test.rlm rlm_test.c
 */

#include <stdio.h>
#include <string.h>
#include <rlm/rlm.h>


void rlm_test_RLMStart(void *ptr,const char *arg)
{
  char str[1024];
  sprintf(str,"rlm_test: started on %s",
	  RLMDateTime(ptr,0,"MM/dd/yyyy at hh:mm:ss"));
  RLMLog(ptr,LOG_NOTICE,str);
}


void rlm_test_RLMFree(void *ptr)
{
}


void rlm_test_print_carttype(int type)
{
  switch(type) {
  case RLM_CARTTYPE_ALL:
    printf("           Type:\n");
    break;

  case RLM_CARTTYPE_AUDIO:
    printf("           Type: AUDIO\n");
    break;

  case RLM_CARTTYPE_MACRO:
    printf("           Type: MACRO\n");
    break;
  }
}

void rlm_test_RLMPadDataSent(void *ptr,const struct rlm_svc *svc,
			     const struct rlm_log *log,
			     const struct rlm_pad *now,
			     const struct rlm_pad *next)
{
  printf("Service: %s\n",svc->svc_name);
  printf("Program Code: %s\n",svc->svc_pgmcode); 
  printf("Log Name: %s\n",log->log_name);
  if(log->log_onair==0) {
    printf(" OnAir = false\n");
  }
  else {
    printf(" OnAir = true\n");
  }
  switch(log->log_mode) {
  case RLM_LOGMODE_LIVEASSIST:
    printf(" Mode = Live Assist\n");
    break;

  case RLM_LOGMODE_AUTOMATIC:
    printf(" Mode = Automatic\n");
    break;

  case RLM_LOGMODE_MANUAL:
    printf(" Mode = Manual\n");
    break;
  }
  switch(log->log_mach) {
    case 0:
      printf(" -- On Main Log ---------------------------------------------\n");
      break;

    case 1:
      printf(" -- On Aux 1 Log --------------------------------------------\n");
      break;

    case 2:
      printf(" -- On Aux 2 Log --------------------------------------------\n");
      break;
  }
  printf("Playing NOW\n");
  printf("    Cart number: %06u\n",now->rlm_cartnum);
  rlm_test_print_carttype(now->rlm_carttype);
  printf("Start Date/Time: %04u-%02u-%02u %02u:%02u:%02u.%03u\n",
	 now->rlm_start_year,now->rlm_start_mon,now->rlm_start_day,
	 now->rlm_start_hour,now->rlm_start_min,now->rlm_start_sec,
	 now->rlm_start_msec);
  printf("         Length: %u mS\n",now->rlm_len);
  printf("          Title: %s\n",now->rlm_title);
  printf("         Artist: %s\n",now->rlm_artist);
  printf("          Label: %s\n",now->rlm_label);
  printf("      Conductor: %s\n",now->rlm_conductor);
  printf("         SongId: %s\n",now->rlm_song_id);
  printf("         Client: %s\n",now->rlm_client);
  printf("         Agency: %s\n",now->rlm_agency);
  printf("       Composer: %s\n",now->rlm_comp);
  printf("      Publisher: %s\n",now->rlm_pub);
  printf("    UserDefined: %s\n",now->rlm_userdef);
  printf("         Outcue: %s\n",now->rlm_outcue);
  printf("Cut Description: %s\n",now->rlm_description);
  printf("           ISRC: %s\n",now->rlm_isrc);
  printf("      ISCI Code: %s\n",now->rlm_isci);
  printf("       Ext Data: %s\n",now->rlm_ext_data);
  printf("   Ext Event ID: %s\n",now->rlm_ext_eventid);
  printf("  Ext Annc Type: %s\n",now->rlm_ext_annctype);
  printf("\n");
  printf("Playing NEXT\n");
  printf("    Cart number: %06u\n",next->rlm_cartnum);
  rlm_test_print_carttype(next->rlm_carttype);
  printf("Start Date/Time: %04u-%02u-%02u %02u:%02u:%02u.%03u\n",
	 next->rlm_start_year,next->rlm_start_mon,next->rlm_start_day,
	 next->rlm_start_hour,next->rlm_start_min,next->rlm_start_sec,
	 next->rlm_start_msec);
  printf("         Length: %u mS\n",next->rlm_len);
  printf("          Title: %s\n",next->rlm_title);
  printf("         Artist: %s\n",next->rlm_artist);
  printf("          Label: %s\n",next->rlm_label);
  printf("      Conductor: %s\n",next->rlm_conductor);
  printf("         SongId: %s\n",next->rlm_song_id);
  printf("         Client: %s\n",next->rlm_client);
  printf("         Agency: %s\n",next->rlm_agency);
  printf("       Composer: %s\n",next->rlm_comp);
  printf("      Publisher: %s\n",next->rlm_pub);
  printf("    UserDefined: %s\n",next->rlm_userdef);
  printf("         Outcue: %s\n",next->rlm_outcue);
  printf("Cut Description: %s\n",next->rlm_description);
  printf("           ISRC: %s\n",next->rlm_isrc);
  printf("      ISCI Code: %s\n",next->rlm_isci);
  printf("       Ext Data: %s\n",next->rlm_ext_data);
  printf("   Ext Event ID: %s\n",next->rlm_ext_eventid);
  printf("  Ext Annc Type: %s\n",next->rlm_ext_annctype);
}
