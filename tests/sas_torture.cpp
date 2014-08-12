// sas_torture.cpp
//
// A Qt-based application for playing Microsoft WAV files. 
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_torture.cpp,v 1.10 2011/06/21 22:20:44 cvs Exp $
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
#include <unistd.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qdatetime.h>

#include <rd.h>
#include <rddb.h>
#include <sas_torture.h>


MainWidget::MainWidget(QWidget *parent,const char *name)
  :QWidget(parent,name)
{
  unsigned schema=0;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont font("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Open Database
  //
  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();
  QString err;
  test_db=RDInitDb(&schema,&err);
  if(!test_db) {
    QMessageBox::warning(this,"Can't Connect",
			 err,0,1,1);
    exit(0);
  }
  //
  // Generate Button
  //
  QPushButton *button=new QPushButton(this,"generate_button");
  button->setGeometry(10,10,sizeHint().width()-20,50);
  button->setText("Generate Test");
  button->setFont(font);
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  // Remove Button
  //
  button=new QPushButton(this,"remove_button");
  button->setGeometry(10,70,sizeHint().width()-20,50);
  button->setText("Remove Test");
  button->setFont(font);
  connect(button,SIGNAL(clicked()),this,SLOT(removeData()));

  //
  // Exit Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(10,130,sizeHint().width()-20,50);
  button->setText("Exit");
  button->setFont(font);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize MainWidget::sizeHint() const
{
  return QSize(200,190);
}


QSizePolicy MainWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void MainWidget::generateData()
{
  QString sql;
  RDSqlQuery *q;
  QString rml;
  QString desc;

  //
  // Create Carts
  //
  for(int i=0;i<SAS_OUTPUTS;i++) {
    rml=QString();
    for(int j=0;j<SAS_INPUTS;j++) {
      rml+=QString().sprintf("ST %d %d %d!SP %d!",
			     SAS_MATRIX,j+1,i+1,SAS_SLEEP);
    }
    sql=QString().sprintf("insert into CART set NUMBER=%d,TYPE=2,\
                           GROUP_NAME=\"TEMP\",TITLE=\"Walk SAS Output %d\",\
                           ARTIST=\"SAS_TORTURE\",MACROS=\"%s\"",
			  i+CART_START,i+1,(const char *)rml);
    q=new RDSqlQuery(sql);
    delete q;
  }

  //
  // Create Schedule
  //
  QTime time;
  for(int i=0;i<86400000;i+=TIME_INTERVAL) {
    for(int j=0;j<SAS_OUTPUTS;j++) {
      desc=QString().sprintf("Walk SAS Output %d",j+1);
      sql=QString().sprintf("insert into RECORDINGS set STATION_NAME=\"%s\",\
                         SUN=\'Y\',MON=\'Y\',TUE=\'Y\',WED=\'Y\',THU=\'Y\',\
                         FRI=\'Y\',SAT=\'Y\',DESCRIPTION=\"%s\",\
                         CUT_NAME=\"SAS_TORTURE\",MACRO_CART=%d,\
                         START_TIME=\"%s\",TYPE=1",
			    SAS_STATION,
			    (const char *)desc,
			    CART_START+j,
			    (const char *)time.toString("hh:mm:ss"));
      q=new RDSqlQuery(sql);
      delete q;
    }
    time=time.addMSecs(TIME_INTERVAL);
  }
}


void MainWidget::removeData()
{
  QString sql;
  RDSqlQuery *q;

  //
  // Delete Carts
  //
  sql=QString("delete from CART where ARTIST=\"SAS_TORTURE\"");
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete Schedule
  //
  sql=QString("delete from RECORDINGS where CUT_NAME=\"SAS_TORTURE\"");
  q=new RDSqlQuery(sql);
  delete q;
}


void MainWidget::cancelData()
{
  qApp->quit();
}


void MainWidget::closeEvent(QCloseEvent *e)
{
  cancelData();
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);
  
  MainWidget *w=new MainWidget(NULL,"main");
  a.setMainWidget(w);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}


