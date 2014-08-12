//   import_listview.cpp
//
//   The Import Carts ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import_listview.cpp,v 1.21.8.2 2013/12/30 19:56:13 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
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

#include <qdragobject.h>
#include <qheader.h>

#include <rdcart.h>
#include <rdconf.h>
#include <rdcartdrag.h>

#include <edit_note.h>
#include <edit_track.h>

#include <import_listview.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/mic16.xpm"
#include "../icons/notemarker.xpm"


ImportListView::ImportListView(QWidget *parent,const char *name)
  : QListView(parent,name)
{
  import_parent=parent;

  //
  // Create Icons
  //
  import_playout_map=new QPixmap(play_xpm);
  import_macro_map=new QPixmap(rml5_xpm);
  import_notemarker_map=new QPixmap(notemarker_xpm);
  import_mic16_map=new QPixmap(mic16_xpm);

  //
  // Right Button Menu
  //
  import_menu=new QPopupMenu(this,"import_menu");
  connect(import_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  import_menu->
    insertItem(tr("Insert Log Note"),this,SLOT(insertNoteMenuData()),0,0);
  import_menu->
    insertItem(tr("Edit Log Note"),this,SLOT(editNoteMenuData()),0,1);
  import_menu->insertSeparator();
  import_menu->
    insertItem(tr("Insert Voice Track"),this,SLOT(insertTrackMenuData()),0,2);
  import_menu->
    insertItem(tr("Edit Voice Track"),this,SLOT(editTrackMenuData()),0,3);
  import_menu->insertSeparator();
  import_menu->
    insertItem(tr("Set PLAY Transition"),this,SLOT(playMenuData()),0,4);
  import_menu->
    insertItem(tr("Set SEGUE Transition"),this,SLOT(segueMenuData()),0,5);
  import_menu->
    insertItem(tr("Set STOP Transition"),this,SLOT(stopMenuData()),0,6);
  import_menu->insertSeparator();
  import_menu->
    insertItem(tr("Delete"),this,SLOT(deleteMenuData()),0,8);

  import_force_trans=RDLogLine::NoTrans;
  import_allow_stop=true;
  import_allow_first_trans=true;
  import_log=new RDLogEvent();

  setAcceptDrops(true);
}


void ImportListView::setForceTrans(RDLogLine::TransType trans)
{
  import_force_trans=trans;
  validateTransitions();
}


void ImportListView::setAllowStop(bool state)
{
  import_allow_stop=state;
}


void ImportListView::setAllowFirstTrans(bool state)
{
  import_allow_first_trans=state;
}


RDLogEvent *ImportListView::logEvent()
{
  return import_log;
}


void ImportListView::refreshList(int line)
{
  QListViewItem *item;
  QListViewItem *select_item=NULL;
  QString sql;
  RDLogLine *logline;
  int total_len=0;

  clear();
  for(int i=import_log->size()-1;i>=0;i--) {
    item=new QListViewItem(this);
    if((logline=import_log->logLine(i))!=NULL) {
      switch(logline->type()) {
	  case RDLogLine::Cart:
	    item->setPixmap(0,*import_playout_map);
	    item->setText(1,QString().sprintf("%06u",logline->cartNumber()));
	    item->setText(2,logline->groupName());
	    item->setText(4,logline->title());
	    break;
	    
	  case RDLogLine::Macro:
	    item->setPixmap(0,*import_macro_map);
	    item->setText(1,QString().sprintf("%06u",logline->cartNumber()));
	    item->setText(2,logline->groupName());
	    item->setText(4,logline->title());
	    break;

	  case RDLogLine::Marker:
	    item->setPixmap(0,*import_notemarker_map);
	    item->setText(2,tr("Marker"));
	    item->setText(4,tr("[Log Note]"));
	    break;

	  case RDLogLine::Track:
	    item->setPixmap(0,*import_mic16_map);
	    item->setText(2,tr("Track"));
	    item->setText(4,tr("[Voice Track]"));
	    break;

	  default:
	    break;
      }
      item->setText(3,RDGetTimeLength(logline->forcedLength(),false,false));
      total_len+=logline->forcedLength();
      switch(logline->transType()) {
	  case RDLogLine::Play:
	    item->setText(5,tr("PLAY"));
	    break;

	  case RDLogLine::Segue:
	    item->setText(5,tr("SEGUE"));
	    break;

	  case RDLogLine::Stop:
	    item->setText(5,tr("STOP"));
	    break;

	  default:
	    break;
      }
      item->setText(6,QString().sprintf("%d",i));
      if(i==line) {
	select_item=item;
      }
    }
  }
  if(select_item!=NULL) {
    setSelected(select_item,true);
    ensureItemVisible(select_item);
  }
  emit lengthChanged(total_len);
}


void ImportListView::validateTransitions()
{
  if(logEvent()->size()>0) {
    if(import_force_trans!=RDLogLine::NoTrans) {
      logEvent()->logLine(0)->setTransType(import_force_trans);
    }
    else {
      if((logEvent()->logLine(0)->transType()==RDLogLine::Stop)&&
	 (!import_allow_stop)) {
	logEvent()->logLine(0)->setTransType(RDLogLine::Segue);
      }
    }
  }
  for(int i=1;i<logEvent()->size();i++) {
    if(logEvent()->logLine(i)->transType()==RDLogLine::Stop) {
      logEvent()->logLine(1)->setTransType(RDLogLine::Segue);
    }
  }
}


void ImportListView::aboutToShowData()
{
  if(import_menu_item==NULL) {
    import_menu->setItemChecked(0,false);
    import_menu->setItemEnabled(0,true);
    import_menu->setItemChecked(1,false);
    import_menu->setItemEnabled(1,false);
    import_menu->setItemChecked(2,false);
    import_menu->setItemEnabled(2,true);
    import_menu->setItemChecked(3,false);
    import_menu->setItemEnabled(3,false);
    import_menu->setItemChecked(4,false);
    import_menu->setItemEnabled(4,false);
    import_menu->setItemChecked(5,false);
    import_menu->setItemEnabled(5,false);
    import_menu->setItemChecked(6,false);
    import_menu->setItemEnabled(6,false);
    import_menu->setItemChecked(7,false);
    import_menu->setItemEnabled(7,false);
    import_menu->setItemChecked(8,false);
    import_menu->setItemEnabled(8,false);
    return;
  }
  if(import_menu_logline->type()==RDLogLine::Marker) {
    import_menu->setItemEnabled(1,true);
  }
  else {
    import_menu->setItemEnabled(1,false);
  }
  if(import_menu_logline->type()==RDLogLine::Track) {
    import_menu->setItemEnabled(3,true);
  }
  else {
    import_menu->setItemEnabled(3,false);
  }
  import_menu->setItemChecked(4,false);
  import_menu->setItemChecked(5,false);
  import_menu->setItemChecked(6,false);
  import_menu->setItemChecked(7,false);
  if(import_menu_line==0) {
    import_menu->setItemEnabled(4,import_allow_first_trans);
    import_menu->setItemEnabled(5,import_allow_first_trans);
    import_menu->setItemEnabled(7,import_allow_first_trans);
    if((import_menu_line==0)&&import_allow_stop&&import_allow_first_trans) {
//      import_menu->setItemEnabled(4,true);
      import_menu->setItemEnabled(6,true);
    }
    else {
      import_menu->setItemEnabled(6,false);
    }
  }
  else {
    import_menu->setItemEnabled(4,true);
    import_menu->setItemEnabled(5,true);
    import_menu->setItemEnabled(7,true);
    if((import_menu_line==0)&&import_allow_stop) {
      import_menu->setItemEnabled(6,true);
      import_menu->setItemEnabled(6,true);
    }
    else {
      import_menu->setItemEnabled(6,false);
    }
  }
  switch(import_menu_logline->transType()) {
      case RDLogLine::Play:
	import_menu->setItemChecked(4,true);
	break;

      case RDLogLine::Segue:
	import_menu->setItemChecked(5,true);
	break;

      case RDLogLine::Stop:
	import_menu->setItemChecked(6,true);
	break;

      default:
	break;
  }
  import_menu->setItemEnabled(8,true);
}


void ImportListView::insertNoteMenuData()
{
  QString note;
  EditNote *note_dialog=new EditNote(&note,this,"note_dialog");
  if(note_dialog->exec()<0) {
    delete note_dialog;
    return;
  }
  delete note_dialog;
  if(import_menu_item==NULL) {
    import_menu_line=0;
  }
  import_log->insert(import_menu_line,1);
  import_log->logLine(import_menu_line)->setType(RDLogLine::Marker);
  import_log->logLine(import_menu_line)->setMarkerComment(note);
  import_log->logLine(import_menu_line)->setTitle(tr("[Log Note]"));
  import_log->logLine(import_menu_line)->setTransType(RDLogLine::Segue);
  validateTransitions();
  refreshList(import_menu_line);
  emit sizeChanged(childCount());
}


void ImportListView::editNoteMenuData()
{
  QString text=import_log->logLine(import_menu_line)->markerComment();
  EditNote *edit_dialog=new EditNote(&text,this,"edit_dialog");
  if(edit_dialog->exec()<0) {
    delete edit_dialog;
    return;
  }
  import_log->logLine(import_menu_line)->setMarkerComment(text);  
}


void ImportListView::insertTrackMenuData()
{
  QString note;
  EditTrack *track_dialog=new EditTrack(&note,this,"track_dialog");
  if(track_dialog->exec()<0) {
    delete track_dialog;
    return;
  }
  delete track_dialog;
  if(import_menu_item==NULL) {
    import_menu_line=0;
  }
  import_log->insert(import_menu_line,1);
  import_log->logLine(import_menu_line)->setType(RDLogLine::Track);
  import_log->logLine(import_menu_line)->setMarkerComment(note);
  import_log->logLine(import_menu_line)->setTitle(tr("[Voice Track]"));
  import_log->logLine(import_menu_line)->setTransType(RDLogLine::Segue);
  validateTransitions();
  refreshList(import_menu_line);
  emit sizeChanged(childCount());
}


void ImportListView::editTrackMenuData()
{
  QString text=import_log->logLine(import_menu_line)->markerComment();
  EditTrack *edit_dialog=new EditTrack(&text,this,"edit_dialog");
  if(edit_dialog->exec()<0) {
    delete edit_dialog;
    return;
  }
  import_log->logLine(import_menu_line)->setMarkerComment(text);  
}


void ImportListView::playMenuData()
{
  import_menu_logline->setTransType(RDLogLine::Play);
  import_menu_item->setText(5,tr("PLAY"));
}


void ImportListView::segueMenuData()
{
  import_menu_logline->setTransType(RDLogLine::Segue);
  import_menu_item->setText(5,tr("SEGUE"));
}


void ImportListView::stopMenuData()
{
  import_menu_logline->setTransType(RDLogLine::Stop);
  import_menu_item->setText(5,tr("STOP"));
}


void ImportListView::deleteMenuData()
{
  import_log->remove(import_menu_item->text(6).toInt(),1);
  validateTransitions();
  refreshList();
  emit sizeChanged(childCount());
}


void ImportListView::contentsMousePressEvent(QMouseEvent *e)
{
  QListView::contentsMousePressEvent(e);
  import_menu_item=selectedItem();
  if(import_menu_item==NULL) {
    import_menu_logline=NULL;
  }
  else {
    if((import_menu_logline=import_log->
	logLine(import_menu_line=import_menu_item->text(6).toInt()))==NULL) {
      return;
    }
  }
  switch(e->button()) {
      case QMouseEvent::RightButton:
	import_menu->setGeometry(import_parent->geometry().x()+
				 geometry().x()+e->pos().x()+2,
				 import_parent->geometry().y()+
				 geometry().y()+e->pos().y()+
				 header()->geometry().height()+2,
				 import_menu->sizeHint().width(),
				 import_menu->sizeHint().height());
	import_menu->exec();
	break;

      default:
	e->ignore();
	break;
  }
}


void ImportListView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
  QListView::contentsMouseDoubleClickEvent(e);
  import_menu_item=selectedItem();
  if(import_menu_item==NULL) {
    return;
  }
  if(import_log->
     logLine(import_menu_item->text(6).toInt())->type()==RDLogLine::Marker) {
    editNoteMenuData();
  }
}


void ImportListView::focusOutEvent(QFocusEvent *e)
{
  QListViewItem *item=selectedItem();
  if(item==NULL) {
    return;
  }
  setSelected(item,false);
}


void ImportListView::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void ImportListView::dropEvent(QDropEvent *e)
{
  QListViewItem *item;
  unsigned cartnum;
  int line=0;
  QPoint pos(e->pos().x(),e->pos().y()-header()->sectionRect(0).height());

  if(RDCartDrag::decode(e,&cartnum)) {
    if(cartnum==0) {
      if((item=itemAt(pos))==NULL) {
	return;
      }
      import_log->remove(item->text(6).toInt(),1);
    }
    else {
      if((item=itemAt(pos))==NULL) {
	line=childCount();
      }
      else {
	line=item->text(6).toInt();
      }
      import_log->insert(line,1);
      import_log->logLine(line)->
	loadCart(cartnum,RDLogLine::Segue,0,false);
      import_log->logLine(line)->setTransType(RDLogLine::Segue);
    }
  }
  validateTransitions();
  refreshList(line);
  emit sizeChanged(childCount());
}
