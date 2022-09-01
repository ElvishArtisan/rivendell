// rddiscmodel.h
//
// Data model for Audio CD track information
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

#ifndef RDDISCMODEL_H
#define RDDISCMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>
#include <QStringList>
#include <QTimer>

#include <rdcdplayer.h>
#include <rddiscrecord.h>
#include <rddb.h>
#include <rdmatrix.h>
#include <rdnotification.h>

class RDDiscModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RDDiscModel(QObject *parent=0);
  ~RDDiscModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  int trackNumber(const QModelIndex &row) const;
  bool trackContainsData(const QModelIndex &row) const;
  QString trackTitle(const QModelIndex &row) const;
  void setTrackTitle(const QModelIndex &row,const QString &str);
  QString cutName(const QModelIndex &row) const;
  void setCutName(const QModelIndex &row,const QString &cutname);
  int leadTrack(const QModelIndex &row) const;
  void setLeadTrack(const QModelIndex &row,int track);
  void clearCutNames();
  void refresh(const QModelIndex &row);

 public slots:
  void clear();
  void setDisc(RDCdPlayer *player);
  void refresh(RDDiscRecord *rec,RDDiscRecord::DataSource src);

 private:
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QStringList d_cut_names;
  QList<int> d_lead_tracks;
};


#endif  // RDDISCMODEL_H
