//   rddisclookup.h
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

#ifndef RDDISCLOOKUP_H
#define RDDISCLOOKUP_H

#include <stdio.h>

#include <qcombobox.h>
#include <qdir.h> 
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtcpsocket.h>

#include <rdcddbrecord.h>
#include <rddialog.h>

class RDDiscLookup : public RDDialog
{
  Q_OBJECT
 public:
  enum Result {ExactMatch=0,PartialMatch=1,NoMatch=2,
  	       ProtocolError=3,NetworkError=4};
  RDDiscLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);
  ~RDDiscLookup();
  QSize sizeHint() const;
  void setCddbRecord(RDCddbRecord *);
  void lookup();
  bool readIsrc();

 signals:
  void lookupDone(RDDiscLookup::Result);

 protected slots:
  virtual void lookupRecord()=0;
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  RDCddbRecord *cddbRecord();
  void profile(const QString &msg);
  QComboBox *titlesBox();
  QStringList *titlesKey();

 private:
   bool ReadCdText(const QString &cdda_dir,const QString &cdda_dev);
   bool ReadIsrcs(const QString &cdda_dir,const QString &cdda_dev);
   QLabel *lookup_titles_label;
   QComboBox *lookup_titles_box;
   QStringList lookup_titles_key;
   QPushButton *lookup_ok_button;
   QPushButton *lookup_cancel_button;
   RDCddbRecord *lookup_record;
   QTcpSocket *lookup_socket;
   int lookup_state;
   QString lookup_username;
   QString lookup_appname;
   QString lookup_appver;
   QString lookup_hostname;
   QDir lookup_cdda_dir;
   FILE *lookup_profile_msgs;
};

#endif  // RDDISCLOOKUP_H
