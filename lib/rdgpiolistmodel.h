// rdgpiolistmodel.h
//
// Data model for Rivendell GPI/GPO configuration
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

#ifndef RDGPIOLISTMODEL_H
#define RDGPIOLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>
#include <QTimer>

#include <rddb.h>
#include <rdmatrix.h>
#include <rdnotification.h>

class RDGpioListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDGpioListModel(RDMatrix *mtx,RDMatrix::GpioType type,QObject *parent=0);
  ~RDGpioListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  int lineId(const QModelIndex &row) const;
  void refresh(const QModelIndex &row);
  void refresh(int id);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(bool on_values,int row,RDSqlQuery *q);
  QString sqlFields(bool on_fields) const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<int> d_ids;
  RDMatrix *d_mtx;
  RDMatrix::GpioType d_type;
  QString d_table;
};


#endif  // RDGPIOLISTMODEL_H
