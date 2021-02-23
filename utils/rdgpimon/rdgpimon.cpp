// rdgpimon.cpp
//
// A Qt-based application for testing General Purpose Input (GPI) devices.
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
#include <QMessageBox>
#include <QTranslator>

#include <rdescape_string.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "rdgpimon.h"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString err_msg;

  gpi_scroll_mode=false;
  
  //
  // Open the Database
  //
  rda=new RDApplication("RDGpiMon","rdgpimon",RDGPIMON_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDGpiMon - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDGpiMon - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Set Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
  setWindowIcon(rda->iconEngine()->applicationIcon(RDIconEngine::Rivendell,22));

  //
  // RIPC Connection
  //
  rda->ripc()->setIgnoreMask(true);
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  connect(rda->ripc(),SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));
  connect(rda->ripc(),SIGNAL(gpoStateChanged(int,int,bool)),
	  this,SLOT(gpoStateChangedData(int,int,bool)));
  connect(rda->ripc(),SIGNAL(gpiMaskChanged(int,int,bool)),
	  this,SLOT(gpiMaskChangedData(int,int,bool)));
  connect(rda->ripc(),SIGNAL(gpoMaskChanged(int,int,bool)),
	  this,SLOT(gpoMaskChangedData(int,int,bool)));
  connect(rda->ripc(),SIGNAL(gpiCartChanged(int,int,int,int)),
	  this,SLOT(gpiCartChangedData(int,int,int,int)));
  connect(rda->ripc(),SIGNAL(gpoCartChanged(int,int,int,int)),
	  this,SLOT(gpoCartChangedData(int,int,int,int)));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // RDMatrix;
  //
  gpi_matrix=NULL;

  //
  // Type Selector
  //
  gpi_type_box=new QComboBox(this);
  gpi_type_box->setGeometry(80,10,120,21);
  gpi_type_box->insertItem(0,tr("GPI (Inputs)"));
  gpi_type_box->insertItem(1,tr("GPO (Outputs)"));
  QLabel *label=new QLabel(tr("Show:"),this);
  label->setGeometry(20,10,55,21);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(gpi_type_box,SIGNAL(activated(int)),
	  this,SLOT(matrixActivatedData(int)));

  //
  // Matrix Selector
  //
  gpi_matrix_box=new QComboBox(this);
  gpi_matrix_box->setGeometry(280,10,80,21);
  for(int i=0;i<MAX_MATRICES;i++) {
    gpi_matrix_box->
      insertItem(gpi_matrix_box->count(),QString().sprintf("%d",i));
  }
  label=new QLabel(tr("Matrix:"),this);
  label->setGeometry(220,10,55,21);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
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
    new RDTransportButton(RDTransportButton::Up,this);
  gpi_up_button->setGeometry(10,360,80,50);
  connect(gpi_up_button,SIGNAL(clicked()),this,SLOT(upData()));

  //
  // Down Button
  //
  gpi_down_button=
    new RDTransportButton(RDTransportButton::Down,this);
  gpi_down_button->setGeometry(100,360,80,50);
  connect(gpi_down_button,SIGNAL(clicked()),this,SLOT(downData()));

  //
  // Color Key
  //
  label=new QLabel(tr("Green = ON Cart"),this);
  label->setGeometry(200,370,300,12);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  QPalette p=palette();
  p.setColor(QPalette::Active,QPalette::Foreground,Qt::darkGreen);
  p.setColor(QPalette::Inactive,QPalette::Foreground,Qt::darkGreen);
  p.setColor(QPalette::Disabled,QPalette::Foreground,Qt::darkGreen);
  label->setPalette(p);

  label=new QLabel(tr("Red = OFF Cart"),this);
  label->setGeometry(200,392,300,12);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  p.setColor(QPalette::Active,QPalette::Foreground,Qt::darkRed);
  p.setColor(QPalette::Inactive,QPalette::Foreground,Qt::darkRed);
  p.setColor(QPalette::Disabled,QPalette::Foreground,Qt::darkRed);
  label->setPalette(p);

  //
  // Events Log
  //
  label=new QLabel(tr("Events Log"),this);
  label->setFont(sectionLabelFont());
  label->setAlignment(Qt::AlignCenter);
  label->setGeometry(110,423,sizeHint().width()-220,30);

  gpi_events_date_edit=new QDateEdit(this);
  gpi_events_date_edit->setDisplayFormat("dd/MM/yyyy");
  gpi_events_date_edit->setGeometry(155,453,100,20);
  gpi_events_date_edit->setDate(QDate::currentDate());
  gpi_events_date_label=new QLabel(tr("Date")+":",this);
  gpi_events_date_label->setGeometry(100,453,50,20);
  gpi_events_date_label->setFont(labelFont());
  gpi_events_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  gpi_events_state_box=new QComboBox(this);
  gpi_events_state_box->setGeometry(330,453,55,20);
  gpi_events_state_box->insertItem(0,tr("On"));
  gpi_events_state_box->insertItem(1,tr("Off"));
  gpi_events_state_box->insertItem(2,tr("Both"));
  gpi_events_state_label=new QLabel(tr("State")+":",this);
  gpi_events_state_label->setGeometry(275,453,50,20);
  gpi_events_state_label->setFont(labelFont());
  gpi_events_state_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  gpi_events_view=new RDTableView(this);
  gpi_events_view->setGeometry(110,480,sizeHint().width()-220,230);
  gpi_events_view->setSelectionMode(QAbstractItemView::NoSelection);
  gpi_events_model=new RDGpioLogModel(this);
  gpi_events_model->setFont(defaultFont());
  gpi_events_model->setPalette(palette());
  gpi_events_view->setModel(gpi_events_model);
  connect(gpi_events_model,SIGNAL(modelReset()),
	  gpi_events_view,SLOT(resizeColumnsToContents()));
  connect(gpi_events_date_edit,SIGNAL(dateChanged(const QDate &)),
	  gpi_events_model,SLOT(setDateFilter(const QDate &)));
  connect(gpi_events_state_box,SIGNAL(activated(const QString &)),
	  gpi_events_model,SLOT(setStateFilter(const QString &)));
  connect(gpi_events_model,SIGNAL(rowsInserted(const QModelIndex &,int,int)),
	  this,SLOT(rowsInsertedData(const QModelIndex &,int,int)));
  gpi_events_view->resizeColumnsToContents();
  gpi_events_scroll_button=new QPushButton(tr("Scroll"),this);
  gpi_events_scroll_button->setGeometry(sizeHint().width()-100,510,80,50);
  gpi_events_scroll_button->setFont(buttonFont());
  connect(gpi_events_scroll_button,SIGNAL(clicked()),
	  this,SLOT(eventsScrollData()));
  gpi_scroll_color=palette();
  gpi_scroll_color.setColor(QPalette::Active,QPalette::ButtonText,
			    Qt::white);
  gpi_scroll_color.setColor(QPalette::Active,QPalette::Button,
			    Qt::blue);
  gpi_scroll_color.setColor(QPalette::Active,QPalette::Background,
			    Qt::lightGray);
  gpi_scroll_color.setColor(QPalette::Inactive,QPalette::ButtonText,
			    Qt::white);
  gpi_scroll_color.setColor(QPalette::Inactive,QPalette::Button,
			    Qt::blue);
  gpi_scroll_color.setColor(QPalette::Inactive,QPalette::Background,
			    Qt::lightGray);

  gpi_events_report_button=new QPushButton(tr("Report"),this);
  gpi_events_report_button->setGeometry(sizeHint().width()-100,570,80,50);
  gpi_events_report_button->setFont(buttonFont());
  connect(gpi_events_report_button,SIGNAL(clicked()),
	  this,SLOT(eventsReportData()));

  //
  // Start Up Timer
  //
  gpi_events_startup_timer=new QTimer(this);
  gpi_events_startup_timer->setSingleShot(true);
  connect(gpi_events_startup_timer,SIGNAL(timeout()),this,SLOT(startUpData()));
  gpi_events_startup_timer->start(GPIMON_START_UP_DELAY);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(528,78*GPIMON_ROWS+410);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::userData()
{
  QString str;

  str=QString("RDGpiMon")+" v"+VERSION+" - "+tr("User")+":";
  setWindowTitle(str+" "+rda->ripc()->user());
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
    new RDMatrix(rda->config()->stationName(),gpi_matrix_box->currentIndex());
  UpdateLabelsDown(0);
  gpi_up_button->setDisabled(true);
  //  RefreshEventsList();
  gpi_events_model->
    setGpioType((RDMatrix::GpioType)gpi_type_box->currentIndex());
  gpi_events_model->setMatrixNumber(index);

  gpi_events_startup_timer->start(1000);
}


void MainWidget::eventsScrollData()
{
  if(gpi_scroll_mode) {
    gpi_events_scroll_button->setPalette(palette());
    gpi_scroll_mode=false;
  }
  else {
    gpi_events_scroll_button->setPalette(gpi_scroll_color);
    gpi_scroll_mode=true;
    gpi_events_view->
      scrollTo(gpi_events_model->index(gpi_events_model->rowCount()-1,0));
  }
}


void MainWidget::eventsReportData()
{
  QString report;
  QString sql;
  RDSqlQuery *q;

  report=RDReport::center("Rivendell GPIO Event Report",78)+"\n";
  report+=
    RDReport::center(QString("Date: ")+
		     gpi_events_date_edit->date().toString("MM/dd/yyyy")+
		     "       "+rda->station()->name()+":"+
		     QString().sprintf("%d     ",gpi_matrix_box->currentIndex())+
		     " State Filter: "+
		     gpi_events_state_box->currentText(),78)+"\n";
  report+="\n";

  sql=QString("select ")+
    "EVENT_DATETIME,"+  // 00
    "NUMBER,"+          // 01
    "EDGE "+            // 02
    "from GPIO_EVENTS where "+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",gpi_matrix_box->currentIndex())+
    QString().sprintf("(TYPE=%d)&&",gpi_type_box->currentIndex())+
    "(EVENT_DATETIME>=\""+gpi_events_date_edit->date().toString("yyyy-MM-dd")+
    " 00:00:00\")&&"+
    "(EVENT_DATETIME<\""+gpi_events_date_edit->date().addDays(1).
    toString("yyyy-MM-dd")+" 00:00:00\")";
  if(gpi_events_state_box->currentIndex()==0) {
    sql+="&&(EDGE=1)";
  }
  if(gpi_events_state_box->currentIndex()==1) {
    sql+="&&(EDGE=0)";
  }
  report+="                       -- Time --   - Line -   - State -\n";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    report+="                        ";
    report+=q->value(0).toDateTime().toString("hh:mm:ss")+"   ";
    report+=QString().sprintf("   %5d       ",q->value(1).toInt());
    if(q->value(2).toInt()==0) {
      report+=tr("OFF");
    }
    else {
      report+=tr("ON ");
    }
    report+="\n";
  }
  delete q;
  RDTextFile(report);
}


void MainWidget::rowsInsertedData(const QModelIndex &parent,int start,int end)
{
  if(gpi_scroll_mode) {
    gpi_events_view->scrollTo(gpi_events_model->index(end,0));
  }
}


void MainWidget::gpiStateChangedData(int matrix,int line,bool state)
{
  //  printf("gpiStateChanged(%d,%d,%d)\n",matrix,line,state);

  if(gpi_type_box->currentIndex()!=RDMatrix::GpioInput) {
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
  AddEventsItem(line,state);
}


void MainWidget::gpoStateChangedData(int matrix,int line,bool state)
{
  //  printf("gpoStateChanged(%d,%d,%d)\n",matrix,line,state);

  if(gpi_type_box->currentIndex()!=RDMatrix::GpioOutput) {
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
  AddEventsItem(line,state);
}


void MainWidget::gpiMaskChangedData(int matrix,int line,bool state)
{
  if(gpi_type_box->currentIndex()!=RDMatrix::GpioInput) {
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
  //  printf("gpoMaskChangedData(%d,%d,%d)\n",matrix,line,state);
  if(gpi_type_box->currentIndex()!=RDMatrix::GpioOutput) {
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

  if(gpi_type_box->currentIndex()!=RDMatrix::GpioInput) {
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
  //  printf("gpoCartChangedData(%d,%d,%d,%d)\n",matrix,line,off_cartnum,
  //	 on_cartnum);
  if(gpi_type_box->currentIndex()!=RDMatrix::GpioOutput) {
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
  gpi_events_startup_timer->disconnect();
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
  switch((RDMatrix::GpioType)gpi_type_box->currentIndex()) {
    case RDMatrix::GpioInput:
      tablename="GPIS";
      break;

    case RDMatrix::GpioOutput:
      tablename="GPOS";
      break;
  }
  sql=QString("select ")+
    "NUMBER,"+          // 00
    "OFF_MACRO_CART,"+  // 01
    "MACRO_CART "+      // 02
    "from "+tablename+" where "+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",gpi_matrix->matrix())+
    QString().sprintf("(NUMBER<=%d) ",last_line)+
    "order by NUMBER desc";
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

  RefreshGpioStates();
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
  switch((RDMatrix::GpioType)gpi_type_box->currentIndex()) {
    case RDMatrix::GpioInput:
      tablename="GPIS";
      break;

    case RDMatrix::GpioOutput:
      tablename="GPOS";
      break;
  }
  sql=QString("select ")+
    "NUMBER,"+          // 00
    "OFF_MACRO_CART,"+  // 01
    "MACRO_CART "+      // 02
    "from "+tablename+" where "+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",gpi_matrix->matrix())+
    QString().sprintf("(NUMBER>=%d) ",first_line)+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()&&(count<(GPIMON_ROWS*GPIMON_COLS))) {
    gpi_labels[count]->setCart(q->value(1).toUInt(),q->value(2).toUInt());
    if(!first_updated) {
      gpi_first_line=q->value(0).toInt();
      first_updated=true;
    }
    gpi_last_line=q->value(0).toInt();
    count++;
  }
  gpi_down_button->setEnabled(q->next());
  delete q;

  sql=QString("select ")+
    tablename+" "+  // 00
    "from MATRICES where "+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\") && "+
    QString().sprintf("(MATRIX=%d)",gpi_matrix->matrix());
  q=new RDSqlQuery(sql);
  if(q->first()) {
    for(int i=0;i<(GPIMON_ROWS*GPIMON_COLS);i++) {
      if((i+first_line)<q->value(0).toInt()) {
	gpi_labels[i]->setLine(i+first_line);
	gpi_labels[i]->show();
      }
    }
  }
  delete q;

  RefreshGpioStates();
}


void MainWidget::RefreshGpioStates()
{
  if(gpi_type_box->currentIndex()==0) {
    rda->ripc()->sendGpiStatus(gpi_matrix_box->currentIndex());
    rda->ripc()->sendGpiCart(gpi_matrix_box->currentIndex());
    rda->ripc()->sendGpiMask(gpi_matrix_box->currentIndex());
  }
  else {
    rda->ripc()->sendGpoStatus(gpi_matrix_box->currentIndex());
    rda->ripc()->sendGpoCart(gpi_matrix_box->currentIndex());
    rda->ripc()->sendGpoMask(gpi_matrix_box->currentIndex());
  }
}


void MainWidget::AddEventsItem(int line,bool state)
{
  if(gpi_events_startup_timer->isActive()) {
    return;
  }
  if((gpi_events_state_box->currentIndex()==0)&&(!state)) {
    return;
  }
  if((gpi_events_state_box->currentIndex()==1)&&state) {
    return;
  }
  gpi_events_model->addEvent(line,state);
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    QTranslator qt(0);
    qt.load(QString("/usr/share/qt4/translations/qt_")+loc,".");
    a.installTranslator(&qt);

    QTranslator rd(0);
    rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+loc,".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+loc,".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(QString(PREFIX)+QString("/share/rivendell/rdgpimon_")+loc,".");
    a.installTranslator(&tr);
  }

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
