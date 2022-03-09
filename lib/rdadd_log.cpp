// rdadd_log.cpp
//
// Create a Rivendell Log
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include "rdapplication.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdidvalidator.h"
#include "rdadd_log.h"

RDAddLog::RDAddLog(RDLogFilter::FilterMode mode,const QString &caption,
		   QWidget *parent)
  : RDDialog(parent)
{
  QStringList services_list;
  log_name=NULL;
  log_svc=NULL;
  log_filter_mode=mode;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMinimumSize(sizeHint());

  setWindowTitle(caption);

  //
  // Validator
  //
  RDIdValidator *v=new RDIdValidator(this);
  v->addBannedChar(' ');

  //
  // Log Name
  //
  add_name_edit=new QLineEdit(this);
  add_name_edit->setGeometry(115,11,sizeHint().width()-125,19);
  add_name_edit->setMaxLength(64);
  add_name_edit->setValidator(v);
  QLabel *label=new QLabel(tr("New Log Name")+":",this);
  label->setGeometry(10,13,100,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);
  connect(add_name_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(nameChangedData(const QString &)));

  //
  // Service selector
  //
  add_service_box=new QComboBox(this);
  add_service_box->setGeometry(115,33,100,19);
  label=new QLabel(tr("Service")+":",this);
  label->setGeometry(10,33,100,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight);

  //
  //  Ok Button
  //
  add_ok_button=new QPushButton(this);
  add_ok_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  add_ok_button->setDefault(true);
  add_ok_button->setFont(buttonFont());
  add_ok_button->setText(tr("OK"));
  add_ok_button->setDisabled(true);
  connect(add_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  add_cancel_button=new QPushButton(this);
  add_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  add_cancel_button->setFont(buttonFont());
  add_cancel_button->setText(tr("Cancel"));
  connect(add_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDAddLog::~RDAddLog()
{
  delete add_name_edit;
}


QSize RDAddLog::sizeHint() const
{
  return QSize(400,132);
} 


QSizePolicy RDAddLog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDAddLog::exec(QString *logname,QString *svcname)
{
  QString sql;
  RDSqlQuery *q;
  log_name=logname;
  log_svc=svcname;

  switch(log_filter_mode) {
  case RDLogFilter::NoFilter:
    sql=QString("select `NAME` from `SERVICES` order by `NAME`");
    break;

  case RDLogFilter::UserFilter:
    sql=QString("select `SERVICE_NAME` from `USER_SERVICE_PERMS` where ")+
      "`USER_NAME`='"+RDEscapeString(rda->user()->name())+"' "+
      "order by `SERVICE_NAME`";
    break;

  case RDLogFilter::StationFilter:
    sql=QString("select `SERVICE_NAME` from `SERVICE_PERMS` where ")+
      "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"' "+
      "order by `SERVICE_NAME`";
    break;
  }
  q=new RDSqlQuery(sql);
  add_service_box->clear();
  while(q->next()) {
    add_service_box->
      insertItem(add_service_box->count(),q->value(0).toString());
    if(q->value(0).toString()==*log_svc) {
      add_service_box->setCurrentIndex(add_service_box->count()-1);
    }
  }
  add_name_edit->setText(*logname);
  return QDialog::exec();
}


void RDAddLog::okData()
{
  if(add_service_box->currentText().isEmpty()){
    QMessageBox::warning(this,tr("RDLogEdit"),tr("The service is invalid!"));
    return;
  }

  *log_name=add_name_edit->text().trimmed();
  *log_svc=add_service_box->currentText();
  done(0);
}
  

void RDAddLog::cancelData()
{
  done(-1);
}


void RDAddLog::nameChangedData(const QString &str)
{
  add_ok_button->setDisabled(str.isEmpty());
}


void RDAddLog::closeEvent(QCloseEvent *e)
{
  cancelData();
}
