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

#include <qdatetime.h>
#include <qmessagebox.h>
#include <q3process.h>
#include <qregexp.h>
#include <qtimer.h>

#include <discid/discid.h>

#include "rdtempdirectory.h"
#include "rddisclookup.h"
#include "rdprofile.h"

RDDiscLookup::RDDiscLookup(const QString &caption,FILE *profile_msgs,
			   QWidget *parent)
  : RDDialog(parent)
{
  lookup_caption=caption;
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
}


QSize RDDiscLookup::sizeHint() const
{
  return QSize(400,120);
}


void RDDiscLookup::setCddbRecord(RDDiscRecord *rec)
{
  lookup_record=rec;
}


void RDDiscLookup::lookup()
{
  if(cddbRecord()->tracks()==0) {
    return;
  }

  //
  // Get some basic disc parameters (CDDB DiskID, MusicBrainz mbid and,
  // if enabled in rdadmin(1), MCN and ISRCs).
  //
  DiscId *disc=discid_new();
  if(rda->libraryConf()->readIsrc()) {
    if(discid_read(disc,rda->libraryConf()->ripperDevice().toUtf8())==0) {
      QMessageBox::warning(this,caption()+" - "+tr("Error"),
			 tr("Unable to read CD.")+
			   "\n["+QString(discid_get_error_msg(disc))+"]");
      discid_free(disc);
      return;
    }
  }
  else {
    if(discid_read_sparse(disc,rda->libraryConf()->ripperDevice().toUtf8(),0)==0) {
      QMessageBox::warning(this,caption()+" - "+tr("Error"),
			 tr("Unable to read CD.")+
			   "\n["+QString(discid_get_error_msg(disc))+"]");
      discid_free(disc);
      return;
    }
  }
  cddbRecord()->setDiscId(QString(discid_get_freedb_id(disc)).toUInt(NULL,16));
  cddbRecord()->setMbId(discid_get_id(disc));
  cddbRecord()->setMbSubmissionUrl(discid_get_submission_url(disc));
  if(rda->libraryConf()->readIsrc()) {
    cddbRecord()->setMcn(discid_get_mcn(disc));
    int first=discid_get_first_track_num(disc);
    int last=discid_get_last_track_num(disc);
    for(int i=first;i<=last;i++) {
      if((i-first)<lookup_record->tracks()) {
	cddbRecord()->setIsrc(i-first,
		  RDDiscLookup::normalizedIsrc(discid_get_track_isrc(disc,i)));
      }
    }
  }
  discid_free(disc);

  //
  // Call the low-level driver to complete the lookup.
  //
  lookupRecord();
}


QString RDDiscLookup::caption()
{
  return lookup_caption;
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


RDDiscRecord *RDDiscLookup::cddbRecord()
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
