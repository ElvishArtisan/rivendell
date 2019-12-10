//   rdlistselector.cpp
//
//   A List Selector Widget.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qpixmap.h>

#include <rdlistselector.h>

RDListSelector::RDListSelector(QWidget *parent)
  : RDWidget(parent)
{
  list_source_label=new QLabel(this);
  list_source_label->setFont(labelFont());
  list_source_label->setText(tr("Available Services"));
  list_source_label->setAlignment(Qt::AlignCenter);
  list_source_box=new Q3ListBox(this);

  list_add_button=new QPushButton(this);
  list_add_button->setText(tr("Add >>"));
  list_add_button->setDisabled(true);
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));
  list_remove_button=new QPushButton(this);
  list_remove_button->setText(tr("<< Remove"));
  list_remove_button->setDisabled(true);
  connect(list_remove_button,SIGNAL(clicked()),this,SLOT(removeData()));

  list_dest_label=new QLabel(this);
  list_dest_label->setFont(labelFont());
  list_dest_label->setText(tr("Active Services"));
  list_dest_label->setAlignment(Qt::AlignCenter);
  list_dest_box=new Q3ListBox(this);
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


Q3ListBoxItem *RDListSelector::sourceFindItem(const QString &text,
					      Q3ListBox::ComparisonFlags compare) const
{
  return list_source_box->findItem(text,compare);
}


Q3ListBoxItem *RDListSelector::destFindItem(const QString &text,
					    Q3ListBox::ComparisonFlags compare) const
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


void RDListSelector::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  list_source_label->setGeometry(0,0,w/3,12);
  list_source_box->setGeometry(0,12,w/3,h-12);

  list_add_button->setGeometry(w/3+20,20,w/3-40,25);
  list_remove_button->setGeometry(w/3+20,2*h/3-3,w/3-40,25);

  list_dest_label->setGeometry(2*w/3,0,w/3,12);
  list_dest_box->setGeometry(2*w/3,12,w/3,h-12);
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
