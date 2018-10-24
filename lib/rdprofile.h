// rdprofile.h
//
// A class to read an ini formatted configuration file.
//
// (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPROFILE_H
#define RDPROFILE_H

#include <vector>

#include <qstring.h>

class RDProfileLine
{
 public:
  RDProfileLine();
  QString tag() const;
  void setTag(QString tag);
  QString value() const;
  void setValue(QString value);
  void clear();

 private:
  QString line_tag;
  QString line_value;
};




class RDProfileSection
{
 public:
  RDProfileSection();
  QString name() const;
  void setName(QString name);
  bool getValue(QString tag,QString *value) const;
  void addValue(QString tag,QString value);
  void clear();

 private:
  QString section_name;
  std::vector<RDProfileLine> section_line;
};




/**
 * @short Implements an ini configuration file parser.
 * @author Fred Gleason <fredg@paravelsystems.com>
 * 
 * This class implements an ini configuration file parser.  Methods
 * exist for extracting data as strings, ints, bools or floats. 
 **/
class RDProfile
{
 public:
 /**
  * Instantiates the class.
  **/
  RDProfile();
  QString source() const;
  bool setSource(const QString &filename);
  void setSourceString(const QString &str);
  QString stringValue(QString section,QString tag,
		      QString default_value="",bool *ok=0) const;
  int intValue(QString section,QString tag,
	       int default_value=0,bool *ok=0) const;
  int hexValue(QString section,QString tag,
	       int default_value=0,bool *ok=0) const;
  float floatValue(QString section,QString tag,
		   float default_value=0.0,bool *ok=0) const;
  double doubleValue(QString section,QString tag,
		    double default_value=0.0,bool *ok=0) const;
  bool boolValue(QString section,QString tag,
		 bool default_value=false,bool *ok=0) const;
  void clear();

 private:
  QString profile_source;
  std::vector<RDProfileSection> profile_section;
};


#endif  // RDPROFILE_H
