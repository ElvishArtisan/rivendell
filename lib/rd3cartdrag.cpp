// rd3cartdrag.cpp
//
// Stored value drag object for Rivendell carts.
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <string.h>

#include <qstringlist.h>

#include "rd.h"
#include "rd3cartdrag.h"
#include "rdcart.h"
#include "rdprofile.h"

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/trashcan-16x16.xpm"

RD3CartDrag::RD3CartDrag(unsigned cartnum,const QPixmap *icon,const QColor &color,
		       QWidget *src)
  : Q3StoredDrag(RDMIMETYPE_CART,src)
{
  SetData(cartnum,color,QString());
  if(icon==NULL) {
    RDCart *cart=new RDCart(cartnum);
    switch(cart->type()) {
    case RDCart::Audio:
      setPixmap(QPixmap(play_xpm));
      break;

    case RDCart::Macro:
      setPixmap(QPixmap(rml5_xpm));
      break;

    case RDCart::All:
      break;
    }
    delete cart;
  }
  else {
    setPixmap(*icon);
  }
}


RD3CartDrag::RD3CartDrag(unsigned cartnum,const QString &title,
		       const QColor &color,QWidget *src)
  : Q3StoredDrag(RDMIMETYPE_CART,src)
{
  SetData(cartnum,color,title);
  if(cartnum==0) {
    setPixmap(QPixmap(trashcan_xpm));
  }
  else {
    RDCart *cart=new RDCart(cartnum);
    switch(cart->type()) {
    case RDCart::Audio:
      setPixmap(QPixmap(play_xpm));
      break;
      
    case RDCart::Macro:
      setPixmap(QPixmap(rml5_xpm));
      break;
      
    case RDCart::All:
      break;
    }
    delete cart;
  }
}


bool RD3CartDrag::canDecode(QMimeSource *e)
{
  return e->provides(RDMIMETYPE_CART);
}


bool RD3CartDrag::decode(QMimeSource *e,unsigned *cartnum,QColor *color,
			QString *title)
{
  RDProfile *p=new RDProfile();
  p->setSourceString(QString::fromUtf8(e->encodedData(RDMIMETYPE_CART)));
  *cartnum=p->intValue("Rivendell-Cart","Number");
  if(color!=NULL) {
    color->setNamedColor(p->stringValue("Rivendell-Cart","Color"));
  }
  if(title!=NULL) {
    *title=p->stringValue("Rivendell-Cart","ButtonText");
  }

  return true;
}


bool RD3CartDrag::decode(QMimeSource *e,RDLogLine *ll,
			RDLogLine::TransType next_trans,int log_mach,
			bool timescale,RDLogLine::TransType trans)
{
  unsigned cartnum;

  RD3CartDrag::decode(e,&cartnum);
  ll->loadCart(cartnum,next_trans,log_mach,timescale,trans);

  return true;
}


void RD3CartDrag::SetData(unsigned cartnum,const QColor &color,const QString &title)
{
  QString str="[Rivendell-Cart]\n";
  str+="Number="+QString().sprintf("%06u",cartnum)+"\n";
  if(color.isValid()) {
    str+="Color="+color.name()+"\n";
  }
  if(!title.isEmpty()) {
    str+="ButtonText="+title+"\n";
  }
  setEncodedData(str.toUtf8());
}
