// rdcut_path.cpp
//
// Get the full Cut Path for a given Cut Name.
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qobject.h>

#include <rdcut.h>
#include <rdcart.h>
#include <rdcut_path.h>

QString RDCutPath(QString cutname)
{
  QString path;

  RDCut *cut=new RDCut(cutname); 
  if(!cut->exists()) {
    delete cut;
    return QObject::tr("UNKNOWN CUT");
  }
  RDCart *cart=new RDCart(cut->cartNumber());

  if(!cart->exists()) {
    delete cart;
    delete cut;
    return QObject::tr("UNKNOWN CUT");
  }
  if((cart->title().isEmpty())&&(cut->description().isEmpty())) {
    path=QString();
  }
  else {
    path=cart->title()+"->"+cut->description();
  }
  delete cart;
  delete cut;
  return path;
}
