// rdidvalidator.cpp
//
// Validate a string as being valid for a MySQL identifier.
//   See http://dev.mysql.com/doc/refman/5.0/en/identifiers.html
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdidvalidator.cpp,v 1.1.2.1 2014/05/21 18:19:42 cvs Exp $
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

#include <rdidvalidator.h>

RDIdValidator::RDIdValidator(QObject *parent)
  : QValidator(parent)
{
  banned_chars.push_back('/');
  banned_chars.push_back('\\');
  banned_chars.push_back('.');
  banned_chars.push_back(96);    // Apostrophe Quote
  banned_chars.push_back(0);   // NULL
}


QValidator::State RDIdValidator::validate(QString &input,int &pos) const
{
  char c=input.at(pos-1).latin1();
  for(unsigned i=0;i<banned_chars.size();i++) {
    if(banned_chars[i]==c) {
      return QValidator::Invalid;
    }
  }
  return QValidator::Acceptable;
}


void RDIdValidator::addBannedChar(char c)
{
  banned_chars.push_back(c);
}
