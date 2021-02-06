// rdgpiologmodel.h
//
// Data model for Rivendell GPIO log records
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

#ifndef RDGPIOLOGMODEL_H
#define RDGPIOLOGMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdmatrix.h>
#include <rdnotification.h>
#include <rduser.h>

class RDGpioLogModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDGpioLogModel(QObject *parent=0);
  ~RDGpioLogModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  RDMatrix::GpioType gpioType() const;
  int matrixNumber() const;
  QDate dateFilter() const;
  QString stateFilter() const;
  void addEvent(int line,bool state);

 public slots:
  void setGpioType(RDMatrix::GpioType type);
  void setMatrixNumber(int matrix_num);
  void setDateFilter(const QDate &date);
  void setStateFilter(const QString &str);

 protected:
  void updateModel();
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
  QList<QVariant> d_text_colors;
  int d_matrix_number;
  RDMatrix::GpioType d_gpio_type;
  QDate d_date_filter;
  QString d_state_filter;
};


#endif  // RDGPIOLOGMODEL_H
