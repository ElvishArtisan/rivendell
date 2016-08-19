// rdunittest.cpp
//
// Unit testing utility for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>

#include "rdunittest.h"

//
// Icons
//
#include "../icons/rivendell-22x22.xpm"

MainWidget::MainWidget(QWidget *parent)
  :QWidget(parent)
{
  new RDApplication(RDApplication::Gui,"rdunittest",RDUNITTEST_USAGE,true);

  main_pass_count=0;
  main_fail_count=0;

  //
  // Set Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Create And Set Icon
  //
  setIcon(rivendell_xpm);
  setWindowTitle("RDUnitTest");

  //
  // Output Display
  //
  main_edit=new QTextEdit(this);
  main_edit->setReadOnly(true);

  //
  // Start Tests
  //
  QTimer *timer=new QTimer(this);
  timer->setSingleShot(true);
  connect(timer,SIGNAL(timeout()),this,SLOT(runTests()));
  timer->start(1);
}


MainWidget::~MainWidget()
{
}


QSize MainWidget::sizeHint() const
{
  return QSize(800,600);
}


void MainWidget::runTests()
{
  SanityCheck();

  Append("<strong>Connected to database \""+rda->config()->mysqlDbname()+
	 "\".</strong><br>");

  //
  // Run Tests
  //
  RDUnitTestData unit_data;

  LowLevelDbTests(&unit_data);
  Append(&unit_data);

  MojibakeTests(&unit_data);
  Append(&unit_data);

  //
  // Show Statistics
  //
  Append("<br>\n");
  QString line=QString().sprintf("%u ",main_pass_count)+"tests passed and ";
  if(main_fail_count==0) {
    line+="0 tests failed.";
  }
  else {
    line+="<font color=\"#FF0000\"><strong>"+
      QString().sprintf("%u ",main_fail_count)+"tests failed.</strong></font>";
  }
  Append(line);  
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  main_edit->setGeometry(0,0,size().width(),size().height());
}


void MainWidget::SanityCheck()
{
  QString sql;
  QSqlQuery *q;

  if(rda->config()->mysqlDbname().isEmpty()) {
    QMessageBox::warning(this,tr("RDUnitTest - Error"),
			 tr("You must specify a valid DB name."));
    _exit(255);
  }

  sql="show tables";
  q=new QSqlQuery(sql);
  if(q->first()) {
    QMessageBox::warning(this,tr("RDUnitTest - Error"),
			 tr("Database")+" \""+
			 rda->config()->mysqlDbname()+"\" "+
			 tr("is not empty."));
    _exit(255);
  }
  delete q;
}


void MainWidget::Append(RDUnitTestData *data)
{
  QString line;

  Append("");
  Append("<strong>"+data->groupName()+"</strong>");
  for(int i=0;i<data->testQuantity();i++) {
    if(data->testResult(i)) {
      line=data->testName(i)+":";
    }
    else {
      line=data->testName(i)+":";
    }
    if(!data->testResultDescription(i).isEmpty()) {
      line+=" ["+data->testResultDescription(i)+"]";
    }
    if(data->testResult(i)) {
      line+=" <font color=\"#008800\">PASSED</font>";
      main_pass_count++;
    }
    else {
      line+=" <font color=\"#FF0000\"><strong>FAILED</strong></font>";
      main_fail_count++;
    }
    Append("  "+line);
  }
  Append("<br>\n");
}


void MainWidget::Append(const QString &str)
{
  main_edit->setText(main_edit->text()+str+"\n");
}


void MainWidget::Append(const char *str)
{
  Append(QString(str));
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  //
  // Start Event Loop
  //
  MainWidget *w=new MainWidget();
  w->show();
  return a.exec();
}
