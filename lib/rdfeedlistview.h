// rdfeedlistview.h
//
// RDTableView widget for RSS feeds
//
//   (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFEEDLISTVIEW_H
#define RDFEEDLISTVIEW_H

#include <QAction>
#include <QList>
#include <QMenu>

#include <rdtableview.h>
#include <rdtempdirectory.h>
#include "rdxsltengine.h"

class RDFeedListView : public RDTableView
{
  Q_OBJECT
 public:
  RDFeedListView(QWidget *parent=0);
  ~RDFeedListView();

 private slots:
  void aboutToShowMenuData();
  void generateFrontReportData();
  void generateBackReportData();

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  int d_mouse_row;
  QMenu *d_mouse_menu;
  QAction *d_front_report_action;
  QAction *d_back_report_action;
  RDXsltEngine *d_xslt_engine;
};


#endif  // RDFEEDLISTVIEW_H
