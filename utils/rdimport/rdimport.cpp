// rdimport.cpp
//
// A Batch Importer for Rivendell.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <limits.h>
#include <math.h>
#include <sched.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <qapplication.h>
#include <qdir.h>
#include <qstringlist.h>

#include <rd.h>
#include <rdaudioimport.h>
#include <rdconfig.h>
#include <rdconf.h>
#include <rdcut.h>
#include <rddatedecode.h>
#include <rddisclookup.h>
#include <rdescape_string.h>
#include <rdlibrary_conf.h>
#include <rdtempdirectory.h>

#include "rdimport.h"

volatile bool import_run=true;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
  case SIGINT:
  case SIGHUP:
    import_run=false;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  bool ok=false;
  int n=0;
  QString err_msg;

  import_file_key=0;
  import_group=NULL;
  import_verbose=false;
  import_log_syslog=false;
  import_log_file=false;
  import_log_filename="";
  import_single_cart=false;
  import_use_cartchunk_cutid=false;
  import_cart_number_offset=0;
  import_cart_number=0;
  import_title_from_cartchunk_cutid=false;
  import_delete_source=false;
  import_delete_cuts=false;
  import_drop_box=false;
  import_set_user_defined="";
  import_stdin_specified=false;
  import_startdate_offset=0;
  import_enddate_offset=0;
  import_fix_broken_formats=false;
  import_persistent_dropbox_id=-1;
  import_create_dates=false;
  import_create_startdate_offset=0;
  import_create_enddate_offset=0;
  import_string_bpm=0;
  import_string_year=0;
  import_clear_datetimes=false;
  import_clear_dayparts=false;
  import_xml=false;
  import_to_mono=false;
  import_failed_imports=0;
  import_send_mail=false;
  import_mail_per_file=false;
  import_journal=NULL;

  //
  // Open the Database
  //
  rda=new RDApplication("rdimport","rdimport",RDIMPORT_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdimport: %s\n",(const char *)err_msg);
    ErrorExit(RDApplication::ExitNoDb);
  }

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()<2) {
    Log(LOG_ERR,QString().sprintf("\n%s\n",RDIMPORT_USAGE));
    ErrorExit(RDApplication::ExitInvalidOption);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys()-2;i++) {
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      import_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--log-syslog") {
      import_log_syslog=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--log-filename") {
      import_log_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--to-cart") {
      import_cart_number=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(import_cart_number<1)||(import_cart_number>999999)) {
	Log(LOG_ERR,QString("rdimport: invalid cart number\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      if(import_use_cartchunk_cutid) {
	Log(LOG_ERR,QString("rdimport: '--to-cart' and '--use-cartchunk-cutid' are mutually exclusive\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      import_single_cart=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--use-cartchunk-cutid") {
      if(import_cart_number!=0) {
	Log(LOG_ERR,QString("rdimport: '--to-cart' and '--use-cartchunk-cutid' are mutually exclusive\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      import_use_cartchunk_cutid=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cart-number-offset") {
      import_cart_number_offset=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid --cart-number-offset\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--title-from-cartchunk-cutid") {
      import_title_from_cartchunk_cutid=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--delete-source") {
      import_delete_source=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--delete-cuts") {
      import_delete_cuts=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--startdate-offset") {
      import_startdate_offset=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid startdate-offset\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--enddate-offset") {
      import_enddate_offset=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid enddate-offset\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--clear-datetimes") {
      import_clear_datetimes=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-datetimes") {
      QStringList f0=rda->cmdSwitch()->value(i).split(",");
      if(f0.size()!=2) {
	Log(LOG_ERR,QString("rdimport: invalid argument to --set-datetimes\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      for(unsigned j=0;j<2;j++) {
	if((f0[j].length()!=15)||(f0[j].mid(8,1)!="-")) {
	  Log(LOG_ERR,QString("rdimport: invalid argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned year=f0[j].left(4).toUInt(&ok);
	if(!ok) {
	  Log(LOG_ERR,QString("rdimport: invalid year argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned month=f0[j].mid(4,2).toUInt(&ok);
	if((!ok)||(month>12)) {
	  Log(LOG_ERR,QString("rdimport: invalid month argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned day=f0[j].mid(6,2).toUInt(&ok);
	if((!ok)||(day>31)) {
	  Log(LOG_ERR,QString("rdimport: invalid day argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned hour=f0[j].mid(9,2).toUInt(&ok);
	if((!ok)||(hour>23)) {
	  Log(LOG_ERR,QString("rdimport: invalid hour argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned min=f0[j].mid(11,2).toUInt(&ok);
	if((!ok)||(min>59)) {
	  Log(LOG_ERR,QString("rdimport: invalid minute argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned sec=f0[j].right(2).toUInt(&ok);
	if((!ok)||(sec>59)) {
	  Log(LOG_ERR,QString("rdimport: invalid seconds argument to --set-datetimes\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	import_datetimes[j]=QDateTime(QDate(year,month,day),
				      QTime(hour,min,sec));
	if(!import_datetimes[j].isValid()) {
	  Log(LOG_ERR,QString("rdimport: invalid argument to --set-datetimes\n"));
	}
      }
      if(import_datetimes[0]>=import_datetimes[1]) {
	Log(LOG_ERR,QString("rdimport: datetime cannot end before it begins\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-daypart-times") {
      QStringList f0=rda->cmdSwitch()->value(i).split(",");
      if(f0.size()!=2) {
	Log(LOG_ERR,QString("rdimport: invalid argument to --set-daypart-times\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      for(unsigned j=0;j<2;j++) {
	if(f0[j].length()!=6) {
	  Log(LOG_ERR,QString("rdimport: invalid argument to --set-daypart-times\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned hour=f0[j].left(2).toUInt(&ok);
	if((!ok)||(hour>23)) {
	  Log(LOG_ERR,QString("rdimport: invalid hour argument to --set-daypart-times\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned min=f0[j].mid(2,2).toUInt(&ok);
	if((!ok)||(min>59)) {
	  Log(LOG_ERR,QString("rdimport: invalid minute argument to --set-daypart-times\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	unsigned sec=f0[j].right(2).toUInt(&ok);
	if((!ok)||(sec>59)) {
	  Log(LOG_ERR,QString("rdimport: invalid seconds argument to --set-daypart-times\n"));
	  ErrorExit(RDApplication::ExitInvalidOption);
	}
	import_dayparts[j].setHMS(hour,min,sec);
      }
      if(import_dayparts[0]>=import_dayparts[1]) {
	Log(LOG_ERR,QString("rdimport: daypart cannot end before it begins\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--to-mono") {
      import_to_mono=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--clear-daypart-times") {
      import_clear_dayparts=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--drop-box") {
      import_drop_box=true;
      if(import_persistent_dropbox_id<0) {
	import_delete_source=true;
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--add-scheduler-code") {
      import_add_scheduler_codes.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-user-defined") {
      import_set_user_defined=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--metadata-pattern") {
      import_metadata_pattern=rda->cmdSwitch()->value(i);
      if(!VerifyPattern(import_metadata_pattern)) {
	Log(LOG_ERR,QString("rdimport: invalid --metadata-pattern\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--output-pattern") {
      import_output_pattern=rda->cmdSwitch()->value(i);
      if(!VerifyPattern(import_output_pattern)) {
	Log(LOG_ERR,QString("rdimport: invalid --output-pattern\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--fix-broken-formats") {
      import_fix_broken_formats=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--persistent-dropbox-id") {
      import_persistent_dropbox_id=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid persistent dropbox id\n"));
	ErrorExit(RDApplication::ExitNoDropbox);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--create-startdate-offset") {
      import_create_startdate_offset=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid create-startddate-offset\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      import_create_dates=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--create-enddate-offset") {
      import_create_enddate_offset=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok) || 
         (import_create_startdate_offset > import_create_enddate_offset )) {
	Log(LOG_ERR,QString("rdimport: invalid create-enddate-offset\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      import_create_dates=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-agency") {
      import_string_agency=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-album") {
      import_string_album=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-artist") {
      import_string_artist=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-bpm") {
      import_string_bpm=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid value for --set-string-bpm\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-client") {
      import_string_client=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-composer") {
      import_string_composer=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-conductor") {
      import_string_conductor=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-description") {
      import_string_description=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-label") {
      import_string_label=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-outcue") {
      import_string_outcue=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-isci") {
      import_string_isci=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-isrc") {
      if(RDDiscLookup::isrcIsValid(rda->cmdSwitch()->value(i))) {
	import_string_isrc=rda->cmdSwitch()->value(i);
	rda->cmdSwitch()->setProcessed(i,true);
      }
      else {
	Log(LOG_ERR,"invalid ISRC \""+rda->cmdSwitch()->value(i)+"\"\n");
	ErrorExit(RDApplication::ExitInvalidOption);
      }
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-recording-mbid") {
      import_string_recording_mbid=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-release-mbid") {
      import_string_release_mbid=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-publisher") {
      import_string_publisher=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-song-id") {
      import_string_song_id=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-title") {
      if(rda->cmdSwitch()->value(i).isEmpty()) {
	Log(LOG_ERR,QString("rdimport: title field cannot be empty\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      import_string_title=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-user-defined") {
      import_string_user_defined=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-string-year") {
      import_string_year=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	Log(LOG_ERR,QString("rdimport: invalid value for --set-string-year\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--xml") {
      import_xml=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--send-mail") {
      import_send_mail=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--mail-per-file") {
      import_mail_per_file=true;
      import_send_mail=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
  }

  //
  // Sanity Checks
  //
  if(import_datetimes[0].isValid()&&import_clear_datetimes) {
    Log(LOG_ERR,QString("rdimport: --set-datetimes and --clear-datetimes are mutually exclusive\n"));
    ErrorExit(RDApplication::ExitInvalidOption);
  }
  if((!import_dayparts[1].isNull())&&import_clear_dayparts) {
    Log(LOG_ERR,QString("rdimport: --set-daypart-times and --clear-daypart-times are mutually exclusive\n"));
    ErrorExit(RDApplication::ExitInvalidOption);
  }
  if((!import_metadata_pattern.isEmpty())&&import_xml) {
    Log(LOG_ERR,QString().sprintf("rdimport: --metadata-pattern and --xml are mutually exclusive\n"));
    ErrorExit(RDApplication::ExitInvalidOption);
  }
  if((!import_log_filename.isEmpty())&&import_log_syslog) {
    Log(LOG_ERR,QString().sprintf("rdimport: --log-filename and --log-syslog are mutually exclusive\n"));
    ErrorExit(RDApplication::ExitInvalidOption);
  }

  import_cut_markers=new MarkerSet();
  import_cut_markers->loadMarker(rda->cmdSwitch(),"cut");
  import_talk_markers=new MarkerSet();
  import_talk_markers->loadMarker(rda->cmdSwitch(),"talk");
  import_hook_markers=new MarkerSet();
  import_hook_markers->loadMarker(rda->cmdSwitch(),"hook");
  import_segue_markers=new MarkerSet();
  import_segue_markers->loadMarker(rda->cmdSwitch(),"segue");
  import_fadedown_marker=new MarkerSet();
  import_fadedown_marker->loadFade(rda->cmdSwitch(),"fadedown");
  import_fadeup_marker=new MarkerSet();
  import_fadeup_marker->loadFade(rda->cmdSwitch(),"fadeup");

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Verify Group
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i).left(2)!="--") {
      import_group=new RDGroup(rda->cmdSwitch()->key(i));
      if(!import_group->exists()) {
	Log(LOG_ERR,QString().sprintf("rdimport: invalid group specified\n"));
	delete import_group;
	ErrorExit(RDApplication::ExitNoGroup);
      }
      import_file_key=i+1;
      i=rda->cmdSwitch()->keys();
    }
  }
  if(import_group==NULL) {
    Log(LOG_ERR,QString().sprintf("rdimport: invalid group specified\n"));
    ErrorExit(RDApplication::ExitNoGroup);
  }
  if(import_cart_number>0) {
    if(!import_group->cartNumberValid(import_cart_number)) {
      Log(LOG_ERR,QString().sprintf("rdimport: invalid cart number for group\n"));
      delete import_group;
      ErrorExit(RDApplication::ExitInvalidCart);
    }
  }

  //
  // Verify Scheduler Codes
  //
  for(unsigned i=0;i<import_add_scheduler_codes.size();i++) {
    if(!SchedulerCodeExists(import_add_scheduler_codes[i])) {
      Log(LOG_ERR,QString().sprintf("rdimport: scheduler code \"%s\" does not exist\n",
	      (const char *)import_add_scheduler_codes[i].toUtf8()));
      ErrorExit(RDApplication::ExitNoSchedCode);
    }
  }

  //
  // Get Audio Parameters
  //
  import_format=rda->libraryConf()->defaultFormat();
  import_samprate=rda->system()->sampleRate();
  import_bitrate=rda->libraryConf()->defaultBitrate();
  import_channels=rda->libraryConf()->defaultChannels();
  import_normalization_level=rda->libraryConf()->ripperLevel();
  import_autotrim_level=rda->libraryConf()->trimThreshold();
  import_src_converter=rda->libraryConf()->srcConverter();
  import_segue_level=0;
  import_segue_length=0;

  for(unsigned i=0;i<rda->cmdSwitch()->keys()-2;i++) {
    if(rda->cmdSwitch()->key(i)=="--normalization-level") {
      n=rda->cmdSwitch()->value(i).toInt(&ok);
      if(ok&&(n<=0)) {
	import_normalization_level=100*n;
      }
      else {
	Log(LOG_ERR,QString("rdimport: invalid normalization level\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--autotrim-level") {
      n=rda->cmdSwitch()->value(i).toInt(&ok);
      if(ok&&(n<=0)) {
	import_autotrim_level=100*n;
      }
      else {
	Log(LOG_ERR,QString("rdimport: invalid autotrim level\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--segue-level") {
      n=rda->cmdSwitch()->value(i).toInt(&ok);
      if(ok&&(n<=0)) {
	import_segue_level=n;
      }
      else {
	Log(LOG_ERR,QString("rdimport: invalid segue level\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--segue-length") {
      n=rda->cmdSwitch()->value(i).toInt(&ok);
      if(ok&&(n>=0)) {
	import_segue_length=n;
      }
      else {
	Log(LOG_ERR,QString("rdimport: invalid segue length\n"));
	ErrorExit(RDApplication::ExitInvalidOption);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--single-cart") {
      import_single_cart=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if((!rda->cmdSwitch()->processed(i))&&
       (rda->cmdSwitch()->key(i).left(2)=="--")) {
      Log(LOG_ERR,QString().sprintf("rdimport: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i)));
      ErrorExit(RDApplication::ExitInvalidOption);
    }
  }
  if(import_to_mono) {
    import_channels=1;
  }

  //
  // Print Status Messages
  //
  Log(LOG_INFO,QString("rdimport started"));

  Log(LOG_INFO,QString().sprintf("RDImport v%s\n",VERSION));
  if(import_to_mono) {
    Log(LOG_INFO,QString(" Force to Mono is ON\n"));
  }
  else {
    Log(LOG_INFO,QString(" Force to Mono is OFF\n"));
  }
  if(import_normalization_level==0) {
    Log(LOG_INFO,QString(" Normalization is OFF\n"));
  }
  else {
    Log(LOG_INFO,QString().sprintf(" Normalization level = %d dB\n",import_normalization_level/100));
  }
  if(import_autotrim_level==0) {
    Log(LOG_INFO,QString(" AutoTrim is OFF\n"));
  }
  else {
    Log(LOG_INFO,QString().sprintf(" AutoTrim level = %d dB\n",import_autotrim_level/100));
  }
  if(import_cart_number==0) {
    if(import_use_cartchunk_cutid) {
      Log(LOG_INFO,QString(" Destination cart is taken from CartChunk CutID\n"));
    }
    else {
      Log(LOG_INFO,QString(" Destination cart is AUTO\n"));
    }
  }
  else {
    Log(LOG_INFO,QString().sprintf(" Destination cart is %06u\n",import_cart_number));
  }
  if(import_single_cart) {
    Log(LOG_INFO,QString(" Single cart mode is ON\n"));
  }
  else {
    Log(LOG_INFO,QString(" Single cart mode is OFF\n"));
  }
  if(import_title_from_cartchunk_cutid) {
    Log(LOG_INFO,QString(" Destination cart title is taken from CartChunk CutID\n"));
  }
  if(import_cart_number_offset!=0) {
    Log(LOG_INFO,QString().sprintf(" Cart number offset is %d\n",import_cart_number_offset));
  }
  if(import_delete_source) {
    Log(LOG_INFO,QString(" Delete source mode is ON\n"));
  }
  else {
    Log(LOG_INFO,QString(" Delete source mode is OFF\n"));
  }
  if(import_delete_cuts) {
    Log(LOG_INFO,QString(" Delete cuts mode is ON\n"));
  }
  else {
    Log(LOG_INFO,QString(" Delete cuts mode is OFF\n"));
  }
  if(import_drop_box) {
    Log(LOG_INFO,QString(" DropBox mode is ON\n"));
  }
  else {
    Log(LOG_INFO,QString(" DropBox mode is OFF\n"));
  }
  if(import_add_scheduler_codes.size()>0) {
    Log(LOG_INFO,QString(" Adding Scheduler Code(s):\n"));
    for(unsigned i=0;i<import_add_scheduler_codes.size();i++) {
      Log(LOG_INFO,QString().sprintf(" %s\n",(const char *)import_add_scheduler_codes[i].toUtf8()));
    }
  }
  if(!import_set_user_defined.isEmpty()) {
    Log(LOG_INFO,QString().sprintf(" Setting the User Defined field to \"%s\"\n",
				   (const char *)import_set_user_defined));
  }
  if(!import_metadata_pattern.isEmpty()) {
    Log(LOG_INFO,QString().sprintf(" Using metadata pattern: %s\n",
				   (const char *)import_metadata_pattern));
  }
  if(!import_output_pattern.isEmpty()) {
    Log(LOG_INFO,QString().sprintf(" Using output pattern: %s\n",
				   (const char *)import_output_pattern));
  }
  Log(LOG_INFO,QString().sprintf(" Start Date Offset = %d days\n",import_startdate_offset));
  Log(LOG_INFO,QString().sprintf(" End Date Offset = %d days\n",import_enddate_offset));
  if((!import_dayparts[0].isNull())||(!import_dayparts[1].isNull())) {
    Log(LOG_INFO,QString().sprintf(" Start Daypart = %s\n",
				   (const char *)import_dayparts[0].toString("hh:mm:ss")));
    Log(LOG_INFO,QString().sprintf(" End Daypart = %s\n",
				   (const char *)import_dayparts[1].toString("hh:mm:ss")));
  }
  if(import_clear_dayparts) {
    Log(LOG_INFO,QString(" Clearing daypart times\n"));
  }
  if((!import_datetimes[0].isNull())||(!import_datetimes[1].isNull())) {
    Log(LOG_INFO,QString().sprintf(" Start DateTime = %s\n",
				   (const char *)import_datetimes[0].toString("MM/dd/yyyy hh:mm:ss")));
    Log(LOG_INFO,QString().sprintf(" End DateTime = %s\n",
				   (const char *)import_datetimes[1].toString("MM/dd/yyyy hh:mm:ss")));
  }
  if(import_clear_datetimes) {
    Log(LOG_INFO,QString(" Clearing datetimes\n"));
  }
  if(import_fix_broken_formats) {
    Log(LOG_INFO,QString(" Broken format workarounds are ENABLED\n"));
  }
  else {
    Log(LOG_INFO,QString(" Broken format workarounds are DISABLED\n"));
  }
  if(import_send_mail) {
    if(import_mail_per_file) {
      Log(LOG_INFO,QString(" E-mail report per file is ENABLED\n"));
    }
    else {
      Log(LOG_INFO,QString(" Summary e-mail report is ENABLED\n"));
    }
  }
  else {
    Log(LOG_INFO,QString(" E-mail reporting is DISABLED\n"));
  }
  if(import_create_dates) {
    Log(LOG_INFO,QString(" Import Create Dates mode is ON\n"));
    Log(LOG_INFO,QString().sprintf(" Import Create Start Date Offset = %d days\n",import_create_startdate_offset));
    Log(LOG_INFO,QString().sprintf(" Import Create End Date Offset = %d days\n",import_create_enddate_offset));
  }
  else {
    Log(LOG_INFO,QString(" Import Create Dates mode is OFF\n"));
  }
  if(import_persistent_dropbox_id>=0) {
    Log(LOG_INFO,QString().sprintf(" Persistent DropBox ID = %d\n",import_persistent_dropbox_id));
  }
  if(!import_string_agency.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Agency set to: %s\n",(const char *)import_string_agency));
  }
  if(!import_string_album.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Album set to: %s\n",(const char *)import_string_album));
  }
  if(!import_string_artist.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Artist set to: %s\n",(const char *)import_string_artist));
  }
  if(import_string_bpm!=0) {
    Log(LOG_INFO,QString().sprintf(" BPM set to: %d\n",import_string_bpm));
  }
  if(!import_string_client.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Client set to: %s\n",(const char *)import_string_client));
  }
  if(!import_string_composer.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Composer set to: %s\n",(const char *)import_string_composer));
  }
  if(!import_string_conductor.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Conductor set to: %s\n",(const char *)import_string_conductor));
  }
  if(!import_string_description.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Description set to: %s\n",
				   (const char *)import_string_description));
  }
  if(!import_string_label.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Label set to: %s\n",(const char *)import_string_label));
  }
  if(!import_string_outcue.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Outcue set to: %s\n",(const char *)import_string_outcue));
  }
  if(!import_string_isci.isNull()) {
    Log(LOG_INFO,QString().sprintf(" ISCI set to: %s\n",(const char *)import_string_isci));
  }
  if(!import_string_isrc.isNull()) {
    Log(LOG_INFO,QString().sprintf(" ISRC set to: %s\n",(const char *)import_string_isrc));
  }
  if(!import_string_recording_mbid.isNull()) {
    Log(LOG_INFO,QString().sprintf(" MusicBrainz recording ID set to: %s\n",(const char *)import_string_recording_mbid));
  }
  if(!import_string_release_mbid.isNull()) {
    Log(LOG_INFO,QString().sprintf(" MusicBrainz release ID set to: %s\n",(const char *)import_string_release_mbid));
  }
  if(!import_string_publisher.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Publisher set to: %s\n",(const char *)import_string_publisher));
  }
  if(!import_string_song_id.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Song ID set to: %s\n",(const char *)import_string_song_id));
  }
  if(!import_string_title.isNull()) {
    Log(LOG_INFO,QString().sprintf(" Title set to: %s\n",(const char *)import_string_title));
  }
  if(!import_string_user_defined.isNull()) {
    Log(LOG_INFO,QString().sprintf(" User Defined set to: %s\n",
				   (const char *)import_string_user_defined));
  }
  if(import_string_year!=0) {
    Log(LOG_INFO,QString().sprintf(" Year set to: %d\n",import_string_year));
  }
  if(import_xml) {
    Log(LOG_INFO,QString().sprintf(" Importing RDXML metadata from external file\n"));
  }
  import_cut_markers->dump();
  import_talk_markers->dump();
  import_hook_markers->dump();
  import_segue_markers->dump();
  import_fadedown_marker->dump();
  import_fadeup_marker->dump();
  Log(LOG_INFO,QString(" Files to process:\n"));
  for(unsigned i=import_file_key;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="-") {
      if(!import_stdin_specified) {
	Log(LOG_INFO,"   [stdin]\n");
	import_stdin_specified=true;
      }
    }
    else {
      Log(LOG_INFO,QString().sprintf("   \"%s\"\n",
			  (const char *)rda->cmdSwitch()->key(i).toUtf8()));
    }
  }

  //
  // Start the email journal
  //
  import_journal=new Journal(import_mail_per_file);

  // 
  // Setup Signal Handling 
  //
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGINT,SigHandler);
  ::signal(SIGHUP,SigHandler);
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));

  //
  // Verify Permissions
  //
  if(!rda->user()->createCarts()) {
    Log(LOG_ERR,
	QString().sprintf("rdimport: user \"%s\" has no Create Carts permission\n",
			  rda->user()->name().toUtf8().constData()));
    ErrorExit(RDApplication::ExitNoPerms);
  }

  if(import_verbose) {
    printf(" running as user \"%s\"\n",rda->user()->name().toUtf8().constData());
  }

  //
  // Process Files
  //
  if(import_drop_box) {
    RunDropBox();
  }
  else {
    for(unsigned i=import_file_key;i<rda->cmdSwitch()->keys();i++) {
      ProcessFileEntry(rda->cmdSwitch()->key(i));
    }
    if(import_stdin_specified) {
      QTextStream in_stream(stdin,QIODevice::ReadOnly);
      QString line=in_stream.readLine();
      while(!line.isNull()) {
	ProcessFileEntry(line);
	line=in_stream.readLine();
      }
    }
  }

  //
  // Clean Up and Exit
  //
  delete import_group;

  Log(LOG_INFO,QString("rdimport finished\n"));

  NormalExit();
}


void MainObject::RunDropBox()
{
  //
  // Set Process Priority
  //
  struct sched_param sp;
  memset(&sp,0,sizeof(sp));
  if(sched_setscheduler(getpid(),SCHED_BATCH,&sp)!=0) {
    printf(" Unable to set batch permissions, %s",strerror(errno));
  }

  do {
    //
    // Clear the Checked Flag
    //
    for(std::list<struct DropboxList *>::const_iterator 
	  ci=import_dropbox_list.begin();
	ci!=import_dropbox_list.end();ci++) {
      (*ci)->checked=false;
    }

    //
    // Scan for Eligible Imports
    //
    for(unsigned i=import_file_key;i<rda->cmdSwitch()->keys();i++) {
      ProcessFileEntry(rda->cmdSwitch()->key(i));
    }

    //
    // Take Out the Trash
    //
    for(std::list<struct DropboxList *>::iterator 
	  ci=import_dropbox_list.begin();
	ci!=import_dropbox_list.end();ci++) {
      if(!(*ci)->checked) {
	delete *ci;
	import_dropbox_list.erase(ci);
	ci=import_dropbox_list.end();
      }
    }

    sleep(RDIMPORT_DROPBOX_SCAN_INTERVAL);
  } while(import_run);
  Log(LOG_INFO,QString("rdimport stopped\n"));
}


void MainObject::ProcessFileEntry(const QString &entry)
{
  glob_t globbuf;
  int gflags=GLOB_MARK;

  if(entry=="-") {
    import_stdin_specified=true;
    return;
  }
  globbuf.gl_offs=RDIMPORT_GLOB_SIZE;
  while((globbuf.gl_pathc==RDIMPORT_GLOB_SIZE)||(gflags==GLOB_MARK)) {
    glob(RDEscapeString(entry.toUtf8()),gflags,NULL,&globbuf);
    if((globbuf.gl_pathc==0)&&(gflags==GLOB_MARK)&&(!import_drop_box)) {
      Log(LOG_WARNING,QString().sprintf(" Unable to open \"%s\", skipping...\n",
	      (const char *)entry));
      globfree(&globbuf);
    }
    for(size_t i=0;i<globbuf.gl_pathc;i++) {
      if(globbuf.gl_pathv[i][strlen(globbuf.gl_pathv[i])-1]!='/') {
	if(!import_single_cart) {
	  import_cart_number=0;
	}
	if(import_drop_box) {
	  VerifyFile(QString::fromUtf8(globbuf.gl_pathv[i]),&import_cart_number);
	}
	else {
	  //ImportFile(QString::fromUtf8(globbuf.gl_pathv[i]),&import_cart_number);
	  switch(ImportFile(QString::fromUtf8(globbuf.gl_pathv[i]),&import_cart_number)) {
	    case MainObject::Success:
	      break;
	      
 	    case MainObject::DuplicateTitle:
	      break;

	    case MainObject::FileBad:
	      break;
	      
	    case MainObject::NoCart:
	      break;

	    case MainObject::NoCut:
	      break;
	  }
	}
      }
    }
    gflags=GLOB_MARK|GLOB_APPEND;
  }
  globfree(&globbuf);
}


MainObject::Result MainObject::ImportFile(const QString &filename,
					  unsigned *cartnum)
{
  bool found_cart=false;
  bool cart_created=false;
  RDWaveData *wavedata=new RDWaveData();
  RDWaveFile *wavefile=new RDWaveFile(filename);
  RDGroup *effective_group=new RDGroup(import_group->name());
  QString err_msg;
  RDAudioImport::ErrorCode conv_err;
  RDAudioConvert::ErrorCode audio_conv_err;
  QDateTime dt;
  QString sql;
  RDSqlQuery *q=NULL;
  bool ok=false;

  //
  // Open the file
  //
  if(!OpenAudioFile(&wavefile,wavedata)) {
    delete wavedata;
    delete wavefile;
    return MainObject::FileBad;
  }

  //
  // Get file-sourced metadata
  //
  if(!import_metadata_pattern.isEmpty()) {
    QString groupname=effective_group->name();
    found_cart=RunPattern(import_metadata_pattern,RDGetBasePart(filename),
			  wavedata,&groupname);
    if(wavedata->validateDateTimes()) {
      Log(LOG_WARNING,QString().sprintf(
	      " File \"%s\": End date/time cannot be prior to start date/time, ignoring...\n",
	      filename.toUtf8().constData()));
    }

    //
    // Process Group override
    //
    if(groupname!=effective_group->name()) {
      delete effective_group;
      effective_group=new RDGroup(groupname);
      if(!effective_group->exists()) {
	Log(LOG_WARNING,QString().sprintf(" Specified group \"%s\" from file \"%s\" does not exist, using default group...\n",
					  groupname.toUtf8().constData(),
					  filename.toUtf8().constData()));
	delete effective_group;
	effective_group=new RDGroup(import_group->name());
      }
    }
  }

  //
  // Get external XML metadata
  //
  if(import_xml) {
    ReadXmlFile(filename,wavedata);
  }

  //
  // Get CartChunk metadata
  //
  if(import_use_cartchunk_cutid||found_cart) {
    *cartnum=0;
    Log(LOG_INFO," found CartChunk CutID \""+wavedata->cutId()+"\"\n");
    *cartnum=wavedata->cutId().toUInt(&ok);
    (*cartnum)+=import_cart_number_offset;
    if((!ok)||(*cartnum==0)||(*cartnum>999999)||
       (effective_group->enforceCartRange()&&
	(!effective_group->cartNumberValid(*cartnum)))) {
      Log(LOG_WARNING,QString().sprintf(" File \"%s\" has an invalid or out of range Cart Number \"%s\", skipping...\n",
				RDGetBasePart(filename).toUtf8().constData(),
			       	wavedata->cutId().toUtf8().constData()));
      wavefile->closeWave();
      import_failed_imports++;
      import_journal->addFailure(effective_group->name(),filename,
				 tr("invalid/out-of-range cart number"));
      delete wavefile;
      delete wavedata;
      delete effective_group;
      return MainObject::FileBad;
    }
  }

  //
  // Ensure that we have a valid title
  //
  bool cart_exists=false;
  if(*cartnum!=0) {
    cart_exists=RDCart::exists(*cartnum);
  }
  //
  // If the cart already exists and no title was found in metadata,
  // then keep the existing title. Otherwise, generate a default title.
  //
  if((!cart_exists)&&wavedata->metadataFound()&&wavedata->title().isEmpty()) {
    wavedata->setTitle(effective_group->generateTitle(filename));
  }

  //
  // Attempt to find a free cart
  //
  if(*cartnum==0) {
    *cartnum=effective_group->nextFreeCart();
  }
  if(*cartnum==0) {
    Log(LOG_ERR,QString().sprintf("rdimport: no free carts available in specified group\n"));
    wavefile->closeWave();
    import_failed_imports++;
    import_failed_imports++;
    if(import_drop_box) {
      if(!import_run) {
	NormalExit();
      }
      if(!import_temp_fix_filename.isEmpty()) {
	QFile::remove(import_temp_fix_filename);
	import_temp_fix_filename="";
      }
      import_journal->addFailure(effective_group->name(),filename,
				 tr("no free cart available in group"));
      delete wavefile;
      delete wavedata;
      delete effective_group;
      return MainObject::NoCart;
    }
    ErrorExit(RDApplication::ExitImportFailed);
  }

  //
  // Prepare destination cart/cut
  //
  if(import_delete_cuts) {
    DeleteCuts(import_cart_number);
  }
  if(RDCart::exists(*cartnum)) {
    cart_created=false;
  }
  else {
    if((wavedata->title().length()==0)||
       ((wavedata->title().length()>0)&&(wavedata->title()[0] == '\0'))) {
      wavedata->setTitle(effective_group->generateTitle(filename));
    }

    if(!rda->system()->allowDuplicateCartTitles()) {
      if(rda->system()->fixDuplicateCartTitles()) {
	wavedata->setTitle(RDCart::ensureTitleIsUnique(0,wavedata->title()));
      }
      else {
	sql=QString("select ")+
	  "NUMBER "+  // 00
	  "from CART where "+
	  "TITLE=\""+RDEscapeString(wavedata->title())+"\"";
	q=new RDSqlQuery(sql);
	if(q->first()) {
	  QString err_msg=QString().
	    sprintf(" File \"%s\" has duplicate title \"%s\", skipping...\n",
		    RDGetBasePart(filename).toUtf8().constData(),
		    wavedata->title().toUtf8().constData());
	  Log(LOG_WARNING,err_msg);
	  import_journal->addFailure(effective_group->name(),filename,
				     tr("duplicate title"));
	  if(!import_output_pattern.isEmpty()) {
	    printf("Import failed: duplicate title \"%s\"\n",
		   wavedata->title().toUtf8().constData());
	  }
	  delete q;
	  delete wavedata;
	  delete wavefile;
	  return MainObject::DuplicateTitle;
	}
	delete q;
      }
    }
    cart_created=
      RDCart::create(effective_group->name(),RDCart::Audio,&err_msg,*cartnum)!=0;
  }

  //
  // Create cart/cut
  //
  RDCart *cart=new RDCart(*cartnum);
  int cutnum=
    cart->addCut(import_format,import_bitrate,import_channels);
  if(cutnum<0) {
    Log(LOG_WARNING,QString().sprintf("rdimport: no free cuts available in cart %06u\n",*cartnum));
    import_failed_imports++;
    import_journal->addFailure(effective_group->name(),filename,
			       tr("no free cut available in cart"));
    delete cart;
    return MainObject::NoCut;
  }
  RDCut *cut=new RDCut(*cartnum,cutnum);

  //
  // Import audio
  //
  RDAudioImport *conv=new RDAudioImport(this);
  conv->setCartNumber(cart->number());
  conv->setCutNumber(cutnum);
  conv->setSourceFile(wavefile->getName());
  RDSettings *settings=new RDSettings();
  settings->setChannels(import_channels);
  switch(import_format) {
  case 0:
    settings->setFormat(RDSettings::Pcm16);
    break;

  case 1:
    settings->setFormat(RDSettings::MpegL2Wav);
    break;
  }
  settings->setNormalizationLevel(import_normalization_level/100);
  settings->setAutotrimLevel(import_autotrim_level/100);
  conv->setDestinationSettings(settings);
  conv->setUseMetadata(false);
  if(import_string_title.isNull()) {
    Log(LOG_INFO,QString().
	sprintf(" Importing file \"%s\" [%s] to cart %06u ... ",
		RDGetBasePart(filename).toUtf8().constData(),
		wavedata->title().stripWhiteSpace().toUtf8().constData(),
		*cartnum));
  }
  switch(conv_err=conv->runImport(rda->user()->name(),rda->user()->password(),
				  &audio_conv_err)) {
  case RDAudioImport::ErrorOk:
    Log(LOG_INFO,QString().sprintf("done.\n"));
    break;

  default:
    Log(LOG_INFO,QString().sprintf(" %s, skipping %s...\n",
	    (const char *)RDAudioImport::errorText(conv_err,audio_conv_err),
	    (const char *)filename.toUtf8()));
    if(cart_created) {
      cart->remove(rda->station(),rda->user(),rda->config());
    }
    else {
      cart->removeCut(rda->station(),rda->user(),cut->cutName(),rda->config());
    }
    delete cut;
    delete cart;
    wavefile->closeWave();
    if(!import_run) {
      NormalExit();
    }
    if(!import_temp_fix_filename.isEmpty()) {
      QFile::remove(import_temp_fix_filename);
      import_temp_fix_filename="";
    }
    import_failed_imports++;
    import_journal->addFailure(effective_group->name(),filename,
			       tr("corrupt audio file"));
    delete wavefile;
    delete wavedata;
    delete effective_group;
    return MainObject::FileBad;
    break;
  }

  if(import_autotrim_level!=0) {
    wavedata->setStartPos(-1);
    wavedata->setEndPos(-1);
  }

  if(cart_created) {
    cart->setMetadata(wavedata);
  }
  cut->setMetadata(wavedata);
  cut->autoSegue(import_segue_level,import_segue_length,rda->station(),
		 rda->user(),rda->config());
  if(cut->description().isEmpty()) {      // Final backstop, so we don't end up
    cut->setDescription(cart->title());   // with an empty description field.
  }
  if((!import_metadata_pattern.isEmpty())&&(!wavedata->title().isEmpty())) {
    cart->setTitle(wavedata->title());
  }
  if(import_startdate_offset!=0) {
    dt=cut->startDatetime(&ok);
    if(ok) {
      cut->setStartDatetime(dt.addDays(import_startdate_offset),true);
    }
  }
  if(import_enddate_offset!=0) {
    dt=cut->endDatetime(&ok);
    if(ok) {
      cut->setEndDatetime(dt.addDays(import_enddate_offset),true);
    }
  }
  if(import_create_dates)  {
    dt=cut->startDatetime(&ok);
    if (!ok){
      dt=QDateTime(QDate::currentDate(), QTime(0,0,0));
      cut->setStartDatetime(dt.addDays(import_create_startdate_offset),true);
    }
    dt=cut->endDatetime(&ok);
    if(!ok) {
      dt=QDateTime(QDate::currentDate(), QTime(23,59,59));
      cut->setEndDatetime(dt.addDays(import_create_enddate_offset),true);
    }
  }
  cut->setOriginName(rda->station()->name());
  for(unsigned i=0;i<import_add_scheduler_codes.size();i++) {
    cart->addSchedCode(import_add_scheduler_codes[i]);
  }
  if(!import_string_agency.isNull()) {
    cart->setAgency(import_string_agency);
  }
  if(!import_string_album.isNull()) {
    cart->setAlbum(import_string_album);
  }
  if(!import_string_artist.isNull()) {
    cart->setArtist(import_string_artist);
  }
  if(import_string_bpm!=0) {
    cart->setBeatsPerMinute(import_string_bpm);
  }
  if(!import_string_client.isNull()) {
    cart->setClient(import_string_client);
  }
  if(!import_string_composer.isNull()) {
    cart->setComposer(import_string_composer);
  }
  if(!import_string_conductor.isNull()) {
    cart->setConductor(import_string_conductor);
  }
  if(!import_string_description.isNull()) {
    cut->setDescription(import_string_description);
  }
  if(!import_string_label.isNull()) {
    cart->setLabel(import_string_label);
  }
  if(!import_string_outcue.isNull()) {
    cut->setOutcue(import_string_outcue);
  }
  if(!import_string_isci.isNull()) {
    cut->setIsci(import_string_isci);
  }
  if(!import_string_isrc.isNull()) {
    cut->setIsrc(RDDiscLookup::normalizedIsrc(import_string_isrc));
  }
  if(!import_string_recording_mbid.isNull()) {
    cut->setRecordingMbId(import_string_recording_mbid);
  }
  if(!import_string_release_mbid.isNull()) {
    cut->setReleaseMbId(import_string_release_mbid);
  }
  if(!import_string_publisher.isNull()) {
    cart->setPublisher(import_string_publisher);
  }
  if(!import_string_song_id.isNull()) {
    cart->setSongId(import_string_song_id);
  }
  if(!import_string_title.isNull()) {
    cart->setTitle(import_string_title);
  }
  if(!import_set_user_defined.isEmpty()) {
    cart->setUserDefined(import_set_user_defined);
  }
  if(!import_string_user_defined.isNull()) {
    cart->setUserDefined(import_string_user_defined);
  }
  if(import_string_year!=0) {
    cart->setYear(import_string_year);
  }
  if((!import_dayparts[0].isNull())||(!import_dayparts[1].isNull())) {
    cut->setStartDaypart(import_dayparts[0],true);
    cut->setEndDaypart(import_dayparts[1],true);
  }
  if(import_clear_dayparts) {
    cut->setStartDaypart(QTime(),false);
    cut->setEndDaypart(QTime(),false);
  }
  if((!import_datetimes[0].isNull())||(!import_datetimes[1].isNull())) {
    cut->setStartDatetime(import_datetimes[0],true);
    cut->setEndDatetime(import_datetimes[1],true);
  }
  if(import_clear_datetimes) {
    cut->setStartDatetime(QDateTime(),false);
    cut->setEndDatetime(QDateTime(),false);
  }
  import_cut_markers->setAudioLength(wavefile->getExtTimeLength());
  if(import_cut_markers->hasStartValue()) {
    cut->setStartPoint(import_cut_markers->startValue());
    cut->setEndPoint(import_cut_markers->endValue());
    cut->setLength(cut->endPoint()-cut->startPoint());
  }
  int lo=cut->startPoint();
  int hi=cut->endPoint();
  import_talk_markers->setAudioLength(wavefile->getExtTimeLength());
  if(import_talk_markers->hasStartValue()) {
    cut->setTalkStartPoint(import_talk_markers->startValue(lo,hi));
    cut->setTalkEndPoint(import_talk_markers->endValue(lo,hi));
  }
  import_hook_markers->setAudioLength(wavefile->getExtTimeLength());
  if(import_hook_markers->hasStartValue()) {
    cut->setHookStartPoint(import_hook_markers->startValue(lo,hi));
    cut->setHookEndPoint(import_hook_markers->endValue(lo,hi));
  }
  import_segue_markers->setAudioLength(wavefile->getExtTimeLength());
  if(import_segue_markers->hasStartValue()) {
    cut->setSegueStartPoint(import_segue_markers->startValue(lo,hi));
    cut->setSegueEndPoint(import_segue_markers->endValue(lo,hi));
  }
  import_fadedown_marker->setAudioLength(wavefile->getExtTimeLength());
  if(import_fadedown_marker->hasFadeValue()) {
    cut->setFadedownPoint(import_fadedown_marker->fadeValue(lo,hi));
  }
  import_fadeup_marker->setAudioLength(wavefile->getExtTimeLength());
  if(import_fadeup_marker->hasFadeValue()) {
    cut->setFadeupPoint(import_fadeup_marker->fadeValue(lo,hi));
  }
  cart->updateLength();
  if(cart_created) {
    SendNotification(RDNotification::AddAction,cart->number());
  }
  else {
    SendNotification(RDNotification::ModifyAction,cart->number());
  }

  if(!import_output_pattern.isEmpty()) {
    RDLogLine *ll=new RDLogLine(cart->number(),cut->cutNumber());
    printf("%s\n",
	   ll->resolveWildcards(import_output_pattern).toUtf8().constData());
    delete ll;
  }

  import_journal->
    addSuccess(effective_group->name(),filename,*cartnum,cart->title());

  delete settings;
  delete conv;
  delete cut;
  delete cart;
  wavefile->closeWave();
  delete wavefile;
  delete wavedata;
  delete effective_group;

  if(import_delete_source) {
    unlink(filename.toUtf8());
    Log(LOG_INFO,QString().sprintf(" Deleted file \"%s\"\n",(const char *)RDGetBasePart(filename).toUtf8()));
  }
  if(!import_run) {
    NormalExit();
  }
  if(!import_temp_fix_filename.isEmpty()) {
    QFile::remove(import_temp_fix_filename);
    import_temp_fix_filename="";
  }

  return MainObject::Success;
}


bool MainObject::OpenAudioFile(RDWaveFile **wavefile,RDWaveData *wavedata)
{
  QString orig_filename=(*wavefile)->getName();

  if(!(*wavefile)->openWave(wavedata)) {
    if(import_fix_broken_formats) {
      Log(LOG_WARNING,QString().sprintf(" File \"%s\" appears to be malformed, trying workaround ... ",
		   RDGetBasePart(orig_filename).toUtf8().constData()));
      delete *wavefile;
      if(((*wavefile)=FixFile(orig_filename,wavedata))==NULL) {
	Log(LOG_WARNING,QString().sprintf("failed.\n"));
	Log(LOG_WARNING,QString().sprintf(
		" File \"%s\" is not readable or not a recognized format, skipping...\n",
		RDGetBasePart(orig_filename).toUtf8().constData()));
	if(!import_run) {
	  NormalExit();
	}
	if(!import_temp_fix_filename.isEmpty()) {
	  QFile::remove(import_temp_fix_filename);
	  import_temp_fix_filename="";
	}
	import_failed_imports++;
	import_journal->addFailure(import_group->name(),orig_filename,
				   tr("unknown/unrecognized file format"));
	return false;
      }
      Log(LOG_WARNING,QString().sprintf("success.\n"));
    }
    else {
      Log(LOG_WARNING,QString().sprintf(
        " File \"%s\" is not readable or not a recognized format, skipping...\n",
        RDGetBasePart((*wavefile)->getName()).toUtf8().constData()));
      if(!import_run) {
	NormalExit();
      }
      if(!import_temp_fix_filename.isEmpty()) {
	QFile::remove(import_temp_fix_filename);
	import_temp_fix_filename="";
      }
      import_failed_imports++;
      import_journal->addFailure(import_group->name(),orig_filename,
				 tr("unknown/unrecognized file format"));
      return false;
    }
  }

  return true;
}


void MainObject::VerifyFile(const QString &filename,unsigned *cartnum)
{
  bool found=false;
  QDateTime dt;

  for(std::list<struct DropboxList *>::const_iterator 
	ci=import_dropbox_list.begin();
      ci!=import_dropbox_list.end();ci++) {
    if((*ci)->filename==filename) {
      found=true;
      QFileInfo *file=new QFileInfo(filename);
      dt=GetCachedTimestamp(filename);
      if(dt.isNull()||(file->lastModified()>dt)) {
	if((file->size()==(*ci)->size)&&(!(*ci)->failed)) {
	  (*ci)->pass++;
	}
	else {
	  (*ci)->size=file->size();
	  (*ci)->pass=0;
	}
	if((*ci)->failed) {
	  (*ci)->checked=true;
	  if(file->size()!=(*ci)->size) {
	    (*ci)->failed=false;
	    (*ci)->size=file->size();
	    (*ci)->pass=0;
	  }
	}
	if((*ci)->pass>=RDIMPORT_DROPBOX_PASSES) {
	  switch(ImportFile(filename,cartnum)) {
	    case MainObject::Success:
	      WriteTimestampCache(filename,file->lastModified());
	      break;
	      
 	    case MainObject::DuplicateTitle:
	    case MainObject::FileBad:
	      (*ci)->failed=true;
	      (*ci)->checked=true;
	      (*ci)->pass=0;
	      WriteTimestampCache(filename,file->lastModified());
	      break;
	      
	    case MainObject::NoCart:
	    case MainObject::NoCut:
	      (*ci)->pass=0;
	      (*ci)->checked=true;
	      break;
	  }
	}
	else {
	  (*ci)->checked=true;
	}
      }
      delete file;
    }
  }
  if(!found) {
    QFile *file=new QFile(filename);
    import_dropbox_list.push_back(new struct DropboxList());
    import_dropbox_list.back()->filename=filename;
    import_dropbox_list.back()->size=file->size();
    import_dropbox_list.back()->pass=0;
    import_dropbox_list.back()->checked=true;
    import_dropbox_list.back()->failed=false;
    delete file;
  }
}


RDWaveFile *MainObject::FixFile(const QString &filename,RDWaveData *wavedata)
{
  bool fix_needed=false;

  //
  // Determine Fixability
  //
  int fd=open(filename,O_RDONLY);
  if(fd<0) {
    return NULL;
  }
  if(!IsWav(fd)) {
    return NULL;
  }
  if(!FindChunk(fd,"fmt ",&fix_needed)) {
    return NULL;
  }
  if(!FindChunk(fd,"data",&fix_needed)) {
    return NULL;
  }
  if(!fix_needed) {  // This shouldn't ever happen!
    return NULL;
  }
  ::close(fd);

  //
  // Copy File
  //
  char tempfile[PATH_MAX];
  strncpy(tempfile,RDTempDirectory::basePath()+
	  QString().sprintf("/rdimport%dXXXXXX",getpid()),PATH_MAX);
  int dest_fd=mkstemp(tempfile);
  if(dest_fd<0) {
    return NULL;
  }
  import_temp_fix_filename=tempfile;
  if(!RDCopy(filename,dest_fd)) {
    return NULL;
  }
  close(dest_fd);

  //
  // Apply Fix
  //
  if(!FixChunkSizes(import_temp_fix_filename)) {
    return NULL;
  }
  RDWaveFile *wf=new RDWaveFile();
  wf->nameWave(import_temp_fix_filename);
  if(!wf->openWave(wavedata)) {
    delete wf;
    return NULL;
  }
  return wf;
}


bool MainObject::IsWav(int fd)
{
  int i;
  char buffer[5];
  
  //
  // Is this a riff file? 
  //
  lseek(fd,0,SEEK_SET);
  i=read(fd,buffer,4);
  if(i==4) {
    buffer[4]=0;
    if(strcmp("RIFF",buffer)!=0) {
      return false;
    }
  }
  else {
    return false;
  }

  // 
  // Is this a WAVE file? 
  //
  if(lseek(fd,8,SEEK_SET)!=8) {
    return false;
  }
  i=read(fd,buffer,4);
  if(i==4) {
    buffer[4]=0;
    if(strcmp("WAVE",buffer)!=0) {
      return false;
    }
  }
  else {
    return false;
  }
  return true;
}


bool MainObject::FindChunk(int fd,const char *chunk_name,bool *fix_needed)
{
  int i;
  char name[5]={0,0,0,0,0};
  unsigned char buffer[4];
  unsigned chunk_size;

  lseek(fd,12,SEEK_SET);
  i=read(fd,name,4);
  i=read(fd,buffer,4);
  chunk_size=buffer[0]+(256*buffer[1])+(65536*buffer[2])+(16777216*buffer[3]);
  while(i==4) {
    if(strcasecmp(chunk_name,name)==0) {
      return true;
    }
    lseek(fd,chunk_size,SEEK_CUR);
    i=read(fd,name,4);
    if(name[0]==0) {  // Possible chunk size error
      if(isalpha(name[1])==0) {
	return false;
      }
      name[0]=name[1];
      name[1]=name[2];
      name[2]=name[3];
      if(read(fd,name+3,1)<1) {
	return false;
      }
      *fix_needed=true;
    }
    i=read(fd,buffer,4);
    chunk_size=
      buffer[0]+(256*buffer[1])+(65536*buffer[2])+(16777216*buffer[3]);
  }
  return false;
}


bool MainObject::FixChunkSizes(const QString &filename)
{
  int i;
  char name[5]={0,0,0,0,0};
  unsigned char buffer[4];
  unsigned chunk_size;
  int fd;

  //
  // Open File
  //
  if((fd=open(filename,O_RDWR))<0) {
    return false;
  }
  lseek(fd,12,SEEK_SET);
  i=read(fd,name,4);
  i=read(fd,buffer,4);
  off_t last_offset=lseek(fd,0,SEEK_CUR);
  chunk_size=buffer[0]+(256*buffer[1])+(65536*buffer[2])+(16777216*buffer[3]);
  while(i==4) {
    lseek(fd,chunk_size,SEEK_CUR);
    i=read(fd,name,4);
    if(name[0]==0) {  // Possible chunk size error
      if(isalpha(name[1])==0) {
	return false;
      }
      //
      // Fix It Up
      //
      name[0]=name[1];
      name[1]=name[2];
      name[2]=name[3];
      if(read(fd,name+3,1)<1) {
	::close(fd);
	return false;
      }
      off_t pos=lseek(fd,0,SEEK_CUR);
      char buf[4];
      lseek(fd,last_offset,SEEK_SET);
      read(fd,buf,4);
      unsigned size=(0xff&buf[0])+(0xff&(256*buf[1]))+
	(0xff&(65536*buf[2]))+(0xff&(16777216*buf[3]))+1;
      buf[0]=size&0xff;
      buf[1]=(size>>8)&0xff;
      buf[2]=(size>>16)&0xff;
      buf[3]=(size>>24)&0xff;
      lseek(fd,last_offset,SEEK_SET);
      write(fd,buf,4);
      lseek(fd,pos,SEEK_SET);
    }
    last_offset=lseek(fd,0,SEEK_CUR);
    i=read(fd,buffer,4);
    chunk_size=
      buffer[0]+(256*buffer[1])+(65536*buffer[2])+(16777216*buffer[3]);
  }
  ::close(fd);
  return true;
}


bool MainObject::RunPattern(const QString &pattern,const QString &filename,
			    RDWaveData *wavedata,QString *groupname)
{
  //  MAINTAINERS'S NOTE: These mappings must be kept in sync with those
  //                      of the 'resolvePadFields()' method in
  //                      'apis/PyPAD/api/PyPAD.py', as well as the
  //                      'resolveWildcards()' method in the
  //                      'lib/rdlog_line.cpp' file and the
  //                      'VerifyPattern()' method in this file.

  bool macro_active=false;
  int ptr=0;
  QChar field;
  QChar subfield;
  QString value;
  QChar delimiter;
  bool found_cartnum=false;
  bool found_end_date=false;
  bool found_start_time=false;
  bool found_end_time=false;
  QTime time;
  QDate date;

  //
  // Sanity Check
  //
  if(pattern.length()<2) {
    return false;
  }

  //
  // Initialize Pattern Parser
  //
  if((pattern.at(0)=='%')&&(pattern.at(1)!='%')) {
    field=pattern.at(1);
    value="";
    if(field==QChar('w')) {
      if(pattern.length()>=4) {
	subfield=pattern.at(2);
	delimiter=pattern.at(3);
	ptr=4;
      }
      else {
	if(pattern.length()>=3) {
	  subfield=pattern.at(2);
	  delimiter=QChar();
	  ptr=3;
	}
	else {
	  ptr=2;
	}
      }
    }
    else {
      if(pattern.length()>=3) {
	delimiter=pattern.at(2);
	ptr=3;
      }
      else {
	ptr=2;
      }
    }
    macro_active=true;
  }
  else {
    delimiter=pattern.at(0);
    ptr=1;
  }

  for(int i=0;i<=filename.length();i++) {
    if(macro_active) {
      if((i==filename.length())||
	 ((!delimiter.isNull())&&(filename.at(i)==delimiter))) {
	switch(field.toAscii()) {
	case 'a':
	  wavedata->setArtist(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'b':
	  wavedata->setLabel(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'c':
	  wavedata->setClient(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'e':
	  wavedata->setAgency(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'g':
	  *groupname=value;
	  break;

	case 'i':
	  wavedata->setDescription(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'k':
	  time=QTime::fromString(value);
	  if(time.isValid()) {
	    wavedata->setStartTime(time);
	    found_start_time=true;
	    wavedata->setMetadataFound(true);
	  }
	  break;

	case 'K':
	  time=QTime::fromString(value);
	  if(time.isValid()) {
	    wavedata->setEndTime(time);
	    found_end_time=true;
	    wavedata->setMetadataFound(true);
	  }
	  break;

	case 'l':
	  wavedata->setAlbum(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'm':
	  wavedata->setComposer(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'n':
	  wavedata->setCutId(value);
	  wavedata->setMetadataFound(true);
	  found_cartnum=true;
	  break;

	case 'o':
	  wavedata->setOutCue(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'p':
	  wavedata->setPublisher(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'q':
	  date=QDate::fromString(value,Qt::ISODate);
	  if(date.isValid()) {
	    wavedata->setStartDate(date);
	    wavedata->setMetadataFound(true);
	    if(!found_end_date) {
	      wavedata->setEndDate(date);
	      if(!found_end_time) {
		wavedata->setEndTime(QTime(23,59,59));
	      }
	    }
	    if(!found_start_time) {
	      wavedata->setStartTime(QTime(0,0,0,1));
	    }
	  }
	  break;

	case 'Q':
	  date=QDate::fromString(value,Qt::ISODate);
	  if(date.isValid()) {
	    wavedata->setEndDate(date);
	    found_end_date=true;
	    wavedata->setMetadataFound(true);
	    if(!found_end_time) {
	      wavedata->setEndTime(QTime(23,59,59));
	    }
	  }
	  break;

	case 'r':
	  wavedata->setConductor(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 's':
	  wavedata->setTmciSongId(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 't':
	  wavedata->setTitle(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'u':
	  wavedata->setUserDefined(value);
	  wavedata->setMetadataFound(true);
	  break;

	case 'w':
	  switch(subfield.toAscii()) {
	  case 'c':
	    wavedata->setIsci(value);
	    wavedata->setMetadataFound(true);
	    break;

	  case 'i':
	    if(RDDiscLookup::isrcIsValid(value)) {
	      wavedata->setIsrc(RDDiscLookup::normalizedIsrc(value));
	      wavedata->setMetadataFound(true);
	    }
	    else {
	      Log(LOG_ERR,"invalid ISRC \""+value+"\"\n");
	      ErrorExit(RDApplication::ExitInvalidOption);
	    }
	    break;

	  case 'm':
	    wavedata->setRecordingMbId(value);
	    wavedata->setMetadataFound(true);
	    break;

	  case 'r':
	    wavedata->setReleaseMbId(value);
	    wavedata->setMetadataFound(true);
	    break;
	  }
	  break;

	case 'y':
	  wavedata->setReleaseYear(value.toInt());
	  wavedata->setMetadataFound(true);
	  break;
	}
	value="";
	if((ptr>=pattern.length())||(i==filename.length())) {
	  return found_cartnum;
	}
	if((pattern.at(ptr)=='%')&&(pattern.at(ptr+1)!='%')) {
	  field=pattern.at(ptr+1);
	  if(field==QChar('w')) {
	    if(pattern.length()>(ptr+3)) {
	      delimiter=pattern.at(ptr+3);
	    }
	    else {
	      delimiter=QChar();
	    }
	    subfield=pattern.at(ptr+2);
	    ptr+=4;
	    macro_active=true;
	  }
	  else {
	    if(pattern.length()>(ptr+2)) {
	      delimiter=pattern.at(ptr+2);
	    }
	    else {
	      delimiter=QChar();
	    }
	    ptr+=3;
	    macro_active=true;
	  }
	}
	else {
	  if(pattern.length()>ptr) {
	    delimiter=pattern.at(ptr);
	  }
	  else {
	    delimiter=QChar();
	  }
	  ptr++;
	  macro_active=false;
	}
      }
      else {
	value+=filename.at(i);
      }
    }
    else {
      if((ptr>=pattern.length())||(i==filename.length())) {
	return found_cartnum;
      }
      if(filename.at(i)!=delimiter) {
	return found_cartnum;
      }
      if((pattern.at(ptr)=='%')&&(pattern.at(ptr+1)!='%')) {
	field=pattern.at(ptr+1);
	if(field==QChar('w')) {
	  delimiter=pattern.at(ptr+3);
	  subfield=pattern.at(ptr+2);
	  ptr+=4;
	  macro_active=true;
	}
	else {
	  delimiter=pattern.at(ptr+2);
	  ptr+=3;
	  macro_active=true;
	}
      }
      else {
	delimiter=pattern.at(ptr);
	ptr++;
	macro_active=false;
      }
    }
  }

  return found_cartnum;
}


bool MainObject::VerifyPattern(const QString &pattern)
{
  //  MAINTAINERS'S NOTE: These mappings must be kept in sync with those
  //                      of the 'resolvePadFields()' method in
  //                      'apis/PyPAD/api/PyPAD.py', as well as the
  //                      'resolveWildcards()' method in the
  //                      'lib/rdlog_line.cpp' file and the
  //                      'RunPattern()' method in this file.

  bool macro_active=false;
  for(int i=0;i<pattern.length();i++) {
    if(pattern.at(i)==QChar('%')) {
      if(macro_active) {
	return false;
      }
      macro_active=true;
      if(i>=(pattern.length()-1)) {
	return false;
      }
      switch(pattern.at(++i).toAscii()) {
      case 'a':
      case 'b':
      case 'c':
      case 'e':
      case 'g':
      case 'i':
      case 'k':
      case 'K':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'Q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'y':
      case '%':
	break;

      case 'w':
	if(i>=(pattern.length()-1)) {
	  return false;
	}
	switch(pattern.at(++i).toAscii()) {
	case 'i':
	case 'm':
	case 'r':
	  break;
	}
	break;

      default:
	return false;
      }
    }
    else {
      macro_active=false;
    }
  }
  return true;
}


void MainObject::DeleteCuts(unsigned cartnum)
{
  Log(LOG_INFO,QString().sprintf(" Deleting cuts from cart %06u\n",cartnum));

  unsigned dev;
  RDCart *cart=new RDCart(cartnum);
  cart->removeAllCuts(rda->station(),rda->user(),rda->config());
  cart->updateLength();
  cart->resetRotation();
  cart->calculateAverageLength(&dev);
  cart->setLengthDeviation(dev);
  delete cart;
}


QDateTime MainObject::GetCachedTimestamp(const QString &filename)
{
  QString sql;
  RDSqlQuery *q;
  QDateTime dt;
  if(import_persistent_dropbox_id<0) {
    return dt;
  }
  sql=QString().sprintf("select FILE_DATETIME from DROPBOX_PATHS where ")+
    QString().sprintf("(DROPBOX_ID=%d)&&",import_persistent_dropbox_id)+
    "(FILE_PATH=\""+RDEscapeString(filename)+"\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    dt=q->value(0).toDateTime();
  }
  delete q;
  return dt;
}


void MainObject::WriteTimestampCache(const QString &filename,
				     const QDateTime &dt)
{
  QString sql;
  RDSqlQuery *q;
  if(import_persistent_dropbox_id<0) {
    return;
  }
  if(GetCachedTimestamp(filename).isNull()) {
    sql=QString("insert into DROPBOX_PATHS set ")+
      QString().sprintf("DROPBOX_ID=%d,",import_persistent_dropbox_id)+
      "FILE_PATH=\""+RDEscapeString(filename)+"\","+
      "FILE_DATETIME="+RDCheckDateTime(dt,"yyyy-MM-dd hh:mm:ss");
  }
  else {
    sql=QString("update DROPBOX_PATHS set ")+
      "FILE_DATETIME="+RDCheckDateTime(dt,"yyyy-MM-dd hh:mm:ss")+" where "+
      QString().sprintf("(DROPBOX_ID=%d)&&",import_persistent_dropbox_id)+
      "(FILE_PATH=\""+RDEscapeString(filename)+"\")";
  }
  q=new RDSqlQuery(sql);
  delete q;
}


bool MainObject::SchedulerCodeExists(const QString &code) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select CODE from SCHED_CODES where CODE=\"")+
    RDEscapeString(code)+"\"";
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


void MainObject::ReadXmlFile(const QString &basename,RDWaveData *wavedata) const
{
  QString xmlname="";
  FILE *f=NULL;
  char line[1024];
  QString xml="";
  std::vector<RDWaveData> wavedatas;

  //
  // Get XML Filename
  //
  QStringList f0=basename.split(".");
  for(int i=0;i<f0.size()-1;i++) {
    xmlname+=f0[i]+".";
  }
  xmlname+="xml";
  Log(LOG_INFO,QString().sprintf(" Reading xml metadata from \"%s\": ",(const char *)xmlname));

  //
  // Read XML
  //
  wavedata->clear();
  if((f=fopen(xmlname,"r"))==NULL) {
    Log(LOG_WARNING,QString().sprintf("failed [%s]\n",strerror(errno)));
    return;
  }
  Log(LOG_INFO,QString("success\n"));
  while(fgets(line,1024,f)!=NULL) {
    xml+=line;
  }
  fclose(f);

  //
  // Parse
  //
  if(RDCart::readXml(&wavedatas,xml)>0) {
    *wavedata=wavedatas[1];
  }
}


void MainObject::SendNotification(RDNotification::Action action,
				  unsigned cartnum)
{
  RDNotification *notify=
    new RDNotification(RDNotification::CartType,action,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  qApp->processEvents();
  delete notify;
}


void MainObject::Log(int prio,const QString &msg) const
{
  QString m=msg;
  FILE *f=NULL;

  if(import_log_syslog) {
    rda->syslog(prio,"%s",(const char *)msg.trimmed().toUtf8());
  }
  if(!import_log_filename.isEmpty()) {
    QDateTime now=QDateTime::currentDateTime();
    QString filename=RDDateDecode(import_log_filename,now.date(),
				  rda->station(),rda->config());
    if((f=fopen(filename.toUtf8(),"a"))!=NULL) {
      fprintf(f,"%s: %s",
	      (const char *)now.toString("MMM dd hh:mm:ss").toUtf8(),
	      (const char *)msg.toUtf8());
      fflush(f);
      fclose(f);
    }
  }

  if(prio==LOG_ERR) {
    fprintf(stderr,"%s",(const char *)msg.toUtf8());
    fflush(stderr);
  }
  else {
    if(import_verbose) {
      fprintf(stdout,"%s",(const char *)msg.toUtf8());
      fflush(stdout);
    }
  }
}


void MainObject::NormalExit() const
{
  if((import_journal!=NULL)&&(import_send_mail)) {
    import_journal->sendAll();
  }
  if(import_failed_imports>0) {
    exit(RDApplication::ExitImportFailed);
  }
  exit(RDApplication::ExitOk);
}


void MainObject::ErrorExit(RDApplication::ExitCode code) const
{
  if((import_journal!=NULL)&&(import_send_mail)) {
    import_journal->sendAll();
  }
  exit(code);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
