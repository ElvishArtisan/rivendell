// rddb.h
//
// Database driver with automatic reconnect
//
//   (C) Copyright 2007 Dan Mills <dmills@exponent.myzen.co.uk>
//
//      $Id: rddb.h,v 1.9 2011/06/21 22:20:43 cvs Exp $
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

#ifndef RDDB_INC
#define RDDB_INC

#include <qobject.h>
#include <qsqldatabase.h>
#include <qstring.h>

#include <rdconfig.h>


class RDSqlDatabaseStatus : public QObject
{
  Q_OBJECT
 signals:
  void logText(RDConfig::LogPriority prio,const QString &msg);
  void reconnected();
  void connectionFailed ();
 private:
  RDSqlDatabaseStatus ();
  bool discon;
  friend RDSqlDatabaseStatus * RDDbStatus();
 public:
  void sendRecon();
  void sendDiscon(QString query);
};


class RDSqlQuery : public QSqlQuery
{
 public:
 RDSqlQuery ( const QString & query = QString::null, QSqlDatabase * db = 0 );
};

// Setup the default database, returns true on success.
// if error is non NULL, an error string will be appended to it
// if there is a problem.
QSqlDatabase * RDInitDb(unsigned *schema,QString *error=NULL);

// Return a handle to the database status object.
RDSqlDatabaseStatus * RDDbStatus();

#endif
