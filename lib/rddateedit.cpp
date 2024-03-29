// rddateedit.cpp
//
// QDateEdit with date-format awareness
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdapplication.h"
#include "rddateedit.h"

RDDateEdit::RDDateEdit(QWidget *parent)
  : QDateEdit(parent)
{
  setDisplayFormat(rda->shortDateFormat());

  d_read_only=false;
}


bool RDDateEdit::isReadOnly()
{
  return d_read_only;
}


void RDDateEdit::setReadOnly(bool state)
{
  if(d_read_only!=state) {
    if(state) {
      setDateRange(date(),date());
    }
    else {
      setDateRange(QDate(),QDate());
    }
    d_read_only=state;
  }
}
