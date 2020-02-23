// rdcastmanager.cpp
//
// A PodCast Management Utility for Rivendell.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>
#include <qtranslator.h>
#include <qmessagebox.h>

#include <rdescape_string.h>
#include <rdpodcast.h>

#include "globals.h"
#include "list_casts.h"
#include "rdcastmanager.h"

//
// Icons
//
#include "../icons/rdcastmanager-22x22.xpm"
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"

//
// Global Resources
//
QString cast_filter;
QString cast_group;
QString cast_schedcode;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
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
  // Create Icons
  //
  cast_rivendell_map=new QPixmap(rdcastmanager_22x22_xpm);
  setWindowIcon(*cast_rivendell_map);
  cast_greencheckmark_map=new QPixmap(greencheckmark_xpm);
  cast_redx_map=new QPixmap(redx_xpm);

  //
  // Feed List
  //
  cast_feed_list=new RDListView(this);
  cast_feed_list->setAllColumnsShowFocus(true);
  cast_feed_list->setItemMargin(5);
  connect(cast_feed_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(feedDoubleclickedData(Q3ListViewItem *,const QPoint &,int)));
  cast_feed_list->addColumn("");
  cast_feed_list->setColumnAlignment(0,Qt::AlignCenter);

  cast_feed_list->addColumn(tr("Key Name"));
  cast_feed_list->setColumnAlignment(1,Qt::AlignHCenter);

  cast_feed_list->addColumn(tr("Feed Name"));
  cast_feed_list->setColumnAlignment(2,Qt::AlignLeft);

  cast_feed_list->addColumn(tr("Superfeed"));
  cast_feed_list->setColumnAlignment(3,Qt::AlignCenter);

  cast_feed_list->addColumn(tr("Description"));
  cast_feed_list->setColumnAlignment(4,Qt::AlignLeft);

  cast_feed_list->addColumn(tr("Casts"));
  cast_feed_list->setColumnAlignment(5,Qt::AlignCenter);

  //
  // Open Button
  //
  cast_open_button=new QPushButton(this);
  cast_open_button->setFont(buttonFont());
  cast_open_button->setText(tr("&View\nFeed"));
  connect(cast_open_button,SIGNAL(clicked()),this,SLOT(openData()));

  //
  // Close Button
  //
  cast_close_button=new QPushButton(this);
  cast_close_button->setFont(buttonFont());
  cast_close_button->setText(tr("&Close"));
  connect(cast_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  cast_resize=true;
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
  setCaption(str1+": "+rda->config()->stationName()+" "+str2+" "+rda->ripc()->user());
  RefreshList();
}


void MainWidget::openData()
{
  RDListViewItem *item=(RDListViewItem *)cast_feed_list->selectedItem();
  if(item==NULL) {
    return;
  }
  ListCasts *casts=new ListCasts(item->id(),item->text(3)=="Y",this);
  casts->exec();
  RefreshItem(item);
  delete casts;
}


void MainWidget::feedDoubleclickedData(Q3ListViewItem *,const QPoint &,int)
{
  openData();
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if(cast_resize) {
    cast_feed_list->setGeometry(10,10,size().width()-20,size().height()-70);
    cast_open_button->setGeometry(10,size().height()-55,80,50);
    cast_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
  }
}


void MainWidget::RefreshItem(RDListViewItem *item)
{
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString sql;
  int active=0;
  int total=0;

  sql=QString("select ")+
    "CHANNEL_TITLE,"+        // 00
    "IS_SUPERFEED,"+         // 01
    "CHANNEL_DESCRIPTION,"+  // 02
    "ID "+                   // 03
    "from FEEDS where "+
    "KEY_NAME=\""+RDEscapeString(item->text(1))+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("select STATUS from PODCASTS where FEED_ID=%u",
			  q->value(3).toUInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      total++;
      switch((RDPodcast::Status)q1->value(0).toUInt()) {
	case RDPodcast::StatusActive:
	case RDPodcast::StatusExpired:
	  active++;
	  break;

	case RDPodcast::StatusPending:
	  break;
      }
    }
    delete q1;
    if(active==total) {
      item->setPixmap(0,*cast_greencheckmark_map);
    }
    else {
      item->setPixmap(0,*cast_redx_map);
    }
    item->setText(2,q->value(0).toString());
    item->setText(3,q->value(1).toString());
    item->setText(4,q->value(2).toString());
    item->setText(5,QString().sprintf("%d / %d",active,total));
  }
  delete q;
}


void MainWidget::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  int id=-1;
  RDListViewItem *selected_item=NULL;
  RDListViewItem *item=(RDListViewItem *)cast_feed_list->selectedItem();
  if(item!=NULL) {
    id=item->id();
  }
  cast_feed_list->clear();
  sql=QString("select KEY_NAME from FEED_PERMS where ")+
    "USER_NAME=\""+RDEscapeString(rda->user()->name())+"\"";
  q=new RDSqlQuery(sql);
  if(q->size()<=0) {  // No valid feeds!
    delete q;
    return;
  }
  sql="select ID,KEY_NAME from FEEDS where ";
  while(q->next()) {
    sql+=QString("(KEY_NAME=\"")+RDEscapeString(q->value(0).toString())+"\")||";
  }
  delete q;
  sql=sql.left(sql.length()-2);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(cast_feed_list);
    item->setId(q->value(0).toInt());
    item->setText(1,q->value(1).toString());
    RefreshItem(item);
    if(item->id()==id) {
      selected_item=item;
    }
  }
  delete q;
  if(selected_item!=NULL) {
    cast_feed_list->setSelected(selected_item,true);
    cast_feed_list->ensureItemVisible(item);
  }
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

  QTranslator qt(0);
  qt.load(qt_path+QString("qt_")+QTextCodec::locale(),".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(tr_path+QString("librd_")+QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(tr_path+QString("librdhpi_")+QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(tr_path+QString("rdcastmanager_")+QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->setGeometry(w->geometry().x(),w->geometry().y(),w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
