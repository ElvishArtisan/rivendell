// delete_test.h
//
// Test Rivendell remote file deletion routines. 
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

#ifndef DELETE_TEST_H
#define DELETE_TEST_H

#include <list>

#include <qobject.h>
#include <qurl.h>

#define DELETE_TEST_USAGE "[options]\n\nTest the Rivendell deletion routines\n\nOptions are:\n--username=<username>\n\n--password=<password>\n\n--ssh-identity-key=<filename>\n\n--use-identity-file=y|n\n\n--target-url=<url>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  QString username;
  QString password;
  QUrl target_url;
  QString ssh_identity_filename;
  bool use_identity_file;
};


#endif  // DELETE_TEST_H
