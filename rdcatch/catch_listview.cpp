//   catch_listview.cpp
//
//   Events List Widget for RDCatch
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: catch_listview.cpp,v 1.5.6.1 2013/11/13 23:36:35 cvs Exp $
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

#include <qheader.h>

#include <rdcart.h>
#include <rdrecording.h>
#include <rdedit_audio.h>

#include <globals.h>
#include <catch_listview.h>


CatchListView::CatchListView(QWidget *parent,const char *name)
  : RDListView(parent,name)
{
  catch_parent=parent;

  //
  // Right-Click Menu
  //
  catch_menu=new QPopupMenu(NULL,"catch_menu");
  connect(catch_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  catch_menu->
    insertItem(tr("Edit Cue Markers"),this,SLOT(editAudioMenuData()),0,0);
}


void CatchListView::aboutToShowData()
{
  catch_menu->setItemEnabled(0,!catch_cutname.isEmpty());
}


void CatchListView::editAudioMenuData()
{
  RDCart *rdcart=new RDCart(catch_cutname.left(6).toUInt());
  RDEditAudio *edit=
    new RDEditAudio(rdcart,catch_cutname,catch_cae,catch_user,rdstation_conf,
		    catch_config,catch_audition_card,catch_audition_port,
		    1500,-400,this);
  if(edit->exec()!=-1) {
    rdcart->updateLength();
  }
  delete edit;
  delete rdcart;
}


void CatchListView::contentsMousePressEvent(QMouseEvent *e)
{
  QListView::contentsMousePressEvent(e);
  QListView::contentsMousePressEvent(e);
  catch_menu_item=selectedItem();
  if(catch_menu_item==NULL) {
    catch_cutname="";
  }
  else {
    switch(catch_menu_item->text(28).toUInt()) {
	case RDRecording::Recording:
	case RDRecording::Playout:
	case RDRecording::Upload:
	case RDRecording::Download:
	  catch_cutname=catch_menu_item->text(25);
	  break;

	case RDRecording::MacroEvent:
	case RDRecording::SwitchEvent:
	  catch_cutname="";
	  break;
    }
  }
  switch(e->button()) {
      case QMouseEvent::RightButton:
	catch_menu->setGeometry(e->globalX(),e->globalY(),
				catch_menu->sizeHint().width(),
				catch_menu->sizeHint().height());
	catch_menu->exec();
	break;

      default:
	e->ignore();
	break;
  }
}


void CatchListView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
  QListView::contentsMouseDoubleClickEvent(e);
}
