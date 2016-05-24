// rdprofileline.cpp
//
// A container class for profile lines.
//
// (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include <rdprofileline.h>

RDProfileLine::RDProfileLine()
{
  clear();
}


QString RDProfileLine::tag() const
{
  return line_tag;
}


void RDProfileLine::setTag(QString tag)
{
  line_tag=tag;
}


QString RDProfileLine::value() const
{
  return line_value;
}


void RDProfileLine::setValue(QString value)
{
  line_value=value;
}


void RDProfileLine::clear()
{
  line_tag="";
  line_value="";
}
