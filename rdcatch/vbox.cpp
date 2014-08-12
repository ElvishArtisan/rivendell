// vbox.cpp
//
// A QVBox widget with dynamic horizontal resizing.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: vbox.cpp,v 1.5 2010/07/29 19:32:36 cvs Exp $
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

#include <qobjectlist.h>

#include <vbox.h>


VBox::VBox(QWidget *parent,const char *name,WFlags f)
  : QWidget(parent,name,f)
{
  box_spacing=0;
  setGeometry(QWidget::geometry());
}


void VBox::addWidget(QWidget *widget)
{
  box_children.push_back(widget);
}


void VBox::setSpacing(int space)
{
  box_spacing=space;
}


void VBox::setGeometry(int x,int y,int w,int h)
{
  int ypos=0;

  for(unsigned i=0;i<box_children.size();i++) {
    box_children[i]->
      setGeometry(0,ypos,w,box_children[i]->sizeHint().height());
    ypos+=(box_children[i]->sizeHint().height()+box_spacing);
  }
  QWidget::setGeometry(x,y,w,h);
}


void VBox::setGeometry(const QRect &r)
{
  setGeometry(r.x(),r.y(),r.width(),r.height());
}
