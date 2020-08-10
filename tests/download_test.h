// download_test.h
//
// Test Rivendell file downloading
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

#ifndef DOWNLOAD_TEST_H
#define DOWNLOAD_TEST_H

#include <list>

#include <qobject.h>

#define DOWNLOAD_TEST_USAGE "[options]\n\nTest the Rivendell download routines\n\nOptions are:\n--username=<username>\n\n--password=<password>\n\n--source-url=<url>\n\n--destination-file=<filename>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  QString username;
  QString password;
  QString source_url;
  QString destination_filename;
};


#endif  // DOWNLOAD_TEST_H
