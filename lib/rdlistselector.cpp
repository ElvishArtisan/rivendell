//   rdlistselector.cpp
//
//   A List Selector Widget.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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
  list_source_list=new QListWidget(this);
  list_source_list->setSortingEnabled(true);

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
  list_dest_list=new QListWidget(this);
  list_dest_list->setSortingEnabled(true);
}


void RDListSelector::setItemIcon(const QIcon &icon)
{
  list_item_icon=icon;
}


uint RDListSelector::sourceCount() const
{
  return list_source_list->count();
}


uint RDListSelector::destCount() const
{
  return list_dest_list->count();
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
  //  list_source_list->insertItem(index,text);
  list_source_list->insertItem(index,new QListWidgetItem(list_item_icon,text));
  list_source_list->sortItems();
  CheckButtons();
}


void RDListSelector::destInsertItem(const QString &text,int index)
{
  //  list_dest_list->insertItem(index,text);
  list_dest_list->insertItem(index,new QListWidgetItem(list_item_icon,text));
  list_dest_list->sortItems();
  CheckButtons();
}


void RDListSelector::sourceRemoveItem(int index)
{
  list_source_list->removeItemWidget(list_source_list->item(index));
  CheckButtons();
}


void RDListSelector::destRemoveItem(int index)
{
  list_dest_list->removeItemWidget(list_source_list->item(index));
  CheckButtons();
}


QString RDListSelector::sourceText(int index) const
{
  return list_source_list->item(index)->text();
}


QString RDListSelector::destText(int index) const
{
  return list_dest_list->item(index)->text();
}


void RDListSelector::sourceChangeItem(const QString &text,int index)
{
  list_source_list->item(index)->setText(text);
  list_source_list->sortItems();
}


void RDListSelector::destChangeItem(const QString &text,int index)
{
  list_dest_list->item(index)->setText(text);
  list_dest_list->sortItems();
}


int RDListSelector::sourceCurrentItem() const
{
  return list_source_list->currentRow();
}


int RDListSelector::destCurrentItem() const
{
  return list_dest_list->currentRow();
}


QString RDListSelector::sourceCurrentText() const
{
  return list_source_list->currentItem()->text();
}


QString RDListSelector::destCurrentText() const
{
  return list_dest_list->currentItem()->text();
}


void RDListSelector::sourceSetCurrentItem(int item)
{
  list_source_list->setCurrentRow(item);
}


void RDListSelector::destSetCurrentItem(int item)
{
  list_dest_list->setCurrentRow(item);
}


QListWidgetItem *RDListSelector::sourceFindItem(const QString &text,
						Qt::MatchFlags flag)
{
  QList<QListWidgetItem *> items=list_source_list->findItems(text,flag);
  if(items.size()==0) {
    return NULL;
  }
  return items.first();
}


QListWidgetItem *RDListSelector::destFindItem(const QString &text,
					      Qt::MatchFlags flag)
{
  QList<QListWidgetItem *> items=list_dest_list->findItems(text,flag);
  if(items.size()==0) {
    return NULL;
  }
  return items.first();
}


void RDListSelector::clear()
{
  list_source_list->clear();
  list_dest_list->clear();
}


void RDListSelector::addData()
{
  QList<QListWidgetItem *> items;

  if(list_source_list->currentRow()>=0) {
    list_dest_list->
      insertItem(list_dest_list->count(),new QListWidgetItem(list_item_icon,list_source_list->currentItem()->text()));
    delete list_source_list->takeItem(list_source_list->currentRow());
    list_dest_list->sortItems();
    if(list_source_list->count()==0) {
      list_add_button->setDisabled(true);
    }
    list_remove_button->setEnabled(true);
    list_source_list->setCurrentRow(-1);
  }
}


void RDListSelector::removeData()
{
  if(list_dest_list->currentRow()>=0) {
    list_source_list->
      insertItem(list_source_list->count(),new QListWidgetItem(list_item_icon,list_dest_list->currentItem()->text()));
    delete list_dest_list->takeItem(list_dest_list->currentRow());
    list_source_list->sortItems();
    if(list_dest_list->count()==0) {
      list_remove_button->setDisabled(true);
    }
    list_add_button->setEnabled(true);
    list_dest_list->setCurrentRow(-1);
  }
}


void RDListSelector::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  list_source_label->setGeometry(0,0,w/3,12);
  list_source_list->setGeometry(0,12,w/3,h-12);

  list_add_button->setGeometry(w/3+20,20,w/3-40,25);
  list_remove_button->setGeometry(w/3+20,2*h/3-3,w/3-40,25);

  list_dest_label->setGeometry(2*w/3,0,w/3,12);
  list_dest_list->setGeometry(2*w/3,12,w/3,h-12);
}


void RDListSelector::CheckButtons()
{
  if(list_source_list->count()==0) {
    list_add_button->setDisabled(true);
  }
  else {
    list_add_button->setEnabled(true);
  }
  if(list_dest_list->count()==0) {
    list_remove_button->setDisabled(true);
  }
  else {
    list_remove_button->setEnabled(true);
  }
}
