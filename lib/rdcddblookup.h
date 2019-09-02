//   rdcddblookup.h
//
//   A Qt class for accessing the FreeDB CD Database.
//
//   (C) Copyright 2003-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCDDBLOOKUP_H
#define RDCDDBLOOKUP_H

#include <stdio.h>

#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtcpsocket.h>

#include "rdcddbrecord.h"

//
// Default Settings
//
#define RDCDDBLOOKUP_DEFAULT_PORT 8880
#define RDCDDBLOOKUP_DEFAULT_USER "libradio"
#define RDCDDBLOOKUP_DEFAULT_HOSTNAME "linux"

/**
 * @short Lookup CD Data from the FreeDB CD Database
 * @author Fred Gleason <fredg@paravelsystems.com>
 *
 * This class implements an object for accessing a remote FreeDB CD
 * database server.  
 **/

class RDCddbLookup : public QDialog
{
  Q_OBJECT
 public:
  enum Result {ExactMatch=0,PartialMatch=1,NoMatch=2,
	       ProtocolError=3,NetworkError=4};
  RDCddbLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);
  ~RDCddbLookup();
  QSize sizeHint() const;
  void setCddbRecord(RDCddbRecord *);
  void lookupRecord(const QString &cdda_dir,const QString &cdda_dev,
		    const QString &hostname,
		    Q_UINT16 port=RDCDDBLOOKUP_DEFAULT_PORT,
		    const QString &username="",
		    const QString &appname=PACKAGE_NAME,
		    const QString &ver=VERSION);
  bool readIsrc(const QString &cdda_dir,const QString &cdda_dev);

 private slots:
  void readyReadData();
  void errorData(QAbstractSocket::SocketError);
  void okData();
  void cancelData();

 signals:
  void lookupDone(RDCddbLookup::Result);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
   void FinishCddbLookup(RDCddbLookup::Result res);
   QString DecodeString(QString &str);
   void ParsePair(QString *line,QString *tag,QString *value,int *index);
   int GetIndex(QString *tag);
   bool ReadCdText(const QString &cdda_dir,const QString &cdda_dev);
   bool ReadIsrcs(const QString &cdda_dir,const QString &cdda_dev);
   void SendToServer(const QString &msg);
   void Profile(const QString &msg);
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
   FILE *lookup_profile_msgs;
};

#endif  // RDCDDBLOOKUP_H
