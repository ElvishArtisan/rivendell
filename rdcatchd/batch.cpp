// batch.cpp
//
// Batch Routines for the Rivendell netcatcher daemon
//
//   (C) Copyright 2002-2007, 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: batch.cpp,v 1.6.2.1 2012/05/10 16:00:53 cvs Exp $
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

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <errno.h>

#include <vector>

#include <qapplication.h>
#include <qtimer.h>
#include <qsignalmapper.h>
#include <qsessionmanager.h>

#include <rddb.h>
#include <rdconf.h>
#include <rdurl.h>
#include <rdwavefile.h>
#include <rdcut.h>
#include <rdcatchd_socket.h>
#include <rdcatchd.h>
#include <rdrecording.h>
#include <rdttyout.h>
#include <rdmixer.h>
#include <rdcheck_daemons.h>
#include <rddebug.h>
#include <rddatedecode.h>
#include <rdcmd_switch.h>
#include <rdescape_string.h>
#include <rdpodcast.h>
#include <rdsettings.h>
#include <rdlibrary_conf.h>
#include <rdaudioconvert.h>
#include <rdupload.h>
#include <rddownload.h>

void MainObject::catchConnectedData(int serial,bool state)
{
  if(!state) {
    LogLine(RDConfig::LogErr,"unable to connect to rdcatchd(8) daemon");
    exit(256);
  }

  //
  // Dispatch Handler
  //
  switch(batch_event->type()) {
  case RDRecording::Recording:
    RunImport(batch_event);
    break;

  case RDRecording::Download:
    RunDownload(batch_event);
    break;

  case RDRecording::Upload:
    RunUpload(batch_event);
    break;

  default:
    fprintf(stderr,"rdcatchd: nothing to do for this event type\n");
    exit(256);
  }

  exit(0);
}


void MainObject::RunBatch(RDCmdSwitch *cmd)
{
  bool ok=false;
  int id=-1;
  unsigned schema=0;

  //
  // Set Process Priority
  //
  struct sched_param sp;
  memset(&sp,0,sizeof(sp));
  if(sched_setscheduler(getpid(),SCHED_BATCH,&sp)!=0) {
    LogLine(RDConfig::LogWarning,
	    QString().sprintf("unable to set batch permissions, %s",
			      strerror(errno)));
  }

  //
  // Get ID
  //
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--event-id") {
      id=cmd->value(i).toInt(&ok);
      if((!ok)||(id<0)) {
	fprintf(stderr,"rdcatchd: invalid event-id\n");
	exit(256);
      }
    }
  }
  if(id<0) {
    fprintf(stderr,"rdcatchd: missing event-id\n");
    exit(256);
  }

  //
  // Calculate Temporary Directory
  //
  catch_temp_dir=RDTempDir();

  //
  // Open Database
  //
  QString err (tr("ERROR rdcatchd aborting - "));

  catch_db=RDInitDb (&schema,&err);
  if(!catch_db) {
    printf(err.ascii());
    exit(1);
  }
  connect (RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
	   this,SLOT(log(RDConfig::LogPriority,const QString &)));

  //
  // Load Event
  //
  QString sql=LoadEventSql()+QString().sprintf(" where ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    fprintf(stderr,"rdcatchd: id %d not found\n",id);
    exit(256);
  }
  batch_event=new CatchEvent();
  LoadEvent(q,batch_event,false);
  delete q;

  //
  // Open Status Connection
  //
  catch_connect=new RDCatchConnect(0,this);
  connect(catch_connect,SIGNAL(connected(int,bool)),
	  this,SLOT(catchConnectedData(int,bool)));
  catch_connect->
    connectHost("localhost",RDCATCHD_TCP_PORT,catch_config->password());
}


void MainObject::RunImport(CatchEvent *evt)
{
  evt->setTempName(GetTempRecordingName(evt->id()));
  evt->setDeleteTempFile(true);
  Import(evt);
}


void MainObject::RunDownload(CatchEvent *evt)
{
  RDDownload::ErrorCode conv_err;

  //
  // Resolve Wildcards
  //
  RDStation *station=new RDStation(catch_config->stationName());
  evt->resolveUrl(station->timeOffset());
  delete station;

  //
  // Execute Download
  //
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("starting download of %s to %s, id=%d",
		  (const char *)evt->resolvedUrl(),
		  (const char *)evt->tempName(),
		  evt->id()));
  evt->setTempName(BuildTempName(evt,"download"));
  RDDownload *conv=new RDDownload(catch_config->stationName(),this);
  conv->setSourceUrl(evt->resolvedUrl());
  conv->setDestinationFile(evt->tempName());
  QString url_username=evt->urlUsername();
  QString url_password=evt->urlPassword();
  if(url_username.isEmpty()&&
     (QUrl(evt->resolvedUrl()).protocol().lower()=="ftp")) {
    url_username=RD_ANON_FTP_USERNAME;
    url_password=QString(RD_ANON_FTP_PASSWORD)+"-"+VERSION;
  }
  switch((conv_err=conv->runDownload(url_username,url_password,
				     catch_config->logXloadDebugData()))) {
  case RDDownload::ErrorOk:
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("finished download of %s to %s, id=%d",
		    (const char *)evt->tempName(),
		    (const char *)evt->resolvedUrl(),
		    evt->id()));
    break;

  case RDDownload::ErrorInternal:
    catch_connect->setExitCode(evt->id(),RDRecording::InternalError,
			       RDDownload::errorText(conv_err));
    qApp->processEvents();
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("download of %s returned an error: \"%s\", id=%d",
		    (const char *)evt->tempName(),
		    (const char *)RDDownload::errorText(conv_err),
		    evt->id()));
    delete conv;
    unlink(evt->tempName());
    exit(0);

  default:
    catch_connect->setExitCode(evt->id(),RDRecording::ServerError,
			       RDDownload::errorText(conv_err));
    qApp->processEvents();
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("download of %s returned an error: \"%s\", id=%d",
		    (const char *)evt->tempName(),
		    (const char *)RDDownload::errorText(conv_err),
		    evt->id()));
    delete conv;
    unlink(evt->tempName());
    exit(0);
  }
  delete conv;
  
  //
  // Execute Import
  //
  if(Import(evt)) {
    catch_connect->setExitCode(evt->id(),RDRecording::Ok,tr("OK"));
    qApp->processEvents();
  }
  LogLine(RDConfig::LogInfo,QString().sprintf("deleting file %s, id=%d",
					      (const char *)evt->tempName(),
					      evt->id()));
  unlink(evt->tempName());
}

void MainObject::RunUpload(CatchEvent *evt)
{
  RDUpload::ErrorCode conv_err;

  //
  // Resolve Wildcards
  //
  RDStation *station=new RDStation(catch_config->stationName());
  evt->resolveUrl(station->timeOffset());
  delete station;

  //
  // Execute Export
  //
  evt->setTempName(BuildTempName(evt,"upload"));
  evt->setDeleteTempFile(true);
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("started export of cut %s to %s, id=%d",
		  (const char *)evt->cutName(),
		  (const char *)evt->tempName(),
		  evt->id()));
  if(!Export(evt)) {
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("export of cut %s returned an error, id=%d",
		    (const char *)evt->cutName(),
		    evt->id()));
    catch_connect->setExitCode(evt->id(),RDRecording::InternalError,
			       tr("Export Error"));
    qApp->processEvents();
    return;
  }
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("finished export of cut %s to %s, id=%d",
		  (const char *)evt->cutName(),
		  (const char *)evt->tempName(),
		  evt->id()));

  //
  // Load Podcast Parameters
  //
  if(evt->feedId()>0) {
    QFile *file=new QFile(evt->tempName());
    evt->setPodcastLength(file->size());
    delete file;
    RDWaveFile *wave=new RDWaveFile(evt->tempName());
    if(wave->openWave()) {
      evt->setPodcastTime(wave->getExtTimeLength());
    }
    delete wave;
  }
  
  //
  // Execute Upload
  //
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("starting upload of %s to %s, id=%d",
		  (const char *)evt->tempName(),
		  (const char *)evt->
		  resolvedUrl(),
		  evt->id()));
  RDUpload *conv=new RDUpload(catch_config->stationName(),this);
  conv->setSourceFile(evt->tempName());
  conv->setDestinationUrl(evt->resolvedUrl());
  QString url_username=evt->urlUsername();
  QString url_password=evt->urlPassword();
  if(url_username.isEmpty()&&
     (QUrl(evt->resolvedUrl()).protocol().lower()=="ftp")) {
    url_username=RD_ANON_FTP_USERNAME;
    url_password=QString(RD_ANON_FTP_PASSWORD)+"-"+VERSION;
  }
  switch((conv_err=conv->runUpload(url_username,url_password,
				   catch_config->logXloadDebugData()))) {
  case RDUpload::ErrorOk:
    catch_connect->setExitCode(evt->id(),RDRecording::Ok,tr("Ok"));
    qApp->processEvents();
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("finished upload of %s to %s, id=%d",
		    (const char *)evt->tempName(),
		    (const char *)evt->resolvedUrl(),
		    evt->id()));
    break;

  case RDUpload::ErrorInternal:
    catch_connect->setExitCode(evt->id(),RDRecording::InternalError,
			       RDUpload::errorText(conv_err));
    qApp->processEvents();
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("upload of %s returned an error: \"%s\", id=%d",
		    (const char *)evt->tempName(),
		    (const char *)RDUpload::errorText(conv_err),
		    evt->id()));
    break;

  default:
    catch_connect->setExitCode(evt->id(),RDRecording::ServerError,
			       RDUpload::errorText(conv_err));
    qApp->processEvents();
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("upload of %s returned an error: \"%s\", id=%d",
		    (const char *)evt->tempName(),
		    (const char *)RDUpload::errorText(conv_err),
		    evt->id()));
    break;
  }
  delete conv;

  //
  // Clean Up
  //
  if(evt->feedId()>0) {
    CheckInPodcast(evt);
  }
  if(evt->deleteTempFile()) {
    unlink(evt->tempName());
    LogLine(RDConfig::LogDebug,QString().sprintf("deleted file %s",
						 (const char *)evt->tempName()));
  }
  else {
    chown(evt->tempName(),catch_config->uid(),
	  catch_config->gid());
  }
}


bool MainObject::Export(CatchEvent *evt)
{
  bool ret=false;
  RDAudioConvert::ErrorCode conv_err;

  RDCut *cut=new RDCut(evt->cutName());
  if(!cut->exists()) {
    LogLine(RDConfig::LogErr,QString().sprintf("Cut not found: %s, id: %d",
					       (const char *)evt->cutName(),
					       evt->id()));
    delete cut;
    return false;
  }
  RDCart *cart=new RDCart(cut->cartNumber());
  RDAudioConvert *conv=new RDAudioConvert(catch_config->stationName(),this);
  conv->setSourceFile(RDCut::pathName(evt->cutName()));
  conv->setRange(cut->startPoint(),cut->endPoint());
  conv->setDestinationFile(RDEscapeString(evt->tempName()));
  RDSettings *settings=new RDSettings();
  settings->setFormat((RDSettings::Format)evt->format());
  settings->setChannels(evt->channels());
  settings->setSampleRate(evt->sampleRate());
  settings->setBitRate(evt->bitrate());
  settings->setQuality(evt->quality());
  settings->setNormalizationLevel(evt->normalizeLevel()/100);
  conv->setDestinationSettings(settings);
  RDWaveData *wavedata=NULL;
  if(evt->enableMetadata()) {
    wavedata=new RDWaveData();
    cart->getMetadata(wavedata);
    cut->getMetadata(wavedata);
    conv->setDestinationWaveData(wavedata);
  }
  switch((conv_err=conv->convert())) {
  case RDAudioConvert::ErrorOk:
    ret=true;
    break;

  default:
    LogLine(RDConfig::LogErr,
	    QString().sprintf("Export error: %s, id: %d",
			      (const char *)RDAudioConvert::errorText(conv_err),
			      evt->id()));
    ret=false;
    break;
  }
  if(wavedata!=NULL) {
    delete wavedata;
  }
  delete settings;
  delete conv;
  delete cart;
  delete cut;
  return ret;
}


bool MainObject::Import(CatchEvent *evt)
{
  bool ret=false;
  RDAudioConvert::ErrorCode conv_err;

  RDCut *cut=new RDCut(evt->cutName());
  if(!cut->exists()) {
    LogLine(RDConfig::LogErr,
	    QString().sprintf("Cut not found: %s, id: %d",
			      (const char *)evt->cutName(),evt->id()));
    catch_connect->setExitCode(evt->id(),RDRecording::NoCut,tr("No such cut"));
    qApp->processEvents();
    delete cut;
    return false;
  }
  RDWaveFile *wave=new RDWaveFile(evt->tempName());
  if(!wave->openWave()) {
    LogLine(RDConfig::LogErr,
	    QString().sprintf("Unknown file format: %s, id: %d",
			      (const char *)evt->cutName(),evt->id()));
    catch_connect->setExitCode(evt->id(),RDRecording::UnknownFormat,
			       tr("Unknown Format"));
    qApp->processEvents();
    delete wave;
    return false;
  }
  unsigned msecs=wave->getExtTimeLength();
  delete wave;
  RDCart *cart=new RDCart(cut->cartNumber());
  RDAudioConvert *conv=new RDAudioConvert(catch_config->stationName(),this);
  conv->setSourceFile(RDEscapeString(evt->tempName()));
  conv->setDestinationFile(RDCut::pathName(evt->cutName()));
  RDSettings *settings=new RDSettings();
  switch(evt->format()) {
  case RDCae::Pcm16:
    settings->setFormat(RDSettings::Pcm16);
    break;

  case RDCae::MpegL1:
  case RDCae::MpegL2:
  case RDCae::MpegL3:
    settings->setFormat(RDSettings::MpegL2Wav);
    break;
  }
  settings->setChannels(evt->channels());
  settings->setSampleRate(catch_system->sampleRate());
  settings->setBitRate(evt->bitrate());
  settings->setNormalizationLevel(evt->normalizeLevel()/100);
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("started import of %s to cut %s, id=%d",
		  (const char *)evt->tempName(),
		  (const char *)evt->cutName(),
		  evt->id()));
  conv->setDestinationSettings(settings);
  switch((conv_err=conv->convert())) {
  case RDAudioConvert::ErrorOk:
    CheckInRecording(evt->cutName(),evt,msecs,evt->trimThreshold());
    ret=true;
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
    LogLine(RDConfig::LogErr,
	    QString().sprintf("Import error: %s, id: %d",
			      (const char *)RDAudioConvert::errorText(conv_err),
			      evt->id()));
    catch_connect->setExitCode(evt->id(),RDRecording::UnknownFormat,
			       RDAudioConvert::errorText(conv_err));
    qApp->processEvents();
    ret=false;
    break;

  default:
    LogLine(RDConfig::LogErr,
	    QString().sprintf("Import error: %s, id: %d",
			      (const char *)RDAudioConvert::errorText(conv_err),
			      evt->id()));
    catch_connect->setExitCode(evt->id(),RDRecording::InternalError,
			       RDAudioConvert::errorText(conv_err));
    qApp->processEvents();
    ret=false;
    break;
  }
  if((conv->sourceWaveData()!=NULL)&&(evt->enableMetadata())) {
    cart->setMetadata(conv->sourceWaveData());
    cut->setMetadata(conv->sourceWaveData());
  }
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("completed import of %s to cut %s, id=%d",
		  (const char *)evt->tempName(),
		  (const char *)evt->cutName(),
		  evt->id()));
  if(evt->deleteTempFile()) {
    unlink(evt->tempName());
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("deleted file %s",
		    (const char *)evt->tempName()));
  }
  delete settings;
  delete conv;
  delete cart;
  delete cut;

  return ret;
}


