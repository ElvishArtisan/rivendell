// rdgrouplistmodel.h
//
// Data model for Rivendell groups
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDGROUPLISTMODEL_H
#define RDGROUPLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>

class RDGroupListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDGroupListModel(bool show_all,bool show_unchanged,bool user_is_admin,
		   QObject *parent);
  ~RDGroupListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  QModelIndex indexOf(const QString &grpname) const;
  QString groupName(const QModelIndex &row) const;
  QStringList allGroupNames() const;
  QModelIndex addGroup(const QString &name);
  QModelIndex renameGroup(const QString &oldname,const QString &newname);
  void removeGroup(const QModelIndex &row);
  void removeGroup(const QString &grpname);
  void refresh(const QModelIndex &row);
  void refresh(const QString &grpname);

 public slots:
  void changeUser();

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;
  virtual QString filterSql() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QStringList d_service_names;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<QVariant> d_icons;
  QList<QVariant> d_colors;
  bool d_show_all;
  bool d_show_unchanged;
  bool d_user_is_admin;
  QStringList d_visible_groups;
  QStringList d_column_fields;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
};


#endif  // RDGROUPLISTMODEL_H
