// rdlibrarymodel.h
//
// Data model for the Rivendell cart library
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

#ifndef RDLIBRARYMODEL_H
#define RDLIBRARYMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QMap>
#include <QPalette>

#include <rdcart.h>
#include <rddb.h>
#include <rdlog_icons.h>
#include <rdnotification.h>

class RDLibraryModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDLibraryModel(QObject *parent=0);
  ~RDLibraryModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  unsigned cartNumber(int row) const;
  RDCart::Type cartType(int row) const;
  QString cartOwnedBy(int row);
  int addCart(unsigned cartnum);
  void removeCart(unsigned cartnum);
  void refreshRow(int row);
  void refreshCart(unsigned cartnum);

 signals:
  void rowCountChanged(int rows);

 public slots:
  void setFilterSql(const QString &sql);
  void processNotification(RDNotification *notify);

 protected:
  void updateModel(const QString &filter_sql);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QList<QVariant> > d_texts;
  QList<QList<QVariant> > d_icons;
  QList<QVariant> d_alignments;
  QList<QVariant> d_background_colors;
  QList<RDCart::Type> d_cart_types;
  QMap<QString,QVariant> d_group_colors;
  RDLogIcons *d_log_icons;
};


#endif  // RDLIBRARYMODEL_H
