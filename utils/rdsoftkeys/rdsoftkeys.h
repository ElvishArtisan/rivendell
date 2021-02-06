// rdsoftkeys.h
//
// A utility for sending RML Commands
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSOFTKEYS_H
#define RDSOFTKEYS_H

#include <QStringList>
#include <QUdpSocket>
#include <QWidget>

//
// Settings
//
#define RDSOFTKEYS_USAGE "[--map-file=<filename>]\n\nWhere <filename> is the name of the file load soft key definitions from.\nThe default value is master Rivendell configuration file.\n"
#define RDSOFTKEYS_DEFAULT_COLUMNS 1

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void buttonData(int id);
  void closeEvent(QCloseEvent *e);

 private:
  QPixmap *key_icon_map;
  QUdpSocket *key_socket;
  int key_columns;
  unsigned key_ysize;
  QStringList key_macros;
  QStringList key_addrs;
};


#endif  // RDSOFTKEYS_H
