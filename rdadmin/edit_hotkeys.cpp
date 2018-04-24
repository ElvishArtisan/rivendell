// edit_hot_keys.cpp
//
// Edit the Hot Key Configuration for a Rivendell Workstation.
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

#include <QMessageBox>
#include <QPushButton>

#include <rdescape_string.h>
#include <rdhotkeylist.h>

#include "edit_hotkeys.h"
#include "globals.h"

EditHotkeys::EditHotkeys(const QString &station,const QString &module,
			 QWidget *parent)
  : QDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *item=NULL;

  hotkey_conf=station;
  hotkey_module=module;
  station_hotkeys= new RDHotkeys(hotkey_conf,hotkey_module);
  myhotkeylist = new RDHotKeyList();

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

  //
  // Hot Key Configuration Label
  //
  setWindowTitle("RDAdmin - "+hotkey_module.upper()+" "+
		 tr("Hot Key Configuration for")+" "+hotkey_conf);

  QLabel *label=new QLabel(tr("Host Hot Key Configurations"),this);
  label->setFont(font);
  label->setGeometry(14,5,sizeHint().width()-28,19);
  list_widget=new RDTableWidget(this);
  list_widget->setGeometry(10,24,320,220);
  list_widget->setColumnCount(2);
  item=new QTableWidgetItem(tr("Button / Function"));
  item->setFlags(Qt::NoItemFlags);
  list_widget->setHorizontalHeaderItem(0,item);
  item=new QTableWidgetItem(tr("KeyStroke"));
  item->setFlags(Qt::NoItemFlags);
  list_widget->setHorizontalHeaderItem(1,item);

  //  Keystroke Value field
  keystroke = new QLineEdit(this);
  keystroke->setFocusPolicy(Qt::StrongFocus);
  keystroke->setGeometry(sizeHint().width()-270,sizeHint().height()-210,200,35);
  
  
  // Set Button
  //
  QPushButton *set_button = new QPushButton(this);
  set_button->setGeometry(sizeHint().width()-290,sizeHint().height()-160,60,30);
  set_button->setDefault(true);
  set_button->setFont(font);
  set_button->setText(tr("Set"));
  connect(set_button,SIGNAL(clicked()), this, SLOT(SetButtonClicked()) );
  
  // Clear Button
  //
  QPushButton *clear_button = new QPushButton(this);
  clear_button->
    setGeometry(sizeHint().width()-215,sizeHint().height()-160,60,30);
  clear_button->setDefault(true);
  clear_button->setFont(font);
  clear_button->setText(tr("Clear"));
  connect(clear_button,SIGNAL(clicked()), this, SLOT(clearCurrentItem()) );
  
  // Clear All Hot Keys Button
  //
  QPushButton *clear_all_button = new QPushButton(this);
  clear_all_button->
    setGeometry(sizeHint().width()-140,sizeHint().height()-160,130,30);
  clear_all_button->setDefault(true);
  clear_all_button->setFont(font);
  clear_all_button->setText(tr("Clear All Hotkeys"));
  connect(clear_all_button,SIGNAL(clicked()), this,SLOT(clearAll_Hotkeys()) );
  
  // Clone Host Drop Box
  //
  clone_from_host_box=new QComboBox(this);
  clone_from_host_box->
    setGeometry(sizeHint().width()-295,sizeHint().height()-110,130,30);
  clone_from_host_label=
    new QLabel(clone_from_host_box,tr("Set From Host:"),this);
  clone_from_host_label->setFont(font);
  clone_from_host_label->
    setGeometry(sizeHint().width()-420,sizeHint().height()-110,120,30);
  clone_from_host_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  sql=QString("select NAME from STATIONS");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    clone_from_host_box->insertItem(q->value(0).toString());
    if (hotkey_conf == q->value(0).toString()) {
      clone_from_host_box->setCurrentItem(clone_from_host_box->count()-1);
      current_station_clone_item = clone_from_host_box->count()-1;
    }
  }
  delete q;
  connect (clone_from_host_box,SIGNAL(activated(const QString&)),
	   this,SLOT(Clone_RefreshList(const QString&))); 
 
  //
  //  Save Button
  //
  QPushButton *save_button=new QPushButton(this);
  save_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  save_button->setDefault(true);
  save_button->setFont(font);
  save_button->setText(tr("Save"));
  connect(save_button,SIGNAL(clicked()),this,SLOT(save()));
  
  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("Cancel"));
  
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancel()));
  
  keystrokecount=0;
  AltKeyHit  = false;
  CtrlKeyHit = false;
  
  RefreshList();
}


EditHotkeys::~EditHotkeys()
{
}


QSize EditHotkeys::sizeHint() const
{
  return QSize(400,500);
} 


QSizePolicy EditHotkeys::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void EditHotkeys::save()
{
  QString sql;
  RDSqlQuery *q;

  for(int i=0;i<list_widget->rowCount();i++) {
    sql=QString("update RDHOTKEYS set ")+
      "KEY_VALUE=\""+
      RDEscapeString(list_widget->item(i,1)->data(Qt::DisplayRole).toString())+
      "\" where "+
      "(KEY_LABEL=\""+
      RDEscapeString(list_widget->item(i,0)->data(Qt::DisplayRole).toString())+
      "\")&&"+
      "(STATION_NAME=\""+RDEscapeString(hotkey_conf)+"\")&&"+
      "(MODULE_NAME=\""+RDEscapeString(hotkey_module)+"\")";
    q=new RDSqlQuery(sql);
    delete q;
  }

  done(0);
}

void EditHotkeys::SetHotKey()
{
  QItemSelectionModel *s=list_widget->selectionModel();
  if(!s->hasSelection()) {
    return;
  }
  list_widget->item(s->selectedRows()[0].row(),1)->
    setData(Qt::DisplayRole,hotkeystrokes);
  keyupdated = true;
}

void EditHotkeys::clearAll_Hotkeys()
{
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Hotkeys Clear"),
			  tr("Are you sure - This will Clear All Hot Key Settings!"),
			  QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

  for(int i=0;i<list_widget->rowCount();i++) {
    list_widget->item(i,1)->setData(Qt::DisplayRole,QString());
  }

  keystroke->setText("");
  keystroke->setFocus();
  hotkeystrokes=QString("");
  keystrokecount=0;
  keyupdated = true;
}
 


void EditHotkeys::cancel()
{
  if (keyupdated) {
    switch(QMessageBox::warning(this,tr("Hotkeys Updated"),
				"Are you sure - All Hot Keys changes will be Lost!",
				QMessageBox::Yes,QMessageBox::No)) {
    case QMessageBox::No:
    case QMessageBox::NoButton:
      return;
    default:
      break;
    }
  }
  close();
}

void EditHotkeys::keyPressEvent (QKeyEvent *e)
{
  if ( (e->key() == Qt::Key_Backspace) ||
       (e->key() == Qt::Key_Delete) )    {
    keystroke->setText("");
    keystroke->setFocus();
    hotkeystrokes=QString("");
    keystrokecount=0;
    return;
  }

  if (e->key() == Qt::Key_Alt)  {
    keystrokecount++;
    AltKeyHit = true;
  }

  if (e->key() == Qt::Key_Control)  {
    keystrokecount++;
    CtrlKeyHit = true;
  }
}

void EditHotkeys::keyReleaseEvent (QKeyEvent *e)
{
  int keyhit = e->key();
  QString mystring=(*myhotkeylist).GetKeyCode(keyhit);
  
  if (mystring.length() == 0 ) {        // should never happen unless shell got it...
    keystroke->setFocus();
    keystroke->setText("");
    hotkeystrokes=QString("");
    keystrokecount = 0;
    AltKeyHit = false;
    CtrlKeyHit = false;
    return;
  }

  if ( (e->key() == Qt::Key_Backspace) ||
       (e->key() == Qt::Key_Space) ||
       (e->key() == Qt::Key_Delete) )    {
    keystroke->setFocus();
    keystroke->setText("");
    hotkeystrokes=QString("");
    keystrokecount = 0;
    return;
  }

  if (e->key() == Qt::Key_Shift) {
    QWidget::keyReleaseEvent(e);
    return;
  }
  
  if ( (e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Left) ||
       (e->key() == Qt::Key_Right) || (e->key() == Qt::Key_Down) )    {
    QWidget::keyReleaseEvent(e);
    keystrokecount = 0;
    hotkeystrokes = QString ("");
    keystroke->setText("");
    return;
  }

  if ( (e->key() == Qt::Key_Tab)  || (e->key() == Qt::Key_Left) ||
       (e->key() == Qt::Key_Right) ){
    QWidget::keyReleaseEvent(e);
    keystrokecount = 0;
    hotkeystrokes = QString ("");
    keystroke->setText("");
    return;
  }

  if ((e->key() == Qt::Key_Alt) ||
      (e->key() == Qt::Key_Control)) {
    if (keystrokecount != 0 ) hotkeystrokes = QString ("");
    if (AltKeyHit) {
      AltKeyHit = false;
      if (keystrokecount > 0) keystrokecount--;
    }
    if (CtrlKeyHit) {
      CtrlKeyHit = false;
      if (keystrokecount > 0) keystrokecount--;
    }
    keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
    return;
  }
  if (keystrokecount > 2)  {
    keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
    return;
  }
  
  if (!e->isAutoRepeat()) {
    if (keystrokecount == 0)
      hotkeystrokes = QString ("");
    if (AltKeyHit) {
      hotkeystrokes =  (*myhotkeylist).GetKeyCode(Qt::Key_Alt);
      hotkeystrokes +=  QString(" + ");
    }
    if (CtrlKeyHit) {
      if (AltKeyHit) {
	hotkeystrokes +=  (*myhotkeylist).GetKeyCode(Qt::Key_Control);
	hotkeystrokes += QString (" + ");
      }
      else {
	hotkeystrokes =  (*myhotkeylist).GetKeyCode(Qt::Key_Control);
	hotkeystrokes += QString (" + ");
      }
    }

    hotkeystrokes += mystring;
    keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
    keystrokecount = 0;
    return;
  }
}

void EditHotkeys::RefreshList()
{
  station_hotkeys= new RDHotkeys(hotkey_conf,hotkey_module);

  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *item=NULL;

  list_widget->clear();
  keyupdated = false;

  //  Build Rows of List View  I do this in reverse...

  sql=QString("select ")+
    "KEY_LABEL,"+
    "KEY_VALUE "+
    "from RDHOTKEYS where "+
    "(STATION_NAME=\""+RDEscapeString(hotkey_conf)+"\")&&"+
    "(MODULE_NAME=\""+RDEscapeString(hotkey_module)+"\") "+
    "ORDER BY KEY_ID DESC";

  q=new RDSqlQuery(sql);
  list_widget->setRowCount(q->size());
  int row=0;
  while(q->next()) {
    item=new QTableWidgetItem(q->value(0).toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    list_widget->setItem(row,0,item);
    item=new QTableWidgetItem(q->value(1).toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    list_widget->setItem(row,1,item);
    row++;
  }
  delete q;
}

void EditHotkeys::Clone_RefreshList(const QString& clone_station)
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *item=NULL;

  QString tmp_hotkey_conf = QString().sprintf("%s",
					      (const char *)clone_station);
  RDHotkeys *tmp_station_hotkeys= new RDHotkeys(tmp_hotkey_conf,hotkey_module);
  keyupdated = true;
  list_widget->clear();

  sql=QString("select ")+
    "KEY_LABEL,"+
    "KEY_VALUE "+
    "from RDHOTKEYS where "+
    "(STATION_NAME=\""+RDEscapeString(tmp_hotkey_conf)+"\")&&"+
    "(MODULE_NAME=\""+RDEscapeString(hotkey_module)+"\") "+
    "ORDER BY ID DESC";

  q=new RDSqlQuery(sql);
  list_widget->setRowCount(q->size());
  int row=0;
  while(q->next()) {
    item=new QTableWidgetItem(q->value(0).toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    list_widget->setItem(row,0,item);
    item=new QTableWidgetItem(q->value(1).toString());
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    list_widget->setItem(row,1,item);
  }
  delete q;
  hotkeystrokes = QString ("");
  keystroke->setText(QString().sprintf("%s",(const char *)hotkeystrokes));
  delete tmp_station_hotkeys;
}

void EditHotkeys::showCurrentKey()
{
  QItemSelectionModel *s=list_widget->selectionModel();
  if(!s->hasSelection()) {
    return;
  }
  keystroke->setText(list_widget->item(s->selectedRows()[0].row(),1)->
		     data(Qt::DisplayRole).toString());
  keystroke->displayText();
  hotkeystrokes=list_widget->item(s->selectedRows()[0].row(),1)->
    data(Qt::DisplayRole).toString();
}


void EditHotkeys::clearCurrentItem()
{
  keystrokecount=0;
  keystroke->setText("");
  keystroke->setFocus();
  hotkeystrokes=QString("");
  return;
}

void EditHotkeys::SetButtonClicked()
{
  QItemSelectionModel *s=list_widget->selectionModel();
  if(!s->hasSelection()) {
    QMessageBox::warning(this,"RDAdmin - "+tr("No Items Selected"),
                         tr("Please Select an Item From the List"));
    return;
  }
  SetHotKey();
}

