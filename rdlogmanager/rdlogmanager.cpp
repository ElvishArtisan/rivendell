// rdlogmanager.cpp
//
// The Log Generator Utility for Rivendell.
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
#include <qmessagebox.h>
#include <qtranslator.h>

#include "generate_log.h"
#include "globals.h"
#include "list_clocks.h"
#include "list_events.h"
#include "list_grids.h"
#include "list_svcs.h"
#include "rdlogmanager.h"
#include "logobject.h"

//
// Icons
//
#include "../icons/rdlogmanager-22x22.xpm"

//
// Global Resources
//
QString *event_filter;
QString *clock_filter;
bool skip_db_check=false;

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString err_msg;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Open the Database
  //
  rda=new RDApplication("RDLogManager","rdlogmanager",RDLOGMANAGER_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDLogManager - "+tr("Error"),err_msg);
    exit(RDApplication::ExitNoDb);
  }

  setWindowTitle(tr("RDLogManager"));

  //
  // CAE Connection
  //
  rda->cae()->connectHost();

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Create And Set Icon
  //
  log_rivendell_map=new QPixmap(rdlogmanager_22x22_xpm);
  setIcon(*log_rivendell_map);

  //
  // Filters
  //
  event_filter=new QString();
  clock_filter=new QString();

  //
  // Title Label
  //
  QLabel *label=new QLabel(tr("RDLogManager"),this);
  label->setGeometry(0,5,sizeHint().width(),32);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignHCenter);
  label=new QLabel(tr("Select an operation:"),this);
  label->setGeometry(0,25,sizeHint().width(),16);
  label->setFont(subLabelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  //  Edit Events Button
  //
  log_events_button=new QPushButton(this);
  log_events_button->setGeometry(10,45,sizeHint().width()-20,50);
  log_events_button->setFont(buttonFont());
  log_events_button->setText(tr("Edit &Events"));
  connect(log_events_button,SIGNAL(clicked()),this,SLOT(eventsData()));

  //
  //  Edit Clocks Button
  //
  log_clocks_button=new QPushButton(this);
  log_clocks_button->setGeometry(10,95,sizeHint().width()-20,50);
  log_clocks_button->setFont(buttonFont());
  log_clocks_button->setText(tr("Edit C&locks"));
  connect(log_clocks_button,SIGNAL(clicked()),this,SLOT(clocksData()));

  //
  //  Edit Grids Button
  //
  log_grids_button=new QPushButton(this);
  log_grids_button->setGeometry(10,145,sizeHint().width()-20,50);
  log_grids_button->setFont(buttonFont());
  log_grids_button->setText(tr("Edit G&rids"));
  connect(log_grids_button,SIGNAL(clicked()),this,SLOT(gridsData()));

  //
  //  Generate Logs Button
  //
  log_logs_button=new QPushButton(this);
  log_logs_button->setGeometry(10,195,sizeHint().width()-20,50);
  log_logs_button->setFont(buttonFont());
  log_logs_button->setText(tr("&Generate Logs"));
  connect(log_logs_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Generate Reports Button
  //
  log_reports_button=new QPushButton(this);
  log_reports_button->setGeometry(10,245,sizeHint().width()-20,50);
  log_reports_button->setFont(buttonFont());
  log_reports_button->setText(tr("Manage &Reports"));
  connect(log_reports_button,SIGNAL(clicked()),this,SLOT(reportsData()));

  //
  //  Close Button
  //
  log_close_button=new QPushButton(this);
  log_close_button->setGeometry(10,sizeHint().height()-60,
				sizeHint().width()-20,50);
  log_close_button->setFont(buttonFont());
  log_close_button->setText(tr("&Close"));
  log_close_button->setDefault(true);
  connect(log_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));
}


QSize MainWidget::sizeHint() const
{
  return QSize(200,360);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::userData()
{
  //
  // Set Control Perms
  //
  bool templates_allowed=rda->user()->modifyTemplate();
  bool creation_allowed=rda->user()->createLog();
  bool rec_allowed=rda->user()->deleteRec();
  log_events_button->setEnabled(templates_allowed);
  log_clocks_button->setEnabled(templates_allowed);
  log_grids_button->setEnabled(templates_allowed);
  log_logs_button->setEnabled(creation_allowed);
  log_reports_button->setEnabled(creation_allowed|rec_allowed);
}


void MainWidget::eventsData()
{
  ListEvents *events=new ListEvents(NULL,this);
  events->exec();
  delete events;
}


void MainWidget::clocksData()
{
  ListClocks *clocks=new ListClocks(NULL,this);
  clocks->exec();
  delete clocks;
}


void MainWidget::gridsData()
{
  ListGrids *grids=new ListGrids(this);
  grids->exec();
  delete grids;
}


void MainWidget::generateData()
{
  GenerateLog *generatelog=new GenerateLog(this);
  generatelog->exec();
  delete generatelog;
}


void MainWidget::reportsData()
{
  ListSvcs *recs=new ListSvcs(this);
  recs->exec();
  delete recs;
}


void MainWidget::quitMainWidget()
{
  exit(RDApplication::ExitOk);
}


int gui_main(int argc,char *argv[])
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
  tr.load(tr_path+QString("rdlogmanager_")+QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(w->geometry().x(),w->geometry().y()),
		       w->sizeHint()));
  w->show();
  return a.exec();
}


int main(int argc,char *argv[])
{
  //
  // Read Command Options
  //
  bool cmd_protect_existing=false;
  bool cmd_generate=false;
  bool cmd_merge_music=false;
  bool cmd_merge_traffic=false;
  QString cmd_service=NULL;
  QString cmd_report=NULL;
  int cmd_start_offset=0;
  int cmd_end_offset=0;

  RDCmdSwitch *cmd=
    new RDCmdSwitch(argc,argv,"rdlogmanager",RDLOGMANAGER_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if (cmd->key(i)=="-P") {
      cmd_protect_existing = true;
      cmd->setProcessed(i,true);
    }
    if (cmd->key(i)=="-g") {
      printf("generate\n");
      cmd_generate = true;
      cmd->setProcessed(i,true);
    }
    if (cmd->key(i)=="-m") {
      cmd_merge_music = true;
      cmd->setProcessed(i,true);
    }
    if (cmd->key(i)=="-t") {
      cmd_merge_traffic = true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--skip-db-check") {
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="-s") {
      if((i+1)<cmd->keys()) {
	i++;
	cmd_service = cmd->key(i);
      }
      else {
	fprintf(stderr,"rdlogmanager: missing argument to \"-s\"\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="-r") {
      if(i+1<cmd->keys()) {
	i++;
	cmd_report = cmd->key(i);
      }
      else {
	fprintf(stderr,"rdlogmanager: missing argument to \"-r\"\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if (cmd->key(i)=="-d") {
      if (i+1<cmd->keys()) {
	i++;
	cmd_start_offset=cmd->key(i).toInt();
      }
      else {
	fprintf(stderr,"rdlogmanager: missing argument to \"-d\"\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if (cmd->key(i)=="-e") {
      if (i+1<cmd->keys()) {
	i++;
	cmd_end_offset=cmd->key(i).toInt();
      }
      else {
	fprintf(stderr,"rdlogmanager: missing argument to \"-e\"\n");
	exit(RDApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdlogmanager: unknown command option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(RDApplication::ExitInvalidOption);
    }
  }
  delete cmd;
  if((!cmd_report.isNull())&&
     (cmd_generate||cmd_merge_traffic||cmd_merge_music)) {
    fprintf(stderr,
	    "rdlogmanager: log and report operations are mutually exclusive\n");
    exit(RDApplication::ExitInvalidOption);
  }

  if(cmd_generate||cmd_merge_traffic||cmd_merge_music) {
    QApplication a(argc,argv,false);
    new LogObject(cmd_service,cmd_start_offset,cmd_protect_existing,
		  cmd_generate,cmd_merge_music,cmd_merge_traffic);
    return a.exec();
 }
  if(!cmd_report.isEmpty()) {
    return RunReportOperation(argc,argv,cmd_report,cmd_protect_existing,
			      cmd_start_offset,cmd_end_offset);
  }
  return gui_main(argc,argv);
}
