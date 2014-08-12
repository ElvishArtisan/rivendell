// rdcastmanager.cpp
//
// A PodCast Management Utility for Rivendell.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcastmanager.cpp,v 1.15.4.3 2014/01/21 21:59:31 cvs Exp $
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

#ifndef WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rd.h>
#include <rdconf.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdcastmanager.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdpodcast.h>
#include <dbversion.h>

#include <list_casts.h>
#include <globals.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"

//
// Global Resources
//
QString cast_filter;
QString cast_group;
QString cast_schedcode;
RDUser *cast_user;
RDRipc *cast_ripc;
RDStation *rdstation_conf;
RDConfig *config;
RDSystem *cast_system=NULL;

MainWidget::MainWidget(QWidget *parent,const char *name,WFlags f)
  :QMainWindow(parent,name,f)
{
  QString str1;
  QString str2;
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcastmanager","\n");
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }
  delete cmd;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Load Local Configs
  //
  config=new RDConfig();
  config->load();
  str1=QString("RDCastManager")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User: [Unknown]"));
  setCaption(QString().sprintf("%s: %s, %s",(const char *)str1,
			       (const char *)config->stationName(),
			       (const char *)str2));

  //
  // Open Database
  //
  QString err;
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    QMessageBox::warning(this,tr("Can't Connect"),err);
    exit(0);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdcastmanager: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }

  //
  // RIPC Connection
  //
#ifndef WIN32
  cast_ripc=new RDRipc(config->stationName());
  connect(cast_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  cast_ripc->connectHost("localhost",RIPCD_TCP_PORT,config->password());
#else
  cast_ripc=NULL;
#endif  // WIN32

  //
  // Station Configuration
  //
  rdstation_conf=new RDStation(config->stationName(),this);
  cast_system=new RDSystem();

  //
  // User
  //
#ifndef WIN32
  cast_user=NULL;
#else 
  cast_user=new RDUser(RD_USER_LOGIN_NAME);
#endif  // WIN32

  // 
  // Create Fonts
  //
  QFont default_font("Helvetica",12,QFont::Normal);
  default_font.setPixelSize(12);
  qApp->setFont(default_font);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Create Icons
  //
  cast_rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*cast_rivendell_map);
  cast_greencheckmark_map=new QPixmap(greencheckmark_xpm);
  cast_redx_map=new QPixmap(redx_xpm);

  //
  // Feed List
  //
  cast_feed_list=new RDListView(this,"cast_feed_list");
  cast_feed_list->setFont(default_font);
  cast_feed_list->setAllColumnsShowFocus(true);
  cast_feed_list->setItemMargin(5);
  connect(cast_feed_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(feedDoubleclickedData(QListViewItem *,const QPoint &,int)));
  cast_feed_list->addColumn("");
  cast_feed_list->setColumnAlignment(0,Qt::AlignCenter);
  cast_feed_list->addColumn(tr("Key Name"));
  cast_feed_list->setColumnAlignment(1,Qt::AlignHCenter);
  cast_feed_list->addColumn(tr("Feed Name"));
  cast_feed_list->setColumnAlignment(2,Qt::AlignLeft);
  cast_feed_list->addColumn(tr("Description"));
  cast_feed_list->setColumnAlignment(3,Qt::AlignLeft);
  cast_feed_list->addColumn(tr("Casts"));
  cast_feed_list->setColumnAlignment(3,Qt::AlignCenter);

  //
  // Open Button
  //
  cast_open_button=new QPushButton(this,"cast_open_button");
  cast_open_button->setFont(button_font);
  cast_open_button->setText(tr("&View\nFeed"));
  connect(cast_open_button,SIGNAL(clicked()),this,SLOT(openData()));

  //
  // Close Button
  //
  cast_close_button=new QPushButton(this,"cast_close_button");
  cast_close_button->setFont(button_font);
  cast_close_button->setText(tr("&Close"));
  connect(cast_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));
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

  if(cast_user!=NULL) {
    delete cast_user;
  }
  str1=QString("RDCastManager")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setCaption(QString().sprintf("%s: %s, %s: %s",(const char *)str1,
			       (const char *)config->stationName(),
			       (const char *)str2,
			       (const char *)cast_ripc->user()));
  cast_user=new RDUser(cast_ripc->user());
  RefreshList();
}


void MainWidget::openData()
{
  RDListViewItem *item=(RDListViewItem *)cast_feed_list->selectedItem();
  if(item==NULL) {
    return;
  }
  ListCasts *casts=new ListCasts(item->id(),this);
  casts->exec();
  RefreshItem(item);
  delete casts;
}


void MainWidget::feedDoubleclickedData(QListViewItem *,const QPoint &,int)
{
  openData();
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  cast_feed_list->setGeometry(10,10,size().width()-20,size().height()-70);
  cast_open_button->setGeometry(10,size().height()-55,80,50);
  cast_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
}


void MainWidget::RefreshItem(RDListViewItem *item)
{
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString sql;
  int active=0;
  int total=0;

  sql=QString().sprintf("select CHANNEL_TITLE,CHANNEL_DESCRIPTION,ID \
                         from FEEDS where KEY_NAME=\"%s\"",
			(const char *)item->text(1));
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sql=QString().sprintf("select STATUS from PODCASTS where FEED_ID=%u",
			  q->value(2).toUInt());
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
    item->setText(4,QString().sprintf("%d / %d",active,total));
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
  sql=QString().sprintf("select KEY_NAME from FEED_PERMS \
                         where USER_NAME=\"%s\"",
			(const char *)cast_user->name());
  q=new RDSqlQuery(sql);
  if(q->size()<=0) {  // No valid feeds!
    delete q;
    return;
  }
  sql="select ID,KEY_NAME from FEEDS where ";
  while(q->next()) {
    sql+=QString().sprintf("(KEY_NAME=\"%s\")||",
			   (const char *)q->value(0).toString());
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
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString tr_path;
  QString qt_path;
#ifdef WIN32
  QSettings settings;
  settings.insertSearchPath(QSettings::Windows,"/SalemRadioLabs");
  tr_path=QString().sprintf("%s\\",
			    (const char *)settings.
			    readEntry("/Rivendell/InstallDir"));
  qt_path=tr_path;
#else
  tr_path=QString(PREFIX)+QString("/share/rivendell/");
  qt_path=QString(QTDIR)+QString("/translation/");
#endif  // WIN32
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
  MainWidget *w=new MainWidget(NULL,"main",0);
  a.setMainWidget(w);
  w->setGeometry(w->geometry().x(),w->geometry().y(),w->sizeHint().width(),w->sizeHint().height());
  w->show();
  return a.exec();
}
