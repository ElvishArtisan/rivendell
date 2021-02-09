// importcartsview.cpp
//
// RDTableView widget with consistent Rivendell defaults
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

#include <QDragEnterEvent>
#include <QHeaderView>

#include <rdcartdrag.h>

#include "edit_note.h"
#include "edit_track.h"
#include "importcartsmodel.h"
#include "importcartsview.h"

ImportCartsView::ImportCartsView(QWidget *parent)
  :RDTableView(parent)
{
  d_mouse_row=-1;

  setAcceptDrops(true);

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);

  d_insert_note_action=d_mouse_menu->
    addAction(tr("Insert Log Note"),this,SLOT(insertNoteData()));
  d_insert_note_action->setCheckable(false);

  d_insert_track_action=d_mouse_menu->
    addAction(tr("Insert Voice Track"),this,SLOT(insertTrackData()));
  d_insert_track_action->setCheckable(false);

  d_edit_action=d_mouse_menu->
    addAction(tr("Edit"),this,SLOT(editData()));
  d_edit_action->setCheckable(false);

  d_delete_action=d_mouse_menu->
    addAction(tr("Delete"),this,SLOT(deleteData()));
  d_delete_action->setCheckable(false);

  d_mouse_menu->addSeparator();

  d_set_play_action=d_mouse_menu->
    addAction(tr("PLAY Transition"),this,SLOT(setPlayData()));
  d_set_play_action->setCheckable(true);

  d_set_segue_action=d_mouse_menu->
    addAction(tr("SEGUE Transition"),this,SLOT(setSegueData()));
  d_set_segue_action->setCheckable(true);

  d_mouse_menu->addSeparator();

  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));
}


void ImportCartsView::aboutToShowMenuData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->lineCount())) {
    d_insert_note_action->setEnabled(true);
    d_insert_track_action->setEnabled(true);
    d_edit_action->setEnabled(false);
    d_delete_action->setEnabled(false);

    d_set_play_action->setEnabled(false);
    d_set_segue_action->setEnabled(false);
    return;
  }

  QModelIndex row=mod->index(d_mouse_row,0);

  d_insert_note_action->setEnabled(true);
  d_insert_track_action->setEnabled(true);
  d_edit_action->setEnabled((mod->eventType(row)==RDLogLine::Marker)||
			    (mod->eventType(row)==RDLogLine::Track));
  d_delete_action->setEnabled(true);

  d_set_play_action->
    setDisabled((d_mouse_row==0)&&(mod->firstTransitionIsAuto()));
  d_set_play_action->setChecked(mod->transType(row)==RDLogLine::Play);

  d_set_segue_action->
    setDisabled((d_mouse_row==0)&&(mod->firstTransitionIsAuto()));
  d_set_segue_action->setChecked(mod->transType(row)==RDLogLine::Segue);
}


void ImportCartsView::insertNoteData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    return;
  }
  QString text;
  EditNote *d=new EditNote(&text,this);
  if(d->exec()) {
    mod->addMarker(mod->index(d_mouse_row,0),text);
  }
  delete d;
}


void ImportCartsView::insertTrackData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    return;
  }
  QString text;
  EditTrack *d=new EditTrack(&text,this);
  if(d->exec()) {
    mod->addTrack(mod->index(d_mouse_row,0),text);
  }
  delete d;
}


void ImportCartsView::editData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();
  QString text;
  EditTrack *track;
  EditNote *note;

  if((d_mouse_row<0)||(d_mouse_row>=mod->lineCount())) {
    return;
  }
  switch(mod->eventType(mod->index(d_mouse_row,0))) {
  case RDLogLine::Track:
    text=mod->markerComments(mod->index(d_mouse_row,0));
    track=new EditTrack(&text,this);
    if(track->exec()) {
      mod->setMarkerComments(mod->index(d_mouse_row,0),text);
    }
    delete track;
    break;

  case RDLogLine::Marker:
    text=mod->markerComments(mod->index(d_mouse_row,0));
    note=new EditNote(&text,this);
    if(note->exec()) {
      mod->setMarkerComments(mod->index(d_mouse_row,0),text);
    }
    delete note;
    break;

  case RDLogLine::Cart:
  case RDLogLine::Macro:
  case RDLogLine::OpenBracket:
  case RDLogLine::CloseBracket:
  case RDLogLine::Chain:
  case RDLogLine::MusicLink:
  case RDLogLine::TrafficLink:
  case RDLogLine::UnknownType:
    break;
  }    
}


void ImportCartsView::deleteData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->lineCount())) {
    return;
  }
  mod->removeItem(mod->index(d_mouse_row,0));
}


void ImportCartsView::setPlayData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->lineCount())) {
    return;
  }
  mod->setTransType(mod->index(d_mouse_row,0),RDLogLine::Play);
}


void ImportCartsView::setSegueData()
{
  ImportCartsModel *mod=(ImportCartsModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->lineCount())) {
    return;
  }
  mod->setTransType(mod->index(d_mouse_row,0),RDLogLine::Segue);
}


void ImportCartsView::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void ImportCartsView::dragMoveEvent(QDragMoveEvent *e)
{
}


void ImportCartsView::dropEvent(QDropEvent *e)
{
  RDLogLine ll;
  int line=-1;
  int y_pos=e->pos().y();

  if(RDCartDrag::decode(e,&ll)) {
    line=rowAt(y_pos);
    emit cartDropped(line,&ll);
  }
}


void ImportCartsView::mousePressEvent(QMouseEvent *e)
{
  if(e->button()==Qt::RightButton) {
    d_mouse_row=indexAt(e->pos()).row();
    if((d_mouse_row>=0)&&(d_mouse_row<model()->rowCount())) {
      d_mouse_menu->popup(e->globalPos());
    }
    else {
      d_mouse_row=-1;
    }
  }
  QTableView::mousePressEvent(e);
}
