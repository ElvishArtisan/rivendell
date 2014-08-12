// rdtimeedit.cpp
//
// A QTimeEdit with tenth-second precision.
//
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtimeedit.cpp,v 1.6 2010/10/06 19:24:02 cvs Exp $
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

#include <qfontmetrics.h>

#include <rdtimeedit.h>


RDTimeEdit::RDTimeEdit(QWidget *parent,const char *name)
  : QFrame(parent,name)
{
  edit_display=0;
  edit_section=0;
  edit_read_only=false;
  edit_digit=0;
  GetSizeHint();
  setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
  setLineWidth(1);
  setMidLineWidth(3);
  setFocusPolicy(QWidget::StrongFocus);
  QPalette p=palette();
  p.setColor(QPalette::Active,QColorGroup::Background,
	     p.color(QPalette::Active,QColorGroup::Base));
  setPalette(p);
  edit_labels[0]=new QLabel("00",this);
  edit_labels[0]->setAlignment(Qt::AlignCenter);
  edit_labels[1]=new QLabel("00",this);
  edit_labels[1]->setAlignment(Qt::AlignCenter);
  edit_labels[2]=new QLabel("00",this);
  edit_labels[2]->setAlignment(Qt::AlignCenter);
  edit_labels[3]=new QLabel("0",this);
  edit_labels[3]->setAlignment(Qt::AlignCenter);
  edit_sep_labels[0]=new QLabel(":",this);
  edit_sep_labels[0]->setAlignment(Qt::AlignCenter);
  edit_sep_labels[1]=new QLabel(":",this);
  edit_sep_labels[1]->setAlignment(Qt::AlignCenter);
  edit_sep_labels[2]=new QLabel(".",this);
  edit_sep_labels[2]->setAlignment(Qt::AlignCenter);
  edit_up_button=
    new RDTransportButton(RDTransportButton::Up,this,"edit_up_button");
  edit_up_button->setFocusPolicy(QWidget::NoFocus);
  connect(edit_up_button,SIGNAL(clicked()),this,SLOT(upClickedData()));
  edit_down_button=
    new RDTransportButton(RDTransportButton::Down,this,"edit_down_button");
  edit_down_button->setFocusPolicy(QWidget::NoFocus);
  connect(edit_down_button,SIGNAL(clicked()),this,SLOT(downClickedData()));
  setDisplay(RDTimeEdit::Hours|RDTimeEdit::Minutes|RDTimeEdit::Seconds);
}


RDTimeEdit::~RDTimeEdit()
{
  for(unsigned i=0;i<4;i++) {
    delete edit_labels[i];
  }
  for(unsigned i=0;i<3;i++) {
    delete edit_sep_labels[i];
  }
  delete edit_up_button;
  delete edit_down_button;
}


QSize RDTimeEdit::sizeHint() const
{
  return QSize(edit_widths[0]+edit_widths[1]+edit_widths[2]+
	       edit_sep_widths[0]+edit_sep_widths[1],
	       edit_height);
}


QSizePolicy RDTimeEdit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


QTime RDTimeEdit::time() const
{
  return QTime(edit_labels[0]->text().toInt(),
	       edit_labels[1]->text().toInt(),
	       edit_labels[2]->text().toInt(),
	       100*edit_labels[3]->text().toInt());
}


bool RDTimeEdit::isReadOnly() const
{
  return edit_read_only;
}


void RDTimeEdit::setFont(const QFont &f)
{
  QFrame::setFont(f);
  GetSizeHint();
}


uint RDTimeEdit::display() const
{
  return edit_display;
}


void RDTimeEdit::setDisplay(uint disp)
{
  edit_section=-1;
  for(unsigned i=0;i<4;i++) {
    if((disp&(1<<i))==0) {
      edit_labels[i]->hide();
    }
    else {
      edit_labels[i]->show();
      if(edit_section<0) {
	edit_section=i;
      }
    }
  }
  for(unsigned i=1;i<4;i++) {
    if(((disp&(1<<(i-1)))==0)||((disp&(1<<i))==0)) {
      edit_sep_labels[i-1]->hide();
    }
    else {
      edit_sep_labels[i-1]->show();
    }
  }
  edit_display=disp;
  GetSizeHint();
  setGeometry(geometry());
}


void RDTimeEdit::setGeometry(int x,int y,int w,int h)
{
  QFrame::setGeometry(x,y,w,h);
  QFontMetrics fm(font());
  int fy=h-fm.height();
  int fx=contentsRect().x()+fy;

  edit_labels[0]->setGeometry(fx,fy,edit_widths[0],edit_height);
  edit_section_x[0]=fx+edit_widths[0]+edit_sep_widths[0]/2;
  edit_sep_labels[0]->
    setGeometry(fx+edit_widths[0],fy,edit_sep_widths[0],edit_height);
  edit_labels[1]->
    setGeometry(fx+edit_widths[0]+edit_sep_widths[0],fy,edit_widths[1],
		edit_height);
  edit_section_x[1]=
    fx+edit_widths[0]+edit_sep_widths[0]+edit_widths[1]+edit_sep_widths[0]/2;
  edit_sep_labels[1]->
    setGeometry(fx+edit_widths[0]+edit_sep_widths[0]+edit_widths[1],fy,
		edit_sep_widths[1],edit_height);
  edit_labels[2]->
    setGeometry(fx+edit_widths[0]+edit_widths[1]+
		edit_sep_widths[0]+edit_sep_widths[1],fy,
		edit_widths[2],edit_height);

  edit_section_x[2]=
    fx+edit_widths[0]+edit_sep_widths[0]+edit_sep_widths[1]+
    edit_widths[1]+edit_widths[2]+edit_sep_widths[2]/2;
  edit_sep_labels[2]->
    setGeometry(fx+edit_widths[0]+edit_sep_widths[0]+edit_sep_widths[1]+edit_widths[1]+
		edit_widths[2],fy,edit_sep_widths[2],edit_height);
  edit_labels[3]->
    setGeometry(fx+edit_widths[0]+edit_widths[1]+
		edit_sep_widths[0]+edit_sep_widths[1]+
		edit_widths[2]+edit_sep_widths[2],fy,
		edit_widths[3],edit_height);
  int button_x=fx+edit_widths[0]+edit_widths[1]+edit_widths[2]+
    edit_sep_widths[0]+edit_sep_widths[1]+edit_widths[3]+edit_sep_widths[2]+fy;
  edit_up_button->setGeometry(button_x,0,w-button_x,h/2);
  edit_down_button->setGeometry(button_x,h/2,w-button_x,h/2);
}


void RDTimeEdit::setTime(const QTime &time)
{
  edit_labels[0]->setText(time.toString("hh"));
  edit_labels[1]->setText(time.toString("mm"));
  edit_labels[2]->setText(time.toString("ss"));
  edit_labels[3]->setText(QString().sprintf("%d",time.msec()/100));
}


void RDTimeEdit::setReadOnly(bool state)
{
  if(state) {
    setFocusPolicy(QWidget::NoFocus);
  }
  else {
    setFocusPolicy(QWidget::StrongFocus);
  }
  edit_read_only=state;
}


void RDTimeEdit::setFocus()
{
  QPalette p=palette();
  for(int i=0;i<4;i++) {
    edit_labels[i]->setPalette(p);
  }
  p.setColor(QPalette::Active,QColorGroup::Background,
	     p.color(QPalette::Active,QColorGroup::Highlight));
  p.setColor(QPalette::Active,QColorGroup::Foreground,
	     p.color(QPalette::Active,QColorGroup::HighlightedText));
  edit_labels[edit_section]->setPalette(p);
  QFrame::setFocus();
}


void RDTimeEdit::setGeometry(const QRect &r)
{
  setGeometry(r.x(),r.y(),r.width(),r.height());
}


void RDTimeEdit::upClickedData()
{
  int value;

  if(edit_read_only) {
    return;
  }
  setFocus();
  switch(edit_section) {
  case 0:
    if((value=edit_labels[edit_section]->text().toInt())<23) {
      value++;
    }
    else {
      value=0;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%02d",value));
    emit valueChanged(time());
    break;

  case 1:
  case 2:
    if((value=edit_labels[edit_section]->text().toInt())<59) {
      value++;
    }
    else {
      value=0;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%02d",value));
    emit valueChanged(time());
    break;

  case 3:
    if((value=edit_labels[edit_section]->text().toInt())<9) {
      value++;
    }
    else {
      value=0;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%d",value));
    emit valueChanged(time());
    break;
  }
}


void RDTimeEdit::downClickedData()
{
  int value;

  if(edit_read_only) {
    return;
  }
  setFocus();
  switch(edit_section) {
  case 0:
    if((value=edit_labels[edit_section]->text().toInt())>0) {
      value--;
    }
    else {
      value=23;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%02d",value));
    emit valueChanged(time());
    break;

  case 1:
  case 2:
    if((value=edit_labels[edit_section]->text().toInt())>0) {
      value--;
    }
    else {
      value=59;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%02d",value));
    emit valueChanged(time());
    break;

  case 3:
    if((value=edit_labels[edit_section]->text().toInt())>0) {
      value--;
    }
    else {
      value=9;
    }
    edit_labels[edit_section]->setText(QString().sprintf("%d",value));
    emit valueChanged(time());
    break;
  }
}


void RDTimeEdit::mousePressEvent(QMouseEvent *e)
{
  int section=0;

  if(edit_read_only) {
    return;
  }
  if(e->x()<edit_section_x[0]) {
    section=0;
  }
  else {
    if(e->x()<edit_section_x[1]) {
      section=1;
    }
    else {
      if(e->x()<edit_section_x[2]) {
	section=2;
      }
      else {
	section=3;
      }
    }
  }
  if(edit_section!=section) {
    edit_section=section;
    edit_digit=0;
  }
  setFocus();
}


void RDTimeEdit::wheelEvent(QWheelEvent *e)
{
  if(e->delta()>=0) {
    upClickedData();
  }
  else {
    downClickedData();
  }
  e->accept();
}


void RDTimeEdit::keyPressEvent(QKeyEvent *e)
{
  if(edit_read_only) {
    e->ignore();
    return;
  }
  switch(e->key()) {
  case Qt::Key_0:
  case Qt::Key_1:
  case Qt::Key_2:
  case Qt::Key_3:
  case Qt::Key_4:
  case Qt::Key_5:
  case Qt::Key_6:
  case Qt::Key_7:
  case Qt::Key_8:
  case Qt::Key_9:
    ProcessKey(e->ascii());
    e->accept();
    break;

  case Qt::Key_Left:
    if((edit_section>0)&&(((1<<(edit_section-1))&edit_display)!=0)) {
      edit_section--;
      edit_digit=0;
      setFocus();
    }
    e->accept();
    break;

  case Qt::Key_Right:
    if(((1<<(edit_section+1))&edit_display)!=0) {
      edit_section++;
      edit_digit=0;
      setFocus();
    }
    e->accept();
    break;

  case Qt::Key_Up:
    upClickedData();
    break;

  case Qt::Key_Down:
    downClickedData();
    break;

  default:
    e->ignore();
    break;
  }
}


void RDTimeEdit::focusInEvent(QFocusEvent *e)
{
  QFrame::focusInEvent(e);
}


void RDTimeEdit::focusOutEvent(QFocusEvent *e)
{
  QFrame::focusOutEvent(e);
  QPalette p=palette();
  for(int i=0;i<4;i++) {
    edit_labels[i]->setPalette(p);
  }
  QFrame::focusOutEvent(e);
}


void RDTimeEdit::GetSizeHint()
{
  QFontMetrics fm(font());
  if((edit_display&RDTimeEdit::Hours)==0) {
    edit_widths[0]=0;
    edit_sep_widths[0]=0;
  }
  else {
    edit_widths[0]=fm.width("00");
    edit_sep_widths[0]=fm.width(":");
  }
  if((edit_display&RDTimeEdit::Minutes)==0) {
    edit_widths[1]=0;
    edit_sep_widths[1]=0;
  }
  else {
    edit_widths[1]=fm.width("00");
    edit_sep_widths[1]=fm.width(":");
  }
  if((edit_display&RDTimeEdit::Seconds)==0) {
    edit_widths[2]=0;
    edit_sep_widths[2]=0;
  }
  else {
    edit_widths[2]=fm.width("00");
    edit_sep_widths[2]=fm.width(".");
  }
  if((edit_display&RDTimeEdit::Tenths)==0) {
    edit_widths[3]=0;
  }
  else {
    edit_widths[3]=fm.width("0");
  }
  edit_height=fm.ascent();
}


void RDTimeEdit::ProcessKey(int key)
{
  int ten;

  switch(edit_section) {
  case 0:
  case 1:
  case 2:
    if(edit_digit==0) {
      edit_labels[edit_section]->setText(QString().sprintf("0%c",key));
      edit_digit=1;
    }
    else {
      ten=edit_labels[edit_section]->text().toInt();
      if(ten>5) {
	ten=0;
      }
      edit_labels[edit_section]->setText(QString().sprintf("%d%c",ten,key));
    }
    break;

  case 3:
    edit_labels[3]->setText(QString().sprintf("%c",key));
    break;
  }
  emit valueChanged(time());
}
