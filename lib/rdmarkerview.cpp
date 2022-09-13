// rdmarkerview.cpp
//
// Widget for displaying/editing cut markers
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPen>
#include <QScrollBar>

#include "rdescape_string.h"
#include "rdmarkerview.h"
#include "rdtrimaudio.h"

#define LEFT_MARGIN 30

RDMarkerHandle::RDMarkerHandle(RDMarkerHandle::PointerRole role,
			       PointerType type,void *mkrview,
			       QGraphicsItem *parent)
  : QGraphicsPolygonItem(parent)
{
  setFlags(QGraphicsItem::ItemIsMovable);
  d_marker_view=mkrview;
  d_x_diff=0;

  switch(type) {
  case RDMarkerHandle::Start:
    d_triangle << QPointF(0.0,0.0) << QPointF(-16,8) << QPointF(-16,-8);
    d_big_triangle << QPointF(0.0,0.0) << QPointF(-32,16) << QPointF(-32,-16);
    break;

  case RDMarkerHandle::End:
    d_triangle << QPointF(0.0,0.0) << QPointF(16,8) << QPointF(16,-8);
    d_big_triangle << QPointF(0.0,0.0) << QPointF(32,16) << QPointF(32,-16);
    break;
  }
  setPolygon(d_triangle);
  setPen(QPen(RDMarkerHandle::pointerRoleColor(role)));
  setBrush(RDMarkerHandle::pointerRoleColor(role));
  d_name=RDMarkerHandle::pointerRoleTypeText(role);
  d_role=role;
  setToolTip(RDMarkerHandle::pointerRoleTypeText(role));

  setAcceptHoverEvents(true);
}


QString RDMarkerHandle::name() const
{
  return d_name;
}


RDMarkerHandle::PointerRole RDMarkerHandle::role() const
{
  return d_role;
}


void RDMarkerHandle::setMinimum(int pos,int ptr)
{
  d_minimum_pos=pos;
  d_minimum_ptr=ptr;
}


void RDMarkerHandle::setMaximum(int pos,int ptr)
{
  d_maximum_pos=pos;
  d_maximum_ptr=ptr;
}


bool RDMarkerHandle::isSelected() const
{
  return d_is_selected;
}


void RDMarkerHandle::setSelected(bool state)
{
  if(d_is_selected!=state) {
    if(state) {
      setPolygon(d_big_triangle);
    }
    else {
      setPolygon(d_triangle);
    }
    d_is_selected=state;
  }
}


void RDMarkerHandle::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  view->setCursor(Qt::PointingHandCursor);
}


void RDMarkerHandle::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  view->unsetCursor();
}


void RDMarkerHandle::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  if(e->button()==Qt::LeftButton) {
    e->accept();
    d_x_diff=pos().x()-e->scenePos().x();
    if(d_peers.size()==0) {
      QGraphicsScene *s=scene();
      QList<QGraphicsItem *> items=s->items();
      for(int i=0;i<items.size();i++) {
	if(items.at(i)->toolTip()==toolTip()) {
	  d_peers.push_back(items.at(i));
	}
      }
    }
    view->setSelectedMarkers(d_role,RDMarkerHandle::LastRole);
  }

  if(e->button()==Qt::RightButton) {
    e->ignore();
    view->processRightClick(d_role,e->screenPos());
  }
}


void RDMarkerHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  //
  // Check Limit Stops
  //
  int limit_ptr=-1;
  int x=e->scenePos().x()+d_x_diff-LEFT_MARGIN;

  if((d_minimum_pos>=0)&&(x<d_minimum_pos)) {
    d_x_diff=d_minimum_pos-e->scenePos().x()+LEFT_MARGIN;
    x=d_minimum_pos;
    limit_ptr=d_minimum_ptr;
  }
  else {
    if((d_maximum_pos>=0)&&(x>d_maximum_pos)) {
      d_x_diff=d_maximum_pos-e->scenePos().x()+LEFT_MARGIN;
      x=d_maximum_pos;
      limit_ptr=d_maximum_ptr;
    }
  }

  //
  // Update Marker Graphic
  //
  for(int i=0;i<d_peers.size();i++) {
    QGraphicsItem *peer=d_peers.at(i);
    peer->setPos(e->scenePos().x()+d_x_diff,peer->pos().y());
  }

  if(limit_ptr<0) {  // Generate new pointer value
    int64_t pframes=(int64_t)(1152*x*view->shrinkFactor());
    view->updatePosition(d_role,(int)((int64_t)1000*pframes/
				      (int64_t)view->sampleRate()));
  }
  //
  // We're against a limit stop, so use the pointer value of the stop
  // rather than generating a new one as not to risk losing precision.
  //
  else {
    view->updatePosition(d_role,limit_ptr);
  }
}


QString RDMarkerHandle::pointerRoleText(PointerRole role)
{
  QString ret=QObject::tr("Unknown");

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::CutEnd:
    ret=QObject::tr("Cut");
    break;

  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::TalkEnd:
    ret=QObject::tr("Talk");
    break;

  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::SegueEnd:
    ret=QObject::tr("Segue");
    break;

  case RDMarkerHandle::HookStart:
  case RDMarkerHandle::HookEnd:
    ret=QObject::tr("Hook");
    break;

  case RDMarkerHandle::FadeUp:
    ret=QObject::tr("Fade Up");
    break;

  case RDMarkerHandle::FadeDown:
    ret=QObject::tr("Fade Down");
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


QString RDMarkerHandle::pointerRoleTypeText(RDMarkerHandle::PointerRole role)
{
  QString ret=QObject::tr("Unknown");

  switch(role) {
  case RDMarkerHandle::CutStart:
    ret=QObject::tr("Cut Start");
    break;

  case RDMarkerHandle::CutEnd:
    ret=QObject::tr("Cut End");
    break;

  case RDMarkerHandle::TalkStart:
    ret=QObject::tr("Talk Start");
    break;

  case RDMarkerHandle::TalkEnd:
    ret=QObject::tr("Talk End");
    break;

  case RDMarkerHandle::SegueStart:
    ret=QObject::tr("Segue Start");
    break;

  case RDMarkerHandle::SegueEnd:
    ret=QObject::tr("Segue End");
    break;

  case RDMarkerHandle::HookStart:
    ret=QObject::tr("Hook Start");
    break;

  case RDMarkerHandle::HookEnd:
    ret=QObject::tr("Hook End");
    break;

  case RDMarkerHandle::FadeUp:
    ret=QObject::tr("Fade Up");
    break;

  case RDMarkerHandle::FadeDown:
    ret=QObject::tr("Fade Down");
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


QColor RDMarkerHandle::pointerRoleColor(RDMarkerHandle::PointerRole role)
{
  QColor ret;

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::CutEnd:
    ret=Qt::red;
    break;

  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::TalkEnd:
    ret=Qt::blue;
    break;

  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::SegueEnd:
    ret=Qt::cyan;
    break;

  case RDMarkerHandle::HookStart:
  case RDMarkerHandle::HookEnd:
    ret=Qt::magenta;
    break;

  case RDMarkerHandle::FadeUp:
  case RDMarkerHandle::FadeDown:
    ret=Qt::darkYellow;
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


RDMarkerHandle::PointerType RDMarkerHandle::pointerType(RDMarkerHandle::PointerRole role)
{
  RDMarkerHandle::PointerType ret=RDMarkerHandle::Start;

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::HookStart:
  case RDMarkerHandle::FadeDown:
    ret=RDMarkerHandle::Start;
    break;

  case RDMarkerHandle::CutEnd:
  case RDMarkerHandle::TalkEnd:
  case RDMarkerHandle::SegueEnd:
  case RDMarkerHandle::HookEnd:
  case RDMarkerHandle::FadeUp:
    ret=RDMarkerHandle::End;
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


RDMarkerView::RDMarkerView(int width,int height,QWidget *parent)
  : RDWidget(parent)
{
  d_width=width;
  d_height=height;
  d_scene=NULL;
  d_sample_rate=rda->system()->sampleRate();
  d_right_margin=0;
  d_cursor=NULL;
  clear();

  d_view=new QGraphicsView(this);

  d_wave_factory=new RDWaveFactory(RDWaveFactory::MultiTrack);

  d_pointer_fields.push_back("`START_POINT`");
  d_pointer_fields.push_back("`END_POINT`");
  d_pointer_fields.push_back("`TALK_START_POINT`");
  d_pointer_fields.push_back("`TALK_END_POINT`");
  d_pointer_fields.push_back("`SEGUE_START_POINT`");
  d_pointer_fields.push_back("`SEGUE_END_POINT`");
  d_pointer_fields.push_back("`HOOK_START_POINT`");
  d_pointer_fields.push_back("`HOOK_END_POINT`");
  d_pointer_fields.push_back("`FADEDOWN_POINT`");
  d_pointer_fields.push_back("`FADEUP_POINT`");

  //
  // The Main Mouse Menu
  //
  d_main_menu=new QMenu(this);
  connect(d_main_menu,SIGNAL(aboutToShow()),this,SLOT(updateMenuData()));
  d_delete_marker_action=d_main_menu->
    addAction(tr("Delete Marker"),this,SLOT(deleteMarkerData()));
  d_main_menu->addSeparator();
  d_add_talk_action=d_main_menu->
    addAction(tr("Add Talk Markers"),this,SLOT(addTalkData()));
  d_add_segue_action=d_main_menu->
    addAction(tr("Add Segue Markers"),this,SLOT(addSegueData()));
  d_add_hook_action=d_main_menu->
    addAction(tr("Add Hook Markers"),this,SLOT(addHookData()));
  d_add_fadeup_action=d_main_menu->
    addAction(tr("Add Fade Up Marker"),this,SLOT(addFadeupData()));
  d_add_fadedown_action=d_main_menu->
    addAction(tr("Add Fade Down Marker"),this,SLOT(addFadedownData()));
}


RDMarkerView::~RDMarkerView()
{
  if(d_scene!=NULL) {
    delete d_scene;
  }
  delete d_wave_factory;
  delete d_view;
  delete d_main_menu;
}


QSize RDMarkerView::sizeHint() const
{
  return QSize(1000,d_height);
}


QSizePolicy RDMarkerView::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
}


int RDMarkerView::audioGain() const
{
  return d_audio_gain;
}


unsigned RDMarkerView::sampleRate() const
{
  return d_sample_rate;
}


int RDMarkerView::shrinkFactor() const
{
  return d_shrink_factor;
}


bool RDMarkerView::canShrinkTime() const
{
  return d_shrink_factor>1;
}


bool RDMarkerView::canGrowTime() const
{
  return d_shrink_factor<d_max_shrink_factor;
}


int RDMarkerView::pointerValue(RDMarkerHandle::PointerRole role)
{
  return d_pointers[role];
}


bool RDMarkerView::noSegueFade() const
{
  return d_no_segue_fade;
}


int RDMarkerView::playGain() const
{
  return d_play_gain;
}


RDMarkerHandle::PointerRole 
RDMarkerView::selectedMarker(RDMarkerHandle::PointerType type) const
{
  return d_selected_markers[type];
}


bool RDMarkerView::hasUnsavedChanges() const
{
  return d_has_unsaved_changes;
}

void RDMarkerView::processRightClick(RDMarkerHandle::PointerRole role,
				     const QPointF &pos)
{
  d_deleting_roles.clear();

  d_deleting_roles.push_back(role);
  if((role==RDMarkerHandle::SegueStart)||
     (role==RDMarkerHandle::TalkStart)||
     (role==RDMarkerHandle::HookStart)) {
    d_deleting_roles.push_back((RDMarkerHandle::PointerRole)(role+1));
  }
  if((role==RDMarkerHandle::SegueEnd)||
     (role==RDMarkerHandle::TalkEnd)||
     (role==RDMarkerHandle::HookEnd)) {
    d_deleting_roles.push_back((RDMarkerHandle::PointerRole)(role-1));
  }
  d_marker_menu_used=true;
  d_main_menu->setGeometry(pos.x(),pos.y(),
			   d_main_menu->sizeHint().width(),
			   d_main_menu->sizeHint().height());
  d_main_menu->exec();
}


void RDMarkerView::updatePosition(RDMarkerHandle::PointerRole role, int ptr)
{
  d_pointers[role]=ptr;
  d_has_unsaved_changes=true;

  updateInterlocks();

  emit pointerValueChanged(role,d_pointers[role]);
}


void RDMarkerView::setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
				      RDMarkerHandle::PointerRole end_role)
{
  switch(start_role) {
  case RDMarkerHandle::CutEnd:
  case RDMarkerHandle::TalkEnd:
  case RDMarkerHandle::SegueEnd:
  case RDMarkerHandle::HookEnd:
    d_selected_markers[0]=(RDMarkerHandle::PointerRole)((int)start_role-1);
    d_selected_markers[1]=start_role;
    break;

  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::HookStart:
    d_selected_markers[0]=start_role;
    d_selected_markers[1]=(RDMarkerHandle::PointerRole)((int)start_role+1);
    break;

  case RDMarkerHandle::FadeUp:
    d_selected_markers[0]=RDMarkerHandle::LastRole;
    d_selected_markers[1]=RDMarkerHandle::FadeUp;
    break;

  case RDMarkerHandle::FadeDown:
    if(start_role!=d_selected_markers[0]) {
      for(int i=0;i<RDMarkerHandle::LastRole;i++) {
	for(int j=0;j<2;j++) {
	  if(d_handles[i][j]!=NULL) {
	    d_handles[i][j]->
	      setSelected(start_role==(RDMarkerHandle::PointerRole)i);
	  }
	}
      }
    }
    d_selected_markers[0]=RDMarkerHandle::FadeDown;
    d_selected_markers[1]=RDMarkerHandle::LastRole;
    break;

  case RDMarkerHandle::LastRole:
    d_selected_markers[0]=RDMarkerHandle::LastRole;
    d_selected_markers[1]=RDMarkerHandle::FadeUp;
    break;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i+=2) {
    for(int j=0;j<2;j++) {
      if(d_handles[i][j]!=NULL) {
	d_handles[i][j]->
	  setSelected(d_selected_markers[0]==(RDMarkerHandle::PointerRole)i);
      }
      if(d_handles[i+1][j]!=NULL) {
	d_handles[i+1][j]->
	  setSelected(d_selected_markers[1]==(RDMarkerHandle::PointerRole)i+1);
      }
    }
  }
  for(int j=0;j<2;j++) {  // And the FadeUp marker
    if(d_handles[RDMarkerHandle::FadeUp][j]!=NULL) {
      d_handles[RDMarkerHandle::FadeUp][j]->
	setSelected(d_selected_markers[1]==RDMarkerHandle::FadeUp);
    }
  }

  emit selectedMarkersChanged(d_selected_markers[0],d_selected_markers[1]);
}


void RDMarkerView::setNoSegueFade(bool state)
{
  d_has_unsaved_changes=state!=d_no_segue_fade;
  d_no_segue_fade=state;
}


void RDMarkerView::setPlayGain(int db)
{
  if(db!=d_play_gain) {
    d_play_gain=db;
    d_has_unsaved_changes=true;
    SetReferenceLines();
  }
}


void RDMarkerView::trimStart(int dbfs)
{
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(rda->station(),rda->config(),this);
  conv->setCartNumber(d_cart_number);
  conv->setCutNumber(d_cut_number);
  conv->setTrimLevel(100*dbfs);
  switch(conv_err=conv->runTrim(rda->user()->name(),rda->user()->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->startPoint()>=0) {
      updatePosition(RDMarkerHandle::CutStart,conv->startPoint());
      PositionMarker(RDMarkerHandle::CutStart,conv->startPoint());
    }
    break;

  default:
    QMessageBox::warning(this,tr("Edit Audio"),
			 RDTrimAudio::errorText(conv_err));
    break;
  }
  delete conv;
}


void RDMarkerView::trimEnd(int dbfs)
{
  RDTrimAudio::ErrorCode conv_err;
  RDTrimAudio *conv=new RDTrimAudio(rda->station(),rda->config(),this);
  conv->setCartNumber(d_cart_number);
  conv->setCutNumber(d_cut_number);
  conv->setTrimLevel(100*dbfs);
  switch(conv_err=conv->runTrim(rda->user()->name(),rda->user()->password())) {
  case RDTrimAudio::ErrorOk:
    if(conv->endPoint()>=0) {
      updatePosition(RDMarkerHandle::CutEnd,conv->endPoint());
      PositionMarker(RDMarkerHandle::CutEnd,conv->endPoint());
    }
    break;

  default:
    QMessageBox::warning(this,tr("Edit Audio"),
			 RDTrimAudio::errorText(conv_err));
    break;
  }
  delete conv;
}


void RDMarkerView::setAudioGain(int lvl)
{
  if(d_audio_gain!=lvl) {
    d_audio_gain=lvl;
    WriteWave();
  }
}


bool RDMarkerView::setCut(QString *err_msg,unsigned cartnum,int cutnum)
{
  clear();

  d_cart_number=cartnum;
  d_cut_number=cutnum;
  d_right_margin=LEFT_MARGIN;  // Default value
  if(!LoadCutData()) {
    *err_msg=tr("No such cart/cut!");
    return false;
  }
  if(!d_wave_factory->setCut(err_msg,cartnum,cutnum)) {
    return false;
  }
  d_audio_length=(int)((int64_t)d_wave_factory->energySize()*1152000/
		       ((int64_t)d_sample_rate));
  d_max_shrink_factor=1;
  while(d_wave_factory->energySize()>=(d_width*d_max_shrink_factor)) {
    d_max_shrink_factor=d_max_shrink_factor*2;
  }
  d_pad_size=64+(d_width*d_max_shrink_factor-d_wave_factory->energySize())/d_max_shrink_factor-1;
  d_shrink_factor=d_max_shrink_factor;
  WriteWave();

  updateInterlocks();

  return true;
}


void RDMarkerView::setCursorPosition(unsigned msec)
{
  qreal x=(qreal)msec*(qreal)d_sample_rate/(1152000*(qreal)d_shrink_factor)+
    (qreal)LEFT_MARGIN;
  d_cursor->setPos(x,d_cursor->pos().y());
}


void RDMarkerView::gotoCursor()
{
  d_view->horizontalScrollBar()->
    setValue(d_cursor->pos().x()-d_view->width()/2);
}


void RDMarkerView::gotoHome()
{
  d_view->horizontalScrollBar()->setValue(0);
}


void RDMarkerView::gotoEnd()
{
  d_view->horizontalScrollBar()->
    setValue(d_view->horizontalScrollBar()->maximum());
}


void RDMarkerView::maxShrinkTime()
{
  if(canShrinkTime()) {
    SetShrinkFactor(1);
  }    
}


void RDMarkerView::shrinkTime()
{
  if(canShrinkTime()) {
    SetShrinkFactor(d_shrink_factor/2);
  }    
}


void RDMarkerView::growTime()
{
  if(canGrowTime()) {
    SetShrinkFactor(d_shrink_factor*2);
  }    
}


void RDMarkerView::maxGrowTime()
{
  if(canGrowTime()) {
    SetShrinkFactor(d_max_shrink_factor);
  }    
}


void RDMarkerView::save()
{
  QString sql;

  sql=QString("update CUTS set ");
  for(int i=0;i<RDMarkerHandle::PointerRole::LastRole;i++) {
    sql+=d_pointer_fields.at(i)+QString::asprintf("=%d,",d_pointers[i]);
  }
  sql+=QString::asprintf("`LENGTH`=%d,",d_pointers[RDMarkerHandle::CutEnd]-
			 d_pointers[RDMarkerHandle::CutStart]);
  if(d_no_segue_fade) {
    sql+="`SEGUE_GAIN`=0,";
  }
  else {
    sql+=QString::asprintf("`SEGUE_GAIN`=%d,",RD_FADE_DEPTH);
  }
  sql+=QString::asprintf("`PLAY_GAIN`=%d ",100*d_play_gain);
  sql+=QString(" where ")+
    "`CUT_NAME`='"+RDEscapeString(RDCut::cutName(d_cart_number,d_cut_number))+
    "'";
  RDSqlQuery::apply(sql);
  d_has_unsaved_changes=false;
}


void RDMarkerView::clear()
{
  d_wheel_angle=0;
  if(d_scene!=NULL) {
    delete d_scene;
    d_scene=NULL;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i++) {
    d_pointers[i]=-1;
    for(int j=0;j<2;j++) {
      d_handles[i][j]=NULL;
    }
  }
  d_no_segue_fade=false;
  d_play_gain=0;
  d_shrink_factor=1;
  d_max_shrink_factor=1;
  d_pad_size=0;
  d_audio_gain=900;
  d_audio_length=0;
  d_has_unsaved_changes=false;
  d_marker_menu_used=false;
  d_selected_markers[0]=RDMarkerHandle::LastRole;
  d_selected_markers[1]=RDMarkerHandle::LastRole;
  d_cursor=NULL;  // Storage is automatically freed by QGraphicsScene
}


void RDMarkerView::updateMenuData()
{
  bool can_add=
    (d_deleting_roles.size()==0)&&
    (d_msec_pos>=d_pointers[RDMarkerHandle::CutStart])&&
    (d_msec_pos<d_pointers[RDMarkerHandle::CutEnd]);
  bool can_delete=
    d_marker_menu_used&&
    (!d_deleting_roles.contains(RDMarkerHandle::CutStart))&&
    (!d_deleting_roles.contains(RDMarkerHandle::CutEnd));

  d_delete_marker_action->setEnabled(can_delete);
  d_add_fadedown_action->
    setEnabled(can_add&&(d_pointers[RDMarkerHandle::FadeDown]<0)&&
	       ((d_pointers[RDMarkerHandle::FadeUp]<0)||
		(d_pointers[RDMarkerHandle::FadeUp]<Msec(d_mouse_pos))));
  d_add_fadeup_action->
    setEnabled(can_add&&(d_pointers[RDMarkerHandle::FadeUp]<0)&&
	       ((d_pointers[RDMarkerHandle::FadeDown]<0)||
		(d_pointers[RDMarkerHandle::FadeDown]>Msec(d_mouse_pos))));
  d_add_hook_action->
    setEnabled(can_add&&(d_pointers[RDMarkerHandle::HookStart]<0));
  d_add_segue_action->
    setEnabled(can_add&&(d_pointers[RDMarkerHandle::SegueStart]<0));
  d_add_talk_action->
    setEnabled(can_add&&(d_pointers[RDMarkerHandle::TalkStart]<0));
}


void RDMarkerView::addTalkData()
{
  d_pointers[RDMarkerHandle::TalkStart]=d_msec_pos;
  d_pointers[RDMarkerHandle::TalkEnd]=d_msec_pos;

  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::TalkStart,60);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::TalkEnd,60);
  InterlockMarkerPair(RDMarkerHandle::TalkStart);

  d_has_unsaved_changes=true;

  emit pointerValueChanged(RDMarkerHandle::TalkStart,
			   d_pointers[RDMarkerHandle::TalkStart]);
  emit pointerValueChanged(RDMarkerHandle::TalkEnd,
			   d_pointers[RDMarkerHandle::TalkEnd]);
}


void RDMarkerView::addSegueData()
{
  d_pointers[RDMarkerHandle::SegueStart]=d_msec_pos;;
  d_pointers[RDMarkerHandle::SegueEnd]=d_msec_pos;

  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::SegueStart,40);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::SegueEnd,40);
  InterlockMarkerPair(RDMarkerHandle::SegueStart);

  d_has_unsaved_changes=true;

  emit pointerValueChanged(RDMarkerHandle::SegueStart,
			   d_pointers[RDMarkerHandle::SegueStart]);
  emit pointerValueChanged(RDMarkerHandle::SegueEnd,
			   d_pointers[RDMarkerHandle::SegueEnd]);
}


void RDMarkerView::addHookData()
{
  d_pointers[RDMarkerHandle::HookStart]=d_msec_pos;
  d_pointers[RDMarkerHandle::HookEnd]=d_msec_pos;

  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::HookStart,100);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::HookEnd,100);
  InterlockMarkerPair(RDMarkerHandle::HookStart);

  d_has_unsaved_changes=true;

  emit pointerValueChanged(RDMarkerHandle::HookStart,
			   d_pointers[RDMarkerHandle::HookStart]);
  emit pointerValueChanged(RDMarkerHandle::HookEnd,
			   d_pointers[RDMarkerHandle::HookEnd]);
}


void RDMarkerView::addFadeupData()
{
  d_pointers[RDMarkerHandle::FadeUp]=d_msec_pos;

  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::FadeUp,80);
  InterlockFadeMarkerPair();

  d_has_unsaved_changes=true;

  emit pointerValueChanged(RDMarkerHandle::FadeUp,
			   d_pointers[RDMarkerHandle::FadeUp]);
}


void RDMarkerView::addFadedownData()
{
  d_pointers[RDMarkerHandle::FadeDown]=d_msec_pos;

  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::FadeDown,80);
  InterlockFadeMarkerPair();

  d_has_unsaved_changes=true;

  emit pointerValueChanged(RDMarkerHandle::FadeDown,
			   d_pointers[RDMarkerHandle::FadeDown]);
}


void RDMarkerView::deleteMarkerData()
{
  for(int i=0;i<d_deleting_roles.size();i++) {
    RemoveMarker(d_deleting_roles.at(i));
  }
  d_deleting_roles.clear();

  d_has_unsaved_changes=true;
}


void RDMarkerView::updateInterlocks()
{
  //
  // Check for "swiped" markers and remove them
  //
  for(int i=2;i<(RDMarkerHandle::LastRole);i++) {
    RDMarkerHandle::PointerRole role=(RDMarkerHandle::PointerRole)i;
    if((d_pointers[i]>=0)&&
       ((d_pointers[i]<d_pointers[RDMarkerHandle::CutStart])||
	(d_pointers[i]>d_pointers[RDMarkerHandle::CutEnd]))) {
      RemoveMarker(role);
      switch(role) {
      case RDMarkerHandle::TalkStart:
      case RDMarkerHandle::SegueStart:
      case RDMarkerHandle::HookStart:
	RemoveMarker((RDMarkerHandle::PointerRole)(i+1));
	break;

      case RDMarkerHandle::TalkEnd:
      case RDMarkerHandle::SegueEnd:
      case RDMarkerHandle::HookEnd:
	RemoveMarker((RDMarkerHandle::PointerRole)(i-1));
	break;

      case RDMarkerHandle::CutStart:
      case RDMarkerHandle::CutEnd:
      case RDMarkerHandle::FadeUp:
      case RDMarkerHandle::FadeDown:
      case RDMarkerHandle::LastRole:
	break;
      }
    }
  }

  //
  // Update the limit stops
  //
  for(int i=0;i<2;i++) {
    d_handles[RDMarkerHandle::CutStart][i]->setMinimum(0,0);
    d_handles[RDMarkerHandle::CutStart][i]->
      setMaximum(d_handles[RDMarkerHandle::CutEnd][i]->pos().x()-LEFT_MARGIN,
		 d_pointers[RDMarkerHandle::CutEnd]);

    d_handles[RDMarkerHandle::CutEnd][i]->
      setMinimum(d_handles[RDMarkerHandle::CutStart][i]->pos().x()-LEFT_MARGIN,
		 d_pointers[RDMarkerHandle::CutStart]);
    d_handles[RDMarkerHandle::CutEnd][i]->
      setMaximum(d_right_margin-LEFT_MARGIN,d_audio_length);
  }
  InterlockMarkerPair(RDMarkerHandle::TalkStart);
  InterlockMarkerPair(RDMarkerHandle::SegueStart);
  InterlockMarkerPair(RDMarkerHandle::HookStart);
  InterlockFadeMarkerPair();
}


void RDMarkerView::InterlockFadeMarkerPair()
{
  //
  // FadeUp
  //
  for(int i=0;i<2;i++) {
    if(d_handles[RDMarkerHandle::FadeUp][i]!=NULL) {
      d_handles[RDMarkerHandle::FadeUp][i]->
	setMinimum(d_handles[RDMarkerHandle::CutStart][i]->pos().x()-LEFT_MARGIN,
		   d_pointers[RDMarkerHandle::CutStart]);
      if(d_handles[RDMarkerHandle::FadeDown][i]!=NULL) {
	d_handles[RDMarkerHandle::FadeUp][i]->
	  setMaximum(d_handles[RDMarkerHandle::FadeDown][i]->pos().x()-LEFT_MARGIN,
		   d_pointers[RDMarkerHandle::FadeDown]);
      }
      else {
	d_handles[RDMarkerHandle::FadeUp][i]->
	  setMaximum(d_handles[RDMarkerHandle::CutEnd][i]->pos().x()-LEFT_MARGIN,
		     d_pointers[RDMarkerHandle::CutEnd]);	  
      }
    }
  }

  //
  // FadeDown
  //
  for(int i=0;i<2;i++) {
    if(d_handles[RDMarkerHandle::FadeDown][i]!=NULL) {
      d_handles[RDMarkerHandle::FadeDown][i]->
	setMaximum(d_handles[RDMarkerHandle::CutEnd][i]->pos().x()-LEFT_MARGIN,
		   d_pointers[RDMarkerHandle::CutEnd]);
      if(d_handles[RDMarkerHandle::FadeUp][i]!=NULL) {
	d_handles[RDMarkerHandle::FadeDown][i]->
	  setMinimum(d_handles[RDMarkerHandle::FadeUp][i]->pos().x()-LEFT_MARGIN,
		   d_pointers[RDMarkerHandle::FadeUp]);
      }
      else {
	d_handles[RDMarkerHandle::FadeDown][i]->
	  setMinimum(d_handles[RDMarkerHandle::CutStart][i]->pos().x()-LEFT_MARGIN,
		     d_pointers[RDMarkerHandle::CutStart]);	  
      }
    }
  }
}


void RDMarkerView::InterlockMarkerPair(RDMarkerHandle::PointerRole start_marker)
{
  for(int i=0;i<2;i++) {
    if(d_handles[start_marker][i]!=NULL) {
      d_handles[start_marker][i]->
	setMinimum(d_handles[RDMarkerHandle::CutStart][i]->
		   pos().x()-LEFT_MARGIN,d_pointers[RDMarkerHandle::CutStart]);
      if(d_handles[start_marker+1][i]==NULL) {
	d_handles[start_marker][i]->
	  setMaximum(d_handles[RDMarkerHandle::CutEnd][i]->
		     pos().x()-LEFT_MARGIN,d_pointers[RDMarkerHandle::CutEnd]);
      }
      else {
	d_handles[start_marker][i]->
	  setMaximum(d_handles[start_marker+1][i]->
		     pos().x()-LEFT_MARGIN,d_pointers[start_marker+1]);
      }
    }

    if(d_handles[start_marker+1][i]!=NULL) {
      if(d_handles[start_marker][i]==NULL) {
	d_handles[start_marker+1][i]->
	  setMinimum(d_handles[RDMarkerHandle::CutStart][i]->
		     pos().x()-LEFT_MARGIN,d_pointers[RDMarkerHandle::CutStart]);
      }
      else {
	d_handles[start_marker+1][i]->
	  setMinimum(d_handles[start_marker][i]->
		     pos().x()-LEFT_MARGIN,d_pointers[start_marker]);
      }
      d_handles[start_marker+1][i]->
	setMaximum(d_handles[RDMarkerHandle::CutEnd][i]->
		   pos().x()-LEFT_MARGIN,d_pointers[RDMarkerHandle::CutEnd]);
    }
  }
}




void RDMarkerView::resizeEvent(QResizeEvent *e)
{
  d_view->setGeometry(0,0,size().width(),size().height());
}


void RDMarkerView::mousePressEvent(QMouseEvent *e)
{
  int origin=0;

  if(d_view->horizontalScrollBar()!=NULL) {
    origin=d_view->horizontalScrollBar()->value();
  }
  if((e->x()<=LEFT_MARGIN)||((e->x()+origin)>d_right_margin)) {
    QWidget::mousePressEvent(e);
    return;
  }
  d_mouse_pos=e->x()-LEFT_MARGIN;

  if(d_marker_menu_used) {
    d_marker_menu_used=false;
    return;
  }
  d_msec_pos=(int64_t)(d_mouse_pos+origin)*(int64_t)d_shrink_factor*1152000/
    (int64_t)d_sample_rate;
  
  switch(e->button()) {
  case Qt::LeftButton:
    emit positionClicked(d_msec_pos);
    break;

  case Qt::MidButton:
    break;

  case Qt::RightButton:
    d_deleting_roles.clear();

    d_main_menu->setGeometry(e->globalX(),e->globalY(),
			d_main_menu->sizeHint().width(),
			d_main_menu->sizeHint().height());
    d_main_menu->exec();
    break;

  default:
    break;
  }
}


void RDMarkerView::wheelEvent(QWheelEvent *e)
{
  d_wheel_angle+=e->angleDelta().y();

  if(d_wheel_angle>=360) {
    if(shrinkFactor()>1) {
      SetShrinkFactor(shrinkFactor()/2);
    }
    d_wheel_angle=0;
  }
  if(d_wheel_angle<=-360) {
    if(shrinkFactor()<d_max_shrink_factor) {
      SetShrinkFactor(shrinkFactor()*2);
    }
    d_wheel_angle=0;
  }
}


int RDMarkerView::Frame(int msec) const
{
  return (int)((int64_t)msec*(int64_t)d_sample_rate/(d_shrink_factor*1152000));
}


int RDMarkerView::Msec(int frame) const
{
  return ((int64_t)d_view->horizontalScrollBar()->value()*
	  d_shrink_factor*1152000/(int64_t)d_sample_rate)+
    ((int64_t)frame*d_shrink_factor*1152000/(int64_t)d_sample_rate);
}


void RDMarkerView::SetShrinkFactor(int sf)
{
  if((d_shrink_factor!=sf)&&(sf<=d_max_shrink_factor)) {

    //
    // Reposition cursor
    //
    int x=LEFT_MARGIN+(d_cursor->pos().x()-LEFT_MARGIN)*d_shrink_factor/sf;
    d_cursor->setPos(x,0);
    d_shrink_factor=sf;
    WriteWave();
    gotoCursor();
    emit canShrinkTimeChanged(canShrinkTime());
    emit canGrowTimeChanged(canGrowTime());
  }
}


bool RDMarkerView::LoadCutData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  bool ret=false;

  sql=QString("select ")+
    d_pointer_fields.join(",")+","+  // 00 - 09
    "`CHANNELS`,"+                     // 10
    "`SEGUE_GAIN`,"+                   // 11
    "`PLAY_GAIN` "+                    // 12
    "from `CUTS` where "+
    "`CUT_NAME`='"+
    RDEscapeString(RDCut::cutName(d_cart_number,d_cut_number))+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=true;
    for(int i=0;i<RDMarkerHandle::LastRole;i++) {
      d_pointers[i]=q->value(i).toInt();
    }
    d_channels=q->value(10).toInt();
    d_no_segue_fade=q->value(11).toInt()==0;
    d_play_gain=q->value(12).toInt()/100;
  }
  delete q;

  return ret;
}


void RDMarkerView::WriteWave()
{
  //
  // Waveform
  //
  QPixmap wavemap=
    d_wave_factory->generate(d_height-20,d_shrink_factor,d_audio_gain,true);

  if(d_scene!=NULL) {
    d_scene->deleteLater();
  }
  d_scene=new QGraphicsScene(0,0,wavemap.width()+d_pad_size,d_height-20,this);

  d_scene->addRect(0,0,LEFT_MARGIN,d_height-20,QPen(Qt::gray),QBrush(Qt::gray));
  d_scene->addPixmap(wavemap)->setPos(LEFT_MARGIN,0);
  d_scene->addRect(LEFT_MARGIN+wavemap.width(),0,d_pad_size,d_height-20,QPen(Qt::gray),QBrush(Qt::gray));
  d_right_margin=LEFT_MARGIN+wavemap.width();

  //
  // Reference Level Lines
  //
  for(unsigned i=0;i<d_channels;i++) {
    for(unsigned j=0;j<2;j++) {
      d_reference_line_items[i][j]=new QGraphicsLineItem(0,0,d_right_margin,0);
      d_reference_line_items[i][j]->setPen(QPen(Qt::red));
      d_scene->addItem(d_reference_line_items[i][j]);
    }
  }
  SetReferenceLines();

  //
  // Channel Labels
  //
  if(d_channels==2) {
    QGraphicsTextItem *t_item=new QGraphicsTextItem("L");
    t_item->setFont(QFont(font().family(),24,QFont::Normal));
    t_item->setDefaultTextColor(Qt::red);
    d_scene->addItem(t_item);
    t_item->setPos(LEFT_MARGIN,-5);

    t_item=new QGraphicsTextItem("R");
    t_item->setFont(QFont(font().family(),24,QFont::Normal));
    t_item->setDefaultTextColor(Qt::red);
    d_scene->addItem(t_item);
    t_item->setPos(LEFT_MARGIN,d_scene->height()/2-5);
  }

  //
  // Markers
  //
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::FadeDown,100);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::FadeUp,100);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::HookStart,80);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::HookEnd,80);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::TalkStart,60);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::TalkEnd,60);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::SegueStart,40);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::SegueEnd,40);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::CutStart,20);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::CutEnd,20);

  if(d_cursor==NULL) {
    d_cursor=new QGraphicsLineItem(0,0,0,d_height);
  }
  QPen pen(Qt::green);
  pen.setWidth(2);
  d_cursor->setPen(pen);

  d_scene->addItem(d_cursor);

  d_view->setScene(d_scene);

  updateInterlocks();
}


void RDMarkerView::DrawMarker(RDMarkerHandle::PointerType type,
			      RDMarkerHandle::PointerRole role,int handle_pos)
{
  QGraphicsLineItem *l_item=NULL;
  RDMarkerHandle *m_item=NULL;

  if(d_pointers[role]>=0) {
    l_item=new QGraphicsLineItem(0,0,0,d_height);
    l_item->setPen(QPen(RDMarkerHandle::pointerRoleColor(role)));
    l_item->setToolTip(RDMarkerHandle::pointerRoleTypeText(role));
    d_scene->addItem(l_item);
    l_item->setPos(LEFT_MARGIN+Frame(d_pointers[role]),0);

    m_item=new RDMarkerHandle(role,type,this);
    d_scene->addItem(m_item);
    m_item->setPos(LEFT_MARGIN+Frame(d_pointers[role]),handle_pos-12);
    d_handles[role][0]=m_item;
    m_item->
      setSelected((d_selected_markers[0]==role)||(d_selected_markers[1]==role));

    m_item=new RDMarkerHandle(role,type,this);
    d_scene->addItem(m_item);
    m_item->setPos(LEFT_MARGIN+Frame(d_pointers[role]),d_height-handle_pos-8);
    d_handles[role][1]=m_item;
    m_item->
      setSelected((d_selected_markers[0]==role)||(d_selected_markers[1]==role));
  }
}


void RDMarkerView::RemoveMarker(RDMarkerHandle::PointerRole role)
{
  d_pointers[role]=-1;
  QList<QGraphicsItem *> items=d_scene->items();
  for(int i=items.size()-1;i>=0;i--) {
    QGraphicsItem *item=items.at(i);
    if(item->toolTip().contains(RDMarkerHandle::pointerRoleText(role))) {
      d_scene->removeItem(item);
      delete item;
    }
  }
  for(int i=0;i<2;i++) {
    d_handles[role][i]=NULL;
  }

  emit pointerValueChanged(role,-1);
}


void RDMarkerView::PositionMarker(RDMarkerHandle::PointerRole role,int ptr)
{
  QList<QGraphicsItem *> items=d_scene->items();

  for(int i=0;i<items.size();i++) {
    if(items.at(i)->toolTip()==RDMarkerHandle::pointerRoleTypeText(role)) {
      items.at(i)->setPos(LEFT_MARGIN+Frame(ptr),items.at(i)->pos().y());
    }
  }
}


void RDMarkerView::SetReferenceLines()
{
  int sh=d_scene->height();
  double ratio=exp10((double)(d_audio_gain-100*d_play_gain)/2000.0);
  int ref_line=exp10((double)(-REFERENCE_LEVEL)/2000.00)*sh*ratio/
    ((double)d_channels*2.0);
  for(unsigned i=0;i<d_channels;i++) {
    int zero_line=sh/(d_channels*2)+i*sh/(d_channels);
    d_reference_line_items[i][0]->setPos(0,zero_line+ref_line);
    d_reference_line_items[i][1]->setPos(0,zero_line-ref_line);
  }
}
