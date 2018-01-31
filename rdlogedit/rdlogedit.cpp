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

#ifndef WIN32
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif  // WIN32
#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qsqlpropertymap.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <dbversion.h>
#include <rd.h>
#include <rdadd_log.h>
#include <rdapplication.h>
#include <rdcheck_daemons.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdloglock.h>
#include <rdmixer.h>
#include <rdstation.h>
#include <rdtextfile.h>

#include "edit_log.h"
#include "globals.h"
#include "rdlogedit.h"
#ifndef WIN32
#include "voice_tracker.h"
#endif  // WIN32

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
#ifndef WIN32

void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
      pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    import_running=false;
    signal(SIGCHLD,SigHandler);
    return;
  }
}
#endif  // WIN32


MainWidget::MainWidget(QWidget *parent)
  :QMainWindow(parent)
{
  QString str1;
  QString str2;
  QString err_msg;

  log_resize=false;
  log_log_list=NULL;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Ensure that the system daemons are running
  //
#ifndef WIN32
  RDInitializeDaemons();
#endif  // WIN32

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
#ifndef WIN32
  rda->cae()->connectHost();
#endif  // WIN32

  //
  // RIPC Connection
  //
#ifndef WIN32
  connect(rda->ripc(),SIGNAL(connected(bool)),this,SLOT(connectedData(bool)));
  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
#else
  rdripc=NULL;
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
  log_rivendell_map=new QPixmap(rdlogedit_22x22_xpm);
  setIcon(*log_rivendell_map);
  log_greencheckmark_map=new QPixmap(greencheckmark_xpm);
  log_redx_map=new QPixmap(redx_xpm);
  log_whiteball_map=new QPixmap(whiteball_xpm);
  log_greenball_map=new QPixmap(greenball_xpm);
  log_redball_map=new QPixmap(redball_xpm);

  //
  // User
  //
#ifndef WIN32
  //
  // Load Audio Assignments
  //
  RDSetMixerPorts(rda->config()->stationName(),rda->cae());
#else 
  rda->user()->setName(RD_USER_LOGIN_NAME);
#endif  // WIN32

  //
  // Log Filter
  //
  log_filter_widget=
    new RDLogFilter(RDLogFilter::UserFilter,rda->user(),rda->config(),this);
  connect(log_filter_widget,SIGNAL(filterChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Log List
  //
  log_log_list=new QListView(this);
  log_log_list->setFont(default_font);
  log_log_list->setAllColumnsShowFocus(true);
  log_log_list->setSelectionMode(QListView::Extended);
  log_log_list->setItemMargin(5);
  connect(log_log_list,SIGNAL(selectionChanged()),
	  this,SLOT(logSelectionChangedData()));
  connect(log_log_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(logDoubleclickedData(QListViewItem *,const QPoint &,int)));
  log_log_list->addColumn("");
  log_log_list->setColumnAlignment(0,Qt::AlignCenter);
  log_log_list->addColumn(tr("LOG NAME"));
  log_log_list->setColumnAlignment(1,Qt::AlignHCenter);
  log_log_list->addColumn(tr("DESCRIPTION"));
  log_log_list->setColumnAlignment(2,Qt::AlignLeft);
  log_log_list->addColumn(tr("SERVICE"));
  log_log_list->setColumnAlignment(3,Qt::AlignLeft);
  log_log_list->addColumn(tr("MUSIC"));
  log_log_list->setColumnAlignment(4,Qt::AlignCenter);
  log_log_list->addColumn(tr("TRAFFIC"));
  log_log_list->setColumnAlignment(5,Qt::AlignCenter);
  log_log_list->addColumn(tr("TRACKS"));
  log_log_list->setColumnAlignment(6,Qt::AlignHCenter);
  log_log_list->addColumn(tr("VALID FROM"));
  log_log_list->setColumnAlignment(7,Qt::AlignHCenter);
  log_log_list->addColumn(tr("VALID TO"));
  log_log_list->setColumnAlignment(8,Qt::AlignHCenter);
  log_log_list->addColumn(tr("AUTO REFRESH"));
  log_log_list->setColumnAlignment(9,Qt::AlignHCenter);
  log_log_list->addColumn(tr("ORIGIN"));
  log_log_list->setColumnAlignment(10,Qt::AlignLeft);
  log_log_list->addColumn(tr("LAST LINKED"));
  log_log_list->setColumnAlignment(11,Qt::AlignLeft);
  log_log_list->addColumn(tr("LAST MODIFIED"));
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
#ifdef WIN32
  log_track_button->hide();
#endif

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

  str1=QString("RDLogEdit")+"v"+VERSION+" - "+tr("Host");
#ifdef WIN32
  RefreshList();
  str2=tr("User")+": ["+tr("Windows")+"]";
#else
  // 
  // Setup Signal Handling 
  //
  ::signal(SIGCHLD,SigHandler);
  str2=tr("User")+": ["+tr("Unknown")+"]";
#endif  // WIN32
  setCaption(QString().sprintf("%s: %s, %s",(const char *)str1,
			       (const char *)rda->config()->stationName(),
			       (const char *)str2));

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


void MainWidget::userData()
{
  QString str1;
  QString str2;

  str1=QString("RDLogEdit")+" v"+VERSION+" - "+tr("Host");
  str2=QString(tr("User"));
  setCaption(str1+": "+rda->config()->stationName()+", "+str2+": "+
	     rda->ripc()->user());
  rda->user()->setName(rda->ripc()->user());


  log_filter_widget->setUser(rda->user());
  RefreshList();

  //
  // Set Control Perms
  //
  log_add_button->setEnabled(rda->user()->createLog());
  log_delete_button->setEnabled(rda->user()->deleteLog());
  log_track_button->setEnabled(rda->user()->voicetrackLog());
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
    log=new RDAddLog(&logname,&svcname,RDLogFilter::UserFilter,rda->user(),
		     rda->station(),tr("Add Log"),this);
    if(log->exec()!=0) {
      delete log;
      return;
    }
    delete log;
#ifdef WIN32
    QString username(RD_USER_LOGIN_NAME);
#else
    QString username(rda->ripc()->user());
#endif  // WIN32
    QString err_msg;
    if(!RDLog::create(logname,svcname,username,&err_msg,rda->config())) {
      QMessageBox::warning(this,"RDLogEdit - "+tr("Error"),err_msg);
      return;
    }
    EditLog *editlog=new EditLog(logname,&log_filter,&log_group,&log_schedcode,
				 &log_clipboard,&newlogs,this);
    editlog->exec();
    delete editlog;
    ListListViewItem *item=new ListListViewItem(log_log_list);
    item->setText(1,logname);
    RefreshItem(item);
    log_log_list->setSelected(item,true);
    log_log_list->ensureItemVisible((QListViewItem *)item);
    for(unsigned i=0;i<newlogs.size();i++) {
      item=new ListListViewItem(log_log_list);
      item->setText(1,newlogs[i]);
      RefreshItem(item);
    }
  }
}


void MainWidget::editData()
{
  std::vector<ListListViewItem *> items;
  if(SelectedLogs(&items)!=1) {
    return;
  }

  std::vector<QString> newlogs;
  EditLog *log=
    new EditLog(items.at(0)->text(1),&log_filter,&log_group,&log_schedcode,
		&log_clipboard,&newlogs,this);
  log->exec();
  delete log;
  RefreshItem(items.at(0));
  for(unsigned i=0;i<newlogs.size();i++) {
    ListListViewItem *item=new ListListViewItem(log_log_list);
    item->setText(1,newlogs[i]);
    RefreshItem(item);
  }
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

    for(unsigned i=0;i<items.size();i++) {
      QString username;
      QString stationname;
      QHostAddress addr;
      RDLogLock *log_lock=new RDLogLock(items.at(i)->text(1),rda->user(),
					rda->station(),this);
      if(log_lock->tryLock(&username,&stationname,&addr)) {
	RDLog *log=new RDLog(items.at(i)->text(1));
	if(log->remove(rda->station(),rda->user(),rda->config())) {
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
  }
}


void MainWidget::trackData()
{
#ifndef WIN32
  std::vector<ListListViewItem *> items;
  if(SelectedLogs(&items)!=1) {
    return;
  }
  VoiceTracker *dialog=new VoiceTracker(items.at(0)->text(1),&log_import_path);
  dialog->exec();
  delete dialog;
  RefreshItem(items.at(0));
#endif  // WIN32
}


void MainWidget::reportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  //
  // Generate Header
  //
  report="                                                     Rivendell Log Listing\n";
  report+=QString().
    sprintf("Generated: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"));
  report+="\n";
  report+="Rdy -Log Name-------------------- -Description----------------- -Service------------ Mus Tfc Tracks- Start Date -End Date- -Mod Date-\n";

  //
  // Report Body
  //
  sql="select NAME,DESCRIPTION,SERVICE,MUSIC_LINKS,MUSIC_LINKED,\
       TRAFFIC_LINKS,TRAFFIC_LINKED,COMPLETED_TRACKS,SCHEDULED_TRACKS,\
       START_DATE,END_DATE,MODIFIED_DATETIME from LOGS order by NAME";
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
    report+=QString().sprintf("%-29s ",
			      (const char *)q->value(0).toString().left(29));
    
    //
    // Description
    //
    report+=QString().sprintf("%-29s ",
			      (const char *)q->value(1).toString().left(29));

    //
    // Service
    //
    report+=QString().sprintf("%-20s ",
			      (const char *)q->value(2).toString().left(20));

    //
    // Music Linked
    //
    if(q->value(3).toInt()>0) {
      report+=QString().sprintf(" %s  ",
				(const char *)q->value(4).toString());
    }
    else {
      report+="n/a ";
    }

    //
    // Traffic Linked
    //
    if(q->value(5).toInt()>0) {
      report+=QString().sprintf(" %s  ",
				(const char *)q->value(6).toString());
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
      report+=QString().sprintf("%s ",
		(const char *)q->value(9).toDate().toString("MM/dd/yyyy"));
    }
    
    //
    // End Date
    //
    if(q->value(10).toDate().isNull()) {
      report+="[none]     ";
    }
    else {
      report+=QString().sprintf("%s ",
	(const char *)q->value(10).toDate().toString("MM/dd/yyyy"));
    }
    
    //
    // Last Modified Date
    //
    report+=QString().sprintf("%s",
	      (const char *)q->value(11).toDate().toString("MM/dd/yyyy"));

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
  log_delete_button->setEnabled(count>0);
  log_track_button->setEnabled(count==1);
}


void MainWidget::logDoubleclickedData(QListViewItem *,const QPoint &,int)
{
  editData();
}


void MainWidget::quitMainWidget()
{
  log_db->removeDatabase(rda->config()->mysqlDbname());
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

  sql=QString().sprintf("select DESCRIPTION,SERVICE,START_DATE,END_DATE,\
                         ORIGIN_USER,ORIGIN_DATETIME,COMPLETED_TRACKS,\
                         SCHEDULED_TRACKS,MUSIC_LINKS,MUSIC_LINKED,\
                         TRAFFIC_LINKS,TRAFFIC_LINKED,LINK_DATETIME,\
                         MODIFIED_DATETIME,AUTO_REFRESH from LOGS\
                         where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&(NAME=\"%s\")",
			(const char *)item->text(1));
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
