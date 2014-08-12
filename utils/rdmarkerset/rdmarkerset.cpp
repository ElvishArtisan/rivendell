// rdmarkerset.cpp
//
// Command-line tool for setting Rivendell Cut Markers
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmarkerset.cpp,v 1.1.2.1 2014/01/16 02:45:00 cvs Exp $
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

#include <limits.h>
#include <glob.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <rddb.h>
#include <rd.h>
#include <rdmarkerset.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdlog.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>
#include <rdgroup.h>
#include <rdwavefile.h>
#include <rdaudioinfo.h>
#include <rdtrimaudio.h>
#include <dbversion.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool ok=false;
  bool skip_db_check=false;
  unsigned schema=0;
  set_all_groups=false;
  set_auto_trim=1;
  set_verbose=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdmarkerset",RDMARKERSET_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--all-groups") {
      set_all_groups=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--group") {
      set_group_names.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }    
    if(cmd->key(i)=="--auto-trim") {
      set_auto_trim=cmd->value(i).toInt(&ok);
      if((!ok)||(set_auto_trim>0)) {
	fprintf(stderr,
		"rdmarkerset: invalid level value specified for --auto-trim\n");
      }
      cmd->setProcessed(i,true);
    }    
    if(cmd->key(i)=="--verbose") {
      set_verbose=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdmarkerset: unrecognized option \"%s\"\n",
	      (const char *)cmd->key(i));
    }
  }

  //
  // Sanity Checks
  //
  if((!set_all_groups)&&(set_group_names.size()==0)) {
    fprintf(stderr,"rdmarkerset: either --all-groups or --group=<group> options must be specified\n");
    exit(256);
  }
  if(set_all_groups&&(set_group_names.size()>0)) {
    fprintf(stderr,"rdmarkerset: the --all-groups and --group=<group> options are mutually exclusive\n");
    exit(256);
  }

  //
  // Check for Root Perms
  //
  if(geteuid()!=0) {
    fprintf(stderr,"rdmarkerset: must be user \"root\"\n");
    exit(256);
  }

  //
  // Read Configuration
  //
  set_config=new RDConfig();
  set_config->load();

  //
  // Open Database
  //
  QString err (tr("rdmarkerset: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdmarkerset: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }

  //
  // Validate Station
  //
  set_station=new RDStation(set_config->stationName());
  if(!set_station->exists()) {
    fprintf(stderr,"rdmarkerset: no such host [\"%s\"]\n",
	    (const char *)set_config->stationName());
    exit(256);
  }  

  //
  // Validate Groups
  //
  if(set_all_groups) {
    QString sql="select NAME from GROUPS order by NAME";
    QSqlQuery *q=new QSqlQuery(sql);
    while(q->next()) {
      set_group_names.push_back(q->value(0).toString());
    }
    delete q;
  }
  else {
    for(unsigned i=0;i<set_group_names.size();i++) {
      bool bad=false;
      RDGroup *grp=new RDGroup(set_group_names[i]);
      if(!grp->exists()) {
	fprintf(stderr,"rdmarkerset: no such group named \"%s\"\n",
		(const char *)set_group_names[i]);
	bad=true;
      }
      delete grp;
      if(bad) {
	exit(256);
      }
    }
  }

  //
  // RIPCD Connection
  //
  set_user=NULL;
  set_ripc=new RDRipc(set_config->stationName(),this);
  connect(set_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  set_ripc->connectHost("localhost",RIPCD_TCP_PORT,set_config->password());
}


void MainObject::userChangedData()
{
  if(set_user!=NULL) {
    fprintf(stderr,"rdmarkerset: change of user context ignored\n");
    return;
  }
  set_user=new RDUser(set_ripc->user());

  for(unsigned i=0;i<set_group_names.size();i++) {
    Print("Processing group \""+set_group_names[i]+"\"...");
    ProcessGroup(set_group_names[i]);
    Print("");
  }

  exit(0);
}


void MainObject::ProcessGroup(const QString &group_name)
{
  QString sql;
  QSqlQuery *q;

  sql=QString("select CUTS.CUT_NAME,CART.TITLE,CUTS.DESCRIPTION ")+
    "from CART left join CUTS "+
    "on CART.NUMBER=CUTS.CART_NUMBER where (CART.GROUP_NAME=\""+
    RDEscapeString(group_name)+"\")&&"+
    QString().sprintf("(CART.TYPE!=%d)",RDCart::Macro);
  q=new QSqlQuery(sql);
  while(q->next()) {
    if(set_auto_trim<0) {
      SetAutoTrim(RDCut::cartNumber(q->value(0).toString()),
		  RDCut::cutNumber(q->value(0).toString()),
		  q->value(1).toString(),q->value(2).toString());
    }
    else {
      ClearAutoTrim(RDCut::cartNumber(q->value(0).toString()),
		    RDCut::cutNumber(q->value(0).toString()),
		    q->value(1).toString(),q->value(2).toString());
    }
  }
  delete q;
}


void MainObject::SetAutoTrim(unsigned cartnum,int cutnum,const QString &title,
			     const QString &desc)
{
  RDTrimAudio::ErrorCode err;
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDTrimAudio *trimmer=new RDTrimAudio(set_station,set_config,this);
  trimmer->setCartNumber(cartnum);
  trimmer->setCutNumber(cutnum);
  trimmer->setTrimLevel(100*set_auto_trim);
  if((err=trimmer->runTrim(set_user->name(),set_user->password()))==
     RDTrimAudio::ErrorOk) {
    int start=trimmer->startPoint();
    int end=trimmer->endPoint();
    if(cut->talkStartPoint()>=0) {
      if(start>cut->talkStartPoint()) {
	cut->setTalkStartPoint(start);
      }
    }
    if(cut->talkEndPoint()>=0) {
      if(end<cut->talkEndPoint()) {
	cut->setTalkEndPoint(end);
      }
    }
    if(cut->segueStartPoint()>=0) {
      if(start>cut->segueStartPoint()) {
	cut->setSegueStartPoint(start);
      }
    }
    if(cut->segueEndPoint()>=0) {
      if(end<cut->segueEndPoint()) {
	cut->setSegueEndPoint(end);
      }
    }
    if(cut->hookStartPoint()>=0) {
      if(start>cut->hookStartPoint()) {
	cut->setHookStartPoint(start);
      }
    }
    if(cut->hookEndPoint()>=0) {
      if(end<cut->hookEndPoint()) {
	cut->setHookEndPoint(end);
      }
    }
    if(cut->fadeupPoint()>=0) {
      if(start>cut->fadeupPoint()) {
	cut->setFadeupPoint(-1);
      }
    }
    if(cut->fadedownPoint()>=0) {
      if(end<cut->fadedownPoint()) {
	cut->setFadedownPoint(-1);
      }
    }
    cut->setStartPoint(start);
    cut->setEndPoint(end);
    cut->setLength(end-start);
    cart->updateLength();
    Print(QString().sprintf("  auto-trimming %06u / %03d [",
			    cartnum,cutnum)+title+" / "+desc+
	  QString().sprintf("] to %d dBFS",set_auto_trim));
  }
  else {
    if(err!=RDTrimAudio::ErrorNoAudio) {
      fprintf(stderr,"rdmarkerset: cart %06u, cut %d trimmer error [%s]\n",
	      cartnum,cutnum,(const char *)RDTrimAudio::errorText(err));
      exit(256);
    }
  }
  delete trimmer;
  delete cut;
  delete cart;
}


void MainObject::ClearAutoTrim(unsigned cartnum,int cutnum,const QString &title,
			       const QString &desc)
{
  RDAudioInfo::ErrorCode err;
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDAudioInfo *info=new RDAudioInfo(set_station,set_config,this);
  info->setCartNumber(cartnum);
  info->setCutNumber(cutnum);
  if((err=info->runInfo(set_user->name(),set_user->password()))==
     RDAudioInfo::ErrorOk) {
    cut->setStartPoint(0);
    cut->setEndPoint(info->length());
    cut->setLength(info->length());
    cart->updateLength();
    Print(QString().sprintf("  clearing auto-trim from %06u / %03d [",
			    cartnum,cutnum)+title+" / "+desc+"]");
  }
  else {
    if(err!=RDAudioInfo::ErrorNoAudio) {
      fprintf(stderr,"rdmarkerset: cart %06u, cut %d info error [%s]\n",
	      cartnum,cutnum,(const char *)RDAudioInfo::errorText(err));
      exit(256);
    }
  }
  delete info;
  delete cut;
  delete cart;
}


void MainObject::Print(const QString &msg)
{
  if(set_verbose) {
    printf("%s\n",(const char *)msg);
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
