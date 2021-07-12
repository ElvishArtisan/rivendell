// rdcastmanager.cpp
//
// A PodCast Management Utility for Rivendell.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QClipboard>
#include <QTranslator>
#include <QMessageBox>

#include <rdconf.h>
#include <rdescape_string.h>
#include <rdpodcast.h>

#include "edit_cast.h"
#include "globals.h"
#include "list_casts.h"
#include "rdcastmanager.h"

//
// Global Resources
//
QString cast_filter;
QString cast_group;
QString cast_schedcode;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDMainWindow("rdcastmanager",c)
{
  QString str1;
  QString str2;
  QString err_msg;

  cast_resize=false;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Open the Database/
  //
  rda=new RDApplication("RDCastManager","rdcastmanager",RDCASTMANAGER_USAGE,
			this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDCastManager - "+tr("Error"),err_msg);
    exit(1);
  }
  setWindowIcon(rda->iconEngine()->
		applicationIcon(RDIconEngine::RdCastManager,22));

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDCastManager - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  str1=QString("RDCastManager")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User: [Unknown]"));
  setWindowTitle(str1+": "+rda->config()->stationName()+" "+str2);

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Feed List
  //
  cast_feed_view=new RDTreeView(this);
  cast_feed_model=new RDFeedListModel(false,false,this);
  cast_feed_model->setFont(font());
  cast_feed_model->setPalette(palette());
  cast_feed_view->setModel(cast_feed_model);
  connect(cast_feed_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  cast_feed_model,SLOT(processNotification(RDNotification *)));
  connect(cast_feed_view,SIGNAL(doubleClicked(const QModelIndex &)),
  	  this,SLOT(feedDoubleClickedData(const QModelIndex &)));
  connect(cast_feed_view->selectionModel(),
       SIGNAL(selectionChanged(const QItemSelection &,const QItemSelection &)),
       this,
       SLOT(selectionChangedData(const QItemSelection &,
				 const QItemSelection &)));

  //
  // Open Button
  //
  cast_open_button=new QPushButton(this);
  cast_open_button->setFont(buttonFont());
  cast_open_button->setText(tr("View\nFeed"));
  cast_open_button->setDisabled(true);
  connect(cast_open_button,SIGNAL(clicked()),this,SLOT(openData()));

  //
  // Copy Button
  //
  cast_copy_button=new QPushButton(this);
  cast_copy_button->setFont(buttonFont());
  cast_copy_button->setText(tr("Copy URL to\nClipboard"));
  cast_copy_button->setDisabled(true);
  connect(cast_copy_button,SIGNAL(clicked()),this,SLOT(copyData()));

  //
  // Close Button
  //
  cast_close_button=new QPushButton(this);
  cast_close_button->setFont(buttonFont());
  cast_close_button->setText(tr("Close"));
  connect(cast_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  cast_resize=true;

  loadSettings(true);
}


QSize MainWidget::sizeHint() const
{
  return QSize(640,480);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::userChangedData()
{
  QString str1;
  QString str2;

  str1=QString("RDCastManager")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setWindowTitle(str1+": "+rda->config()->stationName()+" "+str2+" "+rda->ripc()->user());

  cast_feed_model->changeUser();
}


void MainWidget::modelResetData()
{
  for(int i=0;i<cast_feed_model->columnCount();i++) {
    cast_feed_view->resizeColumnToContents(i);
  }
}


void MainWidget::selectionChangedData(const QItemSelection &before,
				      const QItemSelection &after)
{
  QModelIndexList rows=cast_feed_view->selectionModel()->selectedRows();

  cast_open_button->setEnabled(rows.size()==1);
  cast_copy_button->setEnabled(rows.size()==1);
  if(cast_feed_model->isFeed(rows.first())) {
    cast_open_button->setText(tr("View\nFeed"));
  }
  if(cast_feed_model->isCast(rows.first())) {
    cast_open_button->setText(tr("Edit"));
  }
}


void MainWidget::openData()
{
  QModelIndexList rows=cast_feed_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  if(cast_feed_model->isFeed(rows.first())) {
    ListCasts *d=new ListCasts(cast_feed_model->feedId(rows.first()),this);
    if(d->exec()) {
      cast_feed_model->refreshRow(rows.first());
    }
    delete d;
  }
  if(cast_feed_model->isCast(rows.first())) {
    EditCast *d=new EditCast(cast_feed_model->castId(rows.first()),this);
    if(d->exec()) {
      cast_feed_model->refreshRow(rows.first());
    }
    delete d;
  }
}


void MainWidget::copyData()
{
  QModelIndexList rows=cast_feed_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  QApplication::clipboard()->setText(cast_feed_model->publicUrl(rows.first()));
}


void MainWidget::feedDoubleClickedData(const QModelIndex &index)
{
  openData();
}


void MainWidget::quitMainWidget()
{
  saveSettings();
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(cast_resize) {
    cast_feed_view->setGeometry(10,10,size().width()-20,size().height()-70);
    cast_open_button->setGeometry(10,size().height()-55,80,50);
    cast_copy_button->setGeometry(120,size().height()-55,100,50);
    cast_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
  }
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  quitMainWidget();
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString tr_path;
  QString qt_path;
  tr_path=QString(PREFIX)+QString("/share/rivendell/");
  qt_path=QString("/usr/share/qt4/translation/");

  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    QTranslator qt(0);
    qt.load(qt_path+QString("qt_")+loc,".");
    a.installTranslator(&qt);

    QTranslator rd(0);
    rd.load(tr_path+QString("librd_")+loc,".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(tr_path+QString("librdhpi_")+loc,".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(tr_path+QString("rdcastmanager_")+loc,".");
    a.installTranslator(&tr);
  }

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
