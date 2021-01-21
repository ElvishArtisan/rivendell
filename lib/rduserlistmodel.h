// rduserlistmodel.h
//
// Data model for Rivendell users
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

#ifndef RDUSERLISTMODEL_H
#define RDUSERLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>
#include <rduser.h>

class RDUserListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDUserListModel(QObject *parent=0);
  ~RDUserListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  QString userName(const QModelIndex &row) const;
  QModelIndex addUser(const QString &name);
  void removeUser(const QModelIndex &row);
  void removeUser(const QString &username);
  void refresh(const QModelIndex &row);
  void refresh(const QString &username);
  RDUser::Type typeFilter() const;

 public slots:
  void setTypeFilter(RDUser::Type type);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;
  virtual QString filterSql(const QString &username) const;

 private:
  QPalette d_palette;
  QFont d_font;
  QStringList d_service_names;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<QVariant> d_icons;
  RDUser::Type d_type_filter;
};


#endif  // RDUSERLISTMODEL_H
