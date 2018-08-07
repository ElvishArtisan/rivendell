//   catch_listview.cpp
//
//   Events List Widget for RDCatch
//
//   (C) Copyright 2002-2006,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <q3header.h>
//Added by qt3to4:
#include <Q3PopupMenu>
#include <QMouseEvent>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdedit_audio.h>
#include <rdrecording.h>

#include "catch_listview.h"
#include "globals.h"

CatchListView::CatchListView(QWidget *parent)
  : RDListView(parent)
{
  catch_parent=parent;

  //
  // Right-Click Menu
  //
  catch_menu=new Q3PopupMenu(NULL);
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
    new RDEditAudio(rdcart,catch_cutname,catch_audition_card,
		    catch_audition_port,1500,-400,this);
  if(edit->exec()!=-1) {
    rdcart->updateLength();
  }
  delete edit;
  delete rdcart;
}


void CatchListView::contentsMousePressEvent(QMouseEvent *e)
{
  Q3ListView::contentsMousePressEvent(e);
  Q3ListView::contentsMousePressEvent(e);
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
  case Qt::RightButton:
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
  Q3ListView::contentsMouseDoubleClickEvent(e);
}
