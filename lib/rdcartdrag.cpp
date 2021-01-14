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

#include <QPixmap>

#include "rd.h"
#include "rdcartdrag.h"

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
