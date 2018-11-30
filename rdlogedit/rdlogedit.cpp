// rdlogedit.cpp
//
// The Log Editor Utility for Rivendell.
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <q3sqlpropertymap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlabel.h>
#include <q3listview.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <rd.h>
#include <rdadd_log.h>
#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdloglock.h>
#include <rdmixer.h>
#include <rdreport.h>
#include <rdstation.h>
#include <rdtextfile.h>

#include "edit_log.h"
#include "globals.h"
#include "rdlogedit.h"
#include "voice_tracker.h"

//
// Icons
//
#include "../icons/rdlogedit-22x22.xpm"
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"

//
// Global Resources
//
RDCartDialog *log_cart_dialog;
bool import_running=false;

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  QString err_msg;

  log_resize=false;
  log_log_list=NULL;
  log_list_locked=false;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Open the Database
  //
  rda=new RDApplication("RDLogEdit","rdlogedit",RDLOGEDIT_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),err_msg);
    exit(1);
  }
  log_import_path=RDGetHomeDir();

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDLogEdit - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
          this,SLOT(caeConnectedData(bool)));
  rda->cae()->connectHost();

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),this,SLOT(connectedData(bool)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()
    ->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

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
  log_rivendell_map=new QPixmap(rdlogedit_22x22_xpm);
  setWindowIcon(*log_rivendell_map);
  log_greencheckmark_map=new QPixmap(greencheckmark_xpm);
  log_redx_map=new QPixmap(redx_xpm);
  log_whiteball_map=new QPixmap(whiteball_xpm);
  log_greenball_map=new QPixmap(greenball_xpm);
  log_redball_map=new QPixmap(redball_xpm);

  //
  // Log Filter
  //
  log_filter_widget=
    new RDLogFilter(RDLogFilter::UserFilter,this);
  connect(log_filter_widget,SIGNAL(filterChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Log List
  //
  log_log_list=new Q3ListView(this);
  log_log_list->setFont(default_font);
  log_log_list->setAllColumnsShowFocus(true);
  log_log_list->setSelectionMode(Q3ListView::Extended);
  log_log_list->setItemMargin(5);
  connect(log_log_list,SIGNAL(selectionChanged()),
	  this,SLOT(logSelectionChangedData()));
  connect(log_log_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(logDoubleclickedData(Q3ListViewItem *,const QPoint &,int)));
  log_log_list->addColumn("");
  log_log_list->setColumnAlignment(0,Qt::AlignCenter);
  log_log_list->addColumn(tr("Log Name"));
  log_log_list->setColumnAlignment(1,Qt::AlignHCenter);
  log_log_list->addColumn(tr("Description"));
  log_log_list->setColumnAlignment(2,Qt::AlignLeft);
  log_log_list->addColumn(tr("Service"));
  log_log_list->setColumnAlignment(3,Qt::AlignLeft);
  log_log_list->addColumn(tr("Music"));
  log_log_list->setColumnAlignment(4,Qt::AlignCenter);
  log_log_list->addColumn(tr("Traffic"));
  log_log_list->setColumnAlignment(5,Qt::AlignCenter);
  log_log_list->addColumn(tr("Tracks"));
  log_log_list->setColumnAlignment(6,Qt::AlignHCenter);
  log_log_list->addColumn(tr("Valid From"));
  log_log_list->setColumnAlignment(7,Qt::AlignHCenter);
  log_log_list->addColumn(tr("Valid To"));
  log_log_list->setColumnAlignment(8,Qt::AlignHCenter);
  log_log_list->addColumn(tr("Auto Refresh"));
  log_log_list->setColumnAlignment(9,Qt::AlignHCenter);
  log_log_list->addColumn(tr("Origin"));
  log_log_list->setColumnAlignment(10,Qt::AlignLeft);
  log_log_list->addColumn(tr("Last Linked"));
  log_log_list->setColumnAlignment(11,Qt::AlignLeft);
  log_log_list->addColumn(tr("Last Modified"));
  log_log_list->setColumnAlignment(12,Qt::AlignLeft);

  //
  // Add Button
  //
  log_add_button=new QPushButton(this);
  log_add_button->setFont(button_font);
  log_add_button->setText(tr("&Add"));
  connect(log_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Edit Button
  //
  log_edit_button=new QPushButton(this);
  log_edit_button->setFont(button_font);
  log_edit_button->setText(tr("&Edit"));
  connect(log_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  // Delete Button
  //
  log_delete_button=new QPushButton(this);
  log_delete_button->setFont(button_font);
  log_delete_button->setText(tr("&Delete"));
  connect(log_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  // Tracker Button
  //
  log_track_button=new QPushButton(this);
  log_track_button->setFont(button_font);
  log_track_button->setText(tr("Voice\n&Tracker"));
  connect(log_track_button,SIGNAL(clicked()),this,SLOT(trackData()));

  //
  // Log Report Button
  //
  log_report_button=new QPushButton(this);
  log_report_button->setFont(button_font);
  log_report_button->setText(tr("Log\nReport"));
  connect(log_report_button,SIGNAL(clicked()),this,SLOT(reportData()));

  //
  // Close Button
  //
  log_close_button=new QPushButton(this);
  log_close_button->setFont(button_font);
  log_close_button->setText(tr("&Close"));
  connect(log_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  setWindowTitle(QString("RDLogEdit")+"v"+VERSION+" - "+tr("Host")+": "+
	     rda->config()->stationName()+", "+
	     tr("User")+": ["+tr("Unknown")+"]");

  log_resize=true;
}


QSize MainWidget::sizeHint() const
{
  return QSize(640,480);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::connectedData(bool state)
{
}

void MainWidget::caeConnectedData(bool state)
{
  std::vector<int> cards;
  RDLogeditConf *conf=new RDLogeditConf(rda->config()->stationName());

  cards.push_back(conf->inputCard());
  cards.push_back(conf->outputCard());
  rda->cae()->enableMetering(&cards);
}

void MainWidget::userData()
{
  QString str1;
  QString str2;

  str1=QString("RDLogEdit")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setCaption(str1+": "+rda->config()->stationName()+", "+str2+": "+
	     rda->ripc()->user());

  log_filter_widget->changeUser();
  RefreshList();

  //
  // Set Control Perms
  //
  log_add_button->setEnabled(rda->user()->createLog());
  log_delete_button->setEnabled(false);
  log_track_button->setEnabled(false);
}


void MainWidget::recentData(bool state)
{
  RefreshList();
}


void MainWidget::addData()
{
  QString logname;
  QString svcname;
  std::vector<QString> newlogs;
  RDAddLog *log;

  if(rda->user()->createLog()) {
    log=new RDAddLog(&logname,&svcname,RDLogFilter::UserFilter,
		     tr("Add Log"),this);
    if(log->exec()!=0) {
      delete log;
      return;
    }
    delete log;
    QString username(rda->ripc()->user());
    QString err_msg;
    if(!RDLog::create(logname,svcname,QDate(),username,&err_msg,
		      rda->config())) {
      QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),err_msg);
      return;
    }
    LockList();
    SendNotification(RDNotification::AddAction,logname);
    EditLog *editlog=new EditLog(logname,&log_filter,&log_group,&log_schedcode,
				 &log_clipboard,&newlogs,this);
    editlog->exec();
    delete editlog;
    ListListViewItem *item=new ListListViewItem(log_log_list);
    item->setText(1,logname);
    RefreshItem(item);
    log_log_list->setSelected(item,true);
    log_log_list->ensureItemVisible((Q3ListViewItem *)item);
    for(unsigned i=0;i<newlogs.size();i++) {
      item=new ListListViewItem(log_log_list);
      item->setText(1,newlogs[i]);
      RefreshItem(item);
    }
    UnlockList();
  }
}


void MainWidget::editData()
{
  std::vector<ListListViewItem *> items;
  if(SelectedLogs(&items)!=1) {
    return;
  }
  QString logname=items.at(0)->text(1);
  std::vector<QString> newlogs;
  LockList();
  EditLog *log=
    new EditLog(logname,&log_filter,&log_group,&log_schedcode,
		&log_clipboard,&newlogs,this);
  if(log->exec()) {
    RefreshItem(items.at(0));
    for(unsigned i=0;i<newlogs.size();i++) {
      ListListViewItem *item=new ListListViewItem(log_log_list);
      item->setText(1,newlogs[i]);
      RefreshItem(item);
    }
  }
  UnlockList();
  delete log;
}


void MainWidget::deleteData()
{
  QString filename;
  QString str1;
  QString str2;
  unsigned tracks=0;
  ListListViewItem *item=(ListListViewItem *)log_log_list->firstChild();
  std::vector<ListListViewItem *> items;

  if(rda->user()->deleteLog()) {
    while(item!=NULL) {
      if(item->isSelected()) {
	items.push_back(item);
	RDLog *log=new RDLog(items.at(0)->text(1));
	tracks+=log->completedTracks();
	delete log;
      }
      item=(ListListViewItem *)item->nextSibling();
    }
    if(items.size()==1) {
      if(QMessageBox::question(this,"RDLogEdit - "+tr("Delete Log"),
			       tr("Are you sure you want to delete the")+" \""+
			       items.at(0)->text(1)+"\" "+tr("log?"),
			       QMessageBox::Yes,
			       QMessageBox::No)!=QMessageBox::Yes) {
	return;
      }
      if(tracks>0) {
	if(QMessageBox::question(this,"RDLogEdit - "+tr("Tracks Exist"),
				 tr("This will also delete the")+
			       QString().sprintf(" %u ",tracks)+
				 tr("voice tracks associated with this log.")+
				 "\n"+tr("Continue?"),
				 QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  return;
	}
      }
    }
    else {
      if(QMessageBox::question(this,"RDLogEdit - "+tr("Delete Log"),
			       tr("Are you sure you want to delete these")+
			       QString().sprintf(" %lu ",items.size())+
			       tr("logs?"),
			       QMessageBox::Yes,
			       QMessageBox::No)!=QMessageBox::Yes) {
	return;
      }
      if(tracks>0) {
	if(QMessageBox::question(this,"RDLogEdit - "+tr("Tracks Exist"),
				 tr("This will also delete the")+
			       QString().sprintf(" %u ",tracks)+
				 tr("voice tracks associated with these logs.")+
				 "\n"+tr("Continue?"),
				 QMessageBox::Yes,QMessageBox::No)!=
	   QMessageBox::Yes) {
	  return;
	}
      }
    }

    LockList();
    for(unsigned i=0;i<items.size();i++) {
      QString username;
      QString stationname;
      QHostAddress addr;
      RDLogLock *log_lock=new RDLogLock(items.at(i)->text(1),rda->user(),
					rda->station(),this);
      if(log_lock->tryLock(&username,&stationname,&addr)) {
	RDLog *log=new RDLog(items.at(i)->text(1));
	if(log->remove(rda->station(),rda->user(),rda->config())) {
	  SendNotification(RDNotification::DeleteAction,log->name());
	  delete items.at(i);
	}
	else {
	  QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),
			       tr("Unable to delete log")+" \""+
			       items.at(i)->text(1)+"\", "+
			       tr("audio deletion error!"));
	}
	delete log;
      }
      else {
	QString msg=tr("Log")+" "+items.at(i)->text(1)+"\" "+
	  tr("is in use by")+" "+username+"@"+stationname;
	if(stationname!=addr.toString()) {
	  msg+=" ["+addr.toString()+"]";
	}
	QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),msg);
      }
      delete log_lock;
    }
    UnlockList();
  }
}


void MainWidget::trackData()
{
  std::vector<ListListViewItem *> items;
  if(SelectedLogs(&items)!=1) {
    return;
  }
  LockList();
  VoiceTracker *dialog=new VoiceTracker(items.at(0)->text(1),&log_import_path);
  dialog->exec();
  delete dialog;
  RefreshItem(items.at(0));
  UnlockList();
}


void MainWidget::reportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  //
  // Generate Header
  //
  report=RDReport::center("Rivendell Log Listing",132)+"\n";
  report+=QString("Generated: ")+QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"\n";
  report+="\n";
  report+="Rdy -Log Name-------------------- -Description----------------- -Service------------ Mus Tfc Tracks- Start Date -End Date- -Mod Date-\n";

  //
  // Report Body
  //
  sql=QString("select ")+
    "NAME,"+               // 00
    "DESCRIPTION,"+        // 01
    "SERVICE,"+            // 02
    "MUSIC_LINKS,"+        // 03
    "MUSIC_LINKED,"+       // 04
    "TRAFFIC_LINKS,"+      // 05
    "TRAFFIC_LINKED,"+     // 06
    "COMPLETED_TRACKS,"+   // 07
    "SCHEDULED_TRACKS,"+   // 08
    "START_DATE,"+         // 09
    "END_DATE,"+           // 10
    "MODIFIED_DATETIME "+  // 11
    "from LOGS order by NAME ";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Summary Status
    //
    if(((q->value(3).toInt()==0)||(q->value(4).toString()=="Y"))&&
       ((q->value(5).toInt()==0)||(q->value(6).toString()=="Y"))&&
       (q->value(7).toUInt()==q->value(8).toUInt())) {
      report+=" Y  ";
    }
    else {
      report+=" N  ";
    }

    //
    // Log Name
    //
    report+=RDReport::leftJustify(q->value(0).toString(),29)+" ";
    
    //
    // Description
    //
    report+=RDReport::leftJustify(q->value(1).toString(),29)+" ";

    //
    // Service
    //
    report+=RDReport::leftJustify(q->value(2).toString(),20)+" ";

    //
    // Music Linked
    //
    if(q->value(3).toInt()>0) {
      report+=QString(" ")+q->value(4).toString()+"  ";
    }
    else {
      report+="n/a ";
    }

    //
    // Traffic Linked
    //
    if(q->value(5).toInt()>0) {
      report+=QString(" ")+q->value(6).toString()+"  ";
    }
    else {
      report+="n/a ";
    }

    //
    // Voice Tracks
    //
    report+=
      QString().sprintf("%3u/%3u ",q->value(8).toUInt(),q->value(7).toUInt());
    
    //
    // Start Date
    //
    if(q->value(9).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=q->value(9).toDate().toString("MM/dd/yyyy")+" ";
    }
    
    //
    // End Date
    //
    if(q->value(10).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=q->value(10).toDate().toString("MM/dd/yyyy")+" ";
      report+=QString().sprintf("%s ",
	(const char *)q->value(10).toDate().toString("MM/dd/yyyy"));
    }
    
    //
    // Last Modified Date
    //
    report+=q->value(11).toDate().toString("MM/dd/yyyy");

    //
    // End of Line
    //
    report+="\n";
  }
  delete q;

  RDTextFile(report);
}


void MainWidget::filterChangedData(const QString &str)
{
  RefreshList();
}

void MainWidget::logSelectionChangedData()
{
  int count=0;
  ListListViewItem *item=(ListListViewItem *)log_log_list->firstChild();
  while(item!=NULL) {
    if(item->isSelected()) {
      count++;
    }
    item=(ListListViewItem *)item->nextSibling();
  }
  log_edit_button->setEnabled(count==1);
  log_delete_button->setEnabled(count>0&&rda->user()->deleteLog());
  log_track_button->setEnabled(count==1&&rda->user()->voicetrackLog());
}


void MainWidget::logDoubleclickedData(Q3ListViewItem *,const QPoint &,int)
{
  editData();
}


void MainWidget::notificationReceivedData(RDNotification *notify)
{
  QString sql;
  RDSqlQuery *q;
  ListListViewItem *item=NULL;

  if(notify->type()==RDNotification::LogType) {
    QString logname=notify->id().toString();
    switch(notify->action()) {
    case RDNotification::AddAction:
      sql=QString("select NAME from LOGS where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&")+
	"(NAME=\""+RDEscapeString(logname)+"\") "+
	log_filter_widget->whereSql();
      q=new RDSqlQuery(sql);
      if(q->first()) {
	item=new ListListViewItem(log_log_list);
	item->setText(1,logname);
	RefreshItem(item);
      }
      delete q;
      break;

    case RDNotification::ModifyAction:
      if((item=(ListListViewItem *)log_log_list->findItem(logname,1))!=NULL) {
	RefreshItem(item);
      }
      break;

    case RDNotification::DeleteAction:
      if(log_list_locked) {
	log_deleted_logs.push_back(logname);
      }
      else {
	if((item=(ListListViewItem *)log_log_list->findItem(logname,1))!=NULL) {
	  delete item;
	}
      }
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  if((log_log_list==NULL)||(!log_resize)) {
    return;
  }
  log_filter_widget->setGeometry(10,10,size().width()-10,
				 log_filter_widget->sizeHint().height());

  log_log_list->setGeometry(10,log_filter_widget->sizeHint().height(),
			    size().width()-20,size().height()-
			    log_filter_widget->sizeHint().height()-65);
  log_add_button->setGeometry(10,size().height()-55,80,50);
  log_edit_button->setGeometry(100,size().height()-55,80,50);
  log_delete_button->setGeometry(190,size().height()-55,80,50);
  log_track_button->setGeometry(300,size().height()-55,80,50);
  log_report_button->setGeometry(400,size().height()-55,80,50);
  log_close_button->setGeometry(size().width()-90,size().height()-55,80,50);
}


void MainWidget::RefreshItem(ListListViewItem *item)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("select ")+
    "DESCRIPTION,"+        // 00
    "SERVICE,"+            // 01
    "START_DATE,"+         // 02
    "END_DATE,"+           // 03
    "ORIGIN_USER,"+        // 04
    "ORIGIN_DATETIME,"+    // 05
    "COMPLETED_TRACKS,"+   // 06
    "SCHEDULED_TRACKS,"+   // 07
    "MUSIC_LINKS,"+        // 08
    "MUSIC_LINKED,"+       // 09
    "TRAFFIC_LINKS,"+      // 10
    "TRAFFIC_LINKED,"+     // 11
    "LINK_DATETIME,"+      // 12
    "MODIFIED_DATETIME,"+  // 13
    "AUTO_REFRESH "+       // 14
    "from LOGS where "+
    "(TYPE=0)&&"+        
    "(LOG_EXISTS=\"Y\")&&"+
    "(NAME=\""+RDEscapeString(item->text(1))+"\")";
  q=new RDSqlQuery(sql);
  if(q->next()) {
    item->setText(2,q->value(0).toString());
    item->setText(3,q->value(1).toString());
    if((q->value(6).toInt()==q->value(7).toInt())&&
       ((q->value(8).toInt()==0)||(q->value(9).toString()=="Y"))&&
       ((q->value(10).toInt()==0)||(q->value(11).toString()=="Y"))) {
      item->setPixmap(0,*log_greencheckmark_map);
    }
    else {
      item->setPixmap(0,*log_redx_map);
    }
    if(q->value(8).toInt()==0) {
      item->setPixmap(4,*log_whiteball_map);
    }
    else {
      if(q->value(9).toString()=="Y") {
	item->setPixmap(4,*log_greenball_map);
      }
      else {
	item->setPixmap(4,*log_redball_map);
      }
    }
    if(q->value(10).toInt()==0) {
      item->setPixmap(5,*log_whiteball_map);
    }
    else {
      if(q->value(11).toString()=="Y") {
	item->setPixmap(5,*log_greenball_map);
      }
      else {
	item->setPixmap(5,*log_redball_map);
      }
    }
    item->setTracks(q->value(6).toInt());
    item->setTotalTracks(q->value(7).toInt());
    item->setTrackColumn(6);
    if(!q->value(2).toDate().isNull()) {
      item->setText(7,q->value(2).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(7,tr("Always"));
    }
    if(!q->value(3).toDate().isNull()) {
      item->setText(8,q->value(3).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(8,tr("TFN"));
    }
    item->setText(9,q->value(14).toString());
    item->setText(10,q->value(4).toString()+QString(" - ")+
      q->value(5).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
    item->
      setText(11,q->value(12).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
    item->
      setText(12,q->value(13).toDateTime().toString("MM/dd/yyyy - hh:mm:ss"));
  }
  delete q;
}


void MainWidget::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  ListListViewItem *item;

  log_log_list->clear(); // Note: clear here, in case user has no perms.

  sql="select NAME from LOGS where (TYPE=0)&&(LOG_EXISTS=\"Y\")"+
    log_filter_widget->whereSql();
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new ListListViewItem(log_log_list);
    item->setText(1,q->value(0).toString());
    RefreshItem(item);
  }
  delete q;
  logSelectionChangedData();
}


unsigned MainWidget::SelectedLogs(std::vector<ListListViewItem *> *items,
				  int *tracks) const
{
  ListListViewItem *item=(ListListViewItem *)log_log_list->firstChild();

  items->clear();
  while(item!=NULL) {
    if(item->isSelected()) {
      items->push_back(item);
      if(tracks!=NULL) {
	RDLog *log=new RDLog(item->text(1));
	(*tracks)+=log->completedTracks();
	delete log;
      }
    }
    item=(ListListViewItem *)item->nextSibling();
  }
  return items->size();
}


void MainWidget::SendNotification(RDNotification::Action action,
				  const QString &logname)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,action,
					    QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void MainWidget::LockList()
{
  log_list_locked=true;
}


void MainWidget::UnlockList()
{
  ListListViewItem *item=NULL;

  for(int i=0;i<log_deleted_logs.size();i++) {
    if((item=(ListListViewItem *)log_log_list->
	findItem(log_deleted_logs[i],1))!=NULL) {
      delete item;
    }
  }
  log_deleted_logs.clear();
  log_list_locked=false;
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
  tr.load(tr_path+QString("rdlogedit_")+QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(w->geometry().x(),w->geometry().y()),
		 w->sizeHint()));
  w->show();
  return a.exec();
}
