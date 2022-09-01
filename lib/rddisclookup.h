//   rddisclookup.h
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

#ifndef RDDISCLOOKUP_H
#define RDDISCLOOKUP_H

#include <stdio.h>

#include <discid/discid.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtcpsocket.h>

#include <rddiscrecord.h>
#include <rddialog.h>
#include <rdtempdirectory.h>

class RDDiscLookup : public RDDialog
{
  Q_OBJECT
 public:
  enum Result {ExactMatch=0,NoMatch=1,LookupError=2};
  RDDiscLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);
  ~RDDiscLookup();
  QSize sizeHint() const;
  virtual QString sourceName() const;
  virtual QPixmap sourceLogo() const;
  QPixmap sourceLogo(RDDiscRecord::DataSource src) const;
  virtual QString sourceUrl() const;
  void setCddbRecord(RDDiscRecord *);
  void lookup();
  bool hasCdText() const;
  static bool isrcIsValid(const QString &isrc);
  static QString formattedIsrc(const QString &isrc,bool *ok=NULL);
  static QString normalizedIsrc(const QString &isrc,bool *ok=NULL);
  static bool upcAIsValid(const QString &barcode);
  static QString formattedUpcA(const QString &barcode,bool *ok=NULL);
  static QString normalizedUpcA(const QString &barcode,bool *ok=NULL);

 signals:
  void lookupDone(RDDiscLookup::Result,const QString &err_msg);

 protected slots:
  QString caption();
  void okData();
  void cancelData();

 protected:
  virtual void lookupRecord();
  void processLookup(RDDiscLookup::Result result,const QString &err_msg);
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  RDDiscRecord *discRecord() const;
  void profile(const QString &msg);
  QComboBox *titlesBox();
  QStringList *titlesKey();
  QString tempDirectoryPath() const;

 private:
  bool ReadCdText(const QString &cdda_dev);
  QLabel *lookup_titles_label;
  QComboBox *lookup_titles_box;
  QStringList lookup_titles_key;
  QPushButton *lookup_ok_button;
  QPushButton *lookup_cancel_button;
  RDDiscRecord *lookup_record;
  QTcpSocket *lookup_socket;
  int lookup_state;
  QString lookup_username;
  QString lookup_appname;
  QString lookup_appver;
  QString lookup_hostname;
  FILE *lookup_profile_msgs;
  QString lookup_caption;
  RDTempDirectory *lookup_temp_directory;
  bool lookup_has_cd_text;
  DiscId *lookup_disc;
};

#endif  // RDDISCLOOKUP_H
