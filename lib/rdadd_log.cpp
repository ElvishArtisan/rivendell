// rdadd_log.cpp
//
// Create a Rivendell Log
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>
#include <qsqldatabase.h>
//Added by qt3to4:
#include <QCloseEvent>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdidvalidator.h>
#include <rdadd_log.h>


RDAddLog::RDAddLog(QString *logname,QString *svcname,RDStation *station,
		   QString caption,QWidget *parent,RDUser *rduser)
  : QDialog(parent,"",true)
{
  QStringList services_list;
  log_name=logname;
  log_svc=svcname;
  log_station=station;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Create Log"));

  //
  // Create Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

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
  QLabel *label=new QLabel(add_name_edit,tr("&New Log Name:"),this);
  label->setGeometry(10,13,100,19);
  label->setFont(button_font);
  label->setAlignment(Qt::AlignRight|Qt::TextShowMnemonic);
  connect(add_name_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(nameChangedData(const QString &)));

  //
  // Service selector
  //
  add_service_box=new QComboBox(this);
  add_service_box->setGeometry(115,33,100,19);
  label=new QLabel(add_name_edit,tr("&Service:"),this);
  label->setGeometry(10,33,100,19);
  label->setFont(button_font);
  label->setAlignment(Qt::AlignRight|Qt::TextShowMnemonic);

  //
  //  Ok Button
  //
  add_ok_button=new QPushButton(this);
  add_ok_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  add_ok_button->setDefault(true);
  add_ok_button->setFont(button_font);
  add_ok_button->setText(tr("&OK"));
  connect(add_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  add_cancel_button=new QPushButton(this);
  add_cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  add_cancel_button->setFont(button_font);
  add_cancel_button->setText(tr("&Cancel"));
  connect(add_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  if (rduser != 0) { // RDStation::UserSec
    services_list = rduser->services();
  } else { // RDStation::HostSec
    QString sql;
    if(station==NULL) {
      sql="select NAME from SERVICES order by NAME";
    }
    else {
      sql=QString("select ")+
	"SERVICE_NAME "+
	"from SERVICE_PERMS where "+
	"STATION_NAME=\""+RDEscapeString(station->name())+"\" "+
	"order by SERVICE_NAME";
    }
    RDSqlQuery *q=new RDSqlQuery(sql);
    while(q->next()) {
      services_list.append( q->value(0).toString() );
    }
    delete q;
  }

  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end();
        ++it ) {
    add_service_box->insertItem(*it);
    if(*svcname==*it) {
      add_service_box->setCurrentItem(add_service_box->count()-1);
    }
  }
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


void RDAddLog::okData()
{
  if(add_service_box->currentText().isEmpty()){
    QMessageBox::warning(this,tr("RDLogEdit"),tr("The service is invalid!"));
    return;
  }

  *log_name=add_name_edit->text().stripWhiteSpace();
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
