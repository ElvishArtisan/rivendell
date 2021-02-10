// rdclockmodel.h
//
// Abstract a Rivendell Log Manager Clock
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCLOCKMODEL_H
#define RDCLOCKMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rdclock.h>

class RDClockModel : public QAbstractTableModel, public RDClock
{
  Q_OBJECT;
 public:
  RDClockModel(RDStation *station,QObject *parent=0);
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void clear();
  bool load();
  RDEventLine *eventLine(const QModelIndex &row) const;
  RDEventLine *eventLine(int line) const;
  int insert(const QString &event_name,const QTime &start,int len);
  void remove(int line);
  void refresh(const QModelIndex &row);

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
};

#endif  // RDCLOCKMODEL_H
