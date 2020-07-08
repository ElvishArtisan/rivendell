//   import_listview.cpp
//
//   The Import Carts ListView widget for RDLogManager.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <q3dragobject.h>
#include <q3header.h>

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

//
// Menu Items
//
#define MENU_INSERT_NOTE 0
#define MENU_EDIT_NOTE 1
#define MENU_INSERT_TRACK 2
#define MENU_EDIT_TRACK 3
#define MENU_PLAY_TRANS 4
#define MENU_SEGUE_TRANS 5
#define MENU_DELETE 6

ImportListView::ImportListView(QWidget *parent)
  : Q3ListView(parent)
{
  import_parent=parent;

  //
  // Import List
  //
  import_list=new RDEventImportList();

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
  import_menu=new QMenu(this);
  connect(import_menu,SIGNAL(aboutToShow()),this,SLOT(aboutToShowData()));
  import_menu->insertItem(tr("Insert Log Note"),this,SLOT(insertNoteMenuData()),
			  0,MENU_INSERT_NOTE);
  import_menu->insertItem(tr("Edit Log Note"),this,SLOT(editNoteMenuData()),
			  0,MENU_EDIT_NOTE);
  import_menu->insertSeparator();
  import_menu->insertItem(tr("Insert Voice Track"),
			  this,SLOT(insertTrackMenuData()),
			  0,MENU_INSERT_TRACK);
  import_menu->insertItem(tr("Edit Voice Track"),this,SLOT(editTrackMenuData()),
			  0,MENU_EDIT_TRACK);
  import_menu->insertSeparator();
  import_menu->insertItem(tr("PLAY Transition"),this,SLOT(playMenuData()),
			    0,MENU_PLAY_TRANS);
  import_menu->insertItem(tr("SEGUE Transition"),this,SLOT(segueMenuData()),
			  0,MENU_SEGUE_TRANS);
  import_menu->insertSeparator();
  import_menu->insertItem(tr("Delete"),this,SLOT(deleteMenuData()),
			  0,MENU_DELETE);
  import_allow_first_trans=true;
  setAcceptDrops(true);
}


RDEventImportList *ImportListView::eventImportList() const
{
  return import_list;
}


void ImportListView::setAllowFirstTrans(bool state)
{
  Q3ListViewItem *item=NULL;
  RDEventImportItem *i_item=NULL;

  if(state!=import_allow_first_trans) {
    if(childCount()>=2) {
      if((item=firstChild())!=NULL) {
	if(!state) {
	  item->setText(5,tr("[auto]"));
	}
	else {
	  if((i_item=import_list->item(0))!=NULL) {
	    item->setText(5,RDLogLine::transText(i_item->transType()));
	  }
	}
      }
    }
    import_allow_first_trans=state;
  }
}


void ImportListView::move(int from_line,int to_line)
{
  import_list->moveItem(from_line,to_line);
}


void ImportListView::setEventName(const QString &str)
{
  import_list->setEventName(str);
}


bool ImportListView::load(const QString &event_name,
			  RDEventImportList::ImportType type)
{
  import_list->setEventName(event_name);
  import_list->setType(type);
  import_list->load();
  return true;
}


void ImportListView::save(RDLogLine::TransType first_trans)
{
  if(import_allow_first_trans) {
    import_list->save(RDLogLine::NoTrans);
  }
  else {
    import_list->save(first_trans);
  }
}


void ImportListView::refreshList(int line)
{
  Q3ListViewItem *item;
  Q3ListViewItem *select_item=NULL;
  QString sql;
  RDEventImportItem *i_item;
  int total_len=0;
  RDCart *cart=NULL;

  clear();
  for(int i=import_list->size()-1;i>=0;i--) {
    item=new Q3ListViewItem(this);
    if((i_item=import_list->item(i))!=NULL) {
      if(i_item->isEndMarker()) {
	item->setText(4,i_item->markerComment());
      }
      else {
	if((i_item->eventType()==RDLogLine::Cart)||
	   (i_item->eventType()==RDLogLine::Macro)) {
	  cart=new RDCart(i_item->cartNumber());
	}
	switch(i_item->eventType()) {
	case RDLogLine::Cart:
	  item->setPixmap(0,*import_playout_map);
	  item->setText(1,QString().sprintf("%06u",i_item->cartNumber()));
	  item->setText(2,cart->groupName());
	  item->setText(3,RDGetTimeLength(cart->forcedLength(),false,false));
	  item->setText(4,cart->title());
	  total_len+=cart->forcedLength();
	  break;
	    
	case RDLogLine::Macro:
	  item->setPixmap(0,*import_macro_map);
	  item->setText(1,QString().sprintf("%06u",i_item->cartNumber()));
	  item->setText(2,cart->groupName());
	  item->setText(3,RDGetTimeLength(cart->forcedLength(),false,false));
	  item->setText(4,cart->title());
	  total_len+=cart->forcedLength();
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
	if((i==0)&&(!import_allow_first_trans)) {
	  item->setText(5,tr("[auto]"));
	}
	else {
	  item->setText(5,RDLogLine::transText(i_item->transType()));
	}
	item->setText(6,QString().sprintf("%d",i));
	if(cart!=NULL) {
	  delete cart;
	  cart=NULL;
	}
      }
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


void ImportListView::fixupTransitions(RDLogLine::TransType repl_trans)
{
  Q3ListViewItem *item=firstChild();

  if(repl_trans==RDLogLine::Stop) {
    repl_trans=RDLogLine::RDLogLine::Segue;
  }
  for(int i=0;i<import_list->size();i++) {
    if(import_list->item(i)->transType()==RDLogLine::NoTrans) {
      import_list->item(i)->setTransType(repl_trans);
      item->setText(5,RDLogLine::transText(repl_trans));
      item=item->nextSibling();
    }
  }
}


void ImportListView::aboutToShowData()
{
  if((import_menu_item==NULL)||(import_menu_i_item->isEndMarker())) {
    import_menu->setItemEnabled(MENU_INSERT_NOTE,true);
    import_menu->setItemEnabled(MENU_EDIT_NOTE,false);
    import_menu->setItemEnabled(MENU_INSERT_TRACK,true);
    import_menu->setItemEnabled(MENU_EDIT_TRACK,false);
    import_menu->setItemChecked(MENU_PLAY_TRANS,false);
    import_menu->setItemEnabled(MENU_PLAY_TRANS,false);
    import_menu->setItemChecked(MENU_SEGUE_TRANS,false);
    import_menu->setItemEnabled(MENU_SEGUE_TRANS,false);
    import_menu->setItemEnabled(MENU_DELETE,false);
    return;
  }
  if(import_menu_i_item->eventType()==RDLogLine::Marker) {
    import_menu->setItemEnabled(MENU_EDIT_NOTE,true);
  }
  else {
    import_menu->setItemEnabled(MENU_EDIT_NOTE,false);
  }
  if(import_menu_i_item->eventType()==RDLogLine::Track) {
    import_menu->setItemEnabled(MENU_EDIT_TRACK,true);
  }
  else {
    import_menu->setItemEnabled(MENU_EDIT_TRACK,false);
  }
  import_menu->setItemChecked(MENU_PLAY_TRANS,false);
  import_menu->setItemChecked(MENU_SEGUE_TRANS,false);
  if(import_menu_line==0) {
    import_menu->setItemEnabled(MENU_PLAY_TRANS,import_allow_first_trans);
    import_menu->setItemEnabled(MENU_SEGUE_TRANS,import_allow_first_trans);
  }
  else {
    import_menu->setItemEnabled(MENU_PLAY_TRANS,true);
    import_menu->setItemEnabled(MENU_SEGUE_TRANS,true);
    import_menu->setItemEnabled(MENU_DELETE,true);
  }
  switch(import_menu_i_item->transType()) {
  case RDLogLine::Play:
    import_menu->setItemChecked(MENU_PLAY_TRANS,true);
    import_menu->setItemChecked(MENU_SEGUE_TRANS,false);
    break;

  case RDLogLine::Segue:
    import_menu->setItemChecked(MENU_PLAY_TRANS,false);
    import_menu->setItemChecked(MENU_SEGUE_TRANS,true);
    break;

  case RDLogLine::Stop:
    import_menu->setItemChecked(MENU_PLAY_TRANS,false);
    import_menu->setItemChecked(MENU_SEGUE_TRANS,false);
    break;

  default:
    break;
  }
  import_menu->setItemEnabled(MENU_DELETE,true);
}


void ImportListView::insertNoteMenuData()
{
  QString note;
  EditNote *note_dialog=new EditNote(&note,this);
  if(note_dialog->exec()<0) {
    delete note_dialog;
    return;
  }
  delete note_dialog;
  if(import_menu_item==NULL) {
    import_menu_line=0;
  }
  else {
    if((import_menu_i_item!=NULL)&&(import_menu_i_item->isEndMarker())) {
      import_menu_line=import_list->size()-1;
    }
  }
  RDEventImportItem *i_item=new RDEventImportItem();
  i_item->setEventType(RDLogLine::Marker);
  i_item->setMarkerComment(note);
  i_item->setTransType(RDLogLine::NoTrans);
  import_list->takeItem(import_menu_line,i_item);
  emit validationNeeded();
  refreshList(import_menu_line);
  emit sizeChanged(childCount());
}


void ImportListView::editNoteMenuData()
{
  QString text=import_list->item(import_menu_line)->markerComment();
  EditNote *edit_dialog=new EditNote(&text,this);
  if(edit_dialog->exec()<0) {
    delete edit_dialog;
    return;
  }
  import_list->item(import_menu_line)->setMarkerComment(text);  
}


void ImportListView::insertTrackMenuData()
{
  QString note;
  EditTrack *track_dialog=new EditTrack(&note,this);
  if(track_dialog->exec()<0) {
    delete track_dialog;
    return;
  }
  delete track_dialog;
  if(import_menu_item==NULL) {
    import_menu_line=0;
  }
  else {
    if((import_menu_i_item!=NULL)&&(import_menu_i_item->isEndMarker())) {
      import_menu_line=import_list->size()-1;
    }
  }
  RDEventImportItem *i_item=new RDEventImportItem();
  i_item->setEventType(RDLogLine::Track);
  i_item->setMarkerComment(note);
  i_item->setTransType(RDLogLine::NoTrans);
  import_list->takeItem(import_menu_line,i_item);
  emit validationNeeded();
  refreshList(import_menu_line);
  emit sizeChanged(childCount());
}


void ImportListView::editTrackMenuData()
{
  QString text=import_list->item(import_menu_line)->markerComment();
  EditTrack *edit_dialog=new EditTrack(&text,this);
  if(edit_dialog->exec()<0) {
    delete edit_dialog;
    return;
  }
  import_list->item(import_menu_line)->setMarkerComment(text);  
}


void ImportListView::playMenuData()
{
  import_menu_i_item->setTransType(RDLogLine::Play);
  import_menu_item->setText(5,tr("PLAY"));
}


void ImportListView::segueMenuData()
{
  import_menu_i_item->setTransType(RDLogLine::Segue);
  import_menu_item->setText(5,tr("SEGUE"));
}


void ImportListView::stopMenuData()
{
  import_menu_i_item->setTransType(RDLogLine::Stop);
  import_menu_item->setText(5,tr("STOP"));
}


void ImportListView::deleteMenuData()
{
  import_list->removeItem(import_menu_item->text(6).toInt());
  emit validationNeeded();
  refreshList();
  emit sizeChanged(childCount());
}


void ImportListView::contentsMousePressEvent(QMouseEvent *e)
{
  Q3ListView::contentsMousePressEvent(e);
  import_menu_item=selectedItem();
  if(import_menu_item==NULL) {
    import_menu_i_item=NULL;
    return;
  }
  else {
    if(import_menu_item->text(6).isEmpty()) {  // End of List Marker
      import_menu_i_item=import_list->endMarkerItem();
    }
    else {
    import_menu_i_item=import_list->
      item(import_menu_line=import_menu_item->text(6).toInt());
    }
  }
  switch(e->button()) {
  case Qt::RightButton:
    import_menu->setGeometry(import_parent->geometry().x()+
			     geometry().x()+e->pos().x()+2,
			     import_parent->geometry().y()+
			     geometry().y()+e->pos().y()+
			     header()->geometry().height()+2-
			     contentsY(),
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
  Q3ListView::contentsMouseDoubleClickEvent(e);
  import_menu_item=selectedItem();
  if((import_menu_item==NULL)||(import_menu_item->text(6).isEmpty())) {
    return;
  }
  if(import_list->
     item(import_menu_item->text(6).toInt())->eventType()==RDLogLine::Marker) {
    editNoteMenuData();
  }
  if(import_list->
     item(import_menu_item->text(6).toInt())->eventType()==RDLogLine::Track) {
    editTrackMenuData();
  }
}


void ImportListView::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e));
}


void ImportListView::dropEvent(QDropEvent *e)
{
  Q3ListViewItem *item;
  unsigned cartnum;
  int line=0;
  QPoint pos(e->pos().x(),e->pos().y()-header()->sectionRect(0).height());

  if(RDCartDrag::decode(e,&cartnum)) {
    if(cartnum==0) {
      if(((item=itemAt(pos))==NULL)||(item->text(6).isEmpty())) {
	return;
      }
      line=item->text(6).toInt();
      import_list->removeItem(item->text(6).toInt());
    }
    else {
      if((item=itemAt(pos))==NULL) {
	line=childCount()-1;
      }
      else {
	line=item->text(6).toInt();
      }
      RDEventImportItem *i_item=new RDEventImportItem();
      RDCart *cart=new RDCart(cartnum);
      i_item->setCartNumber(cartnum);
      if(cart->type()==RDCart::Audio) {
	i_item->setEventType(RDLogLine::Cart);
      }
      else {
	i_item->setEventType(RDLogLine::Macro);
      }
      i_item->setTransType(RDLogLine::NoTrans);
      import_list->takeItem(line,i_item);
      delete cart;
    }
  }
  emit validationNeeded();
  refreshList(line);
  emit sizeChanged(childCount());
}
