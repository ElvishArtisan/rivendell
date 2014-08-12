// rdpanel_button.cpp
//
// The SoundPanel Button for RDAirPlay.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpanel_button.cpp,v 1.26.6.9 2014/02/06 20:43:47 cvs Exp $
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

#include <rdconf.h>
#include <rdpanel_button.h>

#include <qpixmap.h>
#include <qpainter.h>

#include <rdcartdrag.h>

RDPanelButton::RDPanelButton(int row,int col,RDStation *station,bool flash,
			     QWidget *parent,const char *name)
  : QPushButton(parent,name)
{
  button_row=row;
  button_col=col;
  button_station=station;
  button_parent=parent;
  button_flash=flash;
  button_flash_state=false;
  button_hook_mode=false;
  button_move_count=-1;
  button_allow_drags=false;
  clear();
}


void RDPanelButton::clear()
{
  setText("");
  setOutputText("");
  setDefaultColor(button_parent->backgroundColor());
  setColor(button_parent->backgroundColor());
  setCart(0);
  button_deck=-1;
  button_output=-1;
  button_play_deck=NULL;
  button_start_time=QTime();
  button_end_time=QTime();
  button_active_length=0;
  button_length[0]=0;
  button_length[1]=0;
  button_secs=-1;
  button_flashing=false;
  button_flash_state=false;
  button_state=false;
  button_start_source=RDLogLine::StartUnknown;
  button_pause_when_finished=false;
  button_duck_volume=0;
}


QString RDPanelButton::text() const
{
  return button_text;
}


void RDPanelButton::setText(const QString &text)
{
  button_text=text;
  WriteKeycap(-1);
}


QString RDPanelButton::outputText() const
{
  return button_output_text;
}


void RDPanelButton::setOutputText(const QString &text)
{
  button_output_text=text;
}


bool RDPanelButton::state() const
{
  return button_state;
}


void RDPanelButton::setState(bool state)
{
  button_state=state;
}


QColor RDPanelButton::defaultColor() const
{
  return button_default_color;
}


void RDPanelButton::setDefaultColor(QColor color)
{
  if(color==button_default_color) {
    return;
  }
  button_default_color=color;
  if(playDeck()==NULL) {
    setColor(color);
  }
}


RDLogLine::StartSource RDPanelButton::startSource() const
{
  return button_start_source;
}


void RDPanelButton::setStartSource(RDLogLine::StartSource src)
{
  button_start_source=src;
}


void RDPanelButton::setColor(QColor color)
{
  if(color==button_color) {
    return;
  }
  button_color=color;
  if(button_flash) {
    if(color==button_parent->backgroundColor()) {
      button_flashing=false;
    }
    else {
      button_flashing=true;
    }
  }
  WriteKeycap(button_secs);
}


unsigned RDPanelButton::cart() const
{
  return button_cart;
}


void RDPanelButton::setCart(unsigned cart)
{
  button_cart=cart;
}


QString RDPanelButton::cutName() const
{
  return button_cutname;
}


void RDPanelButton::setCutName(const QString &name)
{
  button_cutname=name;
}


int RDPanelButton::deck() const
{
  return button_deck;
}


void RDPanelButton::setDeck(int deck)
{
  button_deck=deck;
}


RDPlayDeck *RDPanelButton::playDeck() const
{
  return button_play_deck;
}


void RDPanelButton::setPlayDeck(RDPlayDeck *deck)
{
  button_play_deck=deck;
}


QTime RDPanelButton::startTime() const
{
  return button_start_time;
}


void RDPanelButton::setStartTime(QTime time)
{
  button_start_time=time;
  button_end_time=button_start_time.addMSecs(button_active_length);
}


bool RDPanelButton::hookMode() const
{
  return button_hook_mode;
}


void RDPanelButton::setHookMode(bool state)
{
  button_hook_mode=state;
}


int RDPanelButton::output() const
{
  return button_output;
}


void RDPanelButton::setOutput(int outnum)
{
  button_output=outnum;
}


int RDPanelButton::length(bool hookmode) const
{
  return button_length[hookmode];
}


void RDPanelButton::setLength(bool hookmode, int msecs)
{
  button_length[hookmode]=msecs;
}


void RDPanelButton::setActiveLength(int msecs)
{
  button_active_length=msecs;
  button_end_time=button_start_time.addMSecs(button_active_length);
  if((button_start_time.isNull())&&(button_cart!=0)) {
    WriteKeycap(-1);
  }
}


void RDPanelButton::reset()
{
  if(button_length[button_hook_mode]>0) {
    setActiveLength(button_length[button_hook_mode]);
  }
  else {
    setActiveLength(button_length[0]);
  }
  button_start_time=QTime();
  button_end_time=QTime();
  setColor(button_default_color);
  if(button_cart!=0) {
    WriteKeycap(-1);
  }
  setDeck(-1);
  button_play_deck=NULL;
}


bool RDPanelButton::pauseWhenFinished() const
{
return button_pause_when_finished;
}


void RDPanelButton::setPauseWhenFinished(bool pause_when_finished)
{
  button_pause_when_finished=pause_when_finished;
}


void RDPanelButton::resetCounter()
{
  WriteKeycap(-1);
}


void RDPanelButton::setAllowDrags(bool state)
{
  button_allow_drags=state;
}


void RDPanelButton::setDuckVolume(int level)
{
	button_duck_volume=level;
}


int RDPanelButton::duckVolume() const
{
	return button_duck_volume;
}


void RDPanelButton::tickClock()
{
  int secs;
  QTime current_time=
    QTime::currentTime().addMSecs(button_station->timeOffset());
  if((button_start_time.isNull())||(current_time>button_end_time)||
     ((secs=current_time.secsTo(button_end_time))==button_secs)) {
    return;
    }
  button_secs=secs;
  WriteKeycap(secs);
}


void RDPanelButton::flashButton(bool state)
{
  if(state==button_flash_state) {
    return;
  }
  button_flash_state=state;
  if(button_flashing&&button_state) {
    WriteKeycap(button_secs);
  }
}


void RDPanelButton::keyPressEvent(QKeyEvent *e)
{
  QWidget::keyPressEvent(e);
}


void RDPanelButton::keyReleaseEvent(QKeyEvent *e)
{
  QWidget::keyReleaseEvent(e);
}


void RDPanelButton::mousePressEvent(QMouseEvent *e)
{
  button_move_count=3;
  QPushButton::mousePressEvent(e);
}


void RDPanelButton::mouseMoveEvent(QMouseEvent *e)
{
  button_move_count--;
  if(button_move_count==0) {
    QPushButton::mouseReleaseEvent(e);
    if(button_allow_drags) {
      RDCartDrag *d=new RDCartDrag(button_cart,button_text,button_color,this);
      d->dragCopy();
    }
  }
}


void RDPanelButton::mouseReleaseEvent(QMouseEvent *e)
{
  button_move_count=-1;
  QPushButton::mouseReleaseEvent(e);
}


void RDPanelButton::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e)&&button_allow_drags&&
  ((button_play_deck==NULL)||(button_play_deck->state()==RDPlayDeck::Stopped)));
}


void RDPanelButton::dropEvent(QDropEvent *e)
{
  unsigned cartnum;
  QColor color;
  QString title;

  if(RDCartDrag::decode(e,&cartnum,&color,&title)) {
    emit cartDropped(button_row,button_col,cartnum,color,title);
  }
}


void RDPanelButton::WriteKeycap(int secs)
{
  QString text=button_text;
  QPixmap *pix=new QPixmap(size().width(),size().height());
  QPainter *p=new QPainter(pix);
  if(button_state) {
    if(button_flash) {
      if(button_flash_state) {
	p->fillRect(0,0,size().width(),size().height(),button_color);
	p->setPen(RDGetTextColor(button_color));
      }
      else {
	p->fillRect(0,0,size().width(),size().height(),button_default_color);
	p->setPen(RDGetTextColor(button_default_color));
      }
    }
    else {
      p->fillRect(0,0,size().width(),size().height(),button_color);
      p->setPen(RDGetTextColor(button_color));
    }
  }
  else {
    p->fillRect(0,0,size().width(),size().height(),button_color);
    p->setPen(RDGetTextColor(button_color));
  }

  //
  // Button Title
  //
  QFont font("helvetica",13,QFont::Normal);
  font.setPixelSize(13);
  QFontMetrics m(font);
  p->setFont(font);
  p->drawText(RDPANEL_BUTTON_MARGIN,m.lineSpacing(),
	      GetNextLine(&text,m,size().width()-3*RDPANEL_BUTTON_MARGIN));
  p->drawText(RDPANEL_BUTTON_MARGIN,2*m.lineSpacing(),
	      GetNextLine(&text,m,size().width()-3*RDPANEL_BUTTON_MARGIN));
  p->drawText(RDPANEL_BUTTON_MARGIN,3*m.lineSpacing(),
	      GetNextLine(&text,m,size().width()-3*RDPANEL_BUTTON_MARGIN));

  //
  // Time Field & Output Text
  //
  if(!button_text.isEmpty()) {
    if(secs<0) {
      QFont out_font("helvetica",13,QFont::Normal);
      out_font.setPixelSize(13);
      p->setFont(out_font);
      if(button_pause_when_finished) {
        p->drawText(RDPANEL_BUTTON_MARGIN,size().height()-RDPANEL_BUTTON_MARGIN,"Finished");
        }
      else {
	if(button_active_length>=0) {
	  p->drawText(RDPANEL_BUTTON_MARGIN,size().height()-
		      RDPANEL_BUTTON_MARGIN,
		      RDGetTimeLength(button_active_length+1000,true,false));
	}
	else {
	  p->drawText(RDPANEL_BUTTON_MARGIN,size().height()-
		      RDPANEL_BUTTON_MARGIN,tr("No Audio"));
	}
      }
    }
    else {
      if(secs>8) {
        p->drawText(RDPANEL_BUTTON_MARGIN,size().height()-RDPANEL_BUTTON_MARGIN,
		  RDGetTimeLength(1000*(secs+1),true,false));
        }
      else {
        QFont out_font("helvetica",18,QFont::Bold);
        out_font.setPixelSize(18);
        p->setFont(out_font);
        QString secstr=QString().sprintf(":%d",secs+1);
        p->drawText(RDPANEL_BUTTON_MARGIN,size().height()-RDPANEL_BUTTON_MARGIN,secstr);
        }
      QFont out_font("helvetica",18,QFont::Bold);
      out_font.setPixelSize(18);
      p->setFont(out_font);
      QFontMetrics om(out_font);
      p->drawText(size().width()-om.width(button_output_text)-
		  RDPANEL_BUTTON_MARGIN,
		  size().height()-RDPANEL_BUTTON_MARGIN,button_output_text);
    }
  }
  p->end();
  setPixmap(*pix);
  delete p;
  delete pix;
}


QString RDPanelButton::WrapText(QString text,int *lines)
{
  QFontMetrics fm(font());
  QString str;
  QString residue = text;
  bool space_found=false;
  int l;

  *lines=0;
  if(!text.isEmpty()) {	  
    while(!residue.isEmpty()) {
      space_found=false;
      for(int i=(int)residue.length();i>=0;i--) {
	      if((i==((int)residue.length()))||(residue.at(i).isSpace())) {
	  if(fm.boundingRect(residue.left(i)).width()<=width()-6) {
	    space_found=true;
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++*lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
      if(!space_found) {
	l=residue.length();
	for(int i=l;i>=0;i--) {
	  if(fm.boundingRect(residue.left(i)).width()<=(width()-6)) {
	    if(!str.isEmpty()) {
	      str+="\n";
	      if(++*lines==3) {
		return str;
	      }
	    }
	    str+=residue.left(i);
	    if(i==(int)residue.length()) {
	      return str;
	    }
	    residue=residue.right(residue.length()-i-1);
	  }
	}
      }
    }
  }
  return text;
}


QString RDPanelButton::GetNextLine(QString *str,const QFontMetrics &m,int len)
{
  QString ret;

  for(unsigned i=0;i<str->length();i++) {
    if(m.width(str->left(i))>len) {
      int l=i;
      while((!str->at(l--).isSpace())&&(l>=0));
      if(l>0) {
	ret=str->left(l+1);
	*str=str->right(str->length()-l-1).stripWhiteSpace();
	return ret;
      }
    }
  }
  ret=*str;
  *str="";
  return ret;
}
