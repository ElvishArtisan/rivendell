// edit_schedcoderules.cpp
//
// Change rules for scheduler codes dialog
//
//   (C) Stefan Gabriel <stg@st-gabriel.de>
//   (C) 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_schedcoderules.h"

EditSchedCodeRules::EditSchedCodeRules(Q3ListViewItem *item,
				       RDSchedRulesList *sched_rules_list,
				       QWidget* parent)
  : RDDialog(parent)
{
  edit_edit_item=item;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());
    
  setWindowTitle("RDLogManager - "+tr("Edit Rules for Code"));

  edit_ok_button=new QPushButton(this);
  edit_ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  edit_ok_button->setText(tr("&OK"));
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  edit_cancel_button->setText(tr("&Cancel"));
  edit_cancel_button->setFont(buttonFont());
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
  
  edit_code_name_label=new QLabel(this);
  edit_code_name_label->setGeometry(QRect(40,10,150,20));
  edit_code_name_label->setText(tr("Code:"));
  edit_code_name_label->setFont(labelFont()); 
  edit_code_name_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_max_row_label=new QLabel(this);
  edit_max_row_label->setGeometry(QRect(30,80,160,20));
  edit_max_row_label->setText(tr("Max. in a row:"));
  edit_max_row_label->setFont(labelFont()); 
  edit_max_row_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_min_wait_label=new QLabel(this);
  edit_min_wait_label->setGeometry(QRect(30,110,160,20));
  edit_min_wait_label->setText(tr("Min. wait:"));
  edit_min_wait_label->setFont(labelFont()); 
  edit_min_wait_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_not_after_label=new QLabel(this);
  edit_not_after_label->setGeometry(QRect(30,140,160,20));
  edit_not_after_label->setText(tr("Do not schedule after:"));
  edit_not_after_label->setFont(labelFont()); 
  edit_not_after_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_or_after_label=new QLabel(this);
  edit_or_after_label->setGeometry(QRect(30,180,160,20));
  edit_or_after_label->setText(tr("Or after:"));
  edit_or_after_label->setFont(labelFont()); 
  edit_or_after_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_or_after_label_II=new QLabel(this);
  edit_or_after_label_II->setGeometry(QRect(30,220,160,20));
  edit_or_after_label_II->setText(tr("Or after:"));
  edit_or_after_label_II->setFont(labelFont()); 
  edit_or_after_label_II->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));
  
  edit_max_row_spin=new QSpinBox(this);
  edit_max_row_spin->setGeometry(QRect(200,80,70,20));
  edit_max_row_spin->setMaxValue(999);
  edit_max_row_spin->setLineStep(1);
  edit_max_row_spin->setValue(item->text(1).toInt());

  edit_min_wait_spin=new QSpinBox(this);
  edit_min_wait_spin->setGeometry(QRect(200,110,70,20));
  edit_min_wait_spin->setMaxValue(999);
  edit_max_row_spin->setLineStep(1);
  edit_min_wait_spin->setValue(item->text(2).toInt());
  
  edit_code_label=new QLabel(this);
  edit_code_label->setGeometry(QRect(200,10,90,20));
  edit_code_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignLeft));
  edit_code_label->setText(item->text(0));
    
  edit_not_after_box=new QComboBox(FALSE,this);
  edit_not_after_box->setGeometry(QRect(200,140,180,30));
  edit_not_after_box->setDuplicatesEnabled(FALSE);

  edit_or_after_box=new QComboBox(FALSE,this);
  edit_or_after_box->setGeometry(QRect(200,180,180,30));
  edit_or_after_box->setDuplicatesEnabled(FALSE);

  edit_or_after_box_II=new QComboBox(FALSE,this);
  edit_or_after_box_II->setGeometry(QRect(200,220,180,30));
  edit_or_after_box_II->setDuplicatesEnabled(FALSE);

  edit_not_after_box->insertItem("");
  edit_or_after_box->insertItem("");
  edit_or_after_box_II->insertItem("");
  for (int i=0; i<sched_rules_list->getNumberOfItems(); i++) {
    edit_not_after_box->insertItem(sched_rules_list->getItemSchedCode(i));
    edit_or_after_box->insertItem(sched_rules_list->getItemSchedCode(i));
    edit_or_after_box_II->insertItem(sched_rules_list->getItemSchedCode(i));
  }
  edit_not_after_box->setCurrentText(item->text(3));
  edit_or_after_box->setCurrentText(item->text(4));
  edit_or_after_box_II->setCurrentText(item->text(5));

  edit_description_label=new QLabel(this);
  edit_description_label->setGeometry(QRect(200,40,300,40));
  edit_description_label->setAlignment(int(Qt::AlignTop | Qt::AlignLeft));
  edit_description_label->setText(item->text(6));
}


EditSchedCodeRules::~EditSchedCodeRules()
{
}

QSize EditSchedCodeRules::sizeHint() const
{
  return QSize(500,350);
} 


QSizePolicy EditSchedCodeRules::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}



void EditSchedCodeRules::okData()
{
  edit_edit_item->setText(1,edit_max_row_spin->text());  
  edit_edit_item->setText(2,edit_min_wait_spin->text());  
  edit_edit_item->setText(3,edit_not_after_box->currentText());  
  edit_edit_item->setText(4,edit_or_after_box->currentText());  
  edit_edit_item->setText(5,edit_or_after_box_II->currentText());  
  done(0);
}


void EditSchedCodeRules::cancelData()
{
  done(-1);
}


void EditSchedCodeRules::closeEvent(QCloseEvent *e)
{
  cancelData();
}
