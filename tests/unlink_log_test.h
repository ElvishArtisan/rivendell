// unlink_log_test.h
//
// Exercise the RDSvc::clearLogLinks() method.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef UNLINK_LOG_TEST_H
#define UNLINK_LOG_TEST_H

#include <qdatetime.h>
#include <qobject.h>

#include <rdsvc.h>

#define UNLINK_LOG_TEST_USAGE "[options]\n\nUnlink a Rivendell log\n\nOptions are:\n--class=Music|Traffic\n\n--log-name=<name>\n\n"

//
// Global Variables
//
RDConfig *rdconfig;


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  RDSvc::ImportSource test_src;
  QDate test_log_date;
  QString test_log_name;
};


#endif  // UNLINK_LOG_TEST_H
