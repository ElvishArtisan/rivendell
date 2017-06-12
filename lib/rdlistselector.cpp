//   rdlistselector.cpp
//
//   A List Selector Widget.
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QHBoxLayout>
#include <QLabel>
#include <QModelIndex>
#include <QPixmap>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <rdlistselector.h>

RDListSelector::RDListSelector(QWidget *parent)
  : QWidget(parent)
{
  QFont font;

  //
  // Generate Font
  //
  font=QFont("Helvetica",10,QFont::Bold);
  font.setPixelSize(10);

  QHBoxLayout *hbox_layout=new QHBoxLayout;
  QWidget *source=new QWidget(this);
  hbox_layout->setSpacing(10);
  QVBoxLayout *source_box=new QVBoxLayout();
  list_source_label=new QLabel(tr("Available Services"));
  source_box->addWidget(list_source_label);
  list_source_label->setFont(font);
  list_source_label->setAlignment(Qt::AlignCenter);
  list_source_box=new QListWidget();
  list_source_box->setSortingEnabled(true);
  source_box->addWidget(list_source_box);
  source->setLayout(source_box);
  hbox_layout->addWidget(source);

  QVBoxLayout *button_box=new QVBoxLayout();
  QWidget *buttons=new QWidget(this);
  list_add_button=new QPushButton(tr("Add >>"));
  button_box->addWidget(list_add_button);
  list_add_button->setDisabled(true);
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));
  list_remove_button=new QPushButton(tr("<< Remove"));
  button_box->addWidget(list_remove_button);
  list_remove_button->setDisabled(true);
  connect(list_remove_button,SIGNAL(clicked()),this,SLOT(removeData()));
  buttons->setLayout(button_box);
  hbox_layout->addWidget(buttons);

  QVBoxLayout *dest_box=new QVBoxLayout();
  QWidget *dest=new QWidget(this);
  list_dest_label=new QLabel(tr("Active Services"));
  dest_box->addWidget(list_dest_label);
  list_dest_label->setFont(font);
  list_dest_label->setAlignment(Qt::AlignCenter);
  list_dest_box=new QListWidget();
  list_dest_box->setSortingEnabled(true);
  dest_box->addWidget(list_dest_box);
  dest->setLayout(dest_box);
  hbox_layout->addWidget(dest);
  setLayout(hbox_layout);
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
  list_source_box->setSortingEnabled(false);
  list_source_box->insertItem(index,text);
  list_source_box->setSortingEnabled(true);
  CheckButtons();
}


void RDListSelector::destInsertItem(const QString &text,int index)
{
  list_dest_box->setSortingEnabled(false);
  list_dest_box->insertItem(index,text);
  list_dest_box->setSortingEnabled(true);
  CheckButtons();
}


void RDListSelector::sourceRemoveItem(int index)
{
  QListWidgetItem *item=list_source_box->takeItem(index);
  delete item;
  CheckButtons();
}


void RDListSelector::destRemoveItem(int index)
{
  QListWidgetItem *item=list_dest_box->takeItem(index);
  delete item;
  CheckButtons();
}


QString RDListSelector::sourceText(int index) const
{
  QListWidgetItem *item=list_source_box->item(index);
  if(item==NULL) {
    return "";
  }
  return item->text();
}


QString RDListSelector::destText(int index) const
{
  QListWidgetItem *item=list_dest_box->item(index);
  if(item==NULL) {
    return "";
  }
  return item->text();
}


void RDListSelector::sourceChangeItem(const QString &text,int index)
{
  list_source_box->setSortingEnabled(false);
  QListWidgetItem *item=list_source_box->takeItem(index);
  list_source_box->insertItem(index,item);
  delete item;
  list_source_box->setSortingEnabled(true);
}


void RDListSelector::destChangeItem(const QString &text,int index)
{
  list_dest_box->setSortingEnabled(false);
  QListWidgetItem *item=list_dest_box->takeItem(index);
  list_dest_box->insertItem(index,item);
  delete item;
  list_dest_box->setSortingEnabled(true);
}


int RDListSelector::sourceCurrentItem() const
{
  QList<QListWidgetItem *> items=list_source_box->selectedItems();
  if(items.isEmpty()) {
      return -1;
  }
  return list_source_box->row(items.at(0));
}


int RDListSelector::destCurrentItem() const
{
  QList<QListWidgetItem *> items=list_dest_box->selectedItems();
  if(items.isEmpty()) {
      return -1;
  }
  return list_dest_box->row(items.at(0));
}


QString RDListSelector::sourceCurrentText() const
{
  QListWidgetItem *item=list_source_box->item(sourceCurrentItem());
  if(item==NULL) {
    return "";
  }
  return item->text();
}


QString RDListSelector::destCurrentText() const
{
  QListWidgetItem *item=list_dest_box->item(destCurrentItem());
  if(item==NULL) {
    return "";
  }
  return item->text();
}


void RDListSelector::sourceSetCurrentItem(int item)
{
  if(item==-1) {
    QModelIndex item_index=list_source_box->model()->index(sourceCurrentItem(),0);
    list_source_box->selectionModel()->select(item_index,QItemSelectionModel::Deselect);
  } else {
    QModelIndex item_index=list_source_box->model()->index(item,0);
    list_source_box->selectionModel()->select(item_index,QItemSelectionModel::Select);
  }
}


void RDListSelector::destSetCurrentItem(int item)
{
  if(item==-1) {
    QModelIndex item_index=list_dest_box->model()->index(destCurrentItem(),0);
    list_dest_box->selectionModel()->select(item_index,QItemSelectionModel::Deselect);
  } else {
    QModelIndex item_index=list_dest_box->model()->index(item,0);
    list_dest_box->selectionModel()->select(item_index,QItemSelectionModel::Select);
  }
}


QListWidgetItem *RDListSelector::sourceFindItem(const QString &text,
				     Qt::MatchFlags compare) const
{
  QList<QListWidgetItem *> items=list_source_box->findItems(text,compare);
  if (items.isEmpty()) {
      return NULL;
  }
  return items.at(0);
}


QListWidgetItem *RDListSelector::destFindItem(const QString &text,
				     Qt::MatchFlags compare) const
{
  QList<QListWidgetItem *> items=list_dest_box->findItems(text,compare);
  if (items.isEmpty()) {
      return NULL;
  }
  return items.at(0);
}


void RDListSelector::clear()
{
  list_source_box->clear();
  list_dest_box->clear();
}


void RDListSelector::addData()
{
  if(list_source_box->currentItem()>=0) {
    destInsertItem(sourceCurrentText());
    sourceRemoveItem(sourceCurrentItem());
    if(list_source_box->count()==0) {
      list_add_button->setDisabled(true);
    }
    list_remove_button->setEnabled(true);
    sourceSetCurrentItem(-1);
  }
}


void RDListSelector::removeData()
{
  if(list_dest_box->currentItem()>=0) {
    sourceInsertItem(destCurrentText());
    destRemoveItem(destCurrentItem());
    if(list_dest_box->count()==0) {
      list_remove_button->setDisabled(true);
    }
    list_add_button->setEnabled(true);
    destSetCurrentItem(-1);
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
