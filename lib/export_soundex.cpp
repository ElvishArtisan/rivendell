// export_soundex.cpp
//
// Export a Rivendell SoundExchange Report to an ASCII Text File.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: export_soundex.cpp,v 1.9 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>

#include <qfile.h>
#include <qmessagebox.h>
#include <rddb.h>
#include <rdlog_line.h>
#include <rdairplay_conf.h>
#include <rddatedecode.h>
#include <rdreport.h>
#include <rdget_ath.h>


bool RDReport::ExportSoundEx(const QDate &startdate,const QDate &enddate,
			     const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  unsigned cartnum=0;
  QString artist;
  QString title;
  QString isrc;
  QString album;
  QString label;
  unsigned plays=0;
  QString service_name=serviceName();
  QString trans_category=stationFormat();
  QString channel_name=stationId();

#ifdef WIN32
  QString filename=RDDateDecode(exportPath(RDReport::Windows),startdate);
#else
  QString filename=RDDateDecode(exportPath(RDReport::Linux),startdate);
#endif

  //
  // Get ATH Value
  //
  double ath=0.0;
  RDGetAth *getath=new RDGetAth(&ath);
  if(getath->exec()<0) {
    report_error_code=RDReport::ErrorCanceled;
    return false;
  }

  if((f=fopen((const char *)filename,"w"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }

  //
  // Generate Header
  //
  fprintf(f,"\"NAME_OF_SERVICE\",\"TRANSMISSION_CATEGORY\",\"FEATURED_ARTIST\",\"SOUND_RECORDING_TITLE\",\"ISRC\",\"ALBUM_TITLE\",\"MARKETING_LABEL\",\"ACTUAL_TOTAL_PERFORMANCES\",\"AGGREGATE_TUNING_HOURS\",\"CHANNEL_OR_PROGRAM_NAME\",\"PLAY_FREQUENCY\"\r\n");

  //
  // Roll Up Records
  //
  sql=QString().sprintf("select CART_NUMBER,ARTIST,TITLE,ISRC,ALBUM,LABEL \
                         from `%s_SRT` order by CART_NUMBER",
			(const char *)mixtable);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toUInt()==cartnum) {
      plays++;
    }
    else {
      if(cartnum!=0) {
	fprintf(f,
	 "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",,%9.2lf,\"%s\",%d\n",
		(const char *)service_name,
		(const char *)trans_category,
		(const char *)artist,
		(const char *)title,
		(const char *)isrc,
		(const char *)album,
		(const char *)label,
		ath,
		(const char *)channel_name,
		plays);
      }
      plays=1;
      if(q->value(1).isNull()) {
	artist="";
      }
      else {
	artist=q->value(1).toString();
      }
      title=q->value(2).toString();
      if(q->value(3).isNull()) {
	isrc="";
      }
      else {
	isrc=q->value(3).toString();
      }
      if(q->value(4).isNull()) {
	album="";
      }
      else {
	album=q->value(4).toString();
      }
      if(q->value(5).isNull()) {
	label="";
      }
      else {
	label=q->value(5).toString();
      }
    }
    cartnum=q->value(0).toUInt();
  }
  delete q;
  if(cartnum!=0) {
    fprintf(f,
	"\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",,%9.2lf,\"%s\",%d\n",
	    (const char *)service_name,
	    (const char *)trans_category,
	    (const char *)artist,
	    (const char *)title,
	    (const char *)isrc,
	    (const char *)album,
	    (const char *)label,
	    ath,
	    (const char *)channel_name,
	    plays);
  }
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

