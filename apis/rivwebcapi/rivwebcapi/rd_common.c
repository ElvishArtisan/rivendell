/* rd_common.c
 *
 * Common Utility Functions for the Rivendell Access Library
 *
 * (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include "rd_common.h"

unsigned RD_ReadBool(const char *val)
{
  if((strcasecmp(val,"true")==0)||(strcasecmp(val,"yes")==0)||
     (strcasecmp(val,"on")==0)) {
    return 1;
  }
  else {
    return 0;
  }
}

/*******************************************************************
 *   RD_Cnv_DTString_to_tm
 *     Take a DateTime String and converts to a tm struct.
 *     Assumes a Z means Zulu (GMT) Always returns localtime
 *     in tm struct pointer. Returns null tm on invalid date length
*******************************************************************/
struct tm RD_Cnv_DTString_to_tm( const char *datein)
{
    struct tm datetimetm = {0};
    struct tm *tmptr;
    time_t rawtime;
    time_t newrawtime;
    char theyear[5];
    char themonth[3];
    char theday[3];
    char thehr[3];
    char themin[3];
    char thesec[3];
    char plusminusz[2];
    char offsethr[3];
    char offsetmin[3];
    double offsetfromutc = 0;
    int offhr = 0;
    int offmin = 0;
    int val;

    tmptr = &datetimetm;

    if ((strlen(datein) < 19)  ||   // INVALID DATE
        (strlen(datein) > 26) )  
        return datetimetm;

    strlcpy(theyear,datein,4);
    strlcpy(themonth,datein+5,2);
    strlcpy(theday,datein+8,2);
    strlcpy(thehr,datein+11,2);
    strlcpy(themin,datein+14,2);
    strlcpy(thesec,datein+17,2);
    if (strlen(datein) > 19)  
    {
        strlcpy(plusminusz,datein+19,1);
        if ( (( strcmp(plusminusz,"+"))==0) ||
             (( strcmp(plusminusz,"-"))==0)  )
        {
    	    strlcpy(offsethr,datein+20,2);
    	    strlcpy(offsetmin,datein+23,2);
        }
    }
    val = atoi(theyear) - 1900;
    tmptr->tm_year = val;
    val = atoi(themonth) - 1;
    tmptr->tm_mon  = val;
    tmptr->tm_mday = atoi(theday);
    tmptr->tm_hour = atoi(thehr);
    tmptr->tm_min  = atoi(themin);
    tmptr->tm_sec  = atoi(thesec);
    /*  This is to fix if Windows Assumes apply DST */
    #ifdef _WIN32   
        tmptr->tm_isdst = -1;
    #endif     
    rawtime = mktime (tmptr);  
    newrawtime = rawtime; 
    if ((strcmp(plusminusz,"Z")) != 0)
    {
        if (strlen(datein)==25)
        {
            offhr = atoi(offsethr);
            offmin = atoi(offsetmin);
        }
        if ((strcmp(plusminusz,"-"))== 0)
        {
            newrawtime = rawtime + ((offhr * 3600) +
                (offmin * 60));
        }
        else 
        {
            newrawtime = rawtime - ((offhr * 3600) -
                (offmin * 60));
        }
    }
//    Get the local offset from UTC
    offsetfromutc = get_local_offset();
    newrawtime += offsetfromutc;
#ifdef _WIN32
    tmptr = localtime(&newrawtime);
#else
    localtime_r(&newrawtime,tmptr);
#endif
    return datetimetm;
}
    
/****************************************************************
 *   strlcpy
 *     Copies a strng buffer into a string buffer with size bufsiz
 *     Returns size_t length copied. Null Terminates Automatically
*****************************************************************/
size_t strlcpy(char * dest, const char* src, size_t bufsize)
{
    size_t srclen = strlen(src);
    size_t len;
    if (bufsize == 0)
        return bufsize;
    len = (bufsize < srclen) ? bufsize : srclen;
    memcpy(dest,src,len);
    dest[len]='\0';
    return len;
}

/****************************************************************
 *   RD_Cnv_tm_to_DTString
 *     Copies tm struct values into a date string. Returns 
 *     size_t length copied. Returns 0 if invalid tm struct.
 *     Automatically URL Encodes colon and the plus sign.
*****************************************************************/
size_t RD_Cnv_tm_to_DTString(struct tm *tmptr,char * dest)
{
    char theyear[5];
    char themonth[3];
    char theday[3];
    char thehr[3];
    char themin[3];
    char thesec[3];
    char plusminus[3];
    char minus[2] = "-";
    char colon_sep[4] = "%3a";
    char T_sep[2] = "T";
    char offsethr[3];
    char offsetmin[3];
    double offsetfromutc;
    int offhr = 0;
    int offmin = 0;
    int val;
    int leap = 0;
     
    if (!validate_tm(tmptr))
    {
        dest[0]=0;
        return 0;
    }
    offsetfromutc = get_local_offset();
    if (offsetfromutc > 0)
        strlcpy(plusminus,"%2b",3);
    else
        strlcpy(plusminus,"-",1);
    offhr = (int) (offsetfromutc / 3600);
    double hold_min;
    double divby = 3600;
    hold_min = fmod(offsetfromutc,divby);
    offmin = (int) (hold_min / 60);
    sprintf(offsethr,"%02d",abs(offhr));
    sprintf(offsetmin,"%02d",abs(offmin));
    val = tmptr->tm_year + 1900;
    sprintf(theyear,"%d",val);
    val = tmptr->tm_mon + 1;
    sprintf(themonth,"%02d",val);
    sprintf(theday,"%02d",tmptr->tm_mday);
    sprintf(thehr,"%02d",tmptr->tm_hour);
    sprintf(themin,"%02d",tmptr->tm_min);
    sprintf(thesec,"%02d",tmptr->tm_sec);
    strlcpy(dest,theyear,4);
    strcat(dest, minus);
    strcat(dest,themonth);
    strcat(dest, minus);
    strcat(dest,theday);
    strcat(dest,T_sep);
    strcat(dest,thehr);
    strcat(dest,colon_sep);
    strcat(dest,themin);
    strcat(dest,colon_sep);
    strcat(dest,thesec);
    strcat(dest,plusminus);
    strcat(dest,offsethr);
    strcat(dest,colon_sep);
    strcat(dest,offsetmin);
return strlen(dest);
}

/****************************************************************
 *   validate tm
 *     Validates a tm struct YYYY-MM-DD hh:mm:ss
 *     Returns 1 if successful 0 if error
****************************************************************/
int validate_tm (struct tm *tmptr)
{
    int monthdays[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    int val;
    int leap = 0;

    val = tmptr->tm_year + 1900;
    if ((val < 1900) || (val > 9999) )
        return 0;
    if ( (tmptr->tm_mon < 0) ||
         (tmptr->tm_mon > 11))
        return 0;
    if ((tmptr->tm_mday == 29 ) && (tmptr->tm_mon == 1))
    {
        if ( (val % 400) == 0 )
            leap = 1;
        else if ((val %100) == 0)
            leap = 0;
        else if ((val % 4) == 0) 
            leap = 1;
        else
            leap = 0;
        if (!leap) 
            return 0;
    }
    if ( (tmptr->tm_mday > monthdays[tmptr->tm_mon]) ||
         (tmptr->tm_mday <=0) )
        return 0;
    if ( (tmptr->tm_sec > 61) ||
         (tmptr->tm_sec < 0))
        return 0;
    if ( (tmptr->tm_min > 59) ||
         (tmptr->tm_min < 0))
        return 0;
    if ( (tmptr->tm_hour > 23) ||
         (tmptr->tm_hour < 0))
        return 0;
return 1;
}

/****************************************************************
 *   get_local_offset
 *     returns local UTC Offset
 *     Returns 1 if successful 0 if error
****************************************************************/
double get_local_offset()
{
    time_t currtime;
    struct tm * timeinfo;
    time( &currtime );
    timeinfo = gmtime (&currtime);
    time_t utc = mktime (timeinfo);
    timeinfo = localtime(&currtime);
    time_t local = mktime(timeinfo);
    double offsetfromutc = difftime(local,utc);
    #ifdef _WIN32
        if (timeinfo->tm_isdst)
            offsetfromutc += 3600;
    #endif
    return offsetfromutc;
}


/****************************************************************
 *   RD_Cnv_TString_to_msec
 *     Converts a relative-to-midnight start time string ("HH:MM:SS.ZZZ")
 *     and returns it as milliseconds after midnight, or -1 if
 *     the conversion fails.
*****************************************************************/
int RD_Cnv_TString_to_msec(const char *str)
{
  int hour;
  int minute;
  int second;
  int tenth;

  if(strlen(str)<10) {
    return -1;
  }
  if(sscanf(str,"%02d",&hour)!=1) {
    return -1;
  }
  if((hour<0)||(hour>23)) {
    return -1;
  }
  if(sscanf(str+3,"%02d",&minute)!=1) {
    return -1;
  }
  if((minute<0)||(minute>59)) {
    return -1;
  }
  if(sscanf(str+6,"%02d",&second)!=1) {
    return -1;
  }
  if((second<0)||(second>59)) {
    return -1;
  }
  if(sscanf(str+9,"%1d",&tenth)!=1) {
    return -1;
  }
  if((tenth<0)||(tenth>9)) {
    return -1;
  }

  return 3600000*hour+60000*minute+1000*second+100*tenth;
}


/****************************************************************
 *   RD_Cnv_msec_to_TString
 *     Converts a milliseconds after midnight integer to
 *     time string ("HH:MM:SS.ZZZ"), returning the length
 *     of the string.
*****************************************************************/
size_t RD_Cnv_msec_to_TString(char *str, size_t len,int msec)
{
  int hour;
  int minute;
  int second;
  int tenth;

  if(msec<0) {
    str[len]=0;
    return 0;
  }
  hour=msec/3600000;
  minute=(msec-3600000*hour)/60000;
  second=(msec-3600000*hour-60000*minute)/1000;
  tenth=(msec-3600000*hour-60000*minute-1000*second)/100;

  snprintf(str,len,"%02d:%02d:%02d.%1d",hour,minute,second,tenth);

  return strlen(str);
}
