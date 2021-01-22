// rdstationlistmodel.h
//
// Data model for Rivendell hosts
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

#ifndef RDSTATIONLISTMODEL_H
#define RDSTATIONLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>
#include <rduser.h>

//
// MAINTAINERS NOTE
//
// We could arguably just use a QSqlQueryModel for this, but rolling our own
// lets us use the same backend-facing API pattern as the other Rivendell
// models.
//
class RDStationListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDStationListModel(bool incl_none,const QString &localhost_name,
		     QObject *parent=0);
  ~RDStationListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  QString stationName(const QModelIndex &row) const;
  QModelIndex addStation(const QString &hostname);
  void removeStation(const QModelIndex &row);
  void removeStation(const QString &hostname);
  void refresh(const QModelIndex &row);
  void refresh(const QString &username);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QString d_localhost_name;
  bool d_include_none;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QStringList d_hostnames;
  QList<QList<QVariant> > d_texts;
  QList<QList<QVariant> > d_icons;
};


#endif  // RDSTATIONLISTMODEL_H
