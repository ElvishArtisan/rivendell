// rddb.h
//
// Database driver with automatic error reporting and recovery
//
//   (C) Copyright 2007 Dan Mills <dmills@exponent.myzen.co.uk>
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDB_H
#define RDDB_H

#include <QString>
#include <QSqlQuery>
#include <QVariant>

#include <rdconfig.h>

class RDSqlQuery : public QSqlQuery
{
 public:
  RDSqlQuery(const QString &query = QString::null);
  int columns() const;
  static QVariant run(const QString &sql,bool *ok=NULL);

 private:
  int sql_columns;
};

bool RDOpenDb(int *schema,QString *err_str,RDConfig *config);
bool RDInitDb(QString *err_str,RDConfig *config);
bool RDMakeDb(QString *err_str,RDConfig *config);
bool RDUpdateDb(QString *err_str,int schema,RDConfig *config);

#endif  // RDDB_H
