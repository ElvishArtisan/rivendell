// rdgpimon.cpp
//
// A Qt-based application for testing General Purpose Input (GPI) devices.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgpimon.cpp,v 1.14.6.4 2014/01/21 21:59:34 cvs Exp $
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

#include <stdlib.h>

#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include <rddb.h>
#include <rdgpimon.h>
#include <rdcmd_switch.h>
#include <rddbheartbeat.h>

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdgpimon","\n");
  delete cmd;

  //
  // Set Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create Font
  //
  QFont font("helvetica",10,QFont::Normal);
  font.setPixelSize(10);
  setFont(font);
  QFont main_font("helvetica",12,QFont::Bold);
  main_font.setPixelSize(12);

  //
  // Create And Set Icon
  //
  gpi_rivendell_map=new QPixmap(rivendell_xpm);
  setIcon(*gpi_rivendell_map);

  //
  // Load Local Configs
  //
  gpi_config=new RDConfig();
  gpi_config->load();

  //
  // Open Database
  //
  gpi_db=QSqlDatabase::addDatabase(gpi_config->mysqlDriver());
  if(!gpi_db) {
    QMessageBox::warning(this,tr("Database Error"),
		    tr("Can't Connect","Unable to connect to mySQL Server!"));
    exit(0);
  }
  gpi_db->setDatabaseName(gpi_config->mysqlDbname());
  gpi_db->setUserName(gpi_config->mysqlUsername());
  gpi_db->setPassword(gpi_config->mysqlPassword());
  gpi_db->setHostName(gpi_config->mysqlHostname());
  if(!gpi_db->open()) {
    QMessageBox::warning(this,tr("Can't Connect"),
			 tr("Unable to connect to mySQL Server!"));
    gpi_db->removeDatabase(gpi_config->mysqlDbname());
    exit(0);
  }
  new RDDbHeartbeat(gpi_config->mysqlHeartbeatInterval(),this);

  //
  // RIPC Connection
  //
  gpi_ripc=new RDRipc(gpi_config->stationName());
  gpi_ripc->setIgnoreMask(true);
  connect(gpi_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(gpi_ripc,SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));
  connect(gpi_ripc,SIGNAL(gpoStateChanged(int,int,bool)),
	  this,SLOT(gpoStateChangedData(int,int,bool)));
  connect(gpi_ripc,SIGNAL(gpiMaskChanged(int,int,bool)),
	  this,SLOT(gpiMaskChangedData(int,int,bool)));
  connect(gpi_ripc,SIGNAL(gpoMaskChanged(int,int,bool)),
	  this,SLOT(gpoMaskChangedData(int,int,bool)));
  connect(gpi_ripc,SIGNAL(gpiCartChanged(int,int,int,int)),
	  this,SLOT(gpiCartChangedData(int,int,int,int)));
  connect(gpi_ripc,SIGNAL(gpoCartChanged(int,int,int,int)),
	  this,SLOT(gpoCartChangedData(int,int,int,int)));
  gpi_ripc->connectHost("localhost",RIPCD_TCP_PORT,gpi_config->password());

  //
  // RDStation
  //
  gpi_station=new RDStation(gpi_config->stationName());

  //
  // RDMatrix;
  //
  gpi_matrix=NULL;

  //
  // Type Selector
  //
  gpi_type_box=new QComboBox(this,"gpi_type_box");
  gpi_type_box->setGeometry(80,10,120,21);
  gpi_type_box->insertItem(tr("GPI (Inputs)"));
  gpi_type_box->insertItem(tr("GPO (Outputs)"));
  QLabel *label=
    new QLabel(gpi_type_box,tr("Show:"),this,"gpi_type_label");
  label->setGeometry(20,10,55,21);
  label->setFont(main_font);
  label->setAlignment(AlignRight|AlignVCenter);
  connect(gpi_type_box,SIGNAL(activated(int)),
	  this,SLOT(matrixActivatedData(int)));

  //
  // Matrix Selector
  //
  gpi_matrix_box=new QComboBox(this,"gpi_matrix_box");
  gpi_matrix_box->setGeometry(280,10,80,21);
  for(int i=0;i<MAX_MATRICES;i++) {
    gpi_matrix_box->insertItem(QString().sprintf("%d",i));
  }
  label=new QLabel(gpi_matrix_box,tr("Matrix:"),this,"gpi_matrix_label");
  label->setGeometry(220,10,55,21);
  label->setFont(main_font);
  label->setAlignment(AlignRight|AlignVCenter);
  connect(gpi_matrix_box,SIGNAL(activated(int)),
	  this,SLOT(matrixActivatedData(int)));

  //
  // GPI Indicators
  //
  for(int i=0;i<GPIMON_ROWS;i++) {
    for(int j=0;j<GPIMON_COLS;j++) {
      gpi_labels[i*GPIMON_COLS+j]=new GpiLabel(this);
      gpi_labels[i*GPIMON_COLS+j]->setGeometry(10+64*j,40+78*i,
					       gpi_labels[i*GPIMON_COLS+j]->
					       sizeHint().width(),
					       gpi_labels[i*GPIMON_COLS+j]->
					       sizeHint().height());
    }
  }

  //
  // Up Button
  //
  gpi_up_button=
    new RDTransportButton(RDTransportButton::Up,this,"gpi_up_button");
  gpi_up_button->setGeometry(10,sizeHint().height()-60,80,50);
  connect(gpi_up_button,SIGNAL(clicked()),this,SLOT(upData()));

  //
  // Down Button
  //
  gpi_down_button=
    new RDTransportButton(RDTransportButton::Down,this,"gpi_down_button");
  gpi_down_button->setGeometry(100,sizeHint().height()-60,80,50);
  connect(gpi_down_button,SIGNAL(clicked()),this,SLOT(downData()));

  //
  // Color Key
  //
  label=new QLabel(tr("Green = ON Cart"),this);
  label->setGeometry(200,sizeHint().height()-50,300,12);
  label->setFont(main_font);
  label->setAlignment(AlignLeft|AlignVCenter);
  QPalette p=palette();
  p.setColor(QPalette::Active,QColorGroup::Foreground,darkGreen);
  p.setColor(QPalette::Inactive,QColorGroup::Foreground,darkGreen);
  p.setColor(QPalette::Disabled,QColorGroup::Foreground,darkGreen);
  label->setPalette(p);

  label=new QLabel(tr("Red = OFF Cart"),this);
  label->setGeometry(200,sizeHint().height()-32,300,12);
  label->setFont(main_font);
  label->setAlignment(AlignLeft|AlignVCenter);
  p.setColor(QPalette::Active,QColorGroup::Foreground,darkRed);
  p.setColor(QPalette::Inactive,QColorGroup::Foreground,darkRed);
  p.setColor(QPalette::Disabled,QColorGroup::Foreground,darkRed);
  label->setPalette(p);


  //
  // Close Button
  //
  gpi_close_button=new QPushButton(this,"gpi_close_button");
  gpi_close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
				80,50);
  gpi_close_button->setFont(main_font);
  gpi_close_button->setText(tr("&Close"));
  connect(gpi_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  //
  // Start Up Timer
  //
  QTimer *timer=new QTimer(this,"start_up_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(startUpData()));
  timer->start(GPIMON_START_UP_DELAY,true);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(528,78*GPIMON_ROWS+110);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::userData()
{
  QString str;

  str=QString("RDGpiMon")+" v"+VERSION+" - "+tr("User")+":";
  setCaption(str+" "+gpi_ripc->user());
}


void MainWidget::typeActivatedData(int index)
{
}


void MainWidget::matrixActivatedData(int index)
{
  if(gpi_matrix!=NULL) {
    delete gpi_matrix;
  }
  gpi_matrix=
    new RDMatrix(gpi_config->stationName(),gpi_matrix_box->currentItem());
  UpdateLabelsDown(0);
  gpi_up_button->setDisabled(true);
  switch((RDMatrix::GpioType)gpi_type_box->currentItem()) {
    case RDMatrix::GpioInput:
      gpi_ripc->sendGpiStatus(gpi_matrix_box->currentItem());
      gpi_ripc->sendGpiMask(gpi_matrix_box->currentItem());
      gpi_ripc->sendGpiCart(gpi_matrix_box->currentItem());
      break;

    case RDMatrix::GpioOutput:
      gpi_ripc->sendGpoStatus(gpi_matrix_box->currentItem());
      gpi_ripc->sendGpoMask(gpi_matrix_box->currentItem());
      gpi_ripc->sendGpoCart(gpi_matrix_box->currentItem());
      break;
  }
}


void MainWidget::gpiStateChangedData(int matrix,int line,bool state)
{
  // printf("gpiStateChanged(%d,%d,%d)\n",matrix,line,state);

  if(gpi_type_box->currentItem()!=RDMatrix::GpioInput) {
    return;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setState(state);
    }
  }
}


void MainWidget::gpoStateChangedData(int matrix,int line,bool state)
{
  // printf("gpoStateChanged(%d,%d,%d)\n",matrix,line,state);

  if(gpi_type_box->currentItem()!=RDMatrix::GpioOutput) {
    return;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setState(state);
    }
  }
}


void MainWidget::gpiMaskChangedData(int matrix,int line,bool state)
{
  if(gpi_type_box->currentItem()!=RDMatrix::GpioInput) {
    return;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setMask(state);
    }
  }
}


void MainWidget::gpoMaskChangedData(int matrix,int line,bool state)
{
  if(gpi_type_box->currentItem()!=RDMatrix::GpioOutput) {
    return;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setMask(state);
    }
  }
}


void MainWidget::gpiCartChangedData(int matrix,int line,int off_cartnum,
				    int on_cartnum)
{
//  printf("gpiCartChangedData(%d,%d,%d,%d)\n",matrix,line,off_cartnum,
//	 on_cartnum);

  if(gpi_type_box->currentItem()!=RDMatrix::GpioInput) {
    return;
  }
  if(off_cartnum<0) {
    off_cartnum=0;
  }
  if(on_cartnum<0) {
    on_cartnum=0;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setCart(off_cartnum,on_cartnum);
    }
  }
}


void MainWidget::gpoCartChangedData(int matrix,int line,int off_cartnum,
  int on_cartnum)
{
  if(gpi_type_box->currentItem()!=RDMatrix::GpioOutput) {
    return;
  }
  if(off_cartnum<0) {
    off_cartnum=0;
  }
  if(on_cartnum<0) {
    on_cartnum=0;
  }
  if(matrix!=gpi_matrix->matrix()) {
    return;
  }
  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    if(gpi_labels[i]->line()==line) {
      gpi_labels[i]->setCart(off_cartnum,on_cartnum);
    }
  }
}


void MainWidget::startUpData()
{
  matrixActivatedData(0);
}


void MainWidget::upData()
{
  UpdateLabelsUp(gpi_first_line-1);
  gpi_down_button->setEnabled(true);
}


void MainWidget::downData()
{
  UpdateLabelsDown(gpi_last_line+1);
  gpi_up_button->setEnabled(true);
}


void MainWidget::quitMainWidget()
{
  exit(0);
}


void MainWidget::UpdateLabelsUp(int last_line)
{
  QString sql;
  RDSqlQuery *q;
  int count=0;
  int count_limit=GPIMON_ROWS*GPIMON_COLS;
  bool last_updated=false;
  QString tablename="GPIS";

  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    gpi_labels[i]->hide();
  }
  switch((RDMatrix::GpioType)gpi_type_box->currentItem()) {
    case RDMatrix::GpioInput:
      tablename="GPIS";
      break;

    case RDMatrix::GpioOutput:
      tablename="GPOS";
      break;
  }
  sql=QString().sprintf("select NUMBER,OFF_MACRO_CART,MACRO_CART from %s \
                         where (STATION_NAME=\"%s\")&&\
                         (MATRIX=%d)&&\
                         (NUMBER<=%d) order by NUMBER desc",
			(const char *)tablename,
			(const char *)gpi_station->name(),
			gpi_matrix->matrix(),last_line);
  q=new RDSqlQuery(sql);
  if(q->size()<count_limit) {
    count_limit=q->size();
  }
  while(q->next()&&(count<count_limit)) {
    gpi_labels[count_limit-count-1]->setLine(q->value(0).toInt()-1);
    gpi_labels[count_limit-count-1]->
      setCart(q->value(1).toUInt(),q->value(2).toUInt());
    gpi_labels[count_limit-count-1]->show();
    if(!last_updated) {
      gpi_last_line=q->value(0).toInt();
      last_updated=true;
    }
    gpi_first_line=q->value(0).toInt();
    count++;
  }
  gpi_up_button->setEnabled(q->next());
  delete q;
}


void MainWidget::UpdateLabelsDown(int first_line)
{
  QString sql;
  RDSqlQuery *q;
  int count=0;
  bool first_updated=false;
  QString tablename="GPIS";

  for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
    gpi_labels[i]->hide();
  }
  switch((RDMatrix::GpioType)gpi_type_box->currentItem()) {
    case RDMatrix::GpioInput:
      tablename="GPIS";
      break;

    case RDMatrix::GpioOutput:
      tablename="GPOS";
      break;
  }
  sql=QString().sprintf("select NUMBER,OFF_MACRO_CART,MACRO_CART from %s \
                         where (STATION_NAME=\"%s\")&&\
                         (MATRIX=%d)&&\
                         (NUMBER>=%d) order by NUMBER",
			(const char *)tablename,
			(const char *)gpi_station->name(),
			gpi_matrix->matrix(),first_line);
  q=new RDSqlQuery(sql);
  while(q->next()&&(count<(GPIMON_ROWS*GPIMON_COLS))) {
    gpi_labels[count]->setLine(q->value(0).toInt()-1);
    gpi_labels[count]->setCart(q->value(1).toUInt(),q->value(2).toUInt());
    gpi_labels[count]->show();
    if(!first_updated) {
      gpi_first_line=q->value(0).toInt();
      first_updated=true;
    }
    gpi_last_line=q->value(0).toInt();
    count++;
  }
  gpi_down_button->setEnabled(q->next());
  delete q;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QTranslator qt(0);
  qt.load(QString(QTDIR)+QString("/translations/qt_")+QTextCodec::locale(),
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
  tr.load(QString(PREFIX)+QString("/share/rivendell/rdgpimon_")+
	     QTextCodec::locale(),".");
  a.installTranslator(&tr);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
