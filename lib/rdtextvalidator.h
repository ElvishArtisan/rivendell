// rdtextvalidator.h
//
// Validate a string as being valid for a SQL text datatype.
//
//   (C) Copyright 2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdtextvalidator.h,v 1.9 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDTEXTVALIDATOR_H
#define RDTEXTVALIDATOR_H

#include <vector>

#include <qvalidator.h>

using namespace std;

class RDTextValidator : public QValidator
{
 public:
  RDTextValidator(QObject *parent=0,const char *name=0,bool allow_quote=false);
  QValidator::State validate(QString &input,int &pos) const;
  void addBannedChar(char c);
  static QString stripString(QString str);

 private:
  vector<char> banned_chars;
};


#endif  // RDTEXTVALIDATOR_H
