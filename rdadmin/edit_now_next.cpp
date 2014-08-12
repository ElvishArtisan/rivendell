// edit_now_next.cpp
//
// Edit the Now & Next Configuration for a Rivendell Workstation.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_now_next.cpp,v 1.10.2.1 2012/11/26 20:19:38 cvs Exp $
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

#include <qmessagebox.h>
#include <qsignalmapper.h>

#include <rdescape_string.h>
#include <rdtextvalidator.h>
#include <rdlistviewitem.h>
#include <rdcart_dialog.h>

#include <edit_now_next.h>
#include <edit_nownextplugin.h>
#include <globals.h>


EditNowNext::EditNowNext(RDAirPlayConf *conf,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

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
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Edit Now & Next Data"));

  //
  // Text Validator
  //
  RDTextValidator *validator=new RDTextValidator(this,"validator");
  QIntValidator *int_validator=new QIntValidator(1,999999,this);

  //
  // Button Mappers
  //
  QSignalMapper *now_mapper=new QSignalMapper(this,"nowcart_mapper");
  connect(now_mapper,SIGNAL(mapped(int)),this,SLOT(editNowcartData(int)));
  QSignalMapper *next_mapper=new QSignalMapper(this,"nextcart_mapper");
  connect(next_mapper,SIGNAL(mapped(int)),this,SLOT(editNextcartData(int)));

  //
  // Master Log Label
  //
  QLabel *label=new QLabel(tr("Master Log"),this,"masterlog_label");
  label->setGeometry(10,7,100,19);
  label->setFont(section_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Master Log UDP Address
  //
  nownext_address_edit[0]=new QLineEdit(this,"nownext_address_edit[0]");
  nownext_address_edit[0]->setGeometry(135,33,120,19);
  label=new QLabel(nownext_address_edit[0],tr("IP Address:"),this,
		   "nownext_address_label");
  label->setGeometry(10,33,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Master Log UDP Port
  //
  nownext_port_spin[0]=new QSpinBox(this,"nownext_port_spin[0]");
  nownext_port_spin[0]->setGeometry(375,33,60,19);
  nownext_port_spin[0]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[0],tr("UDP Port:"),this,"nownext_port_label");
  label->setGeometry(270,33,100,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Master Log UDP String
  //
  nownext_string_edit[0]=new QLineEdit(this,"nownext_string_edit[0]");
  nownext_string_edit[0]->setGeometry(135,55,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[0],
			   tr("UDP String:"),this,
			   "nownext_string_label");
  label->setGeometry(10,55,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Master Log RML
  //
  nownext_rml_edit[0]=new QLineEdit(this,"nownext_rml_edit[0]");
  nownext_rml_edit[0]->setGeometry(135,77,sizeHint().width()-145,19);
  nownext_rml_edit[0]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[0],
			   tr("RML:"),this,
			   "nownext_rml_label");
  label->setGeometry(10,77,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Main Log Default Now Cart
  //
  nownext_nowcart_edit[0]=new QLineEdit(this,"nownext_nowcart_edit[0]");
  nownext_nowcart_edit[0]->setGeometry(135,104,60,19);
  nownext_nowcart_edit[0]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[0],tr("Default Now Cart:"),this,
		   "nownext_nowcart_label");
  label->setGeometry(10,104,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(205,101,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,0);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Main Log Default Next Cart
  //
  nownext_nextcart_edit[0]=new QLineEdit(this,"nownext_nextcart_edit[0]");
  nownext_nextcart_edit[0]->setGeometry(135,136,60,19);
  nownext_nextcart_edit[0]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[0],tr("Default Next Cart:"),this,
		   "nownext_nextcart_label");
  label->setGeometry(10,136,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this);
  button->setGeometry(205,132,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,0);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Aux Log 1 Label
  //
  label=new QLabel(tr("Aux Log 1"),this,"masterlog_label");
  label->setGeometry(10,175,100,19);
  label->setFont(section_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Aux Log 1 UDP Address
  //
  nownext_address_edit[1]=new QLineEdit(this,"nownext_address_edit[1]");
  nownext_address_edit[1]->setGeometry(135,201,120,19);
  label=new QLabel(nownext_address_edit[1],tr("IP Address:"),this,
		   "nownext_address_label");
  label->setGeometry(10,201,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 1 UDP Port
  //
  nownext_port_spin[1]=new QSpinBox(this,"nownext_port_spin[1]");
  nownext_port_spin[1]->setGeometry(375,201,60,19);
  nownext_port_spin[1]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[1],tr("UDP Port:"),
		   this,"nownext_port_label");
  label->setGeometry(270,201,100,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 1 UDP String
  //
  nownext_string_edit[1]=new QLineEdit(this,"nownext_string_edit[1]");
  nownext_string_edit[1]->setGeometry(135,223,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[1],tr("UDP String:"),this,
			   "nownext_string_label");
  label->setGeometry(10,223,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 1 RML
  //
  nownext_rml_edit[1]=new QLineEdit(this,"nownext_rml_edit[1]");
  nownext_rml_edit[1]->setGeometry(135,245,sizeHint().width()-145,19);
  nownext_rml_edit[1]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[1],
			   tr("RML:"),this,
			   "nownext_rml_label");
  label->setGeometry(10,245,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux 1 Log Default Now Cart
  //
  nownext_nowcart_edit[1]=new QLineEdit(this,"nownext_nowcart_edit[1]");
  nownext_nowcart_edit[1]->setGeometry(135,272,60,19);
  nownext_nowcart_edit[1]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[1],tr("Default Now Cart:"),this,
		   "nownext_nowcart_label");
  label->setGeometry(10,272,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this);
  button->setGeometry(205,269,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,1);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Aux 1 Log Default Next Cart
  //
  nownext_nextcart_edit[1]=new QLineEdit(this,"nownext_nextcart_edit[1]");
  nownext_nextcart_edit[1]->setGeometry(135,304,60,19);
  nownext_nextcart_edit[1]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[1],tr("Default Next Cart:"),this,
		   "nownext_nextcart_label");
  label->setGeometry(10,304,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this);
  button->setGeometry(205,300,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,1);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Aux Log 2 Label
  //
  label=new QLabel(tr("Aux Log 2"),this,"masterlog_label");
  label->setGeometry(10,343,100,19);
  label->setFont(section_font);
  label->setAlignment(AlignLeft|AlignVCenter);

  //
  // Aux Log 2 UDP Address
  //
  nownext_address_edit[2]=new QLineEdit(this,"nownext_address_edit[2]");
  nownext_address_edit[2]->setGeometry(135,369,120,19);
  label=new QLabel(nownext_address_edit[2],tr("IP Address:"),this,
		   "nownext_address_label");
  label->setGeometry(10,369,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 2 UDP Port
  //
  nownext_port_spin[2]=new QSpinBox(this,"nownext_port_spin[2]");
  nownext_port_spin[2]->setGeometry(375,369,60,19);
  nownext_port_spin[2]->setRange(0,65535);
  label=new QLabel(nownext_port_spin[2],tr("UDP Port:"),
		   this,"nownext_port_label");
  label->setGeometry(270,369,100,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 2 UDP String
  //
  nownext_string_edit[2]=new QLineEdit(this,"nownext_string_edit[2]");
  nownext_string_edit[2]->setGeometry(135,391,sizeHint().width()-145,19);
  label=new QLabel(nownext_string_edit[2],tr("UDP String:"),this,
		   "nownext_string_label");
  label->setGeometry(10,391,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux Log 2 RML
  //
  nownext_rml_edit[2]=new QLineEdit(this,"nownext_rml_edit[2]");
  nownext_rml_edit[2]->setGeometry(135,413,sizeHint().width()-145,19);
  nownext_rml_edit[2]->setValidator(validator);
  label=new QLabel(nownext_rml_edit[2],
			   tr("RML:"),this,
			   "nownext_rml_label");
  label->setGeometry(10,413,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Aux 1 Log Default Now Cart
  //
  nownext_nowcart_edit[2]=new QLineEdit(this,"nownext_nowcart_edit[2]");
  nownext_nowcart_edit[2]->setGeometry(135,440,60,19);
  nownext_nowcart_edit[2]->setValidator(int_validator);
  label=new QLabel(nownext_nowcart_edit[2],tr("Default Now Cart:"),this,
		   "nownext_nowcart_label");
  label->setGeometry(10,440,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this);
  button->setGeometry(205,437,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  now_mapper->setMapping(button,2);
  connect(button,SIGNAL(clicked()),now_mapper,SLOT(map()));

  //
  // Aux 1 Log Default Next Cart
  //
  nownext_nextcart_edit[2]=new QLineEdit(this,"nownext_nextcart_edit[2]");
  nownext_nextcart_edit[2]->setGeometry(135,472,60,19);
  nownext_nextcart_edit[2]->setValidator(int_validator);
  label=new QLabel(nownext_nextcart_edit[2],tr("Default Next Cart:"),this,
		   "nownext_nextcart_label");
  label->setGeometry(10,472,120,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this);
  button->setGeometry(205,469,50,26);
  button->setFont(normal_font);
  button->setText(tr("Select"));
  next_mapper->setMapping(button,2);
  connect(button,SIGNAL(clicked()),next_mapper,SLOT(map()));

  //
  // Plugin List
  //
  nownext_plugin_list=new RDListView(this);
  nownext_plugin_list->setGeometry(10,540,sizeHint().width()-20,120);
  nownext_plugin_list->setItemMargin(5);
  nownext_plugin_list->addColumn(tr("Path"));
  nownext_plugin_list->setColumnAlignment(0,AlignLeft|AlignVCenter);
  nownext_plugin_list->addColumn(tr("Argument"));
  nownext_plugin_list->setColumnAlignment(1,AlignLeft|AlignVCenter);
  nownext_plugin_list->setAllColumnsShowFocus(true);
  connect(nownext_plugin_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(pluginDoubleClickedData(QListViewItem *,const QPoint &,int)));

  label=new QLabel(nownext_plugin_list,
			   tr("Loadable Modules:"),this,
			   "nownext_plugins_label");
  label->setGeometry(10,518,sizeHint().width()-20,19);
  label->setFont(section_font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

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
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
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
  sql=QString().sprintf("select ID,PLUGIN_PATH,PLUGIN_ARG \
                           from NOWNEXT_PLUGINS		  \
                           where (STATION_NAME=\"%s\")&&  \
                           (LOG_MACHINE=0)",
			(const char *)nownext_conf->station());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(nownext_plugin_list);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
    item->setText(1,q->value(2).toString());
  }
  delete q;
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
  EditNowNextPlugin *d=new EditNowNextPlugin(&path,&arg,this);
  if(d->exec()==0) {
    RDListViewItem *item=new RDListViewItem(nownext_plugin_list);
    item->setId(-1);
    item->setText(0,path);
    item->setText(1,arg);
    nownext_plugin_list->ensureItemVisible(item);
  }
  delete d;
}


void EditNowNext::editPluginData()
{
  RDListViewItem *item=
    (RDListViewItem *)nownext_plugin_list->selectedItem();
  if(item==NULL) {
    return;
  }
  QString path=item->text(0);
  QString arg=item->text(1);
  EditNowNextPlugin *d=new EditNowNextPlugin(&path,&arg,this);
  if(d->exec()==0) {
    item->setText(0,path);
    item->setText(1,arg);
  }
  delete d;  
}


void EditNowNext::deletePluginData()
{
  RDListViewItem *item=(RDListViewItem *)nownext_plugin_list->selectedItem();
  if(item==NULL) {
    return;
  }
  delete item;
}


void EditNowNext::pluginDoubleClickedData(QListViewItem *item,const QPoint &pt,
					int col)
{
  editPluginData();
}


void EditNowNext::editNowcartData(int lognum)
{
  int cartnum=nownext_nowcart_edit[lognum]->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::All,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    nownext_nowcart_edit[lognum]->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditNowNext::editNextcartData(int lognum)
{
  int cartnum=nownext_nextcart_edit[lognum]->text().toInt();
  if(admin_cart_dialog->exec(&cartnum,RDCart::All,NULL,0,
			     admin_user->name(),admin_user->password())==0) {
    nownext_nextcart_edit[lognum]->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditNowNext::okData()
{
  QHostAddress addr[3];
  QString str1;
  QString str2;
  QString sql;
  RDSqlQuery *q;
  RDListViewItem *item;

  for(int i=0;i<3;i++) {
    if(nownext_address_edit[i]->text().isEmpty()) {
      nownext_address_edit[i]->setText("0.0.0.0");
    }
    if(!addr[i].setAddress(nownext_address_edit[i]->text())) {
      str1=QString(tr("The IP address"));
      str2=QString(tr("is invalid!"));
      QMessageBox::warning(this,tr("Invalid Address"),
			   QString().
			   sprintf("%s \"%s\" %s",(const char *)str1,
				   (const char *)nownext_address_edit[i]->
				   text(),
				   (const char *)str2));
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
    sql=QString().sprintf("delete from NOWNEXT_PLUGINS where \
                           (STATION_NAME=\"%s\")&&(LOG_MACHINE=%d)",
			  (const char *)RDEscapeString(nownext_conf->station()),
			  i);
    q=new RDSqlQuery(sql);
    delete q;
  }
  item=(RDListViewItem *)nownext_plugin_list->firstChild();
  while(item!=NULL) {
    sql=QString().sprintf("insert into NOWNEXT_PLUGINS set \
                           STATION_NAME=\"%s\",	   \
                           LOG_MACHINE=0,		   \
                           PLUGIN_PATH=\"%s\",	   \
                           PLUGIN_ARG=\"%s\"",
			  (const char *)
			  RDEscapeString(nownext_conf->station()),
			  (const char *)RDEscapeString(item->text(0)),
			  (const char *)RDEscapeString(item->text(1)));
    q=new RDSqlQuery(sql);
    delete q;
    item=(RDListViewItem *)item->nextSibling();
  }
  done(0);
}


void EditNowNext::cancelData()
{
  done(-1);
}
