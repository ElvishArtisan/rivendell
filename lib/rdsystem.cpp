// rdsystem.cpp
//
// System-wide Rivendell settings
//
//   (C) Copyright 2009,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rddb.h>
#include <rdconf.h>
#include <rdsystem.h>
#include <rdweb.h>

RDSystem::RDSystem()
{
}


unsigned RDSystem::sampleRate() const
{
  return GetValue("SAMPLE_RATE").toUInt();
}


void RDSystem::setSampleRate(unsigned rate) const
{
  SetRow("SAMPLE_RATE",rate);
}


bool RDSystem::allowDuplicateCartTitles() const
{
  bool ret=false;
  QString sql;
  RDSqlQuery *q;

  sql="select DUP_CART_TITLES from SYSTEM";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDBool(q->value(0).toString());
  }
  delete q;
  return ret;
}


void RDSystem::setAllowDuplicateCartTitles(bool state) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("update SYSTEM set DUP_CART_TITLES=\"%s\"",
			(const char *)RDYesNo(state));
  q=new RDSqlQuery(sql);
  delete q;
}


bool RDSystem::fixDuplicateCartTitles() const
{
  bool ret=false;
  QString sql;
  RDSqlQuery *q;

  sql="select FIX_DUP_CART_TITLES from SYSTEM";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=RDBool(q->value(0).toString());
  }
  delete q;
  return ret;
}


void RDSystem::setFixDuplicateCartTitles(bool state) const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("update SYSTEM set FIX_DUP_CART_TITLES=\"%s\"",
			(const char *)RDYesNo(state));
  q=new RDSqlQuery(sql);
  delete q;
}


unsigned RDSystem::maxPostLength() const
{
  unsigned ret;

  QString sql="select MAX_POST_LENGTH from SYSTEM";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toUInt();
  }
  else {
    ret=RD_DEFAULT_MAX_POST_LENGTH;
  }
  delete q;
  return ret;
}


void RDSystem::setMaxPostLength(unsigned bytes) const
{
  QString sql=QString().sprintf("update SYSTEM set MAX_POST_LENGTH=%u",bytes);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


QString RDSystem::isciXreferencePath() const
{
  return GetValue("ISCI_XREFERENCE_PATH").toString();
}


void RDSystem::setIsciXreferencePath(const QString &str) const
{
  SetRow("ISCI_XREFERENCE_PATH",str);
}


QString RDSystem::tempCartGroup() const
{
  return GetValue("TEMP_CART_GROUP").toString();
}


void RDSystem::setTempCartGroup(const QString &str) const
{
  SetRow("TEMP_CART_GROUP",str);
}


QString RDSystem::xml() const
{
  QString xml="<systemSettings>\n";
  xml+=RDXmlField("sampleRate",sampleRate());
  xml+=RDXmlField("duplicateTitles",allowDuplicateCartTitles());
  xml+=RDXmlField("fixDuplicateTitles",fixDuplicateCartTitles());
  xml+=RDXmlField("maxPostLength",maxPostLength());
  xml+=RDXmlField("isciXreferencePath",isciXreferencePath());
  xml+=RDXmlField("tempCartGroup",tempCartGroup());
  xml+="</systemSettings>\n";

  return xml;
}


QVariant RDSystem::GetValue(const QString &field) const
{
  QVariant ret;
  QString sql=QString().sprintf("select %s from SYSTEM",
				(const char *)field);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0);
  }
  delete q;
  return ret;
}


void RDSystem::SetRow(const QString &param,QString value) const
{
  RDSqlQuery *q;
  QString sql;

  value.replace("\\","\\\\");  // Needed to preserve Windows pathnames
  sql=QString().sprintf("update SYSTEM set %s=\"%s\"",
			(const char *)param,
			(const char *)value);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDSystem::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update SYSTEM set %s=%d",
			(const char *)param,
			value);
  q=new RDSqlQuery(sql);
  delete q;
}
