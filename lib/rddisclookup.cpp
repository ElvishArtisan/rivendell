//   rddisclookup.cpp
//
//   Base class for CD metadata lookup methods
//
//   (C) Copyright 2003-2020 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <stdlib.h>
#include <string.h>

#include <qmessagebox.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <q3process.h>

#include "rdtempdirectory.h"
#include "rddisclookup.h"
#include "rdprofile.h"

RDDiscLookup::RDDiscLookup(const QString &caption,FILE *profile_msgs,
			   QWidget *parent)
  : RDDialog(parent)
{
  lookup_profile_msgs=profile_msgs;

  lookup_titles_label=new QLabel(tr("Multiple Matches Found!"),this);
  lookup_titles_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  lookup_titles_label->setFont(labelFont());

  lookup_titles_box=new QComboBox(this);

  lookup_ok_button=new QPushButton(tr("OK"),this);
  lookup_ok_button->setFont(buttonFont());
  connect(lookup_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  lookup_cancel_button=new QPushButton(tr("Cancel"),this);
  lookup_cancel_button->setFont(buttonFont());
  connect(lookup_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Create Temporary Directory
  //
  char path[PATH_MAX];
  strncpy(path,RDTempDirectory::basePath(),PATH_MAX);
  strcat(path,"/XXXXXX");
  if(mkdtemp(path)==NULL) {
    QMessageBox::warning(this,caption+" - "+tr("Ripper Error"),
			 tr("Unable to create temporary directory!"));
  }
  else {
    lookup_cdda_dir.setPath(path);
  }
  profile("created temp directory \""+lookup_cdda_dir.path()+"\"");
}


RDDiscLookup::~RDDiscLookup()
{
  QStringList files=lookup_cdda_dir.entryList();
  for(int i=0;i<files.size();i++) {
    if((files[i]!=".")&&(files[i]!="..")) {
      lookup_cdda_dir.remove(files[i]);
    }
  }
  rmdir(lookup_cdda_dir.path());
  profile("deleted temp directory \""+lookup_cdda_dir.path()+"\"");
}


QSize RDDiscLookup::sizeHint() const
{
  return QSize(400,120);
}


void RDDiscLookup::setCddbRecord(RDCddbRecord *rec)
{
  lookup_record=rec;
}


void RDDiscLookup::lookup()
{
  profile("starting CD-TEXT lookup");
  if(ReadCdText(lookup_cdda_dir.path(),rda->libraryConf()->ripperDevice())) {
    emit lookupDone(RDDiscLookup::ExactMatch);
    profile("CD-TEXT lookup success");
    return;
  }
  profile("CD-TEXT lookup failure");

  lookupRecord();
}


bool RDDiscLookup::readIsrc()
{
  return ReadIsrcs(lookup_cdda_dir.path(),rda->libraryConf()->ripperDevice());
}


void RDDiscLookup::okData()
{
  done(true);
}


void RDDiscLookup::cancelData()
{
  done(false);
}


void RDDiscLookup::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  lookup_titles_label->setGeometry(15,2,w-30,20);

  lookup_titles_box->setGeometry(10,24,w-20,20);

  lookup_ok_button->setGeometry(w-180,h-60,80,50);
  lookup_cancel_button->setGeometry(w-90,h-60,80,50);
}


RDCddbRecord *RDDiscLookup::cddbRecord()
{
  return lookup_record;
}


void RDDiscLookup::profile(const QString &msg)
{
  if(lookup_profile_msgs!=NULL) {
    printf("%s | RDDiscLookup::%s\n",
	    (const char *)QTime::currentTime().toString("hh:mm:ss.zzz"),
	   (const char *)msg.toUtf8());
  }
}


QComboBox *RDDiscLookup::titlesBox()
{
  return lookup_titles_box;
}


QStringList *RDDiscLookup::titlesKey()
{
  return &lookup_titles_key;
}


bool RDDiscLookup::ReadCdText(const QString &cdda_dir,const QString &cdda_dev)
{
  RDProfile *title_profile=new RDProfile();
  bool ret=false;
  QString str;
  QString cmd;

  //
  // Write the Track Title Data to a Temp File
  //
  QByteArray output;
  Q3Process *proc=new Q3Process(this);
  proc->addArgument("cdda2wav");
  proc->addArgument("-D");
  proc->addArgument(cdda_dev);
  proc->addArgument("--info-only");
  proc->addArgument("-v");
  proc->addArgument("titles");
  proc->setWorkingDirectory(cdda_dir);
  if(!proc->start()) {
    delete proc;
    profile("cdda2wav failed to start!");
    return false;
  }
  while(proc->isRunning()) {
    output=proc->readStderr();
    if(output.size()>0) {  // Work around icedax(1)'s idiotic user prompt
      if(strncmp(output,"load cdrom please and press enter",33)==0) {
	proc->kill();
	delete proc;
	profile("cdda2wav returned \""+output+"\", killing it!");
	return false;
      }
    }
  }
  if(!proc->normalExit()) {
    profile("cdda2wav crashed!");
    delete proc;
    return false;
  }
  if(proc->exitStatus()!=0) {
    profile("cdda2wav return exit code "+
	    QString().sprintf("%d",proc->exitStatus()));
    delete proc;
    return false;
  }
  delete proc;

  //
  // Read the Track Title Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    title_profile->setSource(cdda_dir+QString().sprintf("/audio_%02d.inf",i+1));
    str=title_profile->stringValue("","Albumtitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscTitle(str);
      profile("setting DiscTitle to \""+str+"\"");
      ret=true;
    }

    str=title_profile->stringValue("","Albumperformer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscArtist(str);
      profile("setting DiscArtist to \""+str+"\"");
      ret=true;
    }

    str=title_profile->stringValue("","Tracktitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackTitle(i,str);
      profile("setting TrackTitle "+QString().sprintf("%d",i+1)+" to \""+str+"\"");
      ret=true;
    }

    str=title_profile->stringValue("","Performer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackArtist(i,str);
      profile("setting TrackArtist "+QString().sprintf("%d",i+1)+" to \""+str+"\"");
      ret=true;
    }
  }
  return ret;
}


bool RDDiscLookup::ReadIsrcs(const QString &cdda_dir,const QString &cdda_dev)
{
  int err=0;
  RDProfile *title_profile=new RDProfile();
  RDProfile *isrc_profile=new RDProfile();
  bool ret=false;
  QString str;
  QString cmd;

  //
  // Write the ISRC Data to a Temp File
  //
  cmd=QString("CURDIR=`pwd`;cd ")+
    cdda_dir+";cdda2wav -D "+cdda_dev+
    " --info-only -v trackid 2> /dev/null;cd $CURDIR";
  if((err=system(cmd))!=0) {
    return false;
  }

  //
  // Read the ISRC Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    isrc_profile->setSource(cdda_dir+QString().sprintf("/audio_%02d.inf",i+1));
    str=isrc_profile->stringValue("","ISRC","");
    str.remove("'");
    str.remove("-");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setIsrc(i,str);
      ret=true;
    }
  }
  delete title_profile;
  delete isrc_profile;

  return ret;
}
