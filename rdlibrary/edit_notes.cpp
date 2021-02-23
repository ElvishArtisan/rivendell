// edit_notes.cpp
//
// Edit Cart Notes.
//
//   (C) Copyright 2009-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_notes.h"

EditNotes::EditNotes(RDCart *cart,QWidget *parent)
  : RDDialog(parent)
{
  notes_cart=cart;
  setWindowTitle("RDLibrary - "+tr("Notes for cart")+
	     QString().sprintf(" %06u [",cart->number())+cart->title()+"]");

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Variable Name
  //
  notes_view=new QTextEdit(this);
  notes_view->setAcceptRichText(false);
  notes_view->setReadOnly(false);

  //
  //  Ok Button
  //
  notes_ok_button=new QPushButton(this);
  notes_ok_button->setDefault(true);
  notes_ok_button->setFont(buttonFont());
  notes_ok_button->setText(tr("&OK"));
  notes_ok_button->setDefault(true);
  connect(notes_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  notes_cancel_button=new QPushButton(this);
  notes_cancel_button->setFont(buttonFont());
  notes_cancel_button->setText(tr("&Cancel"));
  connect(notes_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Data
  //
  notes_view->setText(notes_cart->notes());
}


QSize EditNotes::sizeHint() const
{
  return QSize(640,480);
}


QSizePolicy EditNotes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditNotes::resizeEvent(QResizeEvent *e)
{
  notes_view->setGeometry(10,10,size().width()-20,size().height()-80);
  notes_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  notes_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void EditNotes::okData()
{
  notes_cart->setNotes(notes_view->toPlainText().trimmed());
  done(0);
}


void EditNotes::cancelData()
{
  done(-1);
}
