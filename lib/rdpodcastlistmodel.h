// rdpodcastlistmodel.h
//
// Data model for Rivendell podcast episodes
//
//   (C) Copyright 2021-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPODCASTLISTMODEL_H
#define RDPODCASTLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rdnotification.h>
#include <rduser.h>

class RDPodcastListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDPodcastListModel(unsigned feed_id,QObject *parent=0);
  ~RDPodcastListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  unsigned castId(const QModelIndex &row) const;
  QModelIndex addCast(unsigned cast_id);
  void removeCast(const QModelIndex &row);
  void removeCast(unsigned cast_id);
  void refresh(const QModelIndex &row);
  void refresh(unsigned cast_id);

 public slots:
  void setFilterSql(const QString &sql);
  void processNotification(RDNotification *notify);

 protected:
  void updateModel();
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;
  void loadItemImage(int image_id);
  QString imageFP(const QByteArray &img);

 private:
  QPalette d_palette;
  QFont d_font;
  QFontMetrics *d_font_metrics;
  QFont d_bold_font;
  QFontMetrics *d_bold_font_metrics;
  bool d_include_none;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<unsigned> d_cast_ids;
  QList<QList<QVariant> > d_texts;
  QList<int> d_item_image_ids;
  QList<QVariant> d_status_pixmaps;
  //  unsigned d_feed_id;
  QList<unsigned> d_feed_ids;
  QString d_filter_sql;
  QStringList d_column_fields;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
  QMap<int,QVariant> d_item_images;
};


#endif  // RDPODCASTLISTMODEL_H
