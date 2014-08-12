// rdbutton_panel.cpp
//
// The sound panel widget for RDAirPlay
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdbutton_panel.cpp,v 1.12.6.4 2014/02/06 20:43:47 cvs Exp $
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

#include <qcolor.h>

#include <rdbutton_panel.h>
#include <rdbutton_dialog.h>


RDButtonPanel::RDButtonPanel(RDAirPlayConf::PanelType type,int panel,int cols,
			     int rows,RDStation *station,bool flash,
			     QWidget *parent)
{
  panel_button_columns=cols;
  panel_button_rows=rows;
  panel_station=station;
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]=
	new RDPanelButton(i,j,panel_station,flash,parent);
      if(station->enableDragdrop()&&(!station->enforcePanelSetup())) {
	panel_button[i][j]->setAcceptDrops(true);
      }
      panel_button[i][j]->setGeometry((15+PANEL_BUTTON_SIZE_X)*j,
				      (15+PANEL_BUTTON_SIZE_Y)*i,
				      PANEL_BUTTON_SIZE_X,
				      PANEL_BUTTON_SIZE_Y);
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
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      delete panel_button[i][j];
    }
  }
}


QSize RDButtonPanel::sizeHint() const
{
  return QSize(500,535);
}


QSizePolicy RDButtonPanel::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


RDPanelButton *RDButtonPanel::panelButton(int row,int col) const
{
  return panel_button[row][col];
}


void RDButtonPanel::setActionMode(RDAirPlayConf::ActionMode mode)
{
  switch(mode) {
      case RDAirPlayConf::CopyFrom:
	for(int i=0;i<panel_button_rows;i++) {
	  for(int j=0;j<panel_button_columns;j++) {
	    if(panel_button[i][j]->cart()!=0) {
	      panel_button[i][j]->setColor(BUTTON_FROM_BACKGROUND_COLOR);
	    }
	  }
	}
	break;

      case RDAirPlayConf::CopyTo:
	      for(int i=0;i<panel_button_rows;i++) {
	        for(int j=0;j<panel_button_columns;j++) {
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
	      for(int i=0;i<panel_button_rows;i++) {
		for(int j=0;j<panel_button_columns;j++) {
             if(panel_button[i][j]->playDeck()==NULL) {
               panel_button[i][j]->setColor(BUTTON_TO_BACKGROUND_COLOR);
             }
	  }
	}
	break;

      case RDAirPlayConf::DeleteFrom:
	      for(int i=0;i<panel_button_rows;i++) {
	     for(int j=0;j<panel_button_columns;j++) {
             if(panel_button[i][j]->playDeck()==NULL) {
               panel_button[i][j]->setColor(BUTTON_FROM_BACKGROUND_COLOR);
             }
	  }
	}
	break;

      default:
	for(int i=0;i<panel_button_rows;i++) {
	for(int j=0;j<panel_button_columns;j++) {
//	    if(panel_button[i][j]->cart()!=0) {
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
//	    }
	  }
	}
	break;
  }
}


void RDButtonPanel::setAllowDrags(bool state)
{
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]->setAllowDrags(state);
    }
  }
}


void RDButtonPanel::setAcceptDrops(bool state)
{
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]->setAcceptDrops(state);
    }
  }
}


void RDButtonPanel::hide()
{
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]->hide();
    }
  }
}


void RDButtonPanel::show()
{
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]->show();
    }
  }
}


void RDButtonPanel::clear()
{
  for(int i=0;i<panel_button_rows;i++) {
    for(int j=0;j<panel_button_columns;j++) {
      panel_button[i][j]->clear();
    }
  }
}
