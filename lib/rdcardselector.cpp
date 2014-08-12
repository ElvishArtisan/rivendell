// rdcardselector.cpp
//
// Audio card selector widget for Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcardselector.cpp,v 1.21.8.1 2013/03/22 15:11:50 cvs Exp $
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>


#include <rdcardselector.h>


RDCardSelector::RDCardSelector(QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  card_id=-1;
  yoffset=0;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  yoffset=0;

  //
  // Title
  //
  card_title=new QLabel(this,"card_title");
  card_title->setGeometry(0,0,geometry().width(),19);
  card_title->setFont(QFont("Helvetica",12,QFont::Bold));
  card_title->setAlignment(AlignHCenter);
  card_title->hide();

  //
  // Card
  //
  card_card_box=new QSpinBox(this,"card_card_box");
  card_card_box->setGeometry(60,yoffset,50,19);
  card_card_box->setSpecialValueText("None");
  card_card_box->setMinValue(-1);
  card_card_box->setMaxValue(RD_MAX_CARDS-1);
  card_card_box->setValue(-1);
  connect(card_card_box,SIGNAL(valueChanged(int)),this,SLOT(cardData(int)));
  card_card_label=new QLabel(card_card_box,tr("Card:"),this,
				       "card_card_label");
  card_card_label->setGeometry(0,yoffset+2,55,19);
  card_card_label->setAlignment(AlignRight|ShowPrefix);

  //
  // Port
  //
  card_port_box=new QSpinBox(this,"card_port_box");
  card_port_box->setGeometry(60,yoffset+22,50,19);
  card_port_box->setSpecialValueText("None");
  card_port_box->setMinValue(-1);
  card_port_box->setMaxValue(RD_MAX_PORTS-1);
  card_port_box->setValue(-1);
  connect(card_port_box,SIGNAL(valueChanged(int)),this,SLOT(portData(int)));
  card_port_label=new QLabel(card_port_box,tr("Port:"),this,
				       "card_port_label");
  card_port_label->setGeometry(0,yoffset+24,55,19);
  card_port_label->setAlignment(AlignRight|ShowPrefix);
  for(int i=0;i<RD_MAX_CARDS;i++) {
    card_max_ports[i] = 0;
    cardData(i);
  }
  card_port_box->setDisabled(true);
}


RDCardSelector::~RDCardSelector()
{
  delete card_title;
  delete card_card_box;
  delete card_port_box;
}


QSize RDCardSelector::sizeHint() const
{
  return QSize(110,41+yoffset);
}


QSizePolicy RDCardSelector::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool RDCardSelector::isDisabled() const
{
  return (card_card_box->value()<0)||(card_port_box->value()<0);
}


int RDCardSelector::id() const
{
  return card_id;
}


void RDCardSelector::setId(int id)
{
  card_id=id;
}


QString RDCardSelector::title() const
{
  return card_title->text();
}


void RDCardSelector::setTitle(QString title)
{
  card_title->setText(title);
  if(title.isEmpty()) {
    yoffset=0;
    card_title->hide();
  }
  else {
    yoffset=22;
    card_title->show();
  }
  card_card_box->setGeometry(60,yoffset,50,19);
  card_card_label->setGeometry(0,yoffset+2,55,19);
  card_port_box->setGeometry(60,yoffset+44,50,19);
  card_port_label->setGeometry(0,yoffset+46,55,19);
}


int RDCardSelector::card() const
{
  return card_card_box->value();
}


void RDCardSelector::setCard(int card)
{
  card_card_box->setValue(card);
}


int RDCardSelector::port() const
{
  return card_port_box->value();
}


void RDCardSelector::setPort(int port)
{
  card_port_box->setValue(port);
}


int RDCardSelector::maxCards() const
{
  return card_card_box->maxValue()+1;
}


void RDCardSelector::setMaxCards(int num)
{
  card_card_box->setMaxValue(num-1);
}


int RDCardSelector::maxPorts(int card) const
{
  if(card>=0) {
    return card_max_ports[card];
  }
  return 0;
}


void RDCardSelector::setMaxPorts(int card,int num)
{
  if(card>=0) {
    card_max_ports[card]=num;
    if (card == this->card()){
      card_port_box->setMaxValue(num-1);
      if (num == 0){
	card_port_box->setValue(-1);
	card_port_box->setDisabled(true);
      } else {
	card_port_box->setDisabled(false);
      }
    }
  }
}

void RDCardSelector::cardData(int card)
{
  int c;
  if(card>=0) {
    c = card_max_ports[card]-1;
    card_port_box->setMaxValue(c);
    if (c <0){
      card_port_box->setValue(-1);
    }
    card_port_box->setDisabled((c>=0) ? false : true);
  }
  else {
    card_port_box->setValue(-1);
    card_port_box->setDisabled(true);
  }
  emit cardChanged(card);
  emit settingsChanged(card_id,card,card_port_box->value());
}


void RDCardSelector::portData(int port)
{
  emit portChanged(port);
  emit settingsChanged(card_id,card_card_box->value(),port);
}
