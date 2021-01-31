// rdresourcelistmodel.h
//
// Data model for Rivendell console router resource settings
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

#ifndef RDRESOURCELISTMODEL_H
#define RDRESOURCELISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdmatrix.h>
#include <rdnotification.h>
#include <rduser.h>

class RDResourceListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDResourceListModel(RDMatrix *mtx,RDMatrix::VguestType vguest_type,
		      QObject *parent=0);
  ~RDResourceListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  int resourceId(const QModelIndex &row) const;
  int resourceNumber(const QModelIndex &row) const;
  void refresh(const QModelIndex &row);
  void refresh(int id);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<int> d_ids;
  RDMatrix *d_mtx;
  RDMatrix::Type d_type;
  RDMatrix::VguestType d_vguest_type;
};


#endif  // RDRESOURCELISTMODEL_H
