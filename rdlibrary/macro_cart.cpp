// macro_cart.cpp
//
// The macro cart editor for RDLibrary.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: macro_cart.cpp,v 1.13.8.2 2013/12/23 21:51:44 cvs Exp $
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

#include <qbitmap.h>
#include <unistd.h>
#include <qdialog.h>
#include <qstring.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qsqldatabase.h>

#include <rd.h>
#include <rdaudio_exists.h>

#include <cdripper.h>
#include <macro_cart.h>
#include <record_cut.h>
#include <edit_macro.h>
#include <globals.h>


MacroCart::MacroCart(RDCart *cart,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  rdcart_length=0;
  rdcart_cart=cart;
  setCaption(QString().sprintf("%u",rdcart_cart->number())+" - "+
    rdcart_cart->title());
  rdcart_allow_modification=lib_user->modifyCarts();

  //
  // Generate Fonts
  //
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont line_edit_font=QFont("Helvetica",12,QFont::Normal);
  line_edit_font.setPixelSize(12);

  //
  // Add Macro Button
  //
  rdcart_add_button=new QPushButton(this,"add_macro_button");
  rdcart_add_button->setGeometry(10,0,80,50);
  rdcart_add_button->setFont(button_font);
  rdcart_add_button->setText(tr("Add"));
  connect(rdcart_add_button,SIGNAL(clicked()),this,SLOT(addMacroData()));

  //
  // Delete Macro Button
  //
  rdcart_delete_button=new QPushButton(this,"delete_macro_button");
  rdcart_delete_button->setGeometry(10,60,80,50);
  rdcart_delete_button->setFont(button_font);
  rdcart_delete_button->setText(tr("Delete"));
  connect(rdcart_delete_button,SIGNAL(clicked()),this,SLOT(deleteMacroData()));

  //
  // Copy Macro Button
  //
  rdcart_copy_button=new QPushButton(this,"copy_macro_button");
  rdcart_copy_button->setGeometry(10,120,80,50);
  rdcart_copy_button->setFont(button_font);
  rdcart_copy_button->setText(tr("Copy"));
  connect(rdcart_copy_button,SIGNAL(clicked()),this,SLOT(copyMacroData()));

  //
  // Paste Macro Button
  //
  paste_macro_button=new QPushButton(this,"paste_macro_button");
  paste_macro_button->setGeometry(10,180,80,50);
  paste_macro_button->setFont(button_font);
  paste_macro_button->setText(tr("Paste"));
  paste_macro_button->setDisabled(true);
  connect(paste_macro_button,SIGNAL(clicked()),this,SLOT(pasteMacroData()));

  //
  // Cart Macro List
  //
  rdcart_events=new RDMacroEvent(rdstation_conf->address(),rdripc,
				 this,"rdcart_events");
  rdcart_events->load(rdcart_cart->macros());

  rdcart_macro_list=new QListView(this,"rdcart_macro_list");
  rdcart_macro_list->setGeometry(100,0,430,sizeHint().height());
  rdcart_macro_list->setAllColumnsShowFocus(true);
  rdcart_macro_list->setItemMargin(5);
  rdcart_macro_list->setSorting(0);
  connect(rdcart_macro_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  rdcart_macro_list->addColumn(tr("LINE"));
  rdcart_macro_list->setColumnAlignment(0,Qt::AlignHCenter);

  rdcart_macro_list->addColumn(tr("COMMAND"));
  rdcart_macro_list->setColumnAlignment(1,Qt::AlignLeft);

  rdcart_macro_list_label=new QLabel(rdcart_macro_list,tr("Macros"),
				   this,"rdcartmacro_list_label");
  rdcart_macro_list_label->setGeometry(105,345,430,22);
  rdcart_macro_list_label->setFont(QFont("Helvetica",16,QFont::Bold));

  RefreshList();

  //
  // Edit Macro Button
  //
  rdcart_edit_button=new QPushButton(this,"edit_edit_button");
  rdcart_edit_button->setGeometry(550,0,80,50);
  rdcart_edit_button->setFont(button_font);
  rdcart_edit_button->setText(tr("Edit"));
  connect(rdcart_edit_button,SIGNAL(clicked()),this,SLOT(editMacroData()));

  //
  // Run Line Button
  //
  rdcart_runline_button=new QPushButton(this,"run_macro_button");
  rdcart_runline_button->setGeometry(550,120,80,50);
  rdcart_runline_button->setFont(button_font);
  rdcart_runline_button->setText(tr("Run\nLine"));
  connect(rdcart_runline_button,SIGNAL(clicked()),
	  this,SLOT(runLineMacroData()));

  //
  // Run Cart Button
  //
  rdcart_runcart_button=new QPushButton(this,"run_macro_button");
  rdcart_runcart_button->setGeometry(550,180,80,50);
  rdcart_runcart_button->setFont(button_font);
  rdcart_runcart_button->setText(tr("Run\nCart"));
  connect(rdcart_runcart_button,SIGNAL(clicked()),
	  this,SLOT(runCartMacroData()));

  //
  // Set Control Permissions
  //
  rdcart_add_button->setEnabled(rdcart_allow_modification);
  rdcart_delete_button->setEnabled(rdcart_allow_modification);
  rdcart_copy_button->setEnabled(rdcart_allow_modification);
  rdcart_edit_button->setEnabled(rdcart_allow_modification);
}


QSize MacroCart::sizeHint() const
{
  return QSize(640,290);
} 


QSizePolicy MacroCart::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


unsigned MacroCart::length()
{
  return rdcart_events->length();
}


void MacroCart::save()
{
  rdcart_cart->setMacros(rdcart_events->save());
}


void MacroCart::addMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();
  RDMacro cmd;
  unsigned line;

  if(item==NULL) {
    return;
  }
  if(item->text(0).isEmpty()) {
    line=rdcart_events->size();
  }
  else {
    line=item->text(0).toUInt()-1;
  }
  EditMacro *edit=new EditMacro(&cmd,true,this,"edit_macro");
  if(edit->exec()!=-1) {
    AddLine(line,&cmd);
    UpdateLength();
  }
  delete edit;
}


void MacroCart::deleteMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();

  if((item==NULL)||(item->text(0).isEmpty())) {
    return;
  }
  DeleteLine(item);
  UpdateLength();
}


void MacroCart::copyMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();

  if((item==NULL)||(item->text(0).isEmpty())) {
    return;
  }
  rdcart_clipboard=*rdcart_events->command(item->text(0).toUInt()-1);
  paste_macro_button->setEnabled(true);
}


void MacroCart::pasteMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();
  unsigned line;

  if(item==NULL) {
    return;
  }
  if(item->text(0).isEmpty()) {
    line=rdcart_events->size();
  }
  else {
    line=item->text(0).toUInt()-1;
  }
  AddLine(line,&rdcart_clipboard);
  UpdateLength();
}


void MacroCart::editMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();

  if((item==NULL)||(item->text(0).isEmpty())) {
    return;
  }
  unsigned line=item->text(0).toUInt()-1;
  EditMacro *edit=new EditMacro(rdcart_events->command(line),false,this,"edit");
  if(edit->exec()!=-1) {
    RefreshLine(item);
    UpdateLength();
  }
  delete edit;
}


void MacroCart::runLineMacroData()
{
  QListViewItem *item=rdcart_macro_list->selectedItem();

  if((item==NULL)||(item->text(0).isEmpty())) {
    return;
  }
  unsigned line=item->text(0).toUInt()-1;
  rdcart_events->exec(line);
}


void MacroCart::runCartMacroData()
{
  rdcart_events->exec();
}


void MacroCart::doubleClickedData(QListViewItem *,const QPoint &,int)
{
  if(rdcart_allow_modification) {
    QListViewItem *item=rdcart_macro_list->selectedItem();

    if((item==NULL)||(item->text(0).isEmpty())) {
      addMacroData();
    }
    else {
      editMacroData();
    }
  }
}


void MacroCart::RefreshList()
{
  QListViewItem *item;
  char cmdstr[RD_RML_MAX_LENGTH];

  item=new QListViewItem(rdcart_macro_list);
  item->setText(1,tr("--- End of cart ---"));
  for(int i=0;i<rdcart_events->size();i++) {
    item=new QListViewItem(rdcart_macro_list);
    rdcart_events->command(i)->generateString(cmdstr,RD_RML_MAX_LENGTH);
    item->setText(0,QString().sprintf("%03d",i+1));
    item->setText(1,cmdstr);
  }
}


void MacroCart::RefreshLine(QListViewItem *item)
{
  char cmdstr[RD_RML_MAX_LENGTH];
  int line=item->text(0).toInt()-1;
  rdcart_events->command(line)->generateString(cmdstr,RD_RML_MAX_LENGTH);
  item->setText(1,cmdstr);
}


void MacroCart::AddLine(unsigned line,RDMacro *cmd)
{
  unsigned curr_line;

  QListViewItem *item=rdcart_macro_list->firstChild();
  for(int i=0;i<rdcart_macro_list->childCount();i++) {
    if(((curr_line=(item->text(0).toUInt()-1))>=line)&&
      (!item->text(0).isEmpty())) {
      item->setText(0,QString().sprintf("%03u",curr_line+2));
    }
    item=item->nextSibling();
  }
  rdcart_events->insert(line,cmd);
  item=new QListViewItem(rdcart_macro_list);
  item->setText(0,QString().sprintf("%03u",line+1));
  RefreshLine(item);
  rdcart_macro_list->setSelected(item,true);
}


void MacroCart::DeleteLine(QListViewItem *item)
{
  unsigned line=item->text(0).toUInt()-1;
  unsigned curr_line;
  QListViewItem *next=item->nextSibling();
  rdcart_macro_list->removeItem(item);
  rdcart_events->remove(line);
  QListViewItem *l=rdcart_macro_list->firstChild();
  for(int i=0;i<rdcart_macro_list->childCount();i++) {
    if(((curr_line=(l->text(0).toUInt()-1))>line)&&
      (!l->text(0).isEmpty())) {
      l->setText(0,QString().sprintf("%03u",curr_line));
    }
    l=l->nextSibling();
  }
  if(next!=NULL) {
    rdcart_macro_list->setSelected(next,true);
  }
}


void MacroCart::UpdateLength()
{
  if(rdcart_events->length()!=rdcart_length) {
    rdcart_length=rdcart_events->length();
    emit lengthChanged(rdcart_length);
  }
}
