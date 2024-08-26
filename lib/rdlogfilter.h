// rdlogfilter.h
//
// Filter widget for picking Rivendell logs.
//
//   (C) Copyright 2017-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGFILTER_H
#define RDLOGFILTER_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QStringList>
#include <QWidget>

class RDLogFilter : public QWidget
{
  Q_OBJECT;
 public:
  enum FilterMode {NoFilter=0,UserFilter=1,StationFilter=2};
  RDLogFilter(FilterMode mode,QWidget *parent=0);
  ~RDLogFilter();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QString filterServiceName() const;
  QString filterText() const;
  QString whereSql() const;
  QString limitSql() const;

 public slots:
  void changeUser();

 signals:
  void filterChanged(const QString &where_sql,const QString &limit_sql);

 private slots:
  void filterChangedData(const QString &str);
  void filterChangedData();
  void filterClearedData();
  void serviceChangedData(int n);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void LogSearchString(const QString &str);
  FilterMode filter_filter_mode;
  QLabel *filter_service_label;
  QComboBox *filter_service_box;
  QLabel *filter_filter_label;
  QLineEdit *filter_filter_edit;
  QPushButton *filter_clear_button;
  QCheckBox *filter_recent_check;
  QLabel *filter_recent_label;
  QString filter_current_username;
};


#endif  // RDLOGFILTER_H
