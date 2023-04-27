// rdfeedlistview.cpp
//
// RDTableView widget for RSS feeds
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

#include <QDragEnterEvent>
#include <QHeaderView>
#include <QMessageBox>

#include "rdfeed.h"
#include "rdfeedlistmodel.h"
#include "rdfeedlistview.h"
#include "rdtextfile.h"

RDFeedListView::RDFeedListView(QWidget *parent)
  :RDTableView(parent)
{
  d_mouse_row=-1;

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);

  d_front_report_action=d_mouse_menu->
    addAction(tr("Generate Front Report"),this,SLOT(generateFrontReportData()));
  d_front_report_action->setCheckable(false);

  d_back_report_action=d_mouse_menu->
    addAction(tr("Generate Back Report"),this,SLOT(generateBackReportData()));
  d_back_report_action->setCheckable(false);

  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));

  //
  // XSLT Engine (for feed reports)
  //
  d_xslt_engine=
    new RDXsltEngine("/usr/share/rivendell/rdcastmanager-report.xsl",this);
}


RDFeedListView::~RDFeedListView()
{
  delete d_xslt_engine;
}


void RDFeedListView::aboutToShowMenuData()
{
  RDFeedListModel *mod=(RDFeedListModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    d_front_report_action->setEnabled(false);
    d_back_report_action->setEnabled(false);
  }
  else {
    d_front_report_action->setEnabled(true);
    d_back_report_action->setEnabled(true);
  }
}


void RDFeedListView::generateFrontReportData()
{
  QString err_msg;
  RDFeedListModel *m=(RDFeedListModel *)model();
  QString keyname=m->data(m->index(d_mouse_row,0)).toString();
  RDFeed *feed=new RDFeed(keyname,rda->config(),this);
  QByteArray xml;
  if(feed->downloadXml(&xml,&err_msg)) {
    QString output_filename="report.html";
    if(d_xslt_engine->transformXml(&output_filename,xml,&err_msg)) {
      RDWebBrowser("file://"+output_filename);
    }
    else {
      QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
    }
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
  }
}


void RDFeedListView::generateBackReportData()
{
  QString err_msg;
  RDFeedListModel *m=(RDFeedListModel *)model();
  QString url=m->data(m->index(d_mouse_row,6)).toString();

  QString output_filename="report.html";
  if(d_xslt_engine->transformUrl(&output_filename,url,&err_msg)) {
    RDWebBrowser("file://"+output_filename);
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
    return;
  }
}


void RDFeedListView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    d_mouse_row=indexAt(e->pos()).row();
    if((d_mouse_row>=0)&&(d_mouse_row<model()->rowCount())) {
      d_mouse_menu->popup(e->globalPos());
    }
    else {
      d_mouse_row=-1;
    }
  }
  QTableView::mousePressEvent(e);
}
