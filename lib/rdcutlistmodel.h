// rdcutlistmodel.h
//
// Data model for Rivendell cut metadata
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

#ifndef RDCUTLISTMODEL_H
#define RDCUTLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QPalette>

#include <rdcart.h>
#include <rddb.h>
#include <rdnotification.h>

class RDCutListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDCutListModel(bool use_weighting,QObject *parent=0);
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  QString cutName(const QModelIndex &row) const;
  QModelIndex addCut(const QString &name);
  void removeCut(const QModelIndex &index);
  void removeCut(const QString &cutname);
  QModelIndex refresh(const QModelIndex &index);
  QModelIndex refresh(const QString &cutname);
  unsigned cartNumber() const;
  QModelIndex row(const QString &cutname) const;
  bool playOrderDuplicates(QList<int> *values) const;

 public slots:
  void setCartNumber(unsigned cartnum);
  void processNotification(RDNotification *notify);

 protected:
  void updateRow(int row,RDSqlQuery *q);
  void updateCutLine(int line);
  QString sqlFields() const;
  void sortRows(int use_weighting);

 private:
  RDCart::Validity ValidateCut(RDSqlQuery *q,unsigned offset,
			       RDCart::Validity prev_validity,
			       const QDateTime &datetime);
  void DumpIndex(const QString &str) const;
  QPalette d_palette;
  QFont d_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<QVariant> d_colors;
  QList<int> d_row_index;
  unsigned d_cart_number;
  bool d_use_weighting;
};


#endif  // RDCUTLISTMODEL_H
