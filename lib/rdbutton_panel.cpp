// rdbutton_panel.cpp
//
// Component class for sound panel widgets
//
//   (C) Copyright 2002-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <QColor>
#include <QJsonArray>
#include <QJsonObject>

#include "rdbutton_dialog.h"
#include "rdbutton_panel.h"

RDButtonPanel::RDButtonPanel(RDAirPlayConf::PanelType type,int number,
			     const QString &title,QWidget *parent)
  : RDWidget(parent)
{
  panel_type=type;
  panel_number=number;
  panel_title=title;

  panel_button_mapper=new QSignalMapper(this);
  connect(panel_button_mapper,SIGNAL(mapped(int)),
	  this,SLOT(buttonClickedData(int)));

  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]=new RDPanelButton(i,j,rda->station(),
					   rda->panelConf()->flashPanel(),this);
      int id=(number*PANEL_MAX_BUTTON_COLUMNS*PANEL_MAX_BUTTON_ROWS)+
	(i*PANEL_MAX_BUTTON_COLUMNS)+j;
      connect(panel_button[i][j],SIGNAL(clicked()),
	      panel_button_mapper,SLOT(map()));
      panel_button_mapper->setMapping(panel_button[i][j],id);
      if(rda->station()->enableDragdrop()) {
	panel_button[i][j]->setAllowDrags(true);
	if(!rda->station()->enforcePanelSetup()) {
	  panel_button[i][j]->setAcceptDrops(true);
	}
      }
      panel_button[i][j]->hide();
      parent->connect(parent,SIGNAL(buttonFlash(bool)),
		      panel_button[i][j],SLOT(flashButton(bool)));
      QObject::connect(panel_button[i][j],
	 SIGNAL(cartDropped(int,int,unsigned,const QColor &,const QString &)),
	 parent,
	 SLOT(acceptCartDrop(int,int,unsigned,const QColor &,const QString &)));
    }
  }
  clear();
}


RDButtonPanel::~RDButtonPanel()
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      delete panel_button[i][j];
    }
  }
  delete panel_button_mapper;
}


QSize RDButtonPanel::sizeHint() const
{
  return QSize(500,535);
}


QSizePolicy RDButtonPanel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


RDAirPlayConf::PanelType RDButtonPanel::panelType() const
{
  return panel_type;
}


int RDButtonPanel::number() const
{
  return panel_number;
}


QString RDButtonPanel::title() const
{
  return panel_title;
}


void RDButtonPanel::setTitle(const QString &str)
{
  panel_title=str;
}


RDPanelButton *RDButtonPanel::panelButton(int row,int col) const
{
  return panel_button[row][col];
}


void RDButtonPanel::setActionMode(RDAirPlayConf::ActionMode mode)
{
  switch(mode) {
      case RDAirPlayConf::CopyFrom:
	for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
	  for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
	    if(panel_button[i][j]->cart()!=0) {
	      panel_button[i][j]->setColor(BUTTON_FROM_BACKGROUND_COLOR);
	    }
	  }
	}
	break;

      case RDAirPlayConf::CopyTo:
	      for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
	        for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
	      if(panel_button[i][j]->playDeck()!=NULL) {
                if(panel_button[i][j]->playDeck()->state()==RDPlayDeck::Paused) {
		  panel_button[i][j]->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
                  }
                else {
                  panel_button[i][j]->setColor(RDPANEL_PLAY_BACKGROUND_COLOR);
                  }
	        }
	      else {
                panel_button[i][j]->setColor(BUTTON_TO_BACKGROUND_COLOR);
                }
	   }
	}
	break;

      case RDAirPlayConf::AddTo:
	      for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
		for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
             if(panel_button[i][j]->playDeck()==NULL) {
               panel_button[i][j]->setColor(BUTTON_TO_BACKGROUND_COLOR);
             }
	  }
	}
	break;

      case RDAirPlayConf::DeleteFrom:
	      for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
	     for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
             if(panel_button[i][j]->playDeck()==NULL) {
               panel_button[i][j]->setColor(BUTTON_FROM_BACKGROUND_COLOR);
             }
	  }
	}
	break;

      default:
	for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
	  for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
	    if(panel_button[i][j]->playDeck()!=NULL) {
	      if(panel_button[i][j]->playDeck()->state()==RDPlayDeck::Paused) {
		panel_button[i][j]->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
	      }
	      else {
		panel_button[i][j]->setColor(RDPANEL_PLAY_BACKGROUND_COLOR);
	      }
	    }
	    else {
	      if(panel_button[i][j]->state()) {
		panel_button[i][j]->setColor(RDPANEL_PAUSED_BACKGROUND_COLOR);
	      }
	      else {
		panel_button[i][j]->reset();
	      }
	    }
	  }
	}
	break;
  }
}


void RDButtonPanel::setAllowDrags(bool state)
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]->setAllowDrags(state);
    }
  }
}


void RDButtonPanel::setAcceptDrops(bool state)
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]->setAcceptDrops(state);
    }
  }
}


void RDButtonPanel::clear()
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]->clear();
    }
  }
}


QJsonValue RDButtonPanel::json() const
{
  QJsonArray ja0;

  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      if(!panel_button[i][j]->isEmpty()) {
	ja0.insert(ja0.count(),panel_button[i][j]->json());
      }
    }
  }
  QJsonObject jo0;
  if(panel_type==RDAirPlayConf::UserPanel) {
    jo0.insert("owner",rda->user()->name());
  }
  else {
    jo0.insert("owner",QJsonValue());
  }
  if(panel_title.isEmpty()) {
    jo0.insert("title",QJsonValue());
  }
  else {
    jo0.insert("title",panel_title);
  }
  jo0.insert("buttons",ja0);

  return jo0;
}


void RDButtonPanel::setVisible(bool state)
{
  RDWidget::setVisible(state);
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]->setVisible(state);
    }
  }
  if(state) {
    UpdateViewport();
  }
}


void RDButtonPanel::buttonClickedData(int id)
{
  int pnum=id/(PANEL_MAX_BUTTON_COLUMNS*PANEL_MAX_BUTTON_ROWS);
  int pos=id%(PANEL_MAX_BUTTON_COLUMNS*PANEL_MAX_BUTTON_ROWS);

  emit buttonClicked(pnum,pos%PANEL_MAX_BUTTON_COLUMNS,
		     pos/PANEL_MAX_BUTTON_COLUMNS);
}


void RDButtonPanel::resizeEvent(QResizeEvent *e)
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      panel_button[i][j]->setGeometry((14+PANEL_BUTTON_SIZE_X)*j,
				      (14+PANEL_BUTTON_SIZE_Y)*i,
				      PANEL_BUTTON_SIZE_X,
				      PANEL_BUTTON_SIZE_Y);
    }
  }
  UpdateViewport();
}


void RDButtonPanel::UpdateViewport()
{
  for(int i=0;i<PANEL_MAX_BUTTON_ROWS;i++) {
    for(int j=0;j<PANEL_MAX_BUTTON_COLUMNS;j++) {
      RDPanelButton *button=panel_button[i][j];
      button->setVisible(geometry().contains(button->geometry()));
    }
  }
}
