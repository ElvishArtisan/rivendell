// rdcartdrag.cpp
//
// Stored value drag object for Rivendell carts.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcartdrag.cpp,v 1.1.2.7 2014/01/20 19:13:29 cvs Exp $
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

#include <rd.h>
#include <rdcart.h>
#include <rdcartdrag.h>
#include <rdprofile.h>

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/trashcan-16x16.xpm"

RDCartDrag::RDCartDrag(unsigned cartnum,const QPixmap *icon,QWidget *src)
  : QStoredDrag(RDMIMETYPE_CART,src)
{
  SetData(cartnum,QColor(),QString());
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


RDCartDrag::RDCartDrag(unsigned cartnum,const QString &title,
		       const QColor &color,QWidget *src)
  : QStoredDrag(RDMIMETYPE_CART,src)
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


bool RDCartDrag::canDecode(QMimeSource *e)
{
  return e->provides(RDMIMETYPE_CART);
}


bool RDCartDrag::decode(QMimeSource *e,unsigned *cartnum,QColor *color,
			QString *title)
{
  RDProfile *p=new RDProfile();
  p->setSourceString(e->encodedData(RDMIMETYPE_CART));
  *cartnum=p->intValue("Rivendell-Cart","Number");
  if(color!=NULL) {
    color->setNamedColor(p->stringValue("Rivendell-Cart","Color"));
  }
  if(title!=NULL) {
    *title=p->stringValue("Rivendell-Cart","ButtonText");
  }

  return true;
}


bool RDCartDrag::decode(QMimeSource *e,RDLogLine *ll,
			RDLogLine::TransType next_trans,int log_mach,
			bool timescale,RDLogLine::TransType trans)
{
  unsigned cartnum;

  RDCartDrag::decode(e,&cartnum);
  ll->loadCart(cartnum,next_trans,log_mach,timescale,trans);

  return true;
}


void RDCartDrag::SetData(unsigned cartnum,const QColor &color,const QString &title)
{
  QString str="[Rivendell-Cart]\n";
  str+="Number="+QString().sprintf("%06u",cartnum)+"\n";
  if(color.isValid()) {
    str+="Color="+color.name()+"\n";
  }
  if(!title.isEmpty()) {
    str+="ButtonText="+title+"\n";
  }
  QByteArray data(str.length());
  data.duplicate(str,str.length());
  setEncodedData(data);
}
