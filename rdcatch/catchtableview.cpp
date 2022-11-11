//   catchtableview.cpp
//
//   Events List Widget for RDCatch
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

#include <QMouseEvent>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdcut.h>

#include "catchtableview.h"
#include "recordlistmodel.h"

CatchTableView::CatchTableView(QWidget *parent)
  : RDTableView(parent)
{
  d_mouse_row=-1;
  d_cue_card=rda->station()->cueCard();
  d_cue_port=rda->station()->cuePort();

  //
  // Dialogs
  //
  d_marker_dialog=new RDMarkerDialog("RDCatch",d_cue_card,d_cue_port,this);

  //
  // Mouse menu
  //
  d_mouse_menu=new QMenu(this);

  d_edit_audio_action=d_mouse_menu->
    addAction(tr("Edit Cue Markers"),this,SLOT(editAudioMenuData()));
  d_edit_audio_action->setCheckable(false);

  connect(d_mouse_menu,SIGNAL(aboutToShow()),
	  this,SLOT(aboutToShowMenuData()));
}


void CatchTableView::aboutToShowMenuData()
{
  RecordListModel *mod=(RecordListModel *)model();

  if((d_mouse_row<0)||(d_mouse_row>=mod->rowCount())) {
    d_edit_audio_action->setEnabled(false);
    return;
  }
  d_edit_audio_action->
    setEnabled((!mod->cutName(mod->index(d_mouse_row,0)).isEmpty())&&
	       (d_cue_card>=0)&&(d_cue_port>=0));
}


void CatchTableView::editAudioMenuData()
{
  RecordListModel *mod=(RecordListModel *)model();
  QString cutname=mod->cutName(mod->index(d_mouse_row,0));
  RDCart *rdcart=new RDCart(RDCut::cartNumber(cutname));

  if(d_marker_dialog->
     exec(RDCut::cartNumber(cutname),RDCut::cutNumber(cutname))) {
    rdcart->updateLength();
    mod->refresh(mod->index(d_mouse_row,0));
    SendNotification(RDNotification::ModifyAction,
		     mod->recordId(mod->index(d_mouse_row,0)));
  }

  delete rdcart;
}


void CatchTableView::mousePressEvent(QMouseEvent *e)
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


void CatchTableView::SendNotification(RDNotification::Action action,
				      unsigned rec_id) const
{
  RDNotification *notify=
    new RDNotification(RDNotification::CatchEventType,action,rec_id);
  rda->ripc()->sendNotification(*notify);
  delete notify;
}
