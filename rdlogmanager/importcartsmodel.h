// importcartsmodel.h
//
// Data model for rdlogmanager(1) cart stack lists
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

#ifndef IMPORTCARTSMODEL_H
#define IMPORTCARTSMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>
#include <QStringList>

#include <rddb.h>
#include <rdlog_line.h>
#include <rdnotification.h>
#include <rduser.h>

class ImportCartsModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  enum ImportType {PreImport=0,PostImport=1};
  ImportCartsModel(const QString &evt_name,ImportType type,
		   bool auto_first_trans,QObject *parent=0);
  ~ImportCartsModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  bool firstTransitionIsAuto() const;
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  RDLogLine::Type eventType(const QModelIndex &row) const;
  RDLogLine::TransType transType(const QModelIndex &row) const;
  void setTransType(const QModelIndex &row,RDLogLine::TransType type);
  int length(const QModelIndex &row) const;
  QModelIndex addMarker(const QModelIndex &row,const QString &str);
  QModelIndex addTrack(const QModelIndex &row,const QString &str);
  QString markerComments(const QModelIndex &row) const;
  void setMarkerComments(const QModelIndex &row,const QString &str);
  unsigned cartNumber(const QModelIndex &row) const;
  void setCartNumber(const QModelIndex &row,unsigned cartnum);
  int totalLength() const;
  void removeItem(const QModelIndex &row);
  int lineCount() const;
  void save(RDLogLine::TransType first_trans=RDLogLine::NoTrans);

 signals:
  void totalLengthChanged(int msec);

 public slots:
  QModelIndex processCartDrop(int line,RDLogLine *ll);
  bool moveUp(const QModelIndex &row);
  bool moveDown(const QModelIndex &row);

 protected:
  void updateModel();
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<QVariant> d_icons;
  QList<RDLogLine::Type> d_event_types;
  QList<RDLogLine::TransType> d_trans_types;
  QList<int> d_lengths;
  QMap<QString,QVariant> d_group_colors;
  QStringList d_marker_comments;
  QString d_event_name;
  ImportType d_import_type;
  bool d_auto_first_trans;
};


#endif  // IMPORTCARTSMODEL_H
