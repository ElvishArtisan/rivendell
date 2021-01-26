// rdpodcastfilter.h
//
// Filter widget for podcasts
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

#ifndef RDPODCASTFILTER_H
#define RDPODCASTFILTER_H

#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <rdwidget.h>

class RDPodcastFilter : public RDWidget
{
  Q_OBJECT;
 public:
  RDPodcastFilter(QWidget *parent=0);
  ~RDPodcastFilter();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  QString filterSql() const;
  QString filterText() const;
  static QString searchString(const QString &filter,bool active_only);

 public slots:
  void setFilterText(const QString &str);

 signals:
  void filterChanged(const QString &where_sql);

 private slots:
  void filterChangedData(const QString &str);
  void checkChangedData(int n);

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLineEdit *d_filter_edit;
  QLabel *d_filter_label;
  QCheckBox *d_showactive_check;
  QLabel *d_showactive_label;
};


#endif  // RDPODCASTFILTER_H
