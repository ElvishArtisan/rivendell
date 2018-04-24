// upload_test.h
//
// Test Rivendell file uploading
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef UPLOAD_TEST_H
#define UPLOAD_TEST_H

#include <stdlib.h>

#include <list>

#include <qobject.h>

#define UPLOAD_TEST_USAGE "[options]\n\nTest the Rivendell upload routines\n\nOptions are:\n--username=<username>\n\n--password=<password>\n\n--source-file=<filename>\n\n--destination-url=<url>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  QString username;
  QString password;
  QString source_filename;
  QString destination_url;
};


#endif  // UPLOAD_TEST_H
