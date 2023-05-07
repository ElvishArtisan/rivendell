// feedlistview.cpp
//
// RDTableView widget for RSS feeds
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

#include <QDragEnterEvent>
#include <QHeaderView>
#include <QMessageBox>

#include "rdfeedlistmodel.h"
#include "rdreport.h"

#include "feedlistview.h"
#include "rdtextfile.h"

FeedListView::FeedListView(QWidget *parent)
  :RDTableView(parent)
{
  d_mouse_row=-1;

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);

  d_front_item_report_action=d_mouse_menu->
    addAction(tr("Generate Item Report [Front]"),this,SLOT(generateFrontItemReportData()));
  d_front_item_report_action->setCheckable(false);
  d_back_item_report_action=d_mouse_menu->
    addAction(tr("Generate Item Report [Back]"),this,SLOT(generateBackItemReportData()));
  d_back_item_report_action->setCheckable(false);
  d_mouse_menu->addSeparator();
  d_bluebrry_validate_action=d_mouse_menu->
    addAction(tr("Validate with Bluebrry"),this,SLOT(validateBluebrryData()));

  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));
}


FeedListView::~FeedListView()
{
  for(int i=0;i<d_xslt_engines.size();i++) {
    delete d_xslt_engines.at(i);
  }
}


void FeedListView::aboutToShowMenuData()
{
  RDFeedListModel *mod=(RDFeedListModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    d_front_item_report_action->setEnabled(false);
    d_back_item_report_action->setEnabled(false);
  }
  else {
    d_front_item_report_action->setEnabled(true);
    d_back_item_report_action->setEnabled(true);
  }
}


void FeedListView::generateFrontItemReportData()
{
  QDateTime now=QDateTime::currentDateTime();
  bool ok=false;
  QList<unsigned> front_ids;
  QString err_msg;
  QString output_filename="report.html";
  RDFeedListModel *m=(RDFeedListModel *)model();
  QString keyname=m->data(m->index(d_mouse_row,0)).toString();
  RDFeed *feed=new RDFeed(keyname,rda->config(),this);

  if(feed->frontActiveCasts(&front_ids,&err_msg)) {
    QString xml=feed->rssXml(&err_msg,now,&ok,&front_ids);
    d_xslt_engines.
      push_back(new RDXsltEngine("/usr/share/rivendell/rss-front-item-report.xsl",this));
    if(d_xslt_engines.back()->transformXml(&output_filename,xml,&err_msg)) {
      RDWebBrowser("file://"+output_filename);
    }
    else {
      QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
    }
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Error accessing from XML data.")+"\n"+
			 "["+err_msg+"]");
  }
  delete feed;
}


void FeedListView::generateBackItemReportData()
{
  QDateTime now=QDateTime::currentDateTime();
  bool ok=false;
  QList<unsigned> back_ids;
  QString err_msg;
  QString output_filename="report.html";
  RDFeedListModel *m=(RDFeedListModel *)model();
  QString keyname=m->data(m->index(d_mouse_row,0)).toString();
  RDFeed *feed=new RDFeed(keyname,rda->config(),this);

  if(feed->backActiveCasts(&back_ids,&err_msg)) {
    QString xml=feed->rssXml(&err_msg,now,&ok,&back_ids);
    d_xslt_engines.
      push_back(new RDXsltEngine("/usr/share/rivendell/rss-back-item-report.xsl",this));
    if(d_xslt_engines.back()->transformXml(&output_filename,xml,&err_msg)) {
      RDWebBrowser("file://"+output_filename);
    }
    else {
      QMessageBox::warning(this,"RDAdmin - "+tr("Error"),err_msg);
    }
  }
  else {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Error accessing from XML data.")+"\n"+
			 "["+err_msg+"]");
  }
  delete feed;
}


void FeedListView::validateBluebrryData()
{
  RDFeedListModel *m=(RDFeedListModel *)model();
  QString keyname=m->data(m->index(d_mouse_row,0)).toString();
  RDFeed *feed=new RDFeed(keyname,rda->config(),this);

  QString url=QString("https://www.castfeedvalidator.com/validate.php?url=")+
    feed->publicUrl(feed->baseUrl(""),feed->keyName());
  RDWebBrowser(url);
  delete feed;
}


void FeedListView::mousePressEvent(QMouseEvent *e)
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
