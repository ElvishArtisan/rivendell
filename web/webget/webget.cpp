// webget.cpp
//
// Rivendell upload/download utility
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/sha.h>

#include <QApplication>
#include <QProcess>

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdgroup.h>
#include <rdsendmail.h>
#include <rdweb.h>

#include "webget.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  webget_post=NULL;

  //
  // Open the Database
  //
  rda=new RDApplication("webget.cgi","webget.cgi",WEBGET_CGI_USAGE,this);
  if(!rda->open(&err_msg)) {
    TextExit(err_msg,500,LINE_NUMBER);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      TextExit("unknown command option",500,LINE_NUMBER);
    }
  }

  //
  // Drop root permissions
  //
  if(setgid(rda->config()->gid())<0) {
    TextExit("Unable to set Rivendell group",500,LINE_NUMBER);
  }
  if(setuid(rda->config()->uid())<0) {
    TextExit("Unable to set Rivendell user",500,LINE_NUMBER);
  }
  if(getuid()==0) {
    TextExit("Rivendell user should never be \"root\"!",500,LINE_NUMBER);
  }

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    rda->syslog(LOG_WARNING,"missing request method");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("missing REQUEST_METHOD",500,LINE_NUMBER);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()=="get") {
    ServeLogin(200);
    Exit(0);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    rda->syslog(LOG_WARNING,"unsupported web method \"%s\"",
		getenv("REQUEST_METHOD"));
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("invalid web method",400,LINE_NUMBER);
  }
  if(getenv("REMOTE_ADDR")!=NULL) {
    webget_remote_address.setAddress(getenv("REMOTE_ADDR"));
  }
  if(getenv("REMOTE_HOST")!=NULL) {
    webget_remote_hostname=getenv("REMOTE_HOST");
  }
  if(webget_remote_hostname.isEmpty()) {
    webget_remote_hostname=webget_remote_address.toString();
  }

  //
  // Generate Post
  //
  webget_post=new RDFormPost(RDFormPost::AutoEncoded,false);
  if(webget_post->error()!=RDFormPost::ErrorOk) {
    rda->syslog(LOG_WARNING,"post parsing error [%s]",
		webget_post->errorString(webget_post->error()).
		toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit(webget_post->errorString(webget_post->error()),400,LINE_NUMBER);
    Exit(0);
  }

  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    ServeLogin(403);
    Exit(0);
  }

  //
  // Connect to ripcd(8)
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::ripcConnectedData(bool state)
{
  if(!state) {
    TextExit("unable to connect to ripc service",500,LINE_NUMBER);
  }

  QString direction;
  if(!webget_post->getValue("direction",&direction)) {
    ServeForm();
    Exit(0);
  }
  if(direction.toLower()=="get") {
    GetAudio();
    Exit(0);
  }
  if(direction.toLower()=="put") {
    PutAudio();
    Exit(0);
  }

  rda->syslog(LOG_WARNING,"invalid webget direction \"%s\" from %s",
	      direction.toUtf8().constData(),
	      webget_post->clientAddress().toString().toUtf8().constData());
  rda->logAuthenticationFailure(webget_post->clientAddress()); // So Fail2Ban can notice this
  ServeLogin(403);
}


void MainObject::GetAudio()
{
  bool ok=false;

  QString title;
  if(!webget_post->getValue("title",&title)) {
    rda->syslog(LOG_WARNING,"missing \"title\"");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("missing \"title\"",400,LINE_NUMBER);
    Exit(0);
  }

  unsigned preset;
  if(!webget_post->getValue("preset",&preset,&ok)) {
    rda->syslog(LOG_WARNING,"missing \"preset\"");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("missing \"preset\"",400,LINE_NUMBER);
  }
  if(!ok) {
    rda->syslog(LOG_WARNING,"invalid \"preset\" value");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("invalid \"preset\" value",400,LINE_NUMBER);
  }

  unsigned cartnum=0;
  int cutnum=0;
  QString sql=QString("select ")+
    "CUTS.CUT_NAME from "+
    "CART left join CUTS on CART.NUMBER=CUTS.CART_NUMBER where "+
    "CART.TITLE=\""+RDEscapeString(title)+"\" && "+
    QString().sprintf("CART.TYPE=%d ",RDCart::Audio)+
    "order by CUTS.CUT_NAME";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    cartnum=RDCut::cartNumber(q->value(0).toString());
    cutnum=RDCut::cutNumber(q->value(0).toString());
  }
  delete q;
  if(cartnum==0) {
    printf("Content-type: text/html\n");
    printf("Status: 404\n");
    printf("\n");
    printf("No such cart!\n");
    Exit(0);
  }

  //
  // Audio Settings
  //
  RDSettings *settings=new RDSettings();
  if(!settings->loadPreset(preset)) {
    printf("Content-type: text/html\n");
    printf("Status: 400\n");
    printf("\n");
    printf("no such preset!\n");
    rda->syslog(LOG_WARNING,"no such preset %u",preset);
    rda->logAuthenticationFailure(webget_post->clientAddress());
    Exit(0);
  }

  //
  // Generate Metadata
  //
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDWaveData *wavedata=NULL;
  QString rdxl;
  float speed_ratio=1.0;
  wavedata=new RDWaveData();
  if(wavedata!=NULL) {
    cart->getMetadata(wavedata);
    cut->getMetadata(wavedata);
    if(cart->enforceLength()) {
      speed_ratio=(float)cut->length()/(float)cart->forcedLength();
    }
    rdxl=cart->xml(true,false,settings,cutnum);
  }

  //
  // Export Cut
  //
  int fd;
  ssize_t n;
  uint8_t data[2048];
  QString err_msg;
  RDTempDirectory *tempdir=new RDTempDirectory("rdxport-export");
  if(!tempdir->create(&err_msg)) {
    rda->syslog(LOG_WARNING,"unable to create temporary directory [%s]",
		err_msg.toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("unable to create temporary directory ["+err_msg+"]",500,
	     LINE_NUMBER);
  }
  QString tmpfile=tempdir->path()+"/exported_audio";
  RDAudioConvert *conv=new RDAudioConvert(this);
  conv->setSourceFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationFile(tmpfile);
  conv->setDestinationSettings(settings);
  conv->setDestinationWaveData(wavedata);
  conv->setDestinationRdxl(rdxl);
  conv->setRange(cut->startPoint(),cut->endPoint());
  conv->setSpeedRatio(speed_ratio);
  delete cut;
  delete cart;
  RDAudioConvert::ErrorCode conv_err;
  int resp_code=0;
  switch(conv_err=conv->convert()) {
  case RDAudioConvert::ErrorOk:
    switch(settings->format()) {
    case RDSettings::Pcm16:
    case RDSettings::Pcm24:
      printf("Content-type: audio/x-wav\n\n");
      break;

    case RDSettings::MpegL1:
    case RDSettings::MpegL2:
    case RDSettings::MpegL2Wav:
    case RDSettings::MpegL3:
      printf("Content-type: audio/x-mpeg\n\n");
      break;

    case RDSettings::OggVorbis:
      printf("Content-type: audio/ogg\n\n");
      break;

    case RDSettings::Flac:
      printf("Content-type: audio/flac\n\n");
      break;
    }
    fflush(NULL);
    if((fd=open(tmpfile,O_RDONLY))>=0) {
      while((n=read(fd,data,2048))>0) {
	write(1,data,n);
      }
    }
    close(fd);
    unlink(tmpfile);
    delete tempdir;
    Exit(0);
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
  case RDAudioConvert::ErrorInvalidSettings:
    resp_code=415;
    break;

  case RDAudioConvert::ErrorNoSource:
    resp_code=404;
    break;

  case RDAudioConvert::ErrorNoDestination:
  case RDAudioConvert::ErrorInvalidSource:
  case RDAudioConvert::ErrorNoSpace:
  case RDAudioConvert::ErrorInternal:
  case RDAudioConvert::ErrorNoDisc:
  case RDAudioConvert::ErrorNoTrack:
  case RDAudioConvert::ErrorInvalidSpeed:
  case RDAudioConvert::ErrorFormatError:
    resp_code=500;
    break;
  }
  delete conv;
  delete settings;
  if(wavedata!=NULL) {
    delete wavedata;
  }
  delete tempdir;
  if(resp_code==200) {
    Exit(200);
  }
  else {
    rda->syslog(LOG_WARNING,"%s",
		RDAudioConvert::errorText(conv_err).toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit(RDAudioConvert::errorText(conv_err),resp_code,LINE_NUMBER);
  }
}


void MainObject::PutAudio()
{
  QString sql;
  RDSqlQuery *q=NULL;
  QString err_msg;
  RDGroup *group;

  //
  // Verify permissions
  //
  if(!rda->user()->createCarts()) {
    rda->syslog(LOG_WARNING,"user \"%s\" lacks CreateCarts permission",
		rda->user()->name().toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    ServeLogin(403);
    Exit(0);
  }
  if(!rda->user()->editAudio()) {
    rda->syslog(LOG_WARNING,"user \"%s\" lacks EditAudio permission",
		rda->user()->name().toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    ServeLogin(403);
    Exit(0);
  }

  //
  // Get user values
  //
  QString group_name;
  if(!webget_post->getValue("group",&group_name)) {
    rda->syslog(LOG_WARNING,"missing \"group\" in put submission");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("missing \"group\"",400,LINE_NUMBER);
    Exit(0);
  }
  if(!rda->user()->groupAuthorized(group_name)) {
    rda->syslog(LOG_WARNING,"user \"%s\" lacks permission for group \"%s\"",
		rda->user()->name().toUtf8().constData(),
		group_name.toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    ServeLogin(403);
    Exit(0);
  }
  group=new RDGroup(group_name);
  if(!group->exists()) {
    rda->syslog(LOG_WARNING,"group \"%s\" does not exist",
		group_name.toUtf8().constData());
    rda->logAuthenticationFailure(webget_post->clientAddress());
    ServeLogin(403);
    Exit(0);
  }

  QString filename;
  if(!webget_post->getValue("filename",&filename)) {
    rda->syslog(LOG_WARNING,"missing \"filename\" in put submission");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("missing \"missing\"",400,LINE_NUMBER);
    Exit(0);
  }

  if(!webget_post->isFile("filename")) {
    rda->syslog(LOG_WARNING,"\"filename\" is not a file in put submission");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    TextExit("invalid \"filename\"",400,LINE_NUMBER);
    Exit(0);
  }

  //
  // Generate Title
  //
  QStringList f0=filename.split("/",QString::SkipEmptyParts);
  QStringList f1=f0.last().split(".",QString::KeepEmptyParts);
  QString short_name=f0.last();
  f1.removeLast();
  QString title=f1.join(".");

  //
  // Generate destination e-mail addresses
  //
  QStringList to_addrs;
  if(!group->notifyEmailAddress().isEmpty()) {
    to_addrs.push_back(group->notifyEmailAddress());
  }
  if(!rda->user()->emailAddress().isEmpty()) {
    to_addrs.push_back(rda->user()->emailAddress());
  }

  //
  // Validate title uniqueness
  //
  if((!rda->system()->allowDuplicateCartTitles())&&
     (!rda->system()->fixDuplicateCartTitles())) {
    sql=QString("select ")+
      "NUMBER "+  // 00
      "from CART where "+
      "TITLE=\""+RDEscapeString(title)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      QString body;
      body+="Rivendell File Import Report\n";
      body+="\n";
      body+="IMPORT FAILED!\n";
      body+="\n";
      body+="Filename: "+short_name+"\n";
      body+="Submitted by: "+rda->user()->emailContact()+"\n";
      body+="Group: "+group_name+"\n";
      body+="Reason: Title exists\n";
      rda->syslog(LOG_WARNING,
	  "failed import of file \"%s\" from \"%s\" at %s, title already exists",
		  short_name.toUtf8().constData(),
		  rda->user()->name().toUtf8().constData(),
		  webget_post->clientAddress().toString().toUtf8().constData());
      if(to_addrs.size()>0) {
	RDSendMail(&err_msg,tr("Rivendell import FAILURE for file: ")+
		   short_name,body,rda->system()->originEmailAddress(),
		   to_addrs);
	//group->notifyEmailAddress());
      }
      TextExit(tr("Audio import failed: title already exists!"),400,
	       LINE_NUMBER);
    }
    delete q;
  }

  //
  // Import audio
  //
  QStringList args;

  args.push_back(QString("--ticket=")+webget_ticket+":"+
  		 webget_post->clientAddress().toString());
  args.push_back("--send-mail");
  args.push_back("--mail-per-file");
  if(rda->libraryConf()->defaultChannels()==1) {
    args.push_back("--to-mono");
  }
  args.push_back("--output-pattern=Added cart %n [%t]");
  args.push_back("--set-string-title="+title);
  args.push_back(group_name);
  args.push_back(filename);
  QProcess *proc=new QProcess(this);
  proc->start("rdimport",args);
  proc->waitForFinished();
  if(proc->exitStatus()==QProcess::CrashExit) {
    delete proc;
    rda->syslog(LOG_WARNING,"importer process crashed [cmdline: %s]",
		("rdimport "+args.join(" ")).toUtf8().constData());
    TextExit("Importer process crashed!",500,LINE_NUMBER);
  }
  switch((RDApplication::ExitCode)proc->exitCode()) {
  case RDApplication::ExitOk:
  case RDApplication::ExitLast:
    break;

  case RDApplication::ExitPriorInstance:
  case RDApplication::ExitNoDb:
  case RDApplication::ExitSvcFailed:
  case RDApplication::ExitInvalidOption:
  case RDApplication::ExitOutputProtected:
  case RDApplication::ExitNoSvc:
  case RDApplication::ExitNoLog:
  case RDApplication::ExitNoReport:
  case RDApplication::ExitLogGenFailed:
  case RDApplication::ExitLogLinkFailed:
  case RDApplication::ExitNoPerms:
  case RDApplication::ExitReportFailed:
  case RDApplication::ExitNoDropbox:
  case RDApplication::ExitNoGroup:
  case RDApplication::ExitInvalidCart:
  case RDApplication::ExitNoSchedCode:
  case RDApplication::ExitBadTicket:
    rda->syslog(LOG_WARNING,
		"importer process returned exit code %d [cmdline: %s, client addr: %s]",
		proc->exitCode(),
		("rdimport "+args.join(" ")).toUtf8().constData(),
		webget_post->clientAddress().toString().toUtf8().constData());
    delete proc;
    TextExit("Internal error! See syslog for details.",500,LINE_NUMBER);

  case RDApplication::ExitImportFailed:
    rda->syslog(LOG_WARNING,
		"file importation failed [cmdline: %s, client addr: %s]",
		("rdimport "+args.join(" ")).toUtf8().constData(),
		webget_post->clientAddress().toString().toUtf8().constData());
    TextExit("File importation failed!\nSee syslog for details.",500,LINE_NUMBER);
  }

  QString resultstr=proc->readAllStandardOutput();
  rda->syslog(LOG_INFO,"%s from user \"%s\" at %s",
	      resultstr.trimmed().toUtf8().constData(),
	      rda->user()->name().toUtf8().constData(),
	      webget_post->clientAddress().toString().toUtf8().constData());
  TextExit(resultstr.toUtf8().constData(),200,LINE_NUMBER);
}


void MainObject::ServeForm()
{
  QString sql;
  RDSqlQuery *q=NULL;
  time_t t=time(NULL);

  printf("Content-type: text/html\n\n");

  printf("<html>\n");
  printf("  <head>\n");
  printf("    <title>Rivendell Webget [User: %s]</title>\n",
	 rda->user()->name().toUtf8().constData());
  printf("    <script src=\"webget.js?%lu\" type=\"application/javascript\"></script>\n",t);
  printf("    <script type=\"application/javascript\">\n");
  printf("      var preset_ids=new Array();\n");
  printf("      var preset_exts=new Array();\n");
  sql=QString("select ")+
    "ID,"+      // 00
    "FORMAT "+  // 01
    "from ENCODER_PRESETS order by ID";
  int count=0;
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("      preset_ids[%d]=%d;\n",count,q->value(0).toInt());
    printf("      preset_exts[%d]='%s';\n",
	   count,RDSettings::defaultExtension((RDSettings::Format)q->value(1).toUInt()).toUtf8().constData());
    count++;
  }
  delete q;
  printf("    </script>\n");
  printf("  </head>\n");

  printf("  <body>\n");

  //
  // Credentials
  //
  printf("    <input type=\"hidden\" name=\"TICKET\" id=\"TICKET\" value=\"%s\">\n",
	 webget_ticket.toUtf8().constData());

  //
  // Get Audio
  //
  printf("    <table style=\"margin: auto;padding: 10px 0\" cellpadding=\"0\" cellspacing=\"5\" border=\"0\">\n");
  printf("    <tr>\n");
  printf("	<td colspan=\"2\"><img src=\"logos/webget_logo.png\" border=\"0\"></td>\n");
  printf("    </tr>\n");
  printf("    <tr>\n");
  printf("	<td colspan=\"2\"><strong>Get audio from Rivendell</strong></td>\n");
  printf("    </tr>\n");
  printf("    <tr><td colspan=\"2\"><hr></td></tr>\n");
  printf("    <tr>\n");
  printf("	<td style=\"text-align: right\">From Cart Title:</td>\n");
  printf("	<td><input type=\"text\" id=\"title\" size=\"40\" oninput=\"TitleChanged()\"></td>\n");
  printf("    </tr>\n");

  printf("    <tr>\n");
  printf("	<td style=\"text-align: right\">Using Format:</td>\n");
  printf("	<td>\n");
  printf("	  <select id=\"preset\">\n");
  sql=QString("select ")+
    "ID,"+    // 00
    "NAME "+  // 01
    "from ENCODER_PRESETS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("          <option value=\"%u\">%s</option>\n",
	   q->value(0).toUInt(),q->value(1).toString().toUtf8().constData());
  }
  delete q;
  printf("	  </select>\n");
  printf("	</td>\n");
  printf("    </tr>\n");
  printf("    <tr>\n");
  printf("	<td style=\"text-align: right\" id=\"get_spinner\">&nbsp;</td>\n");
  printf("	<td><input type=\"button\" value=\"OK\" id=\"get_button\" onclick=\"ProcessGet()\" disabled></td>\n");
  printf("    </tr>\n");

  printf("    <tr><td>&nbsp;</td></tr>\n");

  //
  // Put Audio
  //
  if(rda->user()->createCarts()) {
    printf("    <tr>\n");
    printf("	<td colspan=\"2\"><strong>Put audio into Rivendell</strong></td>\n");
    printf("    </tr>\n");
    printf("    <tr><td colspan=\"2\"><hr></td></tr>\n");
    printf("    <tr>\n");
    printf("	<td style=\"text-align: right\">From File:</td>\n");
    printf("	<td><input type=\"file\" id=\"filename\" size=\"40\" accept=\"audio/*\" onchange=\"FilenameChanged()\"></td>\n");
    printf("    </tr>\n");

    printf("    <tr>\n");
    printf("	<td style=\"text-align: right\">To Group:</td>\n");
    printf("	<td>\n");
    printf("	  <select id=\"group\">\n");
    sql=QString("select ")+
      "GROUPS.NAME "+  // 00
      "from GROUPS left join USER_PERMS "+
      "on GROUPS.NAME=USER_PERMS.GROUP_NAME where "+
      "USER_PERMS.USER_NAME=\""+RDEscapeString(rda->user()->name())+"\" && "+
      QString().sprintf("GROUPS.DEFAULT_CART_TYPE=%u && ",RDCart::Audio)+
      "GROUPS.DEFAULT_LOW_CART>0 && "+
      "GROUPS.DEFAULT_HIGH_CART>0 "+
      "order by GROUPS.NAME";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      printf("          <option value=\"%s\">%s</option>\n",
	     q->value(0).toString().toUtf8().constData(),
	     q->value(0).toString().toUtf8().constData());
    }
    printf("	  </select>\n");
    printf("	</td>\n");
    printf("    </tr>\n");
    printf("    <tr>\n");
    printf("	<td style=\"text-align: right\" id=\"put_spinner\">&nbsp;</td>\n");
    printf("	<td><input type=\"button\" value=\"OK\" id=\"put_button\" onclick=\"ProcessPut()\" disabled></td>\n");
    printf("    </tr>\n");
  }

  //
  // Footer
  //
  printf("    </table>\n");
  printf("  </body>\n");
  printf("</html>\n");
}


void MainObject::ServeLogin(int resp_code)
{
  printf("Content-type: text/html\n");
  printf("Status: %d\n",resp_code);
  printf("\n");

  //
  // Head
  //
  printf("<html>\n");
  printf("  <head>\n");
  printf("    <title>Rivendell Webget</title>\n");
  printf("  </head>\n");

  //
  // Body
  //
  printf("  <body>\n");
  printf("  <form action=\"/rd-bin/webget.cgi\" method=\"post\" enctype=\"multipart/form-data\">\n");
  printf("    <table style=\"margin: auto;padding: 10px 0\" cellpadding=\"0\" cellspacing=\"5\" border=\"0\">\n");
  printf("    <tr>\n");
  printf("	<td colspan=\"2\"><img src=\"logos/webget_logo.png\" border=\"0\"></td>\n");
  printf("    </tr>\n");
  printf("    <tr>\n");
  printf("	<td colspan=\"2\"><strong>Log in to Rivendell</strong></td>\n");
  printf("    </tr>\n");
  printf("    <tr><td colspan=\"2\"><hr></td></tr>\n");

  printf("    <td style=\"text-align: right\">User Name:</td>\n");
  printf("    <td><input type=\"text\" size=\"32\" name=\"LOGIN_NAME\" autofocus></td>\n");
  printf("    </tr>\n");
  printf("      <tr>\n");
  printf("    <td style=\"text-align: right\">Password:</td>\n");
  printf("    <td><input type=\"password\" size=\"32\" maxsize=\"32\" name=\"PASSWORD\"></td>\n");
  printf("    <tr><td cellspan=\"2\">&nbsp</td></tr>\n");
  printf("    <tr>\n");
  printf("	<td>&nbsp;</td>\n");
  printf("	<td><input type=\"submit\" value=\"OK\"></td>\n");
  printf("    </tr>\n");

  printf("    </table>\n");
  printf("  </form>\n");
  printf("  </body>\n");
  printf("</html>\n");
}


bool MainObject::Authenticate()
{
  QDateTime expire_dt;

  //
  // First try ticket authentication
  //
  if(webget_post->getValue("TICKET",&webget_ticket)) {
    if(RDUser::ticketIsValid(webget_ticket,webget_post->clientAddress(),
			     &webget_remote_username)) {
      rda->user()->setName(webget_remote_username);
      if(!rda->user()->webgetLogin()) {
	rda->logAuthenticationFailure(webget_post->clientAddress(),
				      webget_remote_username);
	return false;
      }
      return true;
    }
  }

  //
  // Then, try to authenticate by username/password
  //
  if(!webget_post->getValue("LOGIN_NAME",&webget_remote_username)) {
    rda->syslog(LOG_WARNING,"missing LOGIN_NAME");
    rda->logAuthenticationFailure(webget_post->clientAddress());
    return false;
  }
  if(!webget_post->getValue("PASSWORD",&webget_remote_password)) {
    rda->syslog(LOG_WARNING,"missing PASSWORD");
    rda->logAuthenticationFailure(webget_post->clientAddress(),
				  webget_remote_username);
    return false;
  }
  rda->user()->setName(webget_remote_username);
  if((!rda->user()->exists())||
     (!rda->user()->checkPassword(webget_remote_password,false))||
     (!rda->user()->webgetLogin())) {
    rda->logAuthenticationFailure(webget_post->clientAddress(),
				  webget_remote_username);
    return false;
  }
  if(!rda->user()->createTicket(&webget_ticket,&expire_dt,
				webget_post->clientAddress())) {
    rda->logAuthenticationFailure(webget_post->clientAddress(),
				  webget_remote_username);
    return false;
  }

  return true;
}


void MainObject::Exit(int code)
{
  if(webget_post!=NULL) {
    delete webget_post;
  }
  exit(code);
}


void MainObject::TextExit(const QString &msg,int code,int srcline) const
{
  if(webget_post!=NULL) {
    delete webget_post;
  }

  printf("Content-type: text/html\n");
  printf("Status: %d\n",code);
  printf("\n");
  printf("%s\n",msg.toUtf8().constData());
#ifdef RDXPORT_DEBUG
  printf("\n");
  printf("[Line: %d]\n",srcline);
#endif  // RDXPORT_DEBUG
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
