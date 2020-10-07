// rdrsscategorybox.cpp
//
// A Combo Box widget for RSS categories.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"
#include "rdrsscategorybox.h"

RDRssCategoryBox::RDRssCategoryBox(QWidget *parent)
  : RDWidget(parent)
{
  c_schema=RDRssSchemas::CustomSchema;

  //
  // Category
  //
  c_box=new QComboBox(this);
  connect(c_box,SIGNAL(activated(const QString &)),
	  this,SLOT(boxActivatedData(const QString &)));
  c_edit=new QLineEdit(this);
  c_edit->setMaxLength(64);

  //
  // Seperator
  //
  c_seperator_label=new QLabel(":",this);
  c_seperator_label->setFont(labelFont());
  c_seperator_label->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);

  //
  // Subcategory
  //
  c_sub_box=new QComboBox(this);
  c_sub_edit=new QLineEdit(this);
  c_sub_edit->setMaxLength(64);
  connect(c_sub_box,SIGNAL(activated(const QString &)),
	  c_sub_edit,SLOT(setText(const QString &)));
}


RDRssCategoryBox::~RDRssCategoryBox()
{
  delete c_box;
  delete c_sub_box;
  delete c_seperator_label;
  delete c_edit;
  delete c_sub_edit;
}


RDRssSchemas::RssSchema RDRssCategoryBox::schema() const
{
  return c_schema;
}


void RDRssCategoryBox::setSchema(RDRssSchemas::RssSchema schema)
{
  if(schema!=c_schema) {
    RefreshCategories(schema,c_edit->text(),c_sub_edit->text());
    c_schema=schema;
  }
}


QString RDRssCategoryBox::category() const
{
  return c_edit->text();
}


QString RDRssCategoryBox::subCategory() const
{
  return c_sub_edit->text();
}


void RDRssCategoryBox::setCategory(const QString &category,
				   const QString &sub_category)
{
  c_edit->setText(category);
  c_sub_edit->setText(sub_category);
  RefreshCategories(c_schema,category,sub_category);
}


void RDRssCategoryBox::boxActivatedData(const QString str)
{
  c_edit->setText(str);
  RefreshSubcategories(c_schema,str,c_sub_edit->text());
}


void RDRssCategoryBox::resizeEvent(QResizeEvent *e)
{
  int w=e->size().width();
  int h=e->size().height();

  c_box->setGeometry(0,0,w/2-3,h);
  c_edit->setGeometry(0,0,w/2-3,h);
  c_seperator_label->setGeometry(w/2-2,0,5,h);
  c_sub_box->setGeometry(w/2+3,0,w/2-2,h);
  c_sub_edit->setGeometry(w/2+3,0,w/2-2,h);
}


void RDRssCategoryBox::RefreshCategories(RDRssSchemas::RssSchema schema,
					 const QString &category,
					 const QString &sub_category)
{
  QStringList categories=rda->rssSchemas()->categories(schema);
  c_edit->setVisible(categories.size()==0);
  c_sub_edit->setVisible(categories.size()==0);
  c_box->setVisible(categories.size()>0);
  c_sub_box->setVisible(categories.size()>0);
  if(categories.size()>0) {  // Update categories list
    c_box->clear();
    for(int i=0;i<categories.size();i++) {
      c_box->insertItem(c_box->count(),categories.at(i));
      if(category==categories.at(i)) {
	c_box->setCurrentIndex(i);
      }
    }
    c_edit->setText(c_box->currentText());
    RefreshSubcategories(schema,c_edit->text(),sub_category);
  }
}


void RDRssCategoryBox::RefreshSubcategories(RDRssSchemas::RssSchema schema,
					    const QString &category,
					    const QString &sub_category)
{
  QStringList subcategories=rda->rssSchemas()->subCategories(schema,category);

  c_sub_box->clear();
  for(int i=0;i<subcategories.size();i++) {
    c_sub_box->insertItem(c_sub_box->count(),subcategories.at(i));
    if(subcategories.at(i)==sub_category) {
      c_sub_box->setCurrentIndex(i);
    }
  }
  c_sub_edit->setText(c_sub_box->currentText());
  c_sub_box->setDisabled(c_sub_box->count()==0);
}
