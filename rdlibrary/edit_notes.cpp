// edit_notes.cpp
//
// Edit Cart Notes.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_notes.cpp,v 1.3.4.1 2014/03/19 22:12:59 cvs Exp $
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

#include <rddb.h>
#include <rdescape_string.h>

#include <edit_notes.h>

EditNotes::EditNotes(RDCart *cart,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  notes_cart=cart;
  setCaption("RDLibrary - "+tr("Notes for cart")+
	     QString().sprintf(" %06u - ",cart->number())+cart->title());

  //
  // Create Fonts
  //
  QFont button_font=QFont("helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Variable Name
  //
  notes_view=new QTextView(this,"notes_view");
  notes_view->setTextFormat(QTextView::PlainText);
  notes_view->setReadOnly(false);

  //
  //  Ok Button
  //
  notes_ok_button=new QPushButton(this,"notes_ok_button");
  notes_ok_button->setDefault(true);
  notes_ok_button->setFont(button_font);
  notes_ok_button->setText(tr("&OK"));
  notes_ok_button->setDefault(true);
  connect(notes_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  notes_cancel_button=new QPushButton(this,"notes_cancel_button");
  notes_cancel_button->setFont(button_font);
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
  notes_cart->setNotes(notes_view->text().stripWhiteSpace());
  done(0);
}


void EditNotes::cancelData()
{
  done(-1);
}
