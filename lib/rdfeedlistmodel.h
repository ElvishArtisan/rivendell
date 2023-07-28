// rdfeedlistmodel.h
//
// Data model for Rivendell RSS feeds
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

#ifndef RDFEEDLISTMODEL_H
#define RDFEEDLISTMODEL_H

#include <QAbstractItemModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QMap>
#include <QPalette>

#include <rdcart.h>
#include <rddb.h>
#include <rdnotification.h>

class RDFeedListModel : public QAbstractItemModel
{
  Q_OBJECT
 public:
  RDFeedListModel(bool is_admin,bool incl_none,QObject *parent);
  ~RDFeedListModel();
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
  bool isFeed(const QModelIndex &index) const;
  bool isCast(const QModelIndex &index) const;
  QString keyName(const QModelIndex &index) const;
  QString keyName(int row) const;
  unsigned feedId(const QModelIndex &index) const;
  QString publicUrl(const QModelIndex &index) const;
  QModelIndex feedRow(const QString &keyname) const;
  QModelIndex castRow(unsigned cast_id) const;
  unsigned castId(const QModelIndex &index) const;
  QString castOwnedBy(const QModelIndex &index);
  QModelIndex addFeed(const QString &keyname);
  void removeFeed(const QString &keyname);
  void refreshRow(const QModelIndex &index);
  void refreshFeed(const QString &keyname);

 signals:
  void rowCountChanged(int rows);

 public slots:
  void changeUser();
  void processNotification(RDNotification *notify);

 protected:
  void updateModel(const QString &filter_sql);
  void updateFeedLine(int line);
  void updateCastLine(int feed_line,int cast_line);
  void updateCastRow(int feed_line,int cast_line,RDSqlQuery *q,int q_offset);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;
  QString castSqlFields() const;

 private:
  QByteArray DumpIndex(const QModelIndex &index,const QString &caption="") const;
  QPalette d_palette;
  QFont d_font;
  QFontMetrics *d_font_metrics;
  QFont d_bold_font;
  QFontMetrics *d_bold_font_metrics;
  // Column Data
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;

  // Row Data
  QList<unsigned> d_feed_ids;
  QList<QList<QVariant> > d_texts;
  QList<QList<QVariant> > d_icons;
  QList<QList<unsigned> > d_cast_ids;
  QList<QList<QVariant> > d_cast_icons;
  QStringList d_key_names;

  // Z Data (episodes)
  QList<QList<QList<QVariant> > > d_cast_texts;

  bool d_is_admin;
  bool d_include_none;
};


#endif  // RDFEEDLISTMODEL_H
