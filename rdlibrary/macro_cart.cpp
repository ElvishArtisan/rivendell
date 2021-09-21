// macro_cart.cpp
//
// The macro cart editor for RDLibrary.
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

#include "cdripper.h"
#include "edit_macro.h"
#include "globals.h"
#include "macro_cart.h"
#include "record_cut.h"

MacroCart::MacroCart(RDCart *cart,QWidget *parent)
  : RDWidget(parent)
{
  rdcart_length=0;
  rdcart_cart=cart;
  rdcart_allow_modification=rda->user()->modifyCarts();

  setWindowTitle(QString::asprintf("%u",rdcart_cart->number())+" - "+
    rdcart_cart->title());

  //
  // Dialogs
  //
  rdcart_edit_macro_dialog=new EditMacro(this);

  //
  // Add Macro Button
  //
  rdcart_add_button=new QPushButton(this);
  rdcart_add_button->setGeometry(10,0,80,50);
  rdcart_add_button->setFont(buttonFont());
  rdcart_add_button->setText(tr("Add"));
  rdcart_add_button->setEnabled(false);
  connect(rdcart_add_button,SIGNAL(clicked()),this,SLOT(addMacroData()));

  //
  // Delete Macro Button
  //
  rdcart_delete_button=new QPushButton(this);
  rdcart_delete_button->setGeometry(10,60,80,50);
  rdcart_delete_button->setFont(buttonFont());
  rdcart_delete_button->setText(tr("Delete"));
  rdcart_delete_button->setEnabled(false);
  connect(rdcart_delete_button,SIGNAL(clicked()),this,SLOT(deleteMacroData()));

  //
  // Copy Macro Button
  //
  rdcart_copy_button=new QPushButton(this);
  rdcart_copy_button->setGeometry(10,120,80,50);
  rdcart_copy_button->setFont(buttonFont());
  rdcart_copy_button->setText(tr("Copy"));
  rdcart_copy_button->setEnabled(false);
  connect(rdcart_copy_button,SIGNAL(clicked()),this,SLOT(copyMacroData()));

  //
  // Paste Macro Button
  //
  paste_macro_button=new QPushButton(this);
  paste_macro_button->setGeometry(10,180,80,50);
  paste_macro_button->setFont(buttonFont());
  paste_macro_button->setText(tr("Paste"));
  paste_macro_button->setDisabled(true);
  paste_macro_button->setEnabled(false);
  connect(paste_macro_button,SIGNAL(clicked()),this,SLOT(pasteMacroData()));

  //
  // Cart Macro List
  //
  rdcart_events=new RDMacroEvent(rda->station()->address(),rda->ripc(),this);
  //  rdcart_events->load(rdcart_cart->macros());

  rdcart_macro_view=new RDTableView(this);
  rdcart_macro_view->setSelectionMode(QAbstractItemView::SingleSelection);
  rdcart_macro_view->setGeometry(100,0,430,sizeHint().height());
  rdcart_macro_model=new RDMacroCartModel(cart->number(),this);
  rdcart_macro_model->setFont(defaultFont());
  rdcart_macro_model->setPalette(palette());
  rdcart_macro_view->setModel(rdcart_macro_model);
  connect(rdcart_macro_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(rdcart_macro_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,
				    const QItemSelection &)));
  connect(rdcart_macro_model,SIGNAL(modelReset()),
	  rdcart_macro_view,SLOT(resizeColumnsToContents()));
  rdcart_macro_view->resizeColumnsToContents();

  //
  // Edit Macro Button
  //
  rdcart_edit_button=new QPushButton(this);
  rdcart_edit_button->setGeometry(550,0,80,50);
  rdcart_edit_button->setFont(buttonFont());
  rdcart_edit_button->setText(tr("Edit"));
  rdcart_edit_button->setEnabled(false);
  connect(rdcart_edit_button,SIGNAL(clicked()),this,SLOT(editMacroData()));

  //
  // Run Line Button
  //
  rdcart_runline_button=new QPushButton(this);
  rdcart_runline_button->setGeometry(550,120,80,50);
  rdcart_runline_button->setFont(buttonFont());
  rdcart_runline_button->setText(tr("Run\nLine"));
  rdcart_runline_button->setEnabled(false);
  connect(rdcart_runline_button,SIGNAL(clicked()),
	  this,SLOT(runLineMacroData()));

  //
  // Run Cart Button
  //
  rdcart_runcart_button=new QPushButton(this);
  rdcart_runcart_button->setGeometry(550,180,80,50);
  rdcart_runcart_button->setFont(buttonFont());
  rdcart_runcart_button->setText(tr("Run\nCart"));
  connect(rdcart_runcart_button,SIGNAL(clicked()),
	  this,SLOT(runCartMacroData()));
}


MacroCart::~MacroCart()
{
  delete rdcart_edit_macro_dialog;
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
  rdcart_macro_model->save();
}


void MacroCart::addMacroData()
{
  QString code;
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  if((rows.size()!=1)||(!rdcart_allow_modification)) {
    return;
  }

  if(rdcart_edit_macro_dialog->exec(&code,true)) {
    rdcart_macro_model->addLine(rows.first(),code);
    UpdateLength();
  }
}


void MacroCart::deleteMacroData()
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  if((rows.size()!=1)||(!rdcart_allow_modification)) {
    return;
  }
  rdcart_macro_model->removeLine(rows.first());
  UpdateLength();
}


void MacroCart::copyMacroData()
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  if((rows.size()!=1)||(!rdcart_allow_modification)) {
    return;
  }
  rdcart_clipboard=rdcart_macro_model->code(rows.first());
  paste_macro_button->setEnabled(rdcart_allow_modification);
}


void MacroCart::pasteMacroData()
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  if((rows.size()!=1)||(!rdcart_allow_modification)) {
    return;
  }
  QModelIndex row=
    rdcart_macro_model->addLine(rows.first(),rdcart_clipboard);
  if(row.isValid()) {
    rdcart_macro_view->selectRow(row.row());
  }
  UpdateLength();
}


void MacroCart::editMacroData()
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();
  if((rows.size()!=1)||(!rdcart_allow_modification)||
     rdcart_macro_model->isEndHandle(rows.first())) {
    return;
  }
  QString code=rdcart_macro_model->code(rows.first());
  if(rdcart_edit_macro_dialog->exec(&code,false)) {
    rdcart_macro_model->refresh(rows.first(),code);
    UpdateLength();
  }
}


void MacroCart::runLineMacroData()
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  rdcart_events->load(rdcart_macro_model->code(rows.first()));
  rdcart_events->exec();
}


void MacroCart::runCartMacroData()
{
  rdcart_events->load(rdcart_macro_model->allCode());
  rdcart_events->exec();
}


void MacroCart::selectionChangedData(const QItemSelection &before,
				     const QItemSelection &after)
{
  QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();

  rdcart_add_button->setEnabled((rows.size()==1)&&rdcart_allow_modification);
  if(!rdcart_clipboard.isNull()) {
    paste_macro_button->setEnabled((rows.size()==1)&&rdcart_allow_modification);
  }
  if((rows.size()>0)&&(!rdcart_macro_model->isEndHandle(rows.first()))) {
    rdcart_runline_button->setEnabled(rdcart_allow_modification);
    rdcart_delete_button->setEnabled(rdcart_allow_modification);
    rdcart_copy_button->setEnabled(rdcart_allow_modification);
    rdcart_edit_button->setEnabled(rdcart_allow_modification);
  }
  else {
    rdcart_runline_button->setEnabled(false);
    rdcart_delete_button->setEnabled(false);
    rdcart_copy_button->setEnabled(false);
    rdcart_edit_button->setEnabled(false);
  }
}


void MacroCart::doubleClickedData(const QModelIndex &index)
{
  if(rdcart_allow_modification) {
    QModelIndexList rows=rdcart_macro_view->selectionModel()->selectedRows();
    if((rows.size()==1)&&rdcart_macro_model->isEndHandle(rows.first())) {
      addMacroData();
    }
    else {
      editMacroData();
    }
  }
}


void MacroCart::UpdateLength()
{
  unsigned len=rdcart_macro_model->totalLength();

  if(len!=rdcart_length) {
    rdcart_length=len;
    emit lengthChanged(len);
  }
}
