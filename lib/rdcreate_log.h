// rdcreate_log.h
//
// Create a new, empty Rivendell log table.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcreate_log.h,v 1.10 2010/07/29 19:32:33 cvs Exp $
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

#include <qsqldatabase.h>

#ifndef RDCREATE_LOG_H
#define RDCREATE_LOG_H

void RDCreateLogTable(const QString &name);
QString RDCreateLogTableSql(QString name);
QString RDCreateClockTableSql(QString name);
QString RDCreateReconciliationTableSql(QString name);
QString RDCreateStackTableSql(QString name);

#endif 
