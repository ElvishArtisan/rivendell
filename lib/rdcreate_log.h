// rdcreate_log.h
//
// Create a new, empty Rivendell log table.
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdconfig.h"

#ifndef RDCREATE_LOG_H
#define RDCREATE_LOG_H

//void RDCreateLogTable(const QString &name,RDConfig *config);
QString RDCreateLogTableSql(QString name,RDConfig *config);
QString RDCreateClockTableSql(QString name,RDConfig *config);
QString RDCreateReconciliationTableSql(QString name,RDConfig *config);
QString RDCreateStackTableSql(QString name,RDConfig *config);

#endif 
