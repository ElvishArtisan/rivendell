// rdlibrarymodel.h
//
// Data model for the Rivendell cart library
//
//   (C) Copyright 2021-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QAbstractItemModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QMap>
#include <QPalette>

#include <rdcart.h>
#include <rddb.h>
#include <rdnotification.h>

class RDLibraryModel : public QAbstractItemModel
{
  Q_OBJECT
 public:
  RDLibraryModel(QObject *parent=0);
  ~RDLibraryModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  QModelIndex index(int row,int col,
		    const QModelIndex &parent=QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  bool hasChildren(const QModelIndex &parent=QModelIndex()) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  bool isCart(const QModelIndex &index) const;
  bool isCut(const QModelIndex &index) const;
  unsigned cartNumber(const QModelIndex &index) const;
  QModelIndex cartRow(unsigned cartnum) const;
  QModelIndex cutRow(const QString &cutname) const;
  RDCart::Type cartType(const QModelIndex &index) const;
  QString cutName(const QModelIndex &index) const;
  QString cartOwnedBy(const QModelIndex &index);
  QModelIndex addCart(unsigned cartnum);
  void removeCart(const QModelIndex &index);
  void removeCart(unsigned cartnum);
  void refreshRow(const QModelIndex &index);
  QModelIndex refreshCart(unsigned cartnum);
  bool showNotes() const;
  int cartLimit() const;
  QString orderBySql() const;

 signals:
  void rowCountChanged(int rows);

 public slots:
  void setShowNotes(int state);
  void setFilterSql(const QString &sql,int cart_limit);

 private slots:
  void processNotification(RDNotification *notify);

 protected:
  void updateModel(const QString &filter_sql);
  void updateCartLine(int cartline);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QByteArray DumpIndex(const QModelIndex &index,const QString &caption="") const;
  bool d_show_notes;
  QString d_filter_sql;
  QString d_order_by_sql;
  int d_cart_limit;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
  QPalette d_palette;
  QFont d_font;
  QFontMetrics *d_font_metrics;
  QFont d_bold_font;
  QFontMetrics *d_bold_font_metrics;
  QList<QVariant> d_headers;
  QStringList d_order_columns;
  QMap<Qt::SortOrder,QString> d_sort_clauses;
  QList<QList<QVariant> > d_texts;
  QList<QList<QVariant> > d_icons;
  QList<QVariant> d_notes;
  QList<QList<QList<QVariant> > > d_cut_texts;
  QList<QStringList> d_cut_cutnames;
  QList<QVariant> d_alignments;
  QList<QVariant> d_background_colors;
  QList<unsigned> d_cart_numbers;
  QList<RDCart::Type> d_cart_types;
  QMap<QString,QVariant> d_group_colors;
  bool d_filter_set;
};


#endif  // RDLIBRARYMODEL_H
