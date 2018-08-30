/* listlog_test.c
 *
 * Test the listlog library.
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>             
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rivwebcapi/rd_listlog.h>
#include <rivwebcapi/rd_createticket.h>
#include <rivwebcapi/rd_getuseragent.h>
#include <rivwebcapi/rd_getversion.h>

int main(int argc,char *argv[])
{
  int i;
  struct rd_logline *logline=0;
  char buf[BUFSIZ];
  unsigned numrecs;
  char *host;
  char *user;
  char *passwd;
  char ticket[41]="";
  char starttime_buf[15];
  char user_agent[255];

  /*      Get the Rivendell Host, User and Password if set in env */
  if (getenv("RIVHOST")!=NULL) {
    host = getenv("RIVHOST");
  }
  else {
    host="localhost";
  }

  if (getenv("RIVUSER")!=NULL) {
    user = getenv("RIVUSER");
  }
  else {
    user="USER";
  }

  if (getenv("RIVPASS")!=NULL) {
    passwd = getenv("RIVPASS");
  }
  else {
    passwd = "";
  } 

  printf("Please enter the Log Name ==> ");
  if (fgets(buf,sizeof(buf),stdin) != NULL)
  {
  } 

  // Add the User Agent and Version
  strcat(user_agent,RD_GetUserAgent());
  strcat(user_agent,RD_GetVersion());
  strcat(user_agent," (Test Suite)");
  
  //
  // Call the function
  //
  int result= RD_ListLog(&logline,
		host,
		user,
		passwd,
		ticket,
		buf,
                user_agent,
		&numrecs);
  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    switch(result) {
      case 400:
        fprintf(stderr,"ERROR:  Missing Cart Number! \n");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }
  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("            LogLine Line Number: %d\n",logline[i].logline_line);
    printf("                     LogLine Id: %d\n",logline[i].logline_id);
    printf("                   LogLine Type: %d\n",logline[i].logline_type);
    printf("              LogLine Cart Type: %d\n",logline[i].logline_cart_type);
    printf("            LogLine Cart Number: %u\n",logline[i].logline_cart_number);
    printf("             LogLine Cut Number: %d\n",logline[i].logline_cut_number);
    printf("             LogLine Group Name: %s\n",logline[i].logline_group_name);
    printf("            LogLine Group Color: %s\n",logline[i].logline_group_color);
    printf("                  LogLine Title: %s\n",logline[i].logline_title);
    printf("                 LogLine Artist: %s\n",logline[i].logline_artist);
    printf("                  LogLine Album: %s\n",logline[i].logline_album);
    printf("                   LogLine Year: %d\n",logline[i].logline_year);
    printf("                  LogLine Label: %s\n",logline[i].logline_label);
    printf("                 LogLine Client: %s\n",logline[i].logline_client);
    printf("                 LogLine Agency: %s\n",logline[i].logline_agency);
    printf("              LogLine Publisher: %s\n",logline[i].logline_publisher);
    printf("               LogLine Composer: %s\n",logline[i].logline_composer);
    printf("              LogLine Conductor: %s\n",logline[i].logline_conductor);
    printf("           LogLine User Defined: %s\n",logline[i].logline_user_defined);
    printf("             LogLine Usage Code: %d\n",logline[i].logline_usage_code);
    printf("         LogLine Enforce Length: %d\n",logline[i].logline_enforce_length);
    printf("          LogLine Forced Length: %d\n",logline[i].logline_forced_length);
    printf("              LogLine Evergreen: %d\n",logline[i].logline_evergreen);
    printf("                 LogLine Source: %d\n",logline[i].logline_source);
    printf("              LogLine Time Type: %d\n",logline[i].logline_time_type);
    if(RD_Cnv_msec_to_TString(starttime_buf,14,logline[i].logline_starttime)==0) {
      printf("             LogLine Start Time: %d\n",
	     logline[i].logline_starttime);
    }
    else {
      printf("             LogLine Start Time: %d [%s]\n",
	     logline[i].logline_starttime,starttime_buf);
    }
    printf("        LogLine Transition Type: %d\n",logline[i].logline_transition_type);
    printf("           LogLine Cut Quantity: %d\n",logline[i].logline_cut_quantity);
    printf("        LogLine Last Cut Played: %d\n",logline[i].logline_last_cut_played);
    printf("         LogLine Marker Comment: %s\n",logline[i].logline_marker_comment);
    printf("           LogLine Marker Label: %s\n",logline[i].logline_marker_label);
    printf("            LogLine Origin User: %s\n",logline[i].logline_origin_user);
    printf("        LogLine Origin DateTime\n");
    printf("LogLine OriginDateTime  year value : %d\n",logline[i].logline_origin_datetime.tm_year);
    printf("LogLine OriginDateTime  mon value  : %d\n",logline[i].logline_origin_datetime.tm_mon);
    printf("LogLine OriginDateTime mday value  : %d\n",logline[i].logline_origin_datetime.tm_mday);
    printf("LogLine OriginDateTime   day value : %d\n",logline[i].logline_origin_datetime.tm_wday);
    printf("LogLine OriginDateTime  hour value : %d\n",logline[i].logline_origin_datetime.tm_hour);
    printf("LogLine OriginDateTime  min value  : %d\n",logline[i].logline_origin_datetime.tm_min);
    printf("LogLine OriginDateTime  sec value  : %d\n",logline[i].logline_origin_datetime.tm_sec);
    printf("LogLine OriginDateTime  isdst      : %d\n",logline[i].logline_origin_datetime.tm_isdst);
    printf("       LogLine Start Point Cart: %d\n",logline[i].logline_start_point_cart);
    printf("         LogLineStart Point Log: %d\n",logline[i].logline_start_point_log);
    printf("         LogLine End Point Cart: %d\n",logline[i].logline_end_point_cart);
    printf("          LogLine End Point Log: %d\n",logline[i].logline_end_point_log);
    printf(" LogLine Segue Start Point Cart: %d\n",logline[i].logline_segue_start_point_cart);
    printf("  LogLine Segue Start Point Log: %d\n",logline[i].logline_segue_start_point_log);
    printf("   LogLine Segue End Point Cart: %d\n",logline[i].logline_segue_end_point_cart);
    printf("    LogLine Segue End Point Log: %d\n",logline[i].logline_segue_end_point_log);
    printf("             LogLine Segue Gain: %d\n",logline[i].logline_segue_gain);
    printf("      LogLine FadeUp Point Cart: %d\n",logline[i].logline_fadeup_point_cart);
    printf("       LogLine FadeUp Point Log: %d\n",logline[i].logline_fadeup_point_log);
    printf("            LogLine FadeUp Gain: %d\n",logline[i].logline_fadeup_gain);
    printf("    LogLine FadeDown Point Cart: %d\n",logline[i].logline_fadedown_point_cart);
    printf("     LogLine FadeDown Point Log: %d\n",logline[i].logline_fadedown_point_log);
    printf("            LogLine DuckUp Gain: %d\n",logline[i].logline_duckup_gain);
    printf("          LogLine DuckDown Gain: %d\n",logline[i].logline_duckdown_gain);
    printf("       LogLine Talk Start Point: %d\n",logline[i].logline_talk_start_point);
    printf("         LogLine Talk End Point: %d\n",logline[i].logline_talk_end_point);
    printf("              LogLine Hook Mode: %d\n",logline[i].logline_hook_mode);
    printf("       LogLine Hook Start Point: %d\n",logline[i].logline_hook_start_point);
    printf("         LogLine Hook End Point: %d\n",logline[i].logline_hook_end_point);

    printf("           LogLine Event Length: %d\n",logline[i].logline_event_length);
    printf("        LogLine Link Event Name: %s\n",logline[i].logline_link_event_name);
    printf("        LogLine Link Start Time: %d\n",logline[i].logline_link_starttime);
    printf("        LogLine Link Start Slop: %d\n",logline[i].logline_link_start_slop);
    printf("          LogLine Link End Slop: %d\n",logline[i].logline_link_end_slop);
    printf("                LogLine Link Id: %d\n",logline[i].logline_link_id);
    printf("          LogLine Link Embedded: %d\n",logline[i].logline_link_embedded);
    printf("         LogLine Ext Start Time: %d\n",logline[i].logline_ext_starttime);
    printf("             LogLine Ext Length: %d\n",logline[i].logline_ext_length);
    printf("          LogLine Ext Cart Name: %s\n",logline[i].logline_ext_cart_name);
    printf("               LogLine Ext Data: %s\n",logline[i].logline_ext_data);
    printf("           LogLine Ext Event Id: %s\n",logline[i].logline_ext_event_id);
    printf("          LogLine Ext Annc Type: %s\n",logline[i].logline_ext_annc_type);

    printf("\n");
  }

  //
  // Free the Log Line list when finished with it
  //
  free(logline);

// Add test of create_ticket function 
    
    struct rd_ticketinfo *myticket=0;
    numrecs=0;

    result = RD_CreateTicket( &myticket,
            host,
            user,
            passwd,
            user_agent,
            &numrecs);

    if ((result< 200 || result > 299) &&
       (result != 0))
    {
        switch(result) {
            case 403:
            fprintf(stderr," ERROR: Invalid User Information During Create Ticket\n");
            break;
        default:
           fprintf(stderr, "Unknown Error occurred ==> %d\n",result);
        }
    exit(256);
    }

    //   We got a ticket created - use it and do the call again
    //
  // List the Results
  //
  for(i=0;i<numrecs;i++) {
    printf("          Ticket: %s\n",myticket[i].ticket);
    printf("Ticket Expire year value  = %d\n",myticket->tkt_expiration_datetime.tm_year);
    printf("Ticket Expire month value = %d\n",myticket->tkt_expiration_datetime.tm_mon);
    printf("Ticket Expire day value   = %d\n",myticket->tkt_expiration_datetime.tm_mday);
    printf("Ticket Expire wday value  = %d\n",myticket->tkt_expiration_datetime.tm_wday);
    printf("Ticket Expire hour value  = %d\n",myticket->tkt_expiration_datetime.tm_hour);
    printf("Ticket Expire min value   = %d\n",myticket->tkt_expiration_datetime.tm_min);
    printf("Ticket Expire sec value   = %d\n",myticket->tkt_expiration_datetime.tm_sec);
    printf("Ticket Expire isdst value = %d\n",myticket->tkt_expiration_datetime.tm_isdst);
    printf("\n");

  }

    user="";
    passwd="";
    strcpy( ticket,myticket->ticket);
    fprintf(stderr, "Ticket was copied - = %s\n",ticket);
  //
  // Call the function
  //
  result= RD_ListLog(&logline,
		host,
		user,
		passwd,
		ticket,
		buf,
                user_agent,
		&numrecs);
  if(result<0) {
    fprintf(stderr,"Error: Web function Failure!\n");
    exit(256);
  }

  if ((result< 200 || result > 299) &&
       (result != 0))
  {
    switch(result) {
      case 400:
        fprintf(stderr,"ERROR:  Missing Cart Number! \n");
        break;
      case 404:
        fprintf(stderr,"ERROR:  No Such Cart Exists! \n");
        break;
      default:
        fprintf(stderr, "Unknown Error occurred ==> %d",result);
    }
    exit(256);
  }
  //
  // List the results
  //
  for(i=0;i<numrecs;i++) {
    printf("            LogLine Line Number: %d\n",logline[i].logline_line);
    printf("                     LogLine Id: %d\n",logline[i].logline_id);
    printf("                   LogLine Type: %d\n",logline[i].logline_type);
    printf("              LogLine Cart Type: %d\n",logline[i].logline_cart_type);
    printf("            LogLine Cart Number: %u\n",logline[i].logline_cart_number);
    printf("             LogLine Cut Number: %d\n",logline[i].logline_cut_number);
    printf("             LogLine Group Name: %s\n",logline[i].logline_group_name);
    printf("            LogLine Group Color: %s\n",logline[i].logline_group_color);
    printf("                  LogLine Title: %s\n",logline[i].logline_title);
    printf("                 LogLine Artist: %s\n",logline[i].logline_artist);
    printf("                  LogLine Album: %s\n",logline[i].logline_album);
    printf("                   LogLine Year: %d\n",logline[i].logline_year);
    printf("                  LogLine Label: %s\n",logline[i].logline_label);
    printf("                 LogLine Client: %s\n",logline[i].logline_client);
    printf("                 LogLine Agency: %s\n",logline[i].logline_agency);
    printf("              LogLine Publisher: %s\n",logline[i].logline_publisher);
    printf("               LogLine Composer: %s\n",logline[i].logline_composer);
    printf("              LogLine Conductor: %s\n",logline[i].logline_conductor);
    printf("           LogLine User Defined: %s\n",logline[i].logline_user_defined);
    printf("             LogLine Usage Code: %d\n",logline[i].logline_usage_code);
    printf("         LogLine Enforce Length: %d\n",logline[i].logline_enforce_length);
    printf("          LogLine Forced Length: %d\n",logline[i].logline_forced_length);
    printf("              LogLine Evergreen: %d\n",logline[i].logline_evergreen);
    printf("                 LogLine Source: %d\n",logline[i].logline_source);
    printf("              LogLine Time Type: %d\n",logline[i].logline_time_type);
    if(RD_Cnv_msec_to_TString(starttime_buf,14,logline[i].logline_starttime)==0) {
      printf("             LogLine Start Time: %d\n",
	     logline[i].logline_starttime);
    }
    else {
      printf("             LogLine Start Time: %d [%s]\n",
	     logline[i].logline_starttime,starttime_buf);
    }
    printf("        LogLine Transition Type: %d\n",logline[i].logline_transition_type);
    printf("           LogLine Cut Quantity: %d\n",logline[i].logline_cut_quantity);
    printf("        LogLine Last Cut Played: %d\n",logline[i].logline_last_cut_played);
    printf("         LogLine Marker Comment: %s\n",logline[i].logline_marker_comment);
    printf("           LogLine Marker Label: %s\n",logline[i].logline_marker_label);
    printf("            LogLine Origin User: %s\n",logline[i].logline_origin_user);
    printf("        LogLine Origin DateTime\n");
    printf("LogLine OriginDateTime  year value : %d\n",logline[i].logline_origin_datetime.tm_year);
    printf("LogLine OriginDateTime  mon value  : %d\n",logline[i].logline_origin_datetime.tm_mon);
    printf("LogLine OriginDateTime mday value  : %d\n",logline[i].logline_origin_datetime.tm_mday);
    printf("LogLine OriginDateTime   day value : %d\n",logline[i].logline_origin_datetime.tm_wday);
    printf("LogLine OriginDateTime  hour value : %d\n",logline[i].logline_origin_datetime.tm_hour);
    printf("LogLine OriginDateTime  min value  : %d\n",logline[i].logline_origin_datetime.tm_min);
    printf("LogLine OriginDateTime  sec value  : %d\n",logline[i].logline_origin_datetime.tm_sec);
    printf("LogLine OriginDateTime  isdst      : %d\n",logline[i].logline_origin_datetime.tm_isdst);
    printf("       LogLine Start Point Cart: %d\n",logline[i].logline_start_point_cart);
    printf("         LogLineStart Point Log: %d\n",logline[i].logline_start_point_log);
    printf("         LogLine End Point Cart: %d\n",logline[i].logline_end_point_cart);
    printf("          LogLine End Point Log: %d\n",logline[i].logline_end_point_log);
    printf(" LogLine Segue Start Point Cart: %d\n",logline[i].logline_segue_start_point_cart);
    printf("  LogLine Segue Start Point Log: %d\n",logline[i].logline_segue_start_point_log);
    printf("   LogLine Segue End Point Cart: %d\n",logline[i].logline_segue_end_point_cart);
    printf("    LogLine Segue End Point Log: %d\n",logline[i].logline_segue_end_point_log);
    printf("             LogLine Segue Gain: %d\n",logline[i].logline_segue_gain);
    printf("      LogLine FadeUp Point Cart: %d\n",logline[i].logline_fadeup_point_cart);
    printf("       LogLine FadeUp Point Log: %d\n",logline[i].logline_fadeup_point_log);
    printf("            LogLine FadeUp Gain: %d\n",logline[i].logline_fadeup_gain);
    printf("    LogLine FadeDown Point Cart: %d\n",logline[i].logline_fadedown_point_cart);
    printf("     LogLine FadeDown Point Log: %d\n",logline[i].logline_fadedown_point_log);
    printf("            LogLine DuckUp Gain: %d\n",logline[i].logline_duckup_gain);
    printf("          LogLine DuckDown Gain: %d\n",logline[i].logline_duckdown_gain);
    printf("       LogLine Talk Start Point: %d\n",logline[i].logline_talk_start_point);
    printf("         LogLine Talk End Point: %d\n",logline[i].logline_talk_end_point);
    printf("              LogLine Hook Mode: %d\n",logline[i].logline_hook_mode);
    printf("       LogLine Hook Start Point: %d\n",logline[i].logline_hook_start_point);
    printf("         LogLine Hook End Point: %d\n",logline[i].logline_hook_end_point);

    printf("           LogLine Event Length: %d\n",logline[i].logline_event_length);
    printf("        LogLine Link Event Name: %s\n",logline[i].logline_link_event_name);
    printf("        LogLine Link Start Time: %d\n",logline[i].logline_link_starttime);
    printf("        LogLine Link Start Slop: %d\n",logline[i].logline_link_start_slop);
    printf("          LogLine Link End Slop: %d\n",logline[i].logline_link_end_slop);
    printf("                LogLine Link Id: %d\n",logline[i].logline_link_id);
    printf("          LogLine Link Embedded: %d\n",logline[i].logline_link_embedded);
    printf("         LogLine Ext Start Time: %d\n",logline[i].logline_ext_starttime);
    printf("             LogLine Ext Length: %d\n",logline[i].logline_ext_length);
    printf("          LogLine Ext Cart Name: %s\n",logline[i].logline_ext_cart_name);
    printf("               LogLine Ext Data: %s\n",logline[i].logline_ext_data);
    printf("           LogLine Ext Event Id: %s\n",logline[i].logline_ext_event_id);
    printf("          LogLine Ext Annc Type: %s\n",logline[i].logline_ext_annc_type);

    printf("\n");
  }

  //
  // Free the Log Line list when finished with it
  //
  free(logline);

  exit(0);
}
