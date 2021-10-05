// rdtextvalidator.h
//
// Validate a string as being valid for a SQL text datatype.
//
//   (C) Copyright 2004-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QChar>
#include <QList>
#include <QValidator>

class RDTextValidator : public QValidator
{
 public:
  RDTextValidator(QObject *parent=0,bool allow_quote=false);
  QValidator::State validate(QString &input,int &pos) const;
  void addBannedChar(char c);
  void addBannedChar(const QChar &c);
  void setUpperCaseOnly(bool state);
  void setLowerCaseOnly(bool state);
  static QString stripString(QString str);

 private:
  QList<QChar> banned_chars;
  bool d_upper_case_only;
  bool d_lower_case_only;
};


#endif  // RDTEXTVALIDATOR_H
