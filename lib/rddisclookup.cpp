//   rddisclookup.cpp
//
//   Base class for CD metadata lookup methods
//
//   (C) Copyright 2003-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <q3process.h>
#include <qregexp.h>
#include <qtimer.h>

#include "rdtempdirectory.h"
#include "rddisclookup.h"
#include "rdprofile.h"

RDDiscLookup::RDDiscLookup(const QString &caption,FILE *profile_msgs,
			   QWidget *parent)
  : RDDialog(parent)
{
  QString err_msg;

  lookup_caption=caption;
  lookup_profile_msgs=profile_msgs;
  lookup_has_cd_text=false;
  lookup_disc=NULL;

  lookup_temp_directory=new RDTempDirectory("rddisclookup");
  if(!lookup_temp_directory->create(&err_msg)) {
    QMessageBox::warning(this,caption+" - "+tr("Error"),
			 tr("Unable to create temporary directory")+
			 " \""+lookup_temp_directory->path()+"\".\n"+
			 "["+err_msg+"]");
  }

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
}


RDDiscLookup::~RDDiscLookup()
{
  delete lookup_temp_directory;
}


QSize RDDiscLookup::sizeHint() const
{
  return QSize(400,140);
}


QString RDDiscLookup::sourceName() const
{
  return QString(tr("CD-TEXT"));
}


QPixmap RDDiscLookup::sourceLogo() const
{
  return QPixmap();
}


QPixmap RDDiscLookup::sourceLogo(RDDiscRecord::DataSource src) const
{
  QPixmap ret=RDLibraryConf::cdServerLogo(RDLibraryConf::DummyType);

  switch(src) {
  case RDDiscRecord::LocalSource:
  case RDDiscRecord::LastSource:
    break;

  case RDDiscRecord::RemoteSource:
    ret=sourceLogo();
  }

  return ret;
}


QString RDDiscLookup::sourceUrl() const
{
  return QString();
}


void RDDiscLookup::setCddbRecord(RDDiscRecord *rec)
{
  lookup_record=rec;
}


void RDDiscLookup::lookup()
{
  QString rip_dev=rda->libraryConf()->ripperDevice();

  if(discRecord()->tracks()==0) {
    return;
  }

  profile("starting CD-TEXT lookup");
  if((lookup_has_cd_text=ReadCdText(rip_dev))) {
    profile("CD-TEXT lookup success");
  }
  else {
    profile("CD-TEXT lookup failure");
  }

  //
  // Get some basic disc parameters,
  //
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if(lookup_disc!=NULL) {
    discid_free(lookup_disc);
    lookup_disc=NULL;
  }
  lookup_disc=discid_new();
  if(discid_read_sparse(lookup_disc,rip_dev.toUtf8(),0)==0) {
    QMessageBox::warning(this,caption()+" - "+tr("Error"),
		       tr("Unable to read CD.")+
		       "\n["+QString::fromUtf8(discid_get_error_msg(lookup_disc))+"]");
    discid_free(lookup_disc);
    lookup_disc=NULL;
    return;
  }
  discRecord()->setDiscId(QString(discid_get_freedb_id(lookup_disc)).toUInt(NULL,16));
  discRecord()->setDiscMbId(discid_get_id(lookup_disc));
  discRecord()->setMbSubmissionUrl(discid_get_submission_url(lookup_disc));
  QApplication::restoreOverrideCursor();

  //
  // Call the low-level driver to do its lookup.
  //
  lookupRecord();
}


void RDDiscLookup::lookupRecord()
{
  processLookup(RDDiscLookup::ExactMatch,"OK");
}


void RDDiscLookup::processLookup(RDDiscLookup::Result result,
				 const QString &err_msg)
{
  QString rip_dev=rda->libraryConf()->ripperDevice();

  //
  // If the low-level driver didn't find ISRCs, and the user has requested
  // them, try to find them on the disc.
  //
  // WARNING: This operation can take a long time if the disc does not in
  //          fact contain ISRCs!
  //
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  if((!discRecord()->hasIsrcs())&&rda->libraryConf()->readIsrc()) {
    if(discid_read(lookup_disc,rip_dev.toUtf8())==0) {
      QMessageBox::warning(this,caption()+" - "+tr("Error"),
		       tr("Unable to read CD.")+
		       "\n["+QString::fromUtf8(discid_get_error_msg(lookup_disc))+"]");
      discid_free(lookup_disc);
      lookup_disc=NULL;
      return;
    }
    discRecord()->setMcn(discid_get_mcn(lookup_disc));
    int first=discid_get_first_track_num(lookup_disc);
    int last=discid_get_last_track_num(lookup_disc);
    for(int i=first;i<=last;i++) {
      if((i-first)<discRecord()->tracks()) {
	discRecord()->setIsrc(i-first,
		  RDDiscLookup::normalizedIsrc(discid_get_track_isrc(lookup_disc,i)));
      }
    }
  }
  QApplication::restoreOverrideCursor();

  discid_free(lookup_disc);
  lookup_disc=NULL;

  emit lookupDone(RDDiscLookup::ExactMatch,"OK");
}


bool RDDiscLookup::hasCdText() const
{
  return lookup_has_cd_text;
}


QString RDDiscLookup::caption()
{
  return lookup_caption;
}


void RDDiscLookup::okData()
{
  done(lookup_titles_box->currentIndex());
}


void RDDiscLookup::cancelData()
{
  done(-1);
}


void RDDiscLookup::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  lookup_titles_label->setGeometry(15,2,w-30,20);

  lookup_titles_box->
    setGeometry(10,24,w-20,lookup_titles_box->sizeHint().height());

  lookup_ok_button->setGeometry(w-180,h-60,80,50);
  lookup_cancel_button->setGeometry(w-90,h-60,80,50);
}


RDDiscRecord *RDDiscLookup::discRecord() const
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


bool RDDiscLookup::isrcIsValid(const QString &isrc)
{
  //
  // For formatting rules for International Standard Recording Codes,
  // see https://en.wikipedia.org/wiki/International_Standard_Recording_Code
  //
  // NOTE: This makes no attempt to validate that the Country or Registrant
  //       codes actually exist in the IFPI registry!
  //
  QString str=isrc;
  bool valid=false;

  str.replace("-","");
  if(str.length()!=12) {
    return false;
  }

  //
  // This could probably be done much more compactly with a regex.
  //
  for(int i=0;i<12;i++) {
    QChar::Category category=str.at(i).category();
    switch(i) {
    case 0:  // Country Code
    case 1:
      valid=(category==QChar::Letter_Uppercase)||
	(category==QChar::Letter_Lowercase);
      break;

    case 2:  // Registrant Code
    case 3:
    case 4:
      valid=(category==QChar::Letter_Uppercase)||
	(category==QChar::Letter_Lowercase)||
	(category==QChar::Number_DecimalDigit);
      break;

    case 5:  // Designation Code
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
      valid=(category==QChar::Number_DecimalDigit);
      break;
    }
    if(!valid) {
      return false;
    }
  }

  return true;
}


QString RDDiscLookup::formattedIsrc(const QString &isrc,bool *ok)
{
  if(RDDiscLookup::isrcIsValid(isrc)) {
    if(ok!=NULL) {
      *ok=true;
    }
    QString str=isrc;
    str.insert(2,"-");
    str.insert(6,"-");
    str.insert(9,"-");
    return str.toUpper();
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return QString();
}


QString RDDiscLookup::normalizedIsrc(const QString &isrc,bool *ok)
{
  if(RDDiscLookup::isrcIsValid(isrc)) {
    if(ok!=NULL) {
      *ok=true;
    }
    QString str=isrc;
    str.replace("-","");
    return str.toUpper();
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return QString();
}


bool RDDiscLookup::upcAIsValid(const QString &barcode)
{
  //
  // For formatting rules for UPC-A barcodes, see
  // https://en.wikipedia.org/wiki/Universal_Product_Code
  //
  QString str=barcode;

  str.replace("-","");
  str.replace(" ","");
  if(str.length()!=12) {
    return false;
  }

  for(int i=0;i<12;i++) {
    if(str.at(i).category()!=QChar::Number_DecimalDigit) {
      return false;
    }
  }

  return true;
}


QString RDDiscLookup::formattedUpcA(const QString &barcode,bool *ok)
{
  if(RDDiscLookup::upcAIsValid(barcode)) {
    if(ok!=NULL) {
      *ok=true;
    }
    QString str=barcode;
    str.insert(1," ");
    str.insert(6,"-");
    str.insert(12,"-");
    str.insert(14," ");
    return str;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return QString();
}


QString RDDiscLookup::normalizedUpcA(const QString &barcode,bool *ok)
{
  if(RDDiscLookup::upcAIsValid(barcode)) {
    if(ok!=NULL) {
      *ok=true;
    }
    QString str=barcode;
    str.replace("-","");
    str.replace(" ","");
    return str;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return QString();
}


QString RDDiscLookup::tempDirectoryPath() const
{
  return lookup_temp_directory->path();
}


bool RDDiscLookup::ReadCdText(const QString &cdda_dev)
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
  proc->setWorkingDirectory(tempDirectoryPath());
  if(!proc->start()) {
    delete proc;
    return false;
  }
  while(proc->isRunning()) {
    output=proc->readStderr();
    if(output.size()>0) {  // Work around icedax(1)'s idiotic user prompt
      if(strncmp(output,"load cdrom please and press enter",33)==0) {
	proc->kill();
	delete proc;
	return false;
      }
    }
  }
  if((!proc->normalExit())||(proc->exitStatus()!=0)) {
    delete proc;
    return false;
  }
  delete proc;

  //
  // Read the Track Title Data File
  //
  for(int i=0;i<lookup_record->tracks();i++) {
    title_profile->setSource(tempDirectoryPath()+
			     QString().sprintf("/audio_%02d.inf",i+1));
    str=title_profile->stringValue("","Albumtitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscTitle(RDDiscRecord::LocalSource,str);
      ret=true;
    }

    str=title_profile->stringValue("","Albumperformer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setDiscArtist(RDDiscRecord::LocalSource,str);
      ret=true;
    }

    str=title_profile->stringValue("","Tracktitle","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackTitle(RDDiscRecord::LocalSource,i,str);
      ret=true;
    }

    str=title_profile->stringValue("","Performer","");
    str.remove("'");
    if((!str.isEmpty())&&(str!="''")) {
      lookup_record->setTrackArtist(RDDiscRecord::LocalSource,i,str);
      ret=true;
    }
  }
  return ret;
}
