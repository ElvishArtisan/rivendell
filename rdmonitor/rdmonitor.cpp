// rdmonitor.cpp
//
// System Monitor for Rivendell
//
//   (C) Copyright 2012-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QProcess>
#include <QTranslator>

#include <dbversion.h>
#include <rdstatus.h>
#include <rdtranslator.h>

#include "rdmonitor.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"

void SigHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
  case SIGINT:
    exit(0);
    break;
  }
}

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent,Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint)
{
  QString str;
  mon_dialog_x=0;
  mon_dialog_y=0;
  mon_rdselect_x=0;
  mon_rdselect_y=0;

  rdt=new RDTranslator("rdmonitor",this);  // Load translations

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("rdmonitor","\n");
  delete cmd;

  //
  // Hide at startup
  //
  setGeometry(0,0,0,0);

  //
  // Process Fonts
  //
  mon_metrics=new QFontMetrics(font());

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));

  //
  // Dialogs
  //
  mon_rdconfig=new RDConfig();
  mon_rdconfig->load();
  mon_rdconfig->setModuleName("RDMonitor");

  mon_desktop_widget=new QDesktopWidget();
  mon_config=new RDMonitorConfig();
  mon_config->load();
  mon_position_dialog=new PositionDialog(mon_desktop_widget,mon_config,
					 mon_rdconfig,this);
  mon_position_dialog->setGeometry(0,0,mon_position_dialog->sizeHint().width(),
				   mon_position_dialog->sizeHint().height());

  //
  // Name Label
  //
  mon_name_label=new QLabel(this);
  mon_name_label->setFont(labelFont());

  //
  // Status Icons
  //
  mon_green_label=new QLabel(this);
  mon_green_label->setPixmap(QPixmap(greenball_xpm));
  mon_green_label->hide();

  mon_red_label=new QLabel(this);
  mon_red_label->setPixmap(QPixmap(redball_xpm));
  mon_red_label->hide();

  //
  // Status Label
  //
  mon_status_label=new QLabel(tr("Status: unknown"),NULL,
			      Qt::CustomizeWindowHint|Qt::WindowStaysOnTopHint);
  mon_status_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
  mon_status_label->
    setStyleSheet(QString("background-color:")+RD_STATUS_BACKGROUND_COLOR);
  mon_status_label->hide();

  //
  // Validation Timer
  //
  mon_validate_timer=new QTimer(this);
  connect(mon_validate_timer,SIGNAL(timeout()),this,SLOT(validate()));
  mon_validate_timer->start(5000);

  mon_name_label->setText(mon_rdconfig->label());
  SetPosition();

  ::signal(SIGTERM,SigHandler);
  ::signal(SIGINT,SigHandler);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::validate()
{
  bool db_ok=false;
  bool snd_ok=false;
  int schema=0;
  std::vector<QString> bad_cuts;

  //
  // Get Configurations
  //
  mon_rdconfig->load();
  mon_name_label->setText(mon_rdconfig->label());

  //
  // Check Audio Store
  //
  snd_ok=RDAudioStoreValid(mon_rdconfig);

  //
  // Check Database
  //
  db_ok=RDDbValid(mon_rdconfig,&schema);

  //
  // Record Results
  //
  SetToolTip(db_ok,schema,snd_ok);
  //  mon_tooltip->
  //    setStatus(QRect(0,0,size().width(),size().height()),db_ok,schema,snd_ok);
  SetSummaryState(db_ok&&(schema==RD_VERSION_DATABASE)&&snd_ok);
  SetPosition();
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::enterEvent(QEvent *e)
{
  mon_status_label->show();
  QWidget::enterEvent(e);
}


void MainWidget::leaveEvent(QEvent *e)
{
  mon_status_label->hide();
  QWidget::leaveEvent(e);
}


void MainWidget::mousePressEvent(QMouseEvent *e)
{
  if(e->button()!=Qt::RightButton) {
    e->ignore();
    return;
  }
  e->accept();
  mon_position_dialog->setGeometry(mon_dialog_x,mon_dialog_y,
				   mon_position_dialog->sizeHint().width(),
				   mon_position_dialog->sizeHint().height());
  if(mon_position_dialog->exec()==0) {
    mon_config->save();
    SetPosition();
  }
}


void MainWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
  if(e->button()!=Qt::LeftButton) {
    e->ignore();
    return;
  }
  e->accept();
  QDir dir(RD_DEFAULT_RDSELECT_DIR);
  dir.setFilter(QDir::Files|QDir::Readable);
  QStringList filters;
  filters.push_back("*.conf");
  dir.setNameFilters(filters);
  if(dir.entryList().size()>1) {
    QProcess *proc=new QProcess(this);
    QStringList args;
    args.push_back("-geometry");
    args.push_back(QString::asprintf("+%d+%d",mon_rdselect_x,mon_rdselect_y));
    proc->start("rdselect",args);
    proc->waitForFinished(-1);
    if(proc->exitStatus()!=QProcess::NormalExit) {
      QMessageBox::critical(this,"RDMonitor - "+tr("Error"),
			    tr("RDSelect crashed!"));
      delete proc;
      return;
    }
    if(proc->exitCode()!=0) {
      QMessageBox::critical(this,"RDMonitor - "+tr("Error"),
			    tr("RDSelect returned non-zero exit code")+
			    QString::asprintf(" %d.",proc->exitCode())+
			    "\n\nERROR MESSAGE:\n"+
			    proc->readAllStandardError());
      delete proc;
      return;
    }
    delete proc;
    validate();
  }
}


void MainWidget::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QPen(Qt::black,2));
  p->drawRect(1,1,size().width(),size().height());

  delete p;
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  mon_name_label->setGeometry(10,5,size().width()-30,size().height()-10);
  mon_green_label->setGeometry(size().width()-20,5,15,20);
  mon_red_label->setGeometry(size().width()-20,5,15,20);
}


void MainWidget::SetSummaryState(bool state)
{
  if(state) {
    mon_red_label->hide();
    mon_green_label->show();
  }
  else {
    mon_green_label->hide();
    mon_red_label->show();
  }
}


void MainWidget::SetPosition()
{
  int width=mon_metrics->width(mon_name_label->text())+40;
  QRect geo=mon_desktop_widget->screenGeometry(mon_config->screenNumber());
  QRect main_geo=mon_desktop_widget->geometry();
  int x=0;
  int dx=mon_config->xOffset();
  int y=0;
  int dy=mon_config->yOffset();
  int dw=mon_position_dialog->size().width()+30;
  int dh=mon_position_dialog->size().height()+30;

  switch(mon_config->position()) {
  case RDMonitorConfig::UpperLeft:
    x=geo.x()+dx;
    y=geo.y()+dy;
    break;

  case RDMonitorConfig::UpperCenter:
    x=geo.x()+(geo.width()-width)/2;
    y=geo.y()+dy;
    break;

  case RDMonitorConfig::UpperRight:
    x=geo.x()-dx+geo.width()-width;
    y=geo.y()+dy;
    break;

  case RDMonitorConfig::LowerLeft:
    x=geo.x()+dx;
    y=geo.y()-dy+geo.height()-RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::LowerCenter:
    x=geo.x()+(geo.width()-width)/2;
    y=geo.y()-dy+geo.height()-RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::LowerRight:
    x=geo.x()-dx+geo.width()-width;
    y=geo.y()-dy+geo.height()-RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }

  //
  // Ensure sane values
  //
  if(x<0) {
    x=0;
  }
  if(x>(main_geo.width()-width)) {
    x=main_geo.width()-width;
  }
  if(y<0) {
    y=0;
  }
  if(y>(main_geo.height()-RDMONITOR_HEIGHT)) {
    y=main_geo.height()-RDMONITOR_HEIGHT;
  }

  //
  // Set the dialog position
  //
  switch(mon_config->position()) {
  case RDMonitorConfig::UpperLeft:
    mon_dialog_x=x;
    mon_rdselect_x=x;
    mon_dialog_y=y+RDMONITOR_HEIGHT;
    mon_rdselect_y=y+RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::UpperCenter:
    mon_dialog_x=x+(width-dw)/2;
    mon_dialog_y=y+RDMONITOR_HEIGHT;
    mon_rdselect_x=x+(width-RDSELECT_WIDTH)/2;
    mon_rdselect_y=y+RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::UpperRight:
    mon_dialog_x=x+width-dw;
    mon_dialog_y=y+RDMONITOR_HEIGHT;
    mon_rdselect_x=x+width-RDSELECT_WIDTH;
    mon_rdselect_y=y+RDMONITOR_HEIGHT;
    break;

  case RDMonitorConfig::LowerLeft:
    mon_dialog_x=x;
    mon_rdselect_x=x;
    mon_dialog_y=y-dh;
    mon_rdselect_y=y-RDSELECT_HEIGHT-30;
    break;

  case RDMonitorConfig::LowerCenter:
    mon_dialog_x=x+(width-dw)/2;
    mon_dialog_y=y-dh;
    mon_rdselect_x=x+(width-RDSELECT_WIDTH)/2;
    mon_rdselect_y=y-RDSELECT_HEIGHT-30;
    break;

  case RDMonitorConfig::LowerRight:
    mon_dialog_x=x+width-dw;
    mon_rdselect_x=x+width-RDSELECT_WIDTH;
    mon_dialog_y=y-dh;
    mon_rdselect_y=y-RDSELECT_HEIGHT-30;
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }
  setGeometry(x,y,width,RDMONITOR_HEIGHT);
  SetStatusPosition();
}


void MainWidget::SetToolTip(bool db_status,int schema,bool snd_status)
{
  mon_status_label->clear();
  if(db_status&&(schema==RD_VERSION_DATABASE)&&snd_status) {
    mon_status_label->setText(tr("Status: OK"));
  }
  else {
    if(!db_status) {
      mon_status_label->setText(tr("Database: CONNECTION FAILED"));
    }
    else {
      if(schema!=RD_VERSION_DATABASE) {
	mon_status_label->setText(tr("Database: SCHEMA SKEWED"));
      }
    }
    if(!snd_status) {
      if(!mon_status_label->text().isEmpty()) {
	mon_status_label->setText(mon_status_label->text()+"\n");
      }
      mon_status_label->
	setText(mon_status_label->text()+tr("Audio Store: FAILED"));
    }
  }
  SetStatusPosition();
}


void MainWidget::SetStatusPosition()
{
  QFontMetrics *fm=new QFontMetrics(mon_status_label->font());
  int h=10+fm->height();
  int w=10+fm->width(mon_status_label->text());
  QRect g=geometry();

  switch(mon_config->position()) {
  case RDMonitorConfig::UpperLeft:
    mon_status_label->setGeometry(20+g.x(),g.y()+(2+g.height()),w,h);
    break;

  case RDMonitorConfig::UpperCenter:
    mon_status_label->
      setGeometry(g.x()+(g.width()-w)/2,g.y()+(2+g.height()),w,h);
    break;

  case RDMonitorConfig::UpperRight:
    mon_status_label->
      setGeometry(g.x()+g.width()-w-20,g.y()+(2+g.height()),w,h);
    break;

  case RDMonitorConfig::LowerLeft:
    mon_status_label->setGeometry(20+g.x(),g.y()-(2+h),w,h);
    break;

  case RDMonitorConfig::LowerCenter:
    mon_status_label->setGeometry(g.x()+(g.width()-w)/2,g.y()-(2+h),w,h);
    break;

  case RDMonitorConfig::LowerRight:
    mon_status_label->setGeometry(g.x()+g.width()-w-20,g.y()-(2+h),w,h);
    break;

  case RDMonitorConfig::LastPosition:
    break;
  }

  delete fm;
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->show();
  return a.exec();
}
