// rdcartdrag.cpp
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

#include <QMimeSource>
#include <QPixmap>

#include "rd.h"
#include "rdcartdrag.h"
#include "rdprofile.h"

RDCartDrag::RDCartDrag(unsigned cartnum,const QString &title,
		       const QColor &color)
  : QMimeData()
{
  SetData(cartnum,color,title);
  d_formats.push_back(RDMIMETYPE_CART);
}


QStringList RDCartDrag::formats() const
{
  return d_formats;
}


bool RDCartDrag::hasFormat(const QString &mimetype) const
{
  return d_formats.contains(mimetype);
}


bool RDCartDrag::canDecode(QMimeSource *e)
{
  return e->provides(RDMIMETYPE_CART);
}


bool RDCartDrag::decode(QMimeSource *e,unsigned *cartnum,QColor *color,
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


bool RDCartDrag::decode(QMimeSource *e,RDLogLine *ll,
			RDLogLine::TransType next_trans,int log_mach,
			bool timescale,RDLogLine::TransType trans)
{
  unsigned cartnum;

  RDCartDrag::decode(e,&cartnum);
  ll->loadCart(cartnum,next_trans,log_mach,timescale,trans);

  return true;
}


QVariant RDCartDrag::retrieveData(const QString &mimetype,QVariant::Type type)
  const
{
  return d_data.value(mimetype,QVariant());
}


void RDCartDrag::SetData(unsigned cartnum,const QColor &color,
			 const QString &title)
{
  QString str="[Rivendell-Cart]\n";
  str+="Number="+QString().sprintf("%06u",cartnum)+"\n";
  if(color.isValid()) {
    str+="Color="+color.name()+"\n";
  }
  if(!title.isEmpty()) {
    str+="ButtonText="+title+"\n";
  }
  d_data[RDMIMETYPE_CART]=str;
}
