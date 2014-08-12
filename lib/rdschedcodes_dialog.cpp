// rdschedcode_dialog.cpp
//
// Scheduler code editor dialog
//
//  (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//   Based on original code by Stefan Gabriel <stg@st-gabriel.de>
//
//     $Id: rdschedcodes_dialog.cpp,v 1.1.2.1 2014/05/28 21:21:40 cvs Exp $
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

#include <qpainter.h>
#include <qmessagebox.h>

#include <rddb.h>
#include "rdschedcodes_dialog.h"

RDSchedCodesDialog::RDSchedCodesDialog(QWidget *parent)
  : QDialog(parent,"",true)
{
  setCaption(tr("Select Scheduler Codes"));

  //
  // Create Fonts
  //
  QFont font("Helvetica",11,QFont::Bold);
  font.setPixelSize(11);

  QFont listfont("Helvetica",11);
  font.setPixelSize(11);


  //
  // Services Selector
  //
  codes_sel=new RDListSelector(this);
  codes_sel->setFont(listfont);
  codes_sel->sourceSetLabel(tr("Available Codes"));

  remove_codes_sel=new RDListSelector(this);
  remove_codes_sel->setFont(listfont);
  remove_codes_sel->sourceSetLabel(tr("Available Codes"));
  remove_codes_sel->destSetLabel(tr("REMOVE from Carts"));
  
  //
  //  Ok Button
  //
  edit_ok_button=new QPushButton(tr("&OK"),this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(font);
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(tr("&Cancel"),this);
  edit_cancel_button->setFont(font);
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


RDSchedCodesDialog::~RDSchedCodesDialog()
{
}


QSize RDSchedCodesDialog::sizeHint() const
{
  return QSize(400,292);
} 


QSizePolicy RDSchedCodesDialog::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int RDSchedCodesDialog::exec(QStringList *sched_codes,QStringList *remove_codes)
{
  QString sql;
  RDSqlQuery *q;

  edit_sched_codes=sched_codes;
  edit_remove_codes=remove_codes;

  //
  // Fix the Window Size
  //
  if(edit_remove_codes==NULL) {
    setMinimumWidth(sizeHint().width());
    setMaximumWidth(sizeHint().width());
    }
  else {
    setMinimumWidth(2*sizeHint().width());
    setMaximumWidth(2*sizeHint().width());
    }
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  codes_sel->clear();
  remove_codes_sel->clear();
  if(edit_remove_codes==NULL) {
    codes_sel->destSetLabel(tr("Assigned Codes"));
    codes_sel->show();
    remove_codes_sel->hide();
  }
  else {
    codes_sel->destSetLabel(tr("ASSIGN to Carts"));
    codes_sel->hide();
    remove_codes_sel->show();
  }

  for(unsigned i=0;i<edit_sched_codes->size();i++) {
    codes_sel->destInsertItem((*edit_sched_codes)[i]);
  } 
  if(edit_remove_codes!=NULL) {
    for(unsigned i=0;i<edit_remove_codes->size();i++) {
      remove_codes_sel->destInsertItem((*remove_codes)[i]);
    } 
  }
       
  sql=QString().sprintf("select CODE from SCHED_CODES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(codes_sel->destFindItem(q->value(0).toString())==0) {
      codes_sel->sourceInsertItem(q->value(0).toString());
    }
    if(edit_remove_codes!=NULL) {
      if(remove_codes_sel->destFindItem(q->value(0).toString())==0) {
        remove_codes_sel->sourceInsertItem(q->value(0).toString());
      }
    }
  }
  delete q;

  return QDialog::exec();
}


void RDSchedCodesDialog::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->moveTo(sizeHint().width(),10);
  p->lineTo(sizeHint().width(),210);
  p->end();
}


void RDSchedCodesDialog::resizeEvent(QResizeEvent *e)
{
  codes_sel->setGeometry(10,10,380,200);
  remove_codes_sel->setGeometry(sizeHint().width()+10,10,380,200);
  edit_ok_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RDSchedCodesDialog::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void RDSchedCodesDialog::okData()
{
  edit_sched_codes->clear();

  //
  // Add New Objects
  //
  for(unsigned i=0;i<codes_sel->destCount();i++) {
    edit_sched_codes->push_back(codes_sel->destText(i));
  }

  if(edit_remove_codes!=NULL) {
    edit_remove_codes->clear();
    for(unsigned i=0;i<remove_codes_sel->destCount();i++) {
      edit_remove_codes->push_back(remove_codes_sel->destText(i));
    }
  }

  done(0);
}


void RDSchedCodesDialog::cancelData()
{
  done(-1);
}




