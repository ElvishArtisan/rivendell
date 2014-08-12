// rdcombobox.cpp
//
// A Combo Box widget for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcombobox.cpp,v 1.8 2011/05/18 18:57:54 cvs Exp $
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

#include <rdcombobox.h>


RDComboBox::RDComboBox(QWidget *parent,const char *name)
  : QComboBox(parent,name)
{
  combo_setup_mode=false;
}


void RDComboBox::insertItem(const QString &str,bool unique)
{
  if(unique) {
    if(!IsItemUnique(str)) {
      return;
    }
  }
  QComboBox::insertItem(str);
}


void RDComboBox::setSetupMode(bool state)
{
  combo_setup_mode=state;
}


void RDComboBox::addIgnoredKey(int key)
{
  ignored_keys.push_back(key);
}


void RDComboBox::mousePressEvent(QMouseEvent *e)
{
  if(combo_setup_mode) {
    emit setupClicked();
  }
  else {
    QComboBox::mousePressEvent(e);
  }
}


void RDComboBox::keyPressEvent(QKeyEvent *e)
{
  for(unsigned i=0;i<ignored_keys.size();i++) {
    if(e->key()==ignored_keys[i]) {
      e->ignore();
      return;
    }
  }
  QComboBox::keyPressEvent(e);
}


bool RDComboBox::IsItemUnique(const QString &str)
{
  for(int i=0;i<count();i++) {
    if(str==text(i)) {
      return false;
    }
  }
  return true;
}
