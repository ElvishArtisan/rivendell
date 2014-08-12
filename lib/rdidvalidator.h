// rdidvalidator.h
//
// Validate a string as being valid for a MySQL identifier.
//   See http://dev.mysql.com/doc/refman/5.0/en/identifiers.html
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdidvalidator.h,v 1.1.2.1 2014/05/21 18:19:42 cvs Exp $
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

#ifndef RDIDVALIDATOR_H
#define RDIDVALIDATOR_H

#include <vector>

#include <qvalidator.h>

class RDIdValidator : public QValidator
{
 public:
  RDIdValidator(QObject *parent=0);
  QValidator::State validate(QString &input,int &pos) const;
  void addBannedChar(char c);

 private:
  std::vector<char> banned_chars;
};


#endif  // RDIDVALIDATOR_H
