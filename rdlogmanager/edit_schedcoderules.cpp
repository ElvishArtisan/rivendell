// edit_schedcoderules.cpp
//
// Change rules for scheduler codes dialog
//
//   (C) Stefan Gabriel <stg@st-gabriel.de>
//   (C) 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdescape_string.h>

#include "edit_schedcoderules.h"

EditSchedCodeRules::EditSchedCodeRules(QWidget* parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
    
  setWindowTitle("RDLogManager - "+tr("Edit Rules for Code"));
  
  edit_code_label=new QLabel(this);
  edit_code_label->setFont(bigLabelFont());
  edit_code_label->setAlignment(Qt::AlignCenter);
    
  edit_max_row_label=new QLabel(this);
  edit_max_row_label->setText(tr("Max. in a row:"));
  edit_max_row_label->setFont(labelFont()); 
  edit_max_row_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_min_wait_label=new QLabel(this);
  edit_min_wait_label->setText(tr("Min. wait:"));
  edit_min_wait_label->setFont(labelFont()); 
  edit_min_wait_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_not_after_label=new QLabel(this);
  edit_not_after_label->setText(tr("Do not schedule after:"));
  edit_not_after_label->setFont(labelFont()); 
  edit_not_after_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_or_after_label=new QLabel(this);
  edit_or_after_label->setText(tr("Or after:"));
  edit_or_after_label->setFont(labelFont()); 
  edit_or_after_label->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));

  edit_or_after_label_II=new QLabel(this);
  edit_or_after_label_II->setText(tr("Or after:"));
  edit_or_after_label_II->setFont(labelFont()); 
  edit_or_after_label_II->setAlignment(int(Qt::AlignVCenter | Qt::AlignRight));
  
  edit_max_row_spin=new QSpinBox(this);
  edit_max_row_spin->setMaxValue(999);
  edit_max_row_spin->setLineStep(1);

  edit_min_wait_spin=new QSpinBox(this);
  edit_min_wait_spin->setMaxValue(999);
  edit_max_row_spin->setLineStep(1);
  
  edit_schedcodes_model=new RDSchedCodeListModel(true,this);
  edit_notafter_boxes[0]=new QComboBox(FALSE,this);
  edit_notafter_boxes[0]->setDuplicatesEnabled(FALSE);
  edit_notafter_boxes[0]->setModel(edit_schedcodes_model);

  edit_notafter_boxes[1]=new QComboBox(FALSE,this);
  edit_notafter_boxes[1]->setDuplicatesEnabled(FALSE);
  edit_notafter_boxes[1]->setModel(edit_schedcodes_model);

  edit_notafter_boxes[2]=new QComboBox(FALSE,this);
  edit_notafter_boxes[2]->setDuplicatesEnabled(FALSE);
  edit_notafter_boxes[2]->setModel(edit_schedcodes_model);

  //
  // OK Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setText(tr("&OK"));
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  // Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setText(tr("&Cancel"));
  edit_cancel_button->setFont(buttonFont());
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

}


EditSchedCodeRules::~EditSchedCodeRules()
{
  delete edit_schedcodes_model;
}

QSize EditSchedCodeRules::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy EditSchedCodeRules::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int EditSchedCodeRules::exec(unsigned rule_id)
{
  edit_rule_id=rule_id;

  QString sql=QString("select ")+
    "RULE_LINES.CODE,"+          // 00
    "SCHED_CODES.DESCRIPTION,"+  // 01
    "RULE_LINES.MAX_ROW,"+       // 01
    "RULE_LINES.MIN_WAIT,"+      // 02
    "RULE_LINES.NOT_AFTER,"+     // 03
    "RULE_LINES.OR_AFTER,"+      // 04
    "RULE_LINES.OR_AFTER_II "+   // 05
    "from RULE_LINES left join SCHED_CODES "+
    "on RULE_LINES.CODE=SCHED_CODES.CODE "+
    QString().sprintf("where RULE_LINES.ID=%u",rule_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(1).toString().isEmpty()) {
      edit_code_label->setText(q->value(0).toString());
    }
    else {
      edit_code_label->
	setText(q->value(0).toString()+": "+q->value(1).toString());
    }
    edit_max_row_spin->setValue(q->value(2).toUInt());
    edit_min_wait_spin->setValue(q->value(3).toUInt());
    for(int i=0;i<3;i++) {
      if(q->value(4+i).toString().isEmpty()) {
	edit_notafter_boxes[i]->setCurrentText(tr("[none]"));
      }
      else {
	edit_notafter_boxes[i]->setCurrentText(q->value(4+i).toString());
      }
    }
  }
  delete q;

  return QDialog::exec();
}


void EditSchedCodeRules::okData()
{
  QStringList codes;
  QStringList fields;
  fields.push_back("NOT_AFTER");
  fields.push_back("OR_AFTER");
  fields.push_back("OR_AFTER_II");

  QString sql=QString("update RULE_LINES set ")+
    QString().sprintf("MAX_ROW=%u,",edit_max_row_spin->value())+
    QString().sprintf("MIN_WAIT=%u,",edit_min_wait_spin->value());
  for(int i=0;i<3;i++) {
    if(edit_notafter_boxes[i]->currentText()!=tr("[none]")&&
       (!codes.contains(edit_notafter_boxes[i]->currentText()))) {
      codes.push_back(edit_notafter_boxes[i]->currentText());
    }
  }
  int count=0;
  for(int i=0;i<3;i++) {
    if(count<codes.size()) {
      sql+=fields.at(i)+"=\""+RDEscapeString(codes.at(count))+"\",";
      count++;
    }
    else {
      sql+=fields.at(i)+"=null,";
    }
  }
  sql=sql.left(sql.length()-1);
  sql+=QString().sprintf(" where ID=%u",edit_rule_id);
  RDSqlQuery::apply(sql);

  done(true);
}


void EditSchedCodeRules::cancelData()
{
  done(false);
}


void EditSchedCodeRules::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditSchedCodeRules::resizeEvent(QResizeEvent *e)
{
  edit_code_label->setGeometry(10,2,size().width()-20,20);

  edit_max_row_label->setGeometry(30,30,160,20);
  edit_max_row_spin->setGeometry(200,30,70,20);

  edit_min_wait_label->setGeometry(30,60,160,20);
  edit_min_wait_spin->setGeometry(200,60,70,20);

  edit_not_after_label->setGeometry(30,90,160,20);
  edit_notafter_boxes[0]->setGeometry(200,90,180,30);

  edit_or_after_label->setGeometry(30,130,160,20);
  edit_notafter_boxes[1]->setGeometry(200,130,180,30);

  edit_or_after_label_II->setGeometry(30,170,160,20);
  edit_notafter_boxes[2]->setGeometry(200,170,180,30);

  edit_ok_button->
    setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
}
