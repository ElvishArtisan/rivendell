//   rdlistselector.cpp
//
//   A List Selector Widget.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdlistselector.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <stdlib.h>
#include <stdio.h>
#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qpixmap.h>

#include <rdlistselector.h>


RDListSelector::RDListSelector(QWidget *parent,const char *name)
  : QHBox(parent,name)
{
  QFont font;

  //
  // Generate Font
  //
  font=QFont("Helvetica",10,QFont::Bold);
  font.setPixelSize(10);

  setSpacing(10);

  QVBox *source_box=new QVBox(this,"source_box");
  list_source_label=new QLabel(source_box,"list_source_label");
  list_source_label->setFont(font);
  list_source_label->setText(tr("Available Services"));
  list_source_label->setAlignment(AlignCenter);
  list_source_box=new QListBox(source_box,"list_source_box");

  QVBox *button_box=new QVBox(this,"button_box");
  list_add_button=new QPushButton(button_box,"list_add_button");
  list_add_button->setText(tr("Add >>"));
  list_add_button->setDisabled(true);
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));
  list_remove_button=new QPushButton(button_box,"list_add_button");
  list_remove_button->setText(tr("<< Remove"));
  list_remove_button->setDisabled(true);
  connect(list_remove_button,SIGNAL(clicked()),this,SLOT(removeData()));

  QVBox *dest_box=new QVBox(this,"dest_box");
  list_dest_label=new QLabel(dest_box,"list_dest_label");
  list_dest_label->setFont(font);
  list_dest_label->setText(tr("Active Services"));
  list_dest_label->setAlignment(AlignCenter);
  list_dest_box=new QListBox(dest_box,"list_dest_box");
}


uint RDListSelector::sourceCount() const
{
  return list_source_box->count();
}


uint RDListSelector::destCount() const
{
  return list_dest_box->count();
}


void RDListSelector::sourceSetLabel(QString label)
{
  list_source_label->setText(label);
}


void RDListSelector::destSetLabel(QString label)
{
  list_dest_label->setText(label);
}


void RDListSelector::sourceInsertItem(const QString &text,int index)
{
  list_source_box->insertItem(text,index);
  list_source_box->sort();
  CheckButtons();
}


void RDListSelector::destInsertItem(const QString &text,int index)
{
  list_dest_box->insertItem(text,index);
  list_dest_box->sort();
  CheckButtons();
}


void RDListSelector::sourceRemoveItem(int index)
{
  list_source_box->removeItem(index);
  CheckButtons();
}


void RDListSelector::destRemoveItem(int index)
{
  list_dest_box->removeItem(index);
  CheckButtons();
}


QString RDListSelector::sourceText(int index) const
{
  return list_source_box->text(index);
}


QString RDListSelector::destText(int index) const
{
  return list_dest_box->text(index);
}


void RDListSelector::sourceChangeItem(const QString &text,int index)
{
  list_source_box->changeItem(text,index);
  list_source_box->sort();
}


void RDListSelector::destChangeItem(const QString &text,int index)
{
  list_dest_box->changeItem(text,index);
  list_dest_box->sort();
}


int RDListSelector::sourceNumItemsVisible() const
{
  return list_source_box->numItemsVisible();
}


int RDListSelector::destNumItemsVisible() const
{
  return list_dest_box->numItemsVisible();
}


int RDListSelector::sourceCurrentItem() const
{
  return list_source_box->currentItem();
}


int RDListSelector::destCurrentItem() const
{
  return list_dest_box->currentItem();
}


QString RDListSelector::sourceCurrentText() const
{
  return list_source_box->currentText();
}


QString RDListSelector::destCurrentText() const
{
  return list_dest_box->currentText();
}


void RDListSelector::sourceSetCurrentItem(int item)
{
  list_source_box->setCurrentItem(item);
}


void RDListSelector::destSetCurrentItem(int item)
{
  list_dest_box->setCurrentItem(item);
}


QListBoxItem *RDListSelector::sourceFindItem(const QString &text,
				      ComparisonFlags compare) const
{
  return list_source_box->findItem(text,compare);
}


QListBoxItem *RDListSelector::destFindItem(const QString &text,
				      ComparisonFlags compare) const
{
  return list_dest_box->findItem(text,compare);
}


void RDListSelector::clear()
{
  list_source_box->clear();
  list_dest_box->clear();
}


void RDListSelector::addData()
{
  if(list_source_box->currentItem()>=0) {
    list_dest_box->
      insertItem(list_source_box->currentText());
    list_source_box->removeItem(list_source_box->currentItem());
    list_dest_box->sort();
    if(list_source_box->count()==0) {
      list_add_button->setDisabled(true);
    }
    list_remove_button->setEnabled(true);
    list_source_box->setCurrentItem(-1);
  }
}


void RDListSelector::removeData()
{
  if(list_dest_box->currentItem()>=0) {
    list_source_box->
      insertItem(list_dest_box->currentText());
    list_dest_box->removeItem(list_dest_box->currentItem());
    list_source_box->sort();
    if(list_dest_box->count()==0) {
      list_remove_button->setDisabled(true);
    }
    list_add_button->setEnabled(true);
    list_dest_box->setCurrentItem(-1);
  }
}


void RDListSelector::CheckButtons()
{
  if(list_source_box->count()==0) {
    list_add_button->setDisabled(true);
  }
  else {
    list_add_button->setEnabled(true);
  }
  if(list_dest_box->count()==0) {
    list_remove_button->setDisabled(true);
  }
  else {
    list_remove_button->setEnabled(true);
  }
}
