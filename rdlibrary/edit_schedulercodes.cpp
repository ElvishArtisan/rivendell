// edit_schedulercodes.cpp
//
// Edit the scheduler codes of a cart
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qmessagebox.h>

#include <rddb.h>
#include <edit_schedulercodes.h>


EditSchedulerCodes::EditSchedulerCodes(QString *sched_codes,QString *remove_codes,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  edit_sched_codes=sched_codes;
  edit_remove_codes=remove_codes;

  QString sql;
  RDSqlQuery *q;
  QString str;

  //
  // Fix the Window Size
  //
  if(edit_remove_codes==NULL) {
    setMinimumWidth(sizeHint().width());
    setMaximumWidth(sizeHint().width());
    }
  else {
    setMinimumWidth(2*sizeHint().width());
    setMaximumWidth(2*sizeHint().width());
    }
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Select Scheduler Codes"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",11,QFont::Bold);
  font.setPixelSize(11);

  QFont listfont=QFont("Helvetica",11);
  font.setPixelSize(11);


  //
  // Services Selector
  //
  codes_sel=new RDListSelector(this,"codes_sel");
  codes_sel->setFont(listfont);
  codes_sel->setGeometry(10,10,380,200);
  codes_sel->sourceSetLabel(tr("Available Codes"));
  if(edit_remove_codes==NULL) {
    codes_sel->destSetLabel(tr("Assigned Codes"));
    }
  else {
    codes_sel->destSetLabel(tr("ASSIGN to Carts"));
    
    remove_codes_sel=new RDListSelector(this,"codes_sel");
    remove_codes_sel->setFont(listfont);
    remove_codes_sel->setGeometry(sizeHint().width()+10,10,380,200);
    remove_codes_sel->sourceSetLabel(tr("Available Codes"));
    remove_codes_sel->destSetLabel(tr("REMOVE from Carts"));
    }
  
  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this,"ok_button");
  if(edit_remove_codes==NULL) 
    ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  else  
    ok_button->setGeometry(2*sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this,"cancel_button");
  if(edit_remove_codes==NULL) 
    cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  else			     
    cancel_button->setGeometry(2*sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));


  for(unsigned i=0;i<edit_sched_codes->length()/11;i++) {
    codes_sel->destInsertItem(edit_sched_codes->mid(i*11,11).stripWhiteSpace());
    } 
  if(edit_remove_codes!=NULL) {
    for(unsigned i=0;i<edit_remove_codes->length()/11;i++) {
      remove_codes_sel->destInsertItem(remove_codes->mid(i*11,11).stripWhiteSpace());
      } 
    }
       
  sql=QString().sprintf("select CODE from SCHED_CODES");
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(codes_sel->destFindItem(q->value(0).toString())==0) {
      codes_sel->sourceInsertItem(q->value(0).toString());
    }
    if(edit_remove_codes!=NULL) {
      if(remove_codes_sel->destFindItem(q->value(0).toString())==0) {
        remove_codes_sel->sourceInsertItem(q->value(0).toString());
      }
    }
  }
  delete q;
}


EditSchedulerCodes::~EditSchedulerCodes()
{
}


QSize EditSchedulerCodes::sizeHint() const
{
  return QSize(400,292);
} 


QSizePolicy EditSchedulerCodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditSchedulerCodes::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->moveTo(sizeHint().width(),10);
  p->lineTo(sizeHint().width(),210);
  p->end();
}


void EditSchedulerCodes::okData()
{
  *edit_sched_codes="";
  //
  // Add New Objects
  //
  for(unsigned i=0;i<codes_sel->destCount();i++) {
    *edit_sched_codes+=codes_sel->destText(i).append("           ").left(11);
  }
  if (edit_sched_codes->length()>254)
    {
    QMessageBox::information(this,"Warning","There is a maximum of 23 Codes per Cart!",QMessageBox::Ok);
    *edit_sched_codes=edit_sched_codes->left(253);
    }
  *edit_sched_codes+=".";

  if(edit_remove_codes!=NULL) {
  	*edit_remove_codes="";
    for(unsigned i=0;i<remove_codes_sel->destCount();i++) {
      *edit_remove_codes+=remove_codes_sel->destText(i).append("           ").left(11);
    }
    if (edit_remove_codes->length()>254)
      {
      QMessageBox::information(this,"Warning","There is a maximum of 23 Codes per Cart!",QMessageBox::Ok);
      *edit_remove_codes=edit_remove_codes->left(253);
      }
    *edit_remove_codes+=".";
    }

  done(0);
}


void EditSchedulerCodes::cancelData()
{
  done(-1);
}




