//   rdintegeredit.cpp
//
//   A widget for editing a list of integer values.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdintegeredit.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#include <rdintegerdialog.h>
#include <rdintegeredit.h>

RDIntegerEdit::RDIntegerEdit(const QString &lbl,int low,int high,
			     QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  edit_low=low;
  edit_high=high;

  setCaption(tr("Set Value"));

  //
  // Generate Fonts
  //
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",10,QFont::Bold);
  button_font.setPixelSize(10);

  //
  // Values List
  //
  edit_values_box=new QListBox(this);

  //
  // Title Label
  //
  edit_label=new QLabel(edit_values_box,lbl,this);
  edit_label->setAlignment(AlignCenter);
  edit_label->setFont(label_font);

  //
  // Add Button
  //
  edit_add_button=new QPushButton(tr("Add"),this);
  edit_add_button->setFont(button_font);
  connect(edit_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  // Delete Button
  //
  edit_delete_button=new QPushButton(tr("Delete"),this);
  edit_delete_button->setFont(button_font);
  connect(edit_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));
}


QSize RDIntegerEdit::sizeHint() const
{
  return QSize(300,100);
}


QSizePolicy RDIntegerEdit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


unsigned RDIntegerEdit::values(std::vector<int> *v) const
{
  v->clear();
  for(int i=0;i<edit_values_box->numRows();i++) {
    v->push_back(edit_values_box->text(i).toInt());
  }
  return v->size();
}


void RDIntegerEdit::setValues(std::vector<int> *v)
{
  for(unsigned i=0;i<v->size();i++) {
    edit_values_box->insertItem(QString().sprintf("%d",v->at(i)));
  }
}


void RDIntegerEdit::setGeometry(int x,int y,int w,int h)
{
  QWidget::setGeometry(x,y,w,h);
  edit_label->setGeometry(0,0,width(),20);
  edit_values_box->setGeometry(0,20,width(),height()-60);
  edit_add_button->setGeometry(0,height()-35,width()/2-5,30);
  edit_delete_button->setGeometry(width()/2+5,height()-35,width()/2-5,30);
}


void RDIntegerEdit::addData()
{
  int value=edit_low;
  int index=-1;

  RDIntegerDialog *ie=
    new RDIntegerDialog(&value,edit_label->text(),edit_low,edit_high,this);
  if(ie->exec()==0) {
    for(unsigned i=0;i<edit_values_box->count();i++) {
      if(edit_values_box->item(i)->text().toInt()<value) {
	index=i;
      }
      if(edit_values_box->item(i)->text().toInt()==value) {
	delete ie;
	return;
      }
    }
    edit_values_box->insertItem(QString().sprintf("%d",value),index+1);
  }
  delete ie;
}


void RDIntegerEdit::deleteData()
{
  QListBoxItem *item=edit_values_box->selectedItem();
  if(item==NULL) {
    return;
  }
  delete item;
}


QListBoxItem *RDIntegerEdit::GetItem(int value)
{
  for(unsigned i=0;i<edit_values_box->count();i++) {
    if(edit_values_box->item(i)->text().toInt()==value) {
      return edit_values_box->item(i);
    }
  }

  return NULL;
}
