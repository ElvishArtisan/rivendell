// rdloglistmodel.h
//
// Data model for Rivendell log metadata
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGLISTMODEL_H
#define RDLOGLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>

class RDLogListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDLogListModel(QObject *parent=0);
  ~RDLogListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  QString logName(const QModelIndex &row) const;
  QModelIndex addLog(const QString &name);
  void removeLog(const QModelIndex &row);
  void removeLog(const QString &logname);
  void refresh(const QModelIndex &row);
  void refresh(const QString &logname);

 public slots:
   void setFilterSql(const QString &where_sql,const QString &limit_sql);

 private slots:
  void processNotification(RDNotification *notify);

 protected:
  void updateModel(const QString &where_sql,const QString &limit_sql);
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QStringList d_service_names;
  QList<QVariant> d_headers;
  QList<QList<QVariant> > d_texts;
  QList<QList<QVariant> > d_icons;
  QList<QVariant> d_alignments;
  QString d_filter_where_sql;
  QString d_filter_limit_sql;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
  QStringList d_column_fields;
};


#endif  // RDLOGLISTMODEL_H
