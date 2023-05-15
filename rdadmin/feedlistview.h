// feedlistview.h
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

#ifndef FEEDLISTVIEW_H
#define FEEDLISTVIEW_H

#include <QAction>
#include <QList>
#include <QMenu>

#include <rdfeed.h>
#include <rdtableview.h>
#include <rdtempdirectory.h>
#include <rdxsltengine.h>

class FeedListView : public RDTableView
{
  Q_OBJECT
 public:
  FeedListView(QWidget *parent=0);
  ~FeedListView();

 private slots:
  void aboutToShowMenuData();
  void generateFrontItemReportData();
  void generateBackItemReportData();
  void viewFrontRawXmlData();
  void viewBackRawXmlData();
  void validateBluebrryData();

 protected:
  void mousePressEvent(QMouseEvent *e);

 private:
  int d_mouse_row;
  QMenu *d_mouse_menu;
  QAction *d_front_item_report_action;
  QAction *d_back_item_report_action;
  QAction *d_front_raw_xml_action;
  QAction *d_back_raw_xml_action;
  QAction *d_bluebrry_validate_action;
  QList<RDXsltEngine *> d_xslt_engines;
};


#endif  // FEEDLISTVIEW_H
