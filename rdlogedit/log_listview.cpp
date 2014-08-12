//   log_listview.cpp
//
//   The Log ListView widget for RDLogEdit.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: log_listview.cpp,v 1.10 2010/07/29 19:32:37 cvs Exp $
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
//

#include <qheader.h>

#include <log_listview.h>


LogListView::LogListView(QWidget *parent,const char *name)
  : RDListView(parent,name)
{
  log_parent=parent;

  //
  // Right Button Menu
  //
  log_menu=new QPopupMenu(this,"log_menu");
  connect(log_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  log_menu->insertItem(tr("PLAY Transition"),this,SLOT(playData()),0,0);
  log_menu->insertItem(tr("SEGUE Transition"),this,SLOT(segueData()),0,1);
  log_menu->insertItem(tr("STOP Transition"),this,SLOT(stopData()),0,2);
}


void LogListView::aboutToShowData()
{
  RDListViewItem *item=(RDListViewItem *)selectedItem();
  log_menu->setItemEnabled(0,item->line()!=TRACKER_MAX_LINENO);
  log_menu->setItemEnabled(1,item->line()!=TRACKER_MAX_LINENO);
  log_menu->setItemEnabled(2,item->line()!=TRACKER_MAX_LINENO);
  log_transition_type=item->text(2).lower();
  log_menu->setItemChecked(0,log_transition_type==tr("play"));
  log_menu->setItemChecked(1,log_transition_type==tr("segue"));
  log_menu->setItemChecked(2,log_transition_type==tr("stop"));
}


void LogListView::playData()
{
  if(log_transition_type=="play") {
    return;
  }
  emit transitionChanged(((RDListViewItem *)selectedItem())->line(),
			 RDLogLine::Play);
}


void LogListView::segueData()
{
  if(log_transition_type=="segue") {
    return;
  }
  emit transitionChanged(((RDListViewItem *)selectedItem())->line(),
			 RDLogLine::Segue);
}


void LogListView::stopData()
{
  if(log_transition_type=="stop") {
    return;
  }
  emit transitionChanged(((RDListViewItem *)selectedItem())->line(),
			 RDLogLine::Stop);
}


void LogListView::contentsMousePressEvent(QMouseEvent *e)
{
  QListView::contentsMousePressEvent(e);
  log_menu_item=(RDListViewItem *)selectedItem();
  switch(e->button()) {
      case QMouseEvent::RightButton:
	//log_menu->setGeometry(log_parent->geometry().x()+
	//			 geometry().x()+e->pos().x()+2,
	//			 log_parent->geometry().y()+
	//			 geometry().y()+e->pos().y()+
	//			 header()->geometry().height()+2,
	//			 log_menu->sizeHint().width(),
	//			 log_menu->sizeHint().height());
        log_menu->setGeometry(e->globalX(),e->globalY(),
				 log_menu->sizeHint().width(),
				 log_menu->sizeHint().height());

	log_menu->exec();
	break;

      default:
	e->ignore();
	break;
  }
}
