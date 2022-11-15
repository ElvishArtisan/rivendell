// rddropboxlistmodel.h
//
// Data model for Rivendell dropboxes
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

#ifndef RDDROPBOXLISTMODEL_H
#define RDDROPBOXLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>

class RDDropboxListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDDropboxListModel(const QString &hostname,QObject *parent=0);
  ~RDDropboxListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  int dropboxId(const QModelIndex &row) const;
  QModelIndex addDropbox(int box_id);
  void removeDropbox(const QModelIndex &row);
  void removeDropbox(int box_id);
  void refresh(const QModelIndex &row);
  void refresh(int box_id);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  bool d_include_none;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<QVariant> d_group_colors;
  QList<int> d_box_ids;
  QString d_hostname;
  QStringList d_column_fields;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
};


#endif  // RDDROPBOXLISTMODEL_H
