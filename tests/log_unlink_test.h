// log_unlink_test.h
//
// Test the Rivendell log unlinker methods
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LOG_UNLINK_TEST_H
#define LOG_UNLINK_TEST_H

#include <qobject.h>

#include <rdripc.h>
#include <rdstation.h>
#include <rdsvc.h>

#define LOG_UNLINK_TEST_USAGE "[options]\n\nTest the Rivendell log unlinker methods\n\nOptions are:\n--log=<log-name>\n     Name of log to unlink.\n\n--source=music|traffic\n     Data source to unlink\n\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
   void userData();

 private:
   RDSvc::ImportSource test_import_source;
  QString test_log_name;
  RDStation *test_station;
  RDRipc *test_ripc;
  RDConfig *test_config;
};


#endif  // LOG_UNLINK_TEST_H
