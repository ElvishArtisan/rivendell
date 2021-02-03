// rdreplcartlistmodel.h
//
// Data model for replicator carts
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

#ifndef RDREPLCARTLISTMODEL_H
#define RDREPLCARTLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>
#include <QTimer>

#include <rddb.h>
#include <rdnotification.h>

class RDReplCartListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDReplCartListModel(QObject *parent=0);
  ~RDReplCartListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  unsigned cartId(const QModelIndex &row) const;
  QModelIndex addCart(unsigned id);
  void removeCart(const QModelIndex &row);
  void removeCart(unsigned id);
  void refresh(const QModelIndex &row);
  void refresh(unsigned id);
  QString replicatorName() const;

 public slots:
  void setReplicatorName(const QString &repl_name);

 private slots:
  void refreshTimeoutData();

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
  QList<QVariant> d_icons;
  QList<unsigned> d_ids;
  QString d_replicator_name;
  QTimer *d_refresh_timer;
};


#endif  // RDREPLCARTLISTMODEL_H
