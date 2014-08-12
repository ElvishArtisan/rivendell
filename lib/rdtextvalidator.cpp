// rdtextvalidator.cpp
//
// Validate a string as being valid for a SQL text datatype.
//
//   (C) Copyright 2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtextvalidator.cpp,v 1.11 2010/07/29 19:32:34 cvs Exp $
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

#include <rdtextvalidator.h>


RDTextValidator::RDTextValidator(QObject *parent,const char *name,bool allow_quote)
  : QValidator(parent,name)
{
  if(!allow_quote) {
    banned_chars.push_back(34);  // Double Quote
    }
  banned_chars.push_back(39);  // Single Quote
  banned_chars.push_back(92);  // Backslash Quote
  banned_chars.push_back(96);  // Apostrophe Quote
}


QValidator::State RDTextValidator::validate(QString &input,int &pos) const
{
  char c=input.at(pos-1).latin1();
  for(unsigned i=0;i<banned_chars.size();i++) {
    if(banned_chars[i]==c) {
      return QValidator::Invalid;
    }
  }
  return QValidator::Acceptable;
}


void RDTextValidator::addBannedChar(char c)
{
  banned_chars.push_back(c);
}


QString RDTextValidator::stripString(QString str)
{
  str.replace(34,"");  // Double Quote
  str.replace(39,"");  // Single Quote
  str.replace(92,"");  // Backslash Quote
  str.replace(96,"");  // Apostrophe Quote

  return str;
}
