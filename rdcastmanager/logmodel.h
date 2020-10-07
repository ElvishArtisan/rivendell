// logmodel.h
//
// Read-only data model for Rivendell logs
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlist.h>

#include <rdlog_event.h>
#include <rdlog_icons.h>

class LogModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  LogModel(QObject *parent=0);
  ~LogModel();
  void setLogEvent(RDLogEvent *log);
  void clearLogEvent();
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;

 private:
  RDLogEvent *d_log;
  QFont d_font;
  QFontMetrics *d_fms;
  QFont d_bold_font;
  QFontMetrics *d_bold_fms;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  RDLogIcons *d_log_icons;
};


#endif  // LOGMODEL_H
