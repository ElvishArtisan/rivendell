// rdlogmodel.h
//
// Data model for Rivendell logs
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGMODEL_H
#define RDLOGMODEL_H

#include <QAbstractTableModel>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QPalette>

#include <rdlog_line.h>
#include <rdnotification.h>

class RDLogModel : public QAbstractTableModel
{
  Q_OBJECT
 public:
  enum StartTimeStyle {Estimated=0,Scheduled=1};
  RDLogModel(const QString &logname,bool read_only,QObject *parent=0);
  RDLogModel(QObject *parent=0);
  ~RDLogModel();
  QPalette palette();
  void setPalette(const QPalette &pal);
  QFont normalFont() const;
  QFont boldFont() const;
  void setFont(const QFont &font);
  int columnCount(const QModelIndex &parent=QModelIndex()) const;
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  QVariant headerData(int section,Qt::Orientation orient,
		      int role=Qt::DisplayRole) const;
  QVariant data(const QModelIndex &index,int role=Qt::DisplayRole) const;
  bool exists();
  bool exists(int line);
  bool exists(const QTime &hard_time,int except_line=-1);
  QString logName() const;
  void setLogName(QString logname);
  QString serviceName() const;
  int load(bool track_ptrs=false);
  void saveModified(RDConfig *config,bool update_tracks=true);
  void save(RDConfig *config,bool update_tracks=true,int line=-1);
  int append(const QString &logname,bool track_ptrs=false);
  int validate(QString *report,const QDate &date);
  void clear();
  void update(int line);
  int lineCount() const;
  void insert(int line,int num_lines,bool preserve_trans=false);
  void remove(int line,int num_lines,bool preserve_trans=false);
  void move(int from_line,int to_line);
  void copy(int from_line,int to_line);
  int length(int from_line,int to_line,QTime *sched_time=NULL);
  int lengthToStop(int from_line,QTime *sched_time=NULL);
  bool blockLength(int *nominal_length,int *actual_length,int line);
  QTime blockStartTime(int line) const;
  RDLogLine *logLine(int line) const;
  void setLogLine(int line,RDLogLine *ll);
  RDLogLine *loglineById(int id, bool ignore_holdovers=false) const;
  int lineById(int id, bool ignore_holdovers=false) const;
  int lineByStartHour(int hour,RDLogLine::StartTimeType type) const;
  int lineByStartHour(int hour) const;
  int nextTimeStart(QTime after);
  RDLogLine::TransType nextTransType(int);
  void removeCustomTransition(int line);
  int nextId() const;
  int nextLinkId() const;
  QString xml() const;

 public slots:
  void setTransition(int line,RDLogLine::TransType trans);
  void processNotification(RDNotification *notify);
  void setStartTimeStyle(StartTimeStyle style);

 protected:
  void emitDataChanged(int row);
  void emitAllDataChanged();
  virtual QStringList headerTexts() const;
  virtual QList<int> columnAlignments() const;
  virtual QPixmap cellIcon(int col,int row,RDLogLine *ll) const;
  virtual QString cellText(int col,int line,RDLogLine *ll) const;
  virtual QFont cellTextFont(int col,int line,RDLogLine *ll) const;
  virtual QColor cellTextColor(int col,int line,RDLogLine *ll) const;
  virtual QColor rowBackgroundColor(int line,RDLogLine *ll) const;

 private:
  QString StartTimeString(int line) const;
  int LoadLines(const QString &logname,int id_offset,bool track_ptrs);
  void SaveLine(int line);
  void InsertLines(QString values);
  void InsertLineValues(QString *query, int line);
  void MakeModel();
  QPalette d_palette;
  QFont d_font;
  QFontMetrics *d_fms;
  QFont d_bold_font;
  QFontMetrics *d_bold_fms;
  QList<QVariant> d_headers;
  QList<QVariant> d_alignments;
  QList<QVariant> d_size_hints;
  StartTimeStyle d_start_time_style;
  QString d_log_name;
  QString d_service_name;
  int d_max_id;
  bool d_read_only;
  QList<RDLogLine *> d_log_lines;
};


#endif  // RDLOGMODEL_H
