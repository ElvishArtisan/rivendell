// edit_now_next.cpp
//
// Edit the Now & Next Configuration for a Rivendell Workstation.
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

#include <QLabel>
#include <QMessageBox>
#include <QSignalMapper>

#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rdescape_string.h>
#include <rdlistviewitem.h>
#include <rdtextvalidator.h>

#include "edit_now_next.h"
#include "edit_nownextplugin.h"
#include "globals.h"

EditNowNext::EditNowNext(RDAirPlayConf *conf,QWidget *parent)
  : QDialog(parent)
{
  nownext_conf=conf;

  //
  // Create Fonts
  //
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont section_font=QFont("Helvetica",14,QFont::Bold);
  section_font.setPixelSize(14);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Edit Now & Next Data"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this);
  QIntValidator *int_validator=new QIntValidator(1,999999,this);

  //
  // Button Mappers
  //
  QSignalMapper *now_mapper=new QSignalMapper(this);
  connect(now_mapper,SIGNAL(mapped(int)),this,SLOT(editNowcartData(int)));
  QSignalMapper *next_mapper=new QSignalMapper(this);
  connect(next_mapper,SIGNAL(mapped(int)),this,SLOT(editNextcartData(int)));

  //
  // Master Log Label
  //
  QLabel *label=new QLabel(tr("Master Log"),this);
  label->setGeometry(10,7,100,19);
  label->setFont(section_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Master Log UDP Address
  //
  nownext_address_edit[0]=new QLineEdit(this);
  nownext_address_edit[0]->setGeometry(135,33,120,19);
  label=new QLabel(nownext_address_edit[0],tr("IP Address:"),this);
  label->setGeometry(10,33,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Master Log UDP Port
  //
  nownext_port_spin[0]=new QSpinBox(this);
  nownext_port_spin[0]->setGeometry(375,33,60,19);
  nownext_port_spin[0]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[0],tr("UDP Port:"),this);
  label->setGeometry(270,33,100,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Master Log UDP String
  //
  nownext_string_edit[0]=new QLineEdit(this);
  nownext_string_edit[0]->setGeometry(135,55,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[0],tr("UDP String:"),this);
  label->setGeometry(10,55,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Master Log RML
  //
  nownext_rml_edit[0]=new QLineEdit(this);
  nownext_rml_edit[0]->setGeometry(135,77,sizeHint().width()-145,19);
  nownext_rml_edit[0]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[0],tr("RML:"),this);
  label->setGeometry(10,77,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Main Log Default Now Cart
  //
  nownext_nowcart_edit[0]=new QLineEdit(this);
  nownext_nowcart_edit[0]->setGeometry(135,104,60,19);
  nownext_nowcart_edit[0]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[0],tr("Default Now Cart:"),this);
  label->setGeometry(10,104,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(205,101,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,0);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Main Log Default Next Cart
  //
  nownext_nextcart_edit[0]=new QLineEdit(this);
  nownext_nextcart_edit[0]->setGeometry(135,136,60,19);
  nownext_nextcart_edit[0]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[0],tr("Default Next Cart:"),this);
  label->setGeometry(10,136,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  button=new QPushButton(this);
  button->setGeometry(205,132,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,0);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Aux Log 1 Label
  //
  label=new QLabel(tr("Aux Log 1"),this);
  label->setGeometry(10,175,100,19);
  label->setFont(section_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Aux Log 1 UDP Address
  //
  nownext_address_edit[1]=new QLineEdit(this);
  nownext_address_edit[1]->setGeometry(135,201,120,19);
  label=new QLabel(nownext_address_edit[1],tr("IP Address:"),this);
  label->setGeometry(10,201,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 1 UDP Port
  //
  nownext_port_spin[1]=new QSpinBox(this);
  nownext_port_spin[1]->setGeometry(375,201,60,19);
  nownext_port_spin[1]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[1],tr("UDP Port:"),this);
  label->setGeometry(270,201,100,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 1 UDP String
  //
  nownext_string_edit[1]=new QLineEdit(this);
  nownext_string_edit[1]->setGeometry(135,223,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[1],tr("UDP String:"),this);
  label->setGeometry(10,223,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 1 RML
  //
  nownext_rml_edit[1]=new QLineEdit(this);
  nownext_rml_edit[1]->setGeometry(135,245,sizeHint().width()-145,19);
  nownext_rml_edit[1]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[1],tr("RML:"),this);
  label->setGeometry(10,245,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux 1 Log Default Now Cart
  //
  nownext_nowcart_edit[1]=new QLineEdit(this);
  nownext_nowcart_edit[1]->setGeometry(135,272,60,19);
  nownext_nowcart_edit[1]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[1],tr("Default Now Cart:"),this);
  label->setGeometry(10,272,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  button=new QPushButton(this);
  button->setGeometry(205,269,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,1);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Aux 1 Log Default Next Cart
  //
  nownext_nextcart_edit[1]=new QLineEdit(this);
  nownext_nextcart_edit[1]->setGeometry(135,304,60,19);
  nownext_nextcart_edit[1]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[1],tr("Default Next Cart:"),this);
  label->setGeometry(10,304,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  button=new QPushButton(this);
  button->setGeometry(205,300,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,1);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Aux Log 2 Label
  //
  label=new QLabel(tr("Aux Log 2"),this);
  label->setGeometry(10,343,100,19);
  label->setFont(section_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Aux Log 2 UDP Address
  //
  nownext_address_edit[2]=new QLineEdit(this);
  nownext_address_edit[2]->setGeometry(135,369,120,19);
  label=new QLabel(nownext_address_edit[2],tr("IP Address:"),this);
  label->setGeometry(10,369,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 2 UDP Port
  //
  nownext_port_spin[2]=new QSpinBox(this);
  nownext_port_spin[2]->setGeometry(375,369,60,19);
  nownext_port_spin[2]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[2],tr("UDP Port:"),this);
  label->setGeometry(270,369,100,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 2 UDP String
  //
  nownext_string_edit[2]=new QLineEdit(this);
  nownext_string_edit[2]->setGeometry(135,391,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[2],tr("UDP String:"),this);
  label->setGeometry(10,391,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux Log 2 RML
  //
  nownext_rml_edit[2]=new QLineEdit(this);
  nownext_rml_edit[2]->setGeometry(135,413,sizeHint().width()-145,19);
  nownext_rml_edit[2]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[2],tr("RML:"),this);
  label->setGeometry(10,413,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);

  //
  // Aux 1 Log Default Now Cart
  //
  nownext_nowcart_edit[2]=new QLineEdit(this);
  nownext_nowcart_edit[2]->setGeometry(135,440,60,19);
  nownext_nowcart_edit[2]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[2],tr("Default Now Cart:"),this);
  label->setGeometry(10,440,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  button=new QPushButton(this);
  button->setGeometry(205,437,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,2);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Aux 1 Log Default Next Cart
  //
  nownext_nextcart_edit[2]=new QLineEdit(this);
  nownext_nextcart_edit[2]->setGeometry(135,472,60,19);
  nownext_nextcart_edit[2]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[2],tr("Default Next Cart:"),this);
  label->setGeometry(10,472,120,19);
  label->setFont(font);
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter|Qt::TextShowMnemonic);
  button=new QPushButton(this);
  button->setGeometry(205,469,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,2);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Plugin List
  //
  nownext_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "PLUGIN_PATH,"+
    "PLUGIN_ARG "+
    "from NOWNEXT_PLUGINS where "+
    "(STATION_NAME=\""+RDEscapeString(nownext_conf->station())+"\")&&"+
    "(LOG_MACHINE=0)";
  nownext_model->setQuery(sql);
  nownext_model->setHeaderData(1,Qt::Horizontal,tr("Path"));
  nownext_model->setHeaderData(2,Qt::Horizontal,tr("Argument"));
  nownext_view=new RDTableView(this);
  nownext_view->setGeometry(10,540,sizeHint().width()-20,120);
  nownext_view->setModel(nownext_model);
  nownext_view->hideColumn(0);
  nownext_view->resizeColumnsToContents();
  connect(nownext_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(pluginDoubleClickedData(const QModelIndex &)));
  label=new QLabel(tr("Loadable Modules:"),this);
  label->setGeometry(10,518,sizeHint().width()-20,19);
  label->setFont(section_font);
  label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextShowMnemonic);

  nownext_add_button=new QPushButton(tr("Add"),this);
  nownext_add_button->setGeometry(sizeHint().width()-210,665,60,25);
  nownext_add_button->setFont(font);
  connect(nownext_add_button,SIGNAL(clicked()),this,SLOT(addPluginData()));

  nownext_edit_button=new QPushButton(tr("Edit"),this);
  nownext_edit_button->setGeometry(sizeHint().width()-140,665,60,25);
  nownext_edit_button->setFont(font);
  connect(nownext_edit_button,SIGNAL(clicked()),this,SLOT(editPluginData()));

  nownext_delete_button=new QPushButton(tr("Delete"),this);
  nownext_delete_button->setGeometry(sizeHint().width()-70,665,60,25);
  nownext_delete_button->setFont(font);
  connect(nownext_delete_button,SIGNAL(clicked()),
	  this,SLOT(deletePluginData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  for(int i=0;i<3;i++) {
    nownext_address_edit[i]->setText(nownext_conf->udpAddress(i).toString());
    nownext_port_spin[i]->setValue(nownext_conf->udpPort(i));
    nownext_string_edit[i]->setText(nownext_conf->udpString(i));
    nownext_rml_edit[i]->setText(nownext_conf->logRml(i));
    if(nownext_conf->logNowCart(i)>0) {
      nownext_nowcart_edit[i]->
	setText(QString().sprintf("%06u",nownext_conf->logNowCart(i)));
    }
    if(nownext_conf->logNextCart(i)>0) {
      nownext_nextcart_edit[i]->
	setText(QString().sprintf("%06u",nownext_conf->logNextCart(i)));
    }
  }
}


EditNowNext::~EditNowNext()
{
}


QSize EditNowNext::sizeHint() const
{
  return QSize(445,770);
} 


QSizePolicy EditNowNext::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditNowNext::addPluginData()
{
  QString path;
  QString arg;
  QString sql;
  RDSqlQuery *q;

  EditNowNextPlugin *d=new EditNowNextPlugin(&path,&arg,this);
  if(d->exec()==0) {
    nownext_model->update();
    sql=QString("insert into NOWNEXT_PLUGINS set ")+
      "STATION_NAME=\""+RDEscapeString(nownext_conf->station())+"\","+
      "PLUGIN_PATH=\""+RDEscapeString(path)+"\","+
      "PLUGIN_ARG=\""+RDEscapeString(arg)+"\"";
    q=new RDSqlQuery(sql);
    nownext_model->update();
    nownext_view->select(0,q->lastInsertId().toInt());
  }
  delete d;
}


void EditNowNext::editPluginData()
{
  QItemSelectionModel *s=nownext_view->selectionModel();
  if(s->hasSelection()) {
    pluginDoubleClickedData(s->selectedRows()[0]);
  }
}


void EditNowNext::deletePluginData()
{
  QString sql;
  RDSqlQuery *q;

  QItemSelectionModel *s=nownext_view->selectionModel();
  if(s->hasSelection()) {
    sql=QString("delete from NOWNEXT_PLUGINS where ")+
      QString().sprintf("ID=%d",s->selectedRows()[0].data().toInt());
    q=new RDSqlQuery(sql);
    delete q;
    nownext_model->update();
  }
}


void EditNowNext::pluginDoubleClickedData(const QModelIndex &index)
{
  QString path=nownext_model->data(index.row(),1).toString();
  QString arg=nownext_model->data(index.row(),2).toString();
  EditNowNextPlugin *d=new EditNowNextPlugin(&path,&arg,this);
  if(d->exec()==0) {
    QString sql=QString("update NOWNEXT_PLUGINS set ")+
      "PLUGIN_PATH=\""+RDEscapeString(path)+"\","+
      "PLUGIN_ARG=\""+RDEscapeString(arg)+"\" "+
      "where "+
      QString().sprintf("ID=%d",nownext_model->data(index.row(),0).
			toInt());
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
    nownext_model->update();
  }
}


void EditNowNext::editNowcartData(int lognum)
{
  int cartnum=nownext_nowcart_edit[lognum]->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::All,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    nownext_nowcart_edit[lognum]->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditNowNext::editNextcartData(int lognum)
{
  int cartnum=nownext_nextcart_edit[lognum]->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::All,NULL,0,
			     rda->user()->name(),rda->user()->password())==0) {
    nownext_nextcart_edit[lognum]->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditNowNext::okData()
{
  QHostAddress addr[3];
  QString str1;
  QString str2;

  for(int i=0;i<3;i++) {
    if(nownext_address_edit[i]->text().isEmpty()) {
      nownext_address_edit[i]->setText("0.0.0.0");
    }
    if(!addr[i].setAddress(nownext_address_edit[i]->text())) {
      str1=QString(tr("The IP address"));
      str2=QString(tr("is invalid!"));
      QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			   tr("The IP address")+
			   " \""+nownext_address_edit[i]->text()+"\" "+
			   tr("is invalid."));
      return;
    }
  }
  for(int i=0;i<3;i++) {
    nownext_conf->setUdpAddress(i,addr[i]);
    nownext_conf->setUdpPort(i,(Q_UINT16)nownext_port_spin[i]->value());
    nownext_conf->setUdpString(i,nownext_string_edit[i]->text());
    nownext_conf->setLogRml(i,nownext_rml_edit[i]->text());
    if(nownext_nowcart_edit[i]->text().isEmpty()) {
      nownext_conf->setLogNowCart(i,0);
    }
    else {
      nownext_conf->setLogNowCart(i,nownext_nowcart_edit[i]->text().toUInt());
    }
    if(nownext_nextcart_edit[i]->text().isEmpty()) {
      nownext_conf->setLogNextCart(i,0);
    }
    else {
      nownext_conf->setLogNextCart(i,nownext_nextcart_edit[i]->text().toUInt());
    }
  }
  done(0);
}


void EditNowNext::cancelData()
{
  done(-1);
}
