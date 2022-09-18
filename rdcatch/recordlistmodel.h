// recordlistmodel.h
//
// Data model for Rivendell RDCatch events
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

#ifndef RECORDLISTMODEL_H
#define RECORDLISTMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QList>
#include <QPalette>

#include <rddb.h>
#include <rddeck.h>
#include <rdrecording.h>
#include <rdnotification.h>

class RecordListModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  RecordListModel(QObject *parent=0);
  ~RecordListModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  void sort(int col,Qt::SortOrder order=Qt::AscendingOrder);
  unsigned recordId(const QModelIndex &row) const;
  RDRecording::Type recordType(const QModelIndex &row) const;
  RDRecording::ExitCode recordExitCode(const QModelIndex &row) const;
  QString hostName(const QModelIndex &row) const;
  QString cutName(const QModelIndex &row) const;
  bool recordIsNext(const QModelIndex &row) const;
  void setRecordIsNext(const QModelIndex &row,bool state);
  void setRecordIsNext(unsigned rec_id,bool state);
  void clearNextRecords();
  RDDeck::Status recordStatus(const QModelIndex &row) const;
  void setRecordStatus(const QModelIndex &row,RDDeck::Status status);
  void setRecordStatus(unsigned rec_id,RDDeck::Status status);
  void channelCounts(int chan,int *waiting,int *active,unsigned *rec_id);
  QModelIndex addRecord(unsigned id);
  void removeRecord(const QModelIndex &row);
  void removeRecord(unsigned id);
  void refresh(const QModelIndex &row);
  bool refresh(unsigned id);
  void setFilterSql(const QString &sql);

 signals:
  void updateNextEvents();

 public slots:
  void notificationReceivedData(RDNotification *notify);

 protected:
  void updateModel(const QString &filter_sql);
  void updateRowLine(int line);
  void updateRow(int row,RDSqlQuery *q);
  QString sqlFields() const;

 private:
  QString GetSourceName(QString station,int matrix,int input,bool *ok);
  QString GetDestinationName(QString station,int matrix,int output);
  void UpdateStatus(int line);
  QString CartText(unsigned cartnum) const;
  QString CutText(const QString &cutname) const;
  QString LevelText(int lvl) const;
  int d_sort_column;
  Qt::SortOrder d_sort_order;
  QMap<Qt::SortOrder,QString> d_sort_clauses;
  QStringList d_order_columns;
  QPalette d_palette;
  QFont d_font;
  QFont d_bold_font;
  QString d_localhost_name;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QList<QVariant> > d_texts;
  QList<unsigned> d_ids;
  QList<int> d_channels;
  QList<RDRecording::Type> d_types;
  QList<RDRecording::ExitCode> d_exit_codes;
  QList<QVariant> d_text_colors;
  QList<QVariant> d_back_colors;
  QList<bool> d_is_nexts;
  QList<RDDeck::Status> d_statuses;
  QList<QList<QVariant> > d_icons;
  QMap<QString,QMap<int,QString> > d_router_names;
  QString d_filter_sql;
};


#endif  // RECORDLISTMODEL_H
