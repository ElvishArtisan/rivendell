// rdprofile.cpp
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

#include <qfile.h>
#include <qstringlist.h>
#include <q3textstream.h>

#include "rdprofile.h"

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





RDProfileSection::RDProfileSection()
{
  clear();
}


QString RDProfileSection::name() const
{
  return section_name;
}


void RDProfileSection::setName(QString name)
{
  section_name=name;
}


bool RDProfileSection::getValue(QString tag,QString *value) const
{
  for(unsigned i=0;i<section_line.size();i++) {
    if(section_line[i].tag()==tag) {
      *value=section_line[i].value();
      return true;
    }
  }
  return false;
}


void RDProfileSection::addValue(QString tag,QString value)
{
  section_line.push_back(RDProfileLine());
  section_line.back().setTag(tag);
  section_line.back().setValue(value);
}


void RDProfileSection::clear()
{
  section_name="";
  section_line.resize(0);
}





RDProfile::RDProfile()
{
}


QString RDProfile::source() const
{
  return profile_source;
}


bool RDProfile::setSource(const QString &filename)
{
  QString section;
  int offset;

  profile_source=filename;
  profile_section.resize(0);
  profile_section.push_back(RDProfileSection());
  profile_section.back().setName("");
  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::ReadOnly)) {
    delete file;
    return false;
  }
  Q3TextStream *text=new Q3TextStream(file);
  QString line=text->readLine().stripWhiteSpace();
  while(!line.isNull()) {
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(RDProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.find('='))!=-1)) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).stripWhiteSpace());
      }
    }
    line=text->readLine().stripWhiteSpace();
  }
  delete text;
  delete file;
  return true;
}


void RDProfile::setSourceString(const QString &str)
{
  QStringList lines;
  QString section;
  int offset;

  profile_source="";
  profile_section.resize(0);
  profile_section.push_back(RDProfileSection());
  profile_section.back().setName("");
  lines=str.split("\n");
  for(int i=0;i<lines.size();i++) {
    QString line=lines[i];
    if((line.left(1)!=";")&&(line.left(1)!="#")) {
      if((line.left(1)=="[")&&(line.right(1)=="]")) {
	section=line.mid(1,line.length()-2);
	profile_section.push_back(RDProfileSection());
	profile_section.back().setName(section);
      }
      else if(((offset=line.find('='))!=-1)) {
	profile_section.back().
	  addValue(line.left(offset),
		   line.right(line.length()-offset-1).stripWhiteSpace());
      }
    }
  }
}


QString RDProfile::stringValue(QString section,QString tag,
			      QString default_str,bool *ok) const
{
  QString result;

  for(unsigned i=0;i<profile_section.size();i++) {
    if(profile_section[i].name()==section) {
      if(profile_section[i].getValue(tag,&result)) {
	if(ok!=NULL) {
	  *ok=true;
	}
	return result;
      }
      if(ok!=NULL) {
	*ok=false;
      }
      return default_str;
    }
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_str;
}


int RDProfile::intValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  int result=stringValue(section,tag).toInt(&valid,10);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


int RDProfile::hexValue(QString section,QString tag,
		       int default_value,bool *ok) const
{
  bool valid;

  QString str=stringValue(section,tag);
  if(str.left(2).lower()=="0x") {
    str=str.right(str.length()-2);
  }
  int result=str.toInt(&valid,16);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


float RDProfile::floatValue(QString section,QString tag,
			   float default_value,bool *ok) const
{
  bool valid;

  float result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


double RDProfile::doubleValue(QString section,QString tag,
			    double default_value,bool *ok) const
{
  bool valid;

  double result=stringValue(section,tag).toDouble(&valid);
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if(ok!=NULL) {
    *ok=true;
  }
  return result;
}


bool RDProfile::boolValue(QString section,QString tag,
			 bool default_value,bool *ok) const
{
  bool valid;

  QString str=stringValue(section,tag,"",&valid).lower();
  if(!valid) {
    if(ok!=NULL) {
      *ok=false;
    }
    return default_value;
  }
  if((str=="yes")||(str=="true")||(str=="on")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return true;
  }
  if((str=="no")||(str=="false")||(str=="off")) {
    if(ok!=NULL) {
      *ok=true;
    }
    return false;
  }
  if(ok!=NULL) {
    *ok=false;
  }
  return default_value;
}


void RDProfile::clear()
{
  profile_source="";
  profile_section.resize(0);
}
