// rdmetadata.cpp
//
// Command-line tool for setting Rivendell Cart Metadata
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSqlQuery>

#include "rd.h"
#include "rdapplication.h"
#include "rdcart.h"
#include "rdschedcode.h"
#include "rdnotification.h"

#include "rdmetadata.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  verbose=false;
  cartnum=0;
  year=0;

  //
  // Check for root permission
  //
  if(geteuid()!=0) {
    fprintf(stderr,"rdmetadata: must be user \"root\"\n");
    exit(256);
  }

  //
  // Open the Database
  //
  rda=new RDApplication("rdmetadata","rdmetadata",RDMETADATA_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdmetadata: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--cart-number") {
      cartnum=rda->cmdSwitch()->value(i).toUInt();
      cartstring=QString().sprintf("%06u",cartnum);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--artist") {
      artist=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--title") {
      title=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--album") {
      album=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--year") {
      year=rda->cmdSwitch()->value(i).toInt();
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--conductor") {
      conductor=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--label") {
      label=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--agency") {
      agency=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--publisher") {
      publisher=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--composer") {
      composer=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--songid") {
      songid=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--bpm") {
      bpm=rda->cmdSwitch()->value(i).toInt();
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--add-schedcode") {
      add_schedcode=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--rem-schedcode") {
      rem_schedcode=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdmetadata: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(!cartnum) {
    fprintf(stderr,"rdmetadata: --cart-number must be specified\n");
    exit(256);
  }

  if(!add_schedcode.isEmpty()) {
    RDSchedCode *sched=new RDSchedCode(add_schedcode);
    if (!sched->exists()) {
      fprintf(stderr,"rdmetadata: scheduler code '%s' does not exist\n",
        qPrintable(sched->code()));
      exit(256);
    }
    else {
      // Get proper case
      add_schedcode=sched->code();
    }
    delete sched;
  }

  if(!rem_schedcode.isEmpty()) {
    RDSchedCode *sched=new RDSchedCode(rem_schedcode);
    if (!sched->exists()) {
      fprintf(stderr,"rdmetadata: scheduler code '%s' does not exist\n",
        qPrintable(sched->code()));
      exit(256);
    }
    else {
      // Get proper case
      rem_schedcode=sched->code();
    }
    delete sched;
  }

  //
  // RIPCD Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userChangedData()
{
  updateMetadata();

  exit(0);
}


void MainObject::updateMetadata()
{
  RDCart *cart;
  QStringList schedcodes;

  cart=new RDCart(cartnum);

  if(!cart->exists()) {
    fprintf(stderr,"rdmetadata: cart %06u does not exist.\n",cartnum);
    exit(1);
  }

  schedcodes=cart->schedCodesList();

  if(!artist.isNull()) {
    cart->setArtist(artist);
    Print(QString("rdmetadata: Set cart %1 artist to '%2'").arg(cartstring).arg(artist));
  }

  if(!title.isNull()) {
    cart->setTitle(title);
    Print(QString("rdmetadata: Set cart %1 title to '%2'").arg(cartstring).arg(title));
  }

  if(!album.isNull()) {
    cart->setAlbum(album);
    Print(QString("rdmetadata: Set cart %1 album to '%2'").arg(cartstring).arg(album));
  }

  if(year) {
    cart->setYear(year);
    Print(QString("rdmetadata: Set cart %1 year to '%2'").arg(cartstring).arg(year));
  }

  if(!conductor.isNull()) {
    cart->setConductor(conductor);
    Print(QString("rdmetadata: Set cart %1 conductor to '%2'").arg(cartstring).arg(conductor));
  }

  if(!label.isNull()) {
    cart->setLabel(label);
    Print(QString("rdmetadata: Set cart %1 label to '%2'").arg(cartstring).arg(label));
  }

  if(!agency.isNull()) {
    cart->setAgency(agency);
    Print(QString("rdmetadata: Set cart %1 agency to '%2'").arg(cartstring).arg(agency));
  }

  if(!publisher.isNull()) {
    cart->setPublisher(publisher);
    Print(QString("rdmetadata: Set cart %1 publisher to '%2'").arg(cartstring).arg(publisher));
  }

  if(!composer.isNull()) {
    cart->setComposer(composer);
    Print(QString("rdmetadata: Set cart %1 composer to '%2'").arg(cartstring).arg(composer));
  }

  if(!songid.isNull()) {
    cart->setSongId(songid);
    Print(QString("rdmetadata: Set cart %1 songid to '%2'").arg(cartstring).arg(songid));
  }

  if(bpm) {
    cart->setBeatsPerMinute(bpm);
    Print(QString("rdmetadata: Set cart %1 bpm to '%2'").arg(cartstring).arg(bpm));
  }

  if(!add_schedcode.isEmpty()&&!schedcodes.contains(add_schedcode)) {
    cart->addSchedCode(add_schedcode);
    Print(QString("rdmetadata: Added scheduler code '%1' to cart %2").arg(add_schedcode).arg(cartstring));
  }

  if(!rem_schedcode.isEmpty()&&schedcodes.contains(rem_schedcode)) {
    cart->removeSchedCode(rem_schedcode);
    Print(QString("rdmetadata: Removed scheduler code '%1' from cart %2").arg(rem_schedcode).arg(cartstring));
  }

  delete cart;

  RDNotification *notify=new RDNotification(
    RDNotification::CartType,RDNotification::ModifyAction,QVariant(cartnum));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void MainObject::Print(const QString &msg)
{
  if(verbose) {
    printf("%s\n",(const char *)msg);
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
