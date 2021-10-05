// rdtextvalidator.cpp
//
// Validate a string as being valid for a SQL text datatype.
//
//   (C) Copyright 2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

RDTextValidator::RDTextValidator(QObject *parent,bool allow_quote)
  : QValidator(parent)
{
  d_upper_case_only=false;
  d_lower_case_only=false;

  if(!allow_quote) {
    banned_chars.push_back(34);  // Double Quote
    }
  banned_chars.push_back(39);  // Single Quote
  banned_chars.push_back(92);  // Backslash Quote
  banned_chars.push_back(96);  // Apostrophe Quote
}


QValidator::State RDTextValidator::validate(QString &input,int &pos) const
{
  for(int i=0;i<banned_chars.size();i++) {
    if(input.contains(banned_chars.at(i))) {
      return QValidator::Invalid;
    }
  }
  if(d_upper_case_only&&(input.toUpper()!=input)) {
    return QValidator::Invalid;
  }
  if(d_lower_case_only&&(input.toLower()!=input)) {
    return QValidator::Invalid;
  }
  return QValidator::Acceptable;
}


void RDTextValidator::addBannedChar(char c)
{
  banned_chars.push_back(QChar(c));
}


void RDTextValidator::addBannedChar(const QChar &c)
{
  banned_chars.push_back(c);
}


void RDTextValidator::setUpperCaseOnly(bool state)
{
  d_upper_case_only=state;
}


void RDTextValidator::setLowerCaseOnly(bool state)
{
  d_lower_case_only=state;
}


QString RDTextValidator::stripString(QString str)
{
  str.replace(34,"");  // Double Quote
  str.replace(39,"");  // Single Quote
  str.replace(92,"");  // Backslash Quote
  str.replace(96,"");  // Apostrophe Quote

  return str;
}
