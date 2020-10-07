// feed_image_test.cpp
//
// Test Rivendell image storage
//
//   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qfile.h>
#include <qimage.h>

#include <rdapplication.h>
#include <rdescape_string.h>

#include "feed_image_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  QString key_name;
  bool ok=false;
  MainObject::Command command=MainObject::None;
  test_image_id=-1;

  //
  // Open the Database
  //
  rda=new RDApplication("feed_image_test","feed_image_test",FEED_IMAGE_TEST_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"feed_image_test: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--feed") {
      key_name=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--filename") {
      test_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--description") {
      test_description=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--image-id") {
      test_image_id=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(test_image_id<0)) {
	fprintf(stderr,"feed_image_test: invalid --image-id\n");
	exit(1);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--list") {
      if(command!=MainObject::None) {
	fprintf(stderr,
	  "feed_image_test: --list, --pop and --push are mutually exclusive\n");
	exit(1);
      }
      command=MainObject::List;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--pop") {
      if(command!=MainObject::None) {
	fprintf(stderr,
	  "feed_image_test: --list, --pop and --push are mutually exclusive\n");
	exit(1);
      }
      command=MainObject::Pop;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--push") {
      if(command!=MainObject::None) {
	fprintf(stderr,
	  "feed_image_test: --list, --pop and --push are mutually exclusive\n");
	exit(1);
      }
      command=MainObject::Push;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"feed_image_test: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(command==MainObject::None) {
    fprintf(stderr,"feed_image_test: you must specify --list, --pop or --push\n");
    exit(1);
  }




  //
  // Get the feed data
  //

  //
  // Dispatch
  //
  switch(command) {
  case MainObject::List:
    if(key_name.isEmpty()) {
      fprintf(stderr,"feed_image_test: you must specify --feed=<key-name>\n");
      exit(1);
    }
    test_feed=new RDFeed(key_name,rda->config(),this);
    if(!test_feed->exists()) {
      fprintf(stderr,"feed_image_test: no such feed\n");
      exit(1);
    }
    RunList();
    break;

  case MainObject::Pop:
    if(test_image_id<0) {
      fprintf(stderr,"feed_image_test: you must specify --image-id\n");
      exit(1);
    }
    if(test_filename.isEmpty()) {
      fprintf(stderr,
	      "feed_image_test: you must specify --filename=<file-name>\n");
      exit(1);
    }
    RunPop();
    break;

  case MainObject::Push:
    if(key_name.isEmpty()) {
      fprintf(stderr,"feed_image_test: you must specify --feed=<key-name>\n");
      exit(1);
    }
    test_feed=new RDFeed(key_name,rda->config(),this);
    if(!test_feed->exists()) {
      fprintf(stderr,"feed_image_test: no such feed\n");
      exit(1);
    }
    if(test_filename.isEmpty()) {
      fprintf(stderr,
	      "feed_image_test: you must specify --filename=<file-name>\n");
      exit(1);
    }
    if(test_description.isEmpty()) {
      fprintf(stderr,
	      "feed_image_test: you must specify --description=<img-desc>\n");
      exit(1);
    }
    RunPush();
    break;

  case MainObject::None:
    break;
  }

  exit(0);
}


void MainObject::RunList()
{
  QString sql;
  RDSqlQuery *q=NULL;

  sql=QString("select ")+
    "ID,"+           // 00
    "WIDTH,"+        // 01
    "HEIGHT,"+       // 02
    "DEPTH,"+        // 03
    "DESCRIPTION "+  // 04
    "from FEED_IMAGES where "+
    "FEED_KEY_NAME=\""+RDEscapeString(test_feed->keyName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    printf("ID: %u\n",q->value(0).toUInt());
    printf("  Description: %s\n",q->value(4).toString().toUtf8().constData());
    printf("  Dimensions: %dx%dx%d\n",q->value(1).toInt(),q->value(2).toInt(),
	   q->value(3).toInt());
    printf("\n");
  }
  delete q;
}


void MainObject::RunPush()
{
  QString sql;

  //
  // Load the image
  //
  QFile file(test_filename);
  if(!file.open(QIODevice::ReadOnly)) {
    fprintf(stderr,"feed_image_test: unable to open image file [%s]\n",
	    strerror(errno));
    exit(1);
  }
  QByteArray data=file.readAll();
  file.close();

  //
  // Validate the image
  //
  QImage *img=new QImage();
  if(!img->loadFromData(data)) {
    fprintf(stderr,"feed_image_test: invalid image file\n");
    exit(1);
  }
  printf("Image is %dx%dx%d\n",img->width(),img->height(),img->depth());

  //
  // Write it to the DB
  //
  sql=QString("insert into FEED_IMAGES set ")+
    QString().sprintf("FEED_ID=%u,",test_feed->id())+
    "FEED_KEY_NAME=\""+RDEscapeString(test_feed->keyName())+"\","+
    QString().sprintf("WIDTH=%d,",img->width())+
    QString().sprintf("HEIGHT=%d,",img->height())+
    QString().sprintf("DEPTH=%d,",img->depth())+
    "DESCRIPTION=\""+RDEscapeString(test_description)+"\","+
    "DATA="+RDEscapeBlob(data);
  RDSqlQuery::apply(sql);
}


void MainObject::RunPop()
{
  QString sql;
  RDSqlQuery *q=NULL;
  QByteArray data;
  FILE *f=NULL;

  sql=QString("select DATA from FEED_IMAGES where ")+
    QString().sprintf("ID=%u",test_image_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if((f=fopen(test_filename.toUtf8(),"w"))==NULL) {
      fprintf(stderr,"feed_image_test: unable to open \"%s\" [%s]\n",
	      test_filename.toUtf8().constData(),strerror(errno));
      exit(1);
    }
    data=q->value(0).toByteArray();
    if(fwrite(data.constData(),1,data.size(),f)<0) {
      fprintf(stderr,"feed_image_test: unable to write to \"%s\" [%s]\n",
	      test_filename.toUtf8().constData(),strerror(errno));
      exit(1);
    }
  }
  else {
    fprintf(stderr,"feed_image_test: no such image\n");
    exit(1);
  }
  delete q;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
