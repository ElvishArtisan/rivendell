// rdselect.h
//
// System Selector for Rivendell
//
//   (C) Copyright 2012-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <qdesktopwidget.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qtranslator.h>

#include <dbversion.h>
#include <rdpaths.h>
#include <rdstatus.h>

#include "rdselect.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdselect","\n");
  delete cmd;

  //
  // Read Configuration
  //
  monitor_config=new RDMonitorConfig();
  monitor_config->load();
  QDesktopWidget *dw=qApp->desktop();
  int width=sizeHint().width();
  int height=sizeHint().height();
  switch(monitor_config->position()) {
  case RDMonitorConfig::UpperLeft:
    setGeometry(0,RDMONITOR_HEIGHT,width,sizeHint().height());
    break;

  case RDMonitorConfig::UpperCenter:
    setGeometry((dw->size().width()-width)/2,RDMONITOR_HEIGHT,width,height);
    break;

  case RDMonitorConfig::UpperRight:
    setGeometry(dw->size().width()-width,RDMONITOR_HEIGHT,width,height);
    break;

  case RDMonitorConfig::LowerLeft:
    setGeometry(0,dw->size().height()-height+RDMONITOR_HEIGHT,width,height);
    break;

  case RDMonitorConfig::LowerCenter:
    setGeometry((dw->size().width()-width)/2,
		dw->size().height()-height+RDMONITOR_HEIGHT,width,height);
    break;

  case RDMonitorConfig::LowerRight:
    setGeometry(dw->size().width()-width,
		dw->size().height()-height+RDMONITOR_HEIGHT,width,height);
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Create And Set Icons
  //
  login_rivendell_map=new QPixmap(rivendell_22x22_xpm);
  setWindowIcon(*login_rivendell_map);
  setWindowTitle(QString("RDSelect - v")+VERSION);
  greencheckmark_map=new QPixmap(greencheckmark_xpm);
  redx_map=new QPixmap(redx_xpm);

  //
  // Load Configs
  //
  select_current_id=-1;
  char target[1500];
  ssize_t n;
  if((n=readlink(RD_CONF_FILE,target,1500))>0) {
    target[n]=0;
  }
  else {
    target[0]=0;
  }
  QDir config_dir(RD_DEFAULT_RDSELECT_DIR);
  config_dir.setFilter(QDir::Files|QDir::Readable);
  config_dir.setNameFilter("*.conf");
  select_filenames=config_dir.entryList();
  for(int i=0;i<select_filenames.size();i++) {
    select_filenames[i]=
      QString(RD_DEFAULT_RDSELECT_DIR)+"/"+select_filenames[i];
    if(select_filenames[i]==target) {
      select_current_id=i;
    }
    select_configs.push_back(new RDConfig());
    select_configs.back()->setFilename(select_filenames[i]);
    select_configs.back()->load();
    select_configs.back()->setModuleName("RDSelect");
  }

  //
  // Current System Label
  //
  select_current_label=new QLabel(this);
  select_current_label->setFont(progressFont());
  select_current_label->setAlignment(Qt::AlignCenter);

  //
  // Selector Box
  //
  select_box=new Q3ListBox(this);
  connect(select_box,SIGNAL(doubleClicked(Q3ListBoxItem *)),
	  this,SLOT(doubleClickedData(Q3ListBoxItem *)));
  for(unsigned i=0;i<select_configs.size();i++) {
    select_box->insertItem(select_configs[i]->label());
  }
  select_label=new QLabel(select_box,tr("Available Systems"),this);
  select_label->setFont(labelFont());

  //
  // Ok Button
  //
  ok_button=new QPushButton(this);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("Select"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  SetSystem(select_current_id);
  SetCurrentItem(select_current_id);
  select_box->clearSelection();

  //
  // Check for Root User 
  //
  /*
  setuid(geteuid());  // So the SETUID bit works as expected
  if(getuid()!=0) {
    QMessageBox::information(this,tr("RDSelect"),
			     tr("Only root can run this utility!"));
    exit(256);
  }
  */
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::doubleClickedData(Q3ListBoxItem *item)
{
  okData();
}


void MainWidget::okData()
{
  QStringList args;
  QProcess *proc=NULL;

  QStringList f0=select_configs[select_box->currentItem()]->filename().
    split("/",QString::SkipEmptyParts);
  args.push_back(f0.last());
  proc=new QProcess(this);
  proc->start(QString(RD_PREFIX)+"/bin/rdselect_helper",args);
  proc->waitForFinished();
  if(proc->exitStatus()!=QProcess::NormalExit) {
    QMessageBox::critical(this,"RDSelect - "+tr("Error"),
			  tr("RDSelect helper process crashed!"));
    delete proc;
    return;
  }
  if(proc->exitCode()!=0) {
    QMessageBox::critical(this,"RDSelect - "+tr("Error"),
			  tr("Unable to select configuration:")+"\n"+
			  RDConfig::rdselectExitCodeText((RDConfig::RDSelectExitCode)proc->exitCode()));
    delete proc;
    return;
  }
  delete proc;

  exit(0);
}


void MainWidget::cancelData()
{
  exit(0);
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  select_current_label->setGeometry(0,10,size().width(),21);
  select_label->setGeometry(10,35,size().width()-20,20);
  select_box->setGeometry(10,55,size().width()-20,size().height()-125);
  ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void MainWidget::SetSystem(int id)
{
  QString text=tr("None");
  if(id>=0) {
    text=select_configs[id]->label();
  }
  select_current_label->setText(tr("Current System:")+" "+text);
  select_current_id=id;
}


void MainWidget::SetCurrentItem(int id)
{
  QPixmap *pix=redx_map;
  int schema=0;
  bool db_ok=RDDbValid(select_configs[select_current_id],&schema);
  bool snd_ok=RDAudioStoreValid(select_configs[select_current_id]);
  
  if(db_ok&(schema==RD_VERSION_DATABASE)&&snd_ok) {
    pix=greencheckmark_map;
  }
  for(unsigned i=0;i<select_box->count();i++) {
    if((int)i==id) {
      select_box->changeItem(*pix,select_configs[i]->label(),i);
    }
    else {
      select_box->changeItem(select_configs[i]->label(),i);
    }
  }
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString("/usr/share/qt4/translations/qt_")+QTextCodec::locale(),
	  ".");
  a.installTranslator(&qt);

  QTranslator rd(0);
  rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rd);

  QTranslator rdhpi(0);
  rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&rdhpi);

  QTranslator tr(0);
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdselect_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  a.setMainWidget(w);
  w->show();
  return a.exec();
}
