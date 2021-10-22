// rdmarkerreadout.cpp
//
// Read-only display for cut marker digital information
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdconf.h"
#include "rdmarkerreadout.h"

#define LABEL_DISABLED_COLOR QString("#888888")

RDMarkerReadout::RDMarkerReadout(RDMarkerHandle::PointerRole role,
				 QWidget *parent)
  : RDPushButton(parent)
{
  d_roles.push_back(role);
  d_selected_marker=RDMarkerHandle::LastRole;

  d_default_palette=palette();
  d_selected_palette=palette();
  d_selected_palette.
    setColor(QPalette::Window,d_selected_palette.color(QPalette::Highlight));
  d_selected_palette.
    setColor(QPalette::WindowText,
	     d_selected_palette.color(QPalette::HighlightedText));

  d_label=new QLabel(RDMarkerHandle::pointerRoleText(role),this);
  d_label->setAlignment(Qt::AlignCenter);
  d_label->setFont(labelFont());

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::HookStart:
    d_roles.push_back((RDMarkerHandle::PointerRole)(role+1));
    for(int i=0;i<3;i++) {
      d_edits.push_back(new QLabel(this));
      d_edits.back()->setFrameShape(QFrame::Box);
      d_edits.back()->setFrameShadow(QFrame::Sunken);
      d_edits.back()->setFont(defaultFont());
      d_edits.back()->setAlignment(Qt::AlignCenter);
      d_edits.back()->setText("0:00:00");
    }
  break;

  case RDMarkerHandle::FadeUp:
  case RDMarkerHandle::FadeDown:
    for(int i=0;i<2;i++) {
      d_edits.push_back(new QLabel(this));
      d_edits.back()->setFrameShape(QFrame::Box);
      d_edits.back()->setFrameShadow(QFrame::Sunken);
      d_edits.back()->setFont(defaultFont());
      d_edits.back()->setAlignment(Qt::AlignCenter);
      d_edits.back()->setText("0:00:00");
    }
  break;

  case RDMarkerHandle::CutEnd:
  case RDMarkerHandle::TalkEnd:
  case RDMarkerHandle::LastRole:
  case RDMarkerHandle::SegueEnd:
  case RDMarkerHandle::HookEnd:
  break;
  }
  setDisabled(true);
}


RDMarkerReadout::~RDMarkerReadout()
{
  for(int i=0;i<d_edits.size();i++) {
    delete d_edits.at(i);
  }
  d_edits.clear();
  delete d_label;
}


QSize RDMarkerReadout::sizeHint() const
{
  return QSize(65,21+18*d_edits.size());
}


QSizePolicy RDMarkerReadout::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDMarkerReadout::setValue(RDMarkerHandle::PointerRole role,int value)
{
  if(d_pointers[role]!=value) {
    d_pointers[role]=value;
    for(int i=0;i<d_roles.size();i++) {
      if(d_roles.at(i)==role) {
	if(value>=0) {
	  d_edits.at(i)->setText(RDGetTimeLength(value,true,true));
	}
	else {
	  d_edits.at(i)->setText("0:00:00");
	}
	setEnabled(value>=0);
      }
    }
    switch(d_roles.first()) {
    case RDMarkerHandle::CutStart:
    case RDMarkerHandle::TalkStart:
    case RDMarkerHandle::SegueStart:
    case RDMarkerHandle::HookStart:
      if((d_pointers[d_roles.first()]<0)||(d_pointers[d_roles.last()]<0)) {
	d_edits.last()->setText("0:00:00");
      }
      else {
	d_edits.last()->setText(RDGetTimeLength(d_pointers[d_roles.last()]-d_pointers[d_roles.first()],true,true));
      }
      break;

    case RDMarkerHandle::FadeUp:
      if(d_pointers[d_roles.first()]<0) {
	d_edits.last()->setText("0:00:00");
      }
      else {
	d_edits.last()->setText(RDGetTimeLength(d_pointers[d_roles.first()]-d_pointers[RDMarkerHandle::CutStart],true,true));
      }
      break;

    case RDMarkerHandle::FadeDown:
      if(d_pointers[d_roles.first()]<0) {
	d_edits.last()->setText("0:00:00");
      }
      else {
	d_edits.last()->setText(RDGetTimeLength(d_pointers[RDMarkerHandle::CutEnd]-d_pointers[d_roles.first()],true,true));
      }
      break;

    case RDMarkerHandle::CutEnd:
    case RDMarkerHandle::TalkEnd:
    case RDMarkerHandle::LastRole:
    case RDMarkerHandle::SegueEnd:
    case RDMarkerHandle::HookEnd:
      break;
    }
  }
}


void RDMarkerReadout::setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
					 RDMarkerHandle::PointerRole end_role)
{
  if(d_roles.contains(start_role)||d_roles.contains(end_role)) {
    for(int i=0;i<d_edits.size();i++) {
      d_edits.at(i)->setPalette(d_selected_palette);
      d_edits.at(i)->
	setStyleSheet("color:"+
		     d_selected_palette.color(QPalette::HighlightedText).name()+
		     ";background-color:"+
		     d_selected_palette.color(QPalette::Window).name());
    }
  }
  else {
    for(int i=0;i<d_edits.size();i++) {
      d_edits.at(i)->setPalette(d_default_palette);
      d_edits.at(i)->setStyleSheet("");
    }
  }
}


void RDMarkerReadout::setEnabled(bool state)
{
  if(state) {
    d_label->
      setStyleSheet("background-color: "+
		    RDMarkerHandle::
		    pointerRoleColor(d_roles.first()).name()+";"+
		    "color: "+
		    RDGetTextColor(RDMarkerHandle::
				   pointerRoleColor(d_roles.first())).name());
    setCursor(Qt::PointingHandCursor);
  }
  else {
    d_label->setStyleSheet("background-color: "+LABEL_DISABLED_COLOR);
    unsetCursor();
  }
  for(int i=0;i<d_edits.size();i++) {
    if(!state) {
      d_edits.at(i)->setStyleSheet("");
    }
    d_edits.at(i)->setEnabled(state);
  }
  QPushButton::setEnabled(state);
}


void RDMarkerReadout::setDisabled(bool state)
{
  setEnabled(!state);
}


void RDMarkerReadout::resizeEvent(QResizeEvent *)
{
  int w=size().width();

  d_label->setGeometry(1,0,w-2,20);
  for(int i=0;i<d_edits.size();i++) {
    d_edits.at(i)->setGeometry(0,20+i*18,w,20);
  }
}
