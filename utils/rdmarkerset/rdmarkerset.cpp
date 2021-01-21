// rdmarkerset.cpp
//
// Command-line tool for setting Rivendell Cut Markers
//
//   (C) Copyright 2014,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
//Added by qt3to4:
#include <QSqlQuery>

#include <rd.h>
#include <rdapplication.h>
#include <rdaudioinfo.h>
#include <rddb.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdescape_string.h>
#include <rdgroup.h>
#include <rdlog.h>
#include <rdtrimaudio.h>
#include <rdwavefile.h>

#include "rdmarkerset.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  bool ok=false;
  QString err_msg;

  set_all_groups=false;
  set_auto_trim=1;
  set_verbose=false;
  set_auto_trim=1;
  set_auto_segue=1;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rdmarkerset","rdmarkerset",RDMARKERSET_USAGE,this));
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdmarkerset: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--all-groups") {
      set_all_groups=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--group") {
      set_group_names.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }    
    if(rda->cmdSwitch()->key(i)=="--auto-trim") {
      set_auto_trim=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(set_auto_trim>0)) {
	fprintf(stderr,
		"rdmarkerset: invalid level value specified for --auto-trim\n");
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }    
    if(rda->cmdSwitch()->key(i)=="--auto-segue") {
      set_auto_segue=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(set_auto_segue>0)) {
	fprintf(stderr,
	       "rdmarkerset: invalid level value specified for --auto-segue\n");
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }    
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      set_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdmarkerset: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
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
  if((set_auto_trim<0)&&(set_auto_segue<0)&&(set_auto_trim>set_auto_segue)) {
    fprintf(stderr,"rdmarkerset: segue cannot be placed after the end marker\n");
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
  // Validate Station
  //
  //  set_station=new RDStation(set_config->stationName());
  if(!rda->station()->exists()) {
    fprintf(stderr,"rdmarkerset: no such host [\"%s\"]\n",
	    (const char *)rda->config()->stationName());
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
  connect(rda,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userChangedData()
{
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
      if(set_auto_trim==0) {
	ClearAutoTrim(RDCut::cartNumber(q->value(0).toString()),
		      RDCut::cutNumber(q->value(0).toString()),
		      q->value(1).toString(),q->value(2).toString());
      }
    }
    if(set_auto_segue<0) {
      SetAutoSegue(RDCut::cartNumber(q->value(0).toString()),
		  RDCut::cutNumber(q->value(0).toString()),
		  q->value(1).toString(),q->value(2).toString());
    }
    else {
      if(set_auto_segue==0) {
	ClearAutoSegue(RDCut::cartNumber(q->value(0).toString()),
		       RDCut::cutNumber(q->value(0).toString()),
		       q->value(1).toString(),q->value(2).toString());
      }
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
  RDTrimAudio *trimmer=new RDTrimAudio(rda->station(),rda->config(),this);
  trimmer->setCartNumber(cartnum);
  trimmer->setCutNumber(cutnum);
  trimmer->setTrimLevel(100*set_auto_trim);
  if((err=trimmer->runTrim(rda->user()->name(),rda->user()->password()))==
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
  RDAudioInfo *info=new RDAudioInfo(this);
  info->setCartNumber(cartnum);
  info->setCutNumber(cutnum);
  if((err=info->runInfo(rda->user()->name(),rda->user()->password()))==
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


void MainObject::SetAutoSegue(unsigned cartnum,int cutnum,const QString &title,
			      const QString &desc)
{
  RDTrimAudio::ErrorCode err;
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDTrimAudio *trimmer=new RDTrimAudio(rda->station(),rda->config(),this);
  trimmer->setCartNumber(cartnum);
  trimmer->setCutNumber(cutnum);
  trimmer->setTrimLevel(100*set_auto_segue);
  if((err=trimmer->runTrim(rda->user()->name(),rda->user()->password()))==
     RDTrimAudio::ErrorOk) {
    int end=trimmer->endPoint();
    if(end<cut->endPoint()) {
      cut->setSegueStartPoint(end);
      cut->setSegueEndPoint(cut->endPoint());
      Print(QString().sprintf("  setting segue-start for %06u / %03d [",
			      cartnum,cutnum)+title+" / "+desc+
	    QString().sprintf("] at %d dBFS",set_auto_segue));
    }
    else {
      Print(QString().sprintf("  segue-start for %06u / %03d [",
			      cartnum,cutnum)+title+" / "+desc+
	    "] cannot be set beyond end marker");
    }
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


void MainObject::ClearAutoSegue(unsigned cartnum,int cutnum,
				const QString &title,const QString &desc)
{
  RDCut *cut=new RDCut(cartnum,cutnum);
  cut->setSegueStartPoint(-1);
  cut->setSegueEndPoint(-1);
  delete cut;
}


void MainObject::Print(const QString &msg)
{
  if(set_verbose) {
    printf("%s\n",(const char *)msg);
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
