//   rdcddblookup.h
//
//   RDDiscLookup instance class for accessing the FreeDB CD Database.
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

#ifndef RDCDDBLOOKUP_H
#define RDCDDBLOOKUP_H

#include <stdio.h>

#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtcpsocket.h>

#include <rddisclookup.h>
#include <rddiscrecord.h>
#include <rddialog.h>

//
// FreeDB Service Settings
//
#define RDCDDBLOOKUP_DEFAULT_PORT 8880

class RDCddbLookup : public RDDiscLookup
{
  Q_OBJECT
 public:
  RDCddbLookup(const QString &caption,FILE *profile_msgs,QWidget *parent=0);
  ~RDCddbLookup();
  QString sourceName() const;

 private slots:
  void readyReadData();
  void errorData(QAbstractSocket::SocketError);

 protected:
  void lookupRecord();

 private:
  void FinishCddbLookup(RDCddbLookup::Result res,const QString &err_msg);
  QString DecodeString(QString &str);
  void ParsePair(QString *line,QString *tag,QString *value,int *index);
  int GetIndex(QString *tag);
  void SendToServer(const QString &msg);
  QTcpSocket *lookup_socket;
  int lookup_state;
  QString lookup_username;
  QString lookup_appname;
  QString lookup_appver;
  QString lookup_hostname;
};

#endif  // RDCDDBLOOKUP_H
