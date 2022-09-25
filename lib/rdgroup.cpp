// rdgroup.cpp
//
// Abstract a Rivendell Group.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/types.h>
#include <unistd.h>

#include <rdconf.h>
#include <rdgroup.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>

//
// Global Classes
//
RDGroup::RDGroup(QString name,bool create)
{
  RDSqlQuery *q;
  QString sql;

  group_name=name;

  if(create) {
    sql=QString("insert into `GROUPS` set ")+
      "`NAME`='"+RDEscapeString(group_name)+"'";
    q=new RDSqlQuery(sql);
    delete q;
  }
  else {
    //
    // Normalize case
    //
    sql=QString("select `NAME` from `GROUPS` where ")+
      "`NAME`='"+RDEscapeString(name)+"'";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      group_name=q->value(0).toString();
    }
    delete q;
  }
}


bool RDGroup::exists() const
{
  return RDDoesRowExist("GROUPS","NAME",group_name);
}


QString RDGroup::name() const
{
  return group_name;
}


QString RDGroup::description() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"DESCRIPTION").
    toString();
}


void RDGroup::setDescription(const QString &desc) const
{
  SetRow("DESCRIPTION",desc);
}


RDCart::Type RDGroup::defaultCartType() const
{
  return (RDCart::Type)RDGetSqlValue("GROUPS","NAME",group_name,
				    "DEFAULT_CART_TYPE").toUInt();
}


void RDGroup::setDefaultCartType(RDCart::Type type) const
{
  SetRow("DEFAULT_CART_TYPE",(unsigned)type);
}


unsigned RDGroup::defaultLowCart() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"DEFAULT_LOW_CART").
    toUInt();
}


void RDGroup::setDefaultLowCart(unsigned cartnum) const
{
  SetRow("DEFAULT_LOW_CART",cartnum);
}


unsigned RDGroup::defaultHighCart() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"DEFAULT_HIGH_CART").
    toUInt();
}


void RDGroup::setDefaultHighCart(unsigned cartnum) const
{
  SetRow("DEFAULT_HIGH_CART",cartnum);
}


int RDGroup::defaultCutLife() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"DEFAULT_CUT_LIFE").
    toInt();  
}


void RDGroup::setDefaultCutLife(int days) const
{
  SetRow("DEFAULT_CUT_LIFE",days);
}


int RDGroup::cutShelflife() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"CUT_SHELFLIFE").
    toInt();
}


void RDGroup::setCutShelflife(int days) const
{
  SetRow("CUT_SHELFLIFE",days);
}


bool RDGroup::deleteEmptyCarts() const
{
  return RDBool(RDGetSqlValue("GROUPS","NAME",group_name,"DELETE_EMPTY_CARTS").
		toString());
}


void RDGroup::setDeleteEmptyCarts(bool state) const
{
  SetRow("DELETE_EMPTY_CARTS",RDYesNo(state));
}


QString RDGroup::defaultTitle() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"DEFAULT_TITLE").
    toString();
}


void RDGroup::setDefaultTitle(const QString &str)
{
  SetRow("DEFAULT_TITLE",str);
}


QString RDGroup::generateTitle(const QString &pathname)
{
  QString title=defaultTitle();
  QString basename=RDGetBasePart(pathname);
  int ptr=basename.lastIndexOf(".");
  title.replace("%p",RDGetPathPart(pathname));
  title.replace("%f",basename.left(ptr));
  title.replace("%e",basename.right(basename.length()-ptr-1));

  return title;
}


bool RDGroup::enforceCartRange() const
{
  return RDBool(RDGetSqlValue("GROUPS","NAME",group_name,"ENFORCE_CART_RANGE").
		toString());
}


void RDGroup::setEnforceCartRange(bool state) const
{
  SetRow("ENFORCE_CART_RANGE",RDYesNo(state));
}


bool RDGroup::exportReport(ExportType type) const
{
  return RDBool(RDGetSqlValue("GROUPS","NAME",group_name,ReportField(type)).
		toString());
}


void RDGroup::setExportReport(ExportType type,bool state) const
{
  SetRow(ReportField(type),RDYesNo(state));
}


QColor RDGroup::color() const
{
  return QColor(RDGetSqlValue("GROUPS","NAME",group_name,"COLOR").
		toString());
}


void RDGroup::setColor(const QColor &color)
{
  SetRow("COLOR",color.name());
}


QString RDGroup::notifyEmailAddress() const
{
  return RDGetSqlValue("GROUPS","NAME",group_name,"NOTIFY_EMAIL_ADDRESS").
    toString();
}


void RDGroup::setNotifyEmailAddress(const QString &addr) const
{
  SetRow("NOTIFY_EMAIL_ADDRESS",addr);
}


unsigned RDGroup::nextFreeCart(unsigned startcart) const
{
  return GetNextFreeCart(startcart);
}


int RDGroup::freeCartQuantity() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "`DEFAULT_LOW_CART`,"+   // 00
    "`DEFAULT_HIGH_CART` "+  // 01
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return -1;
  }
  if((q->value(0).toInt()<0)||(q->value(1).toInt()<0)) {
    delete q;
    return -1;
  }
  int low=q->value(0).toInt();
  int high=q->value(1).toInt();
  sql=QString("select `NUMBER` from `CART` where ")+
    QString::asprintf("`NUMBER`>=%d)&&(`NUMBER`<=%d)",
		      q->value(0).toInt(),q->value(1).toInt());
  delete q;
  q=new RDSqlQuery(sql);
  int free=high-low-q->size();
  delete q;

  return free;
}

bool RDGroup::reserveCarts(QList<unsigned> *cart_nums,
			   const QString &station_name,RDCart::Type type,
			   int quan) const
{
  unsigned next;
  QString sql;
  RDSqlQuery *q;

  cart_nums->clear();
  if((next=GetNextFreeCart(0))==0) {
    return false;
  }
  while(next!=0) {
    if(ReserveCart(station_name,type,next)) {
      cart_nums->push_back(next);
      next++;
    }
    else {
      for(int i=0;i<cart_nums->size();i++) {
	sql=QString::asprintf("delete from `CART` where `NUMBER`=%u",
			      cart_nums->at(i));
	q=new RDSqlQuery(sql);
	delete q;
      }
      cart_nums->clear();
      next=GetNextFreeCart(next+1);
    }
    if(cart_nums->size()==quan) {
      return true;
    }
  }

  return false;
}


bool RDGroup::cartNumberValid(unsigned cartnum) const
{
  if((cartnum<1)||(cartnum>999999)) {
    return false;
  }
  bool ret=false;
  QString sql=QString("select ")+
    "`DEFAULT_LOW_CART`,"+    // 00
    "`DEFAULT_HIGH_CART`,"+   // 01
    "`ENFORCE_CART_RANGE` "+  // 02
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if(!RDBool(q->value(2).toString())) {
      ret=true;
    }
    else {
      if((cartnum>=q->value(0).toUInt())&&(cartnum<=q->value(1).toUInt())) {
	ret=true;
      }
    }
  }
  delete q;
  return ret;
}


QString RDGroup::xml() const
{
  QString sql;
  RDSqlQuery *q;
  QString ret="";

  sql=QString("select ")+
    "`DESCRIPTION`,"+         // 00
    "`DEFAULT_CART_TYPE`,"+   // 01
    "`DEFAULT_LOW_CART`,"+    // 02
    "`DEFAULT_HIGH_CART`,"+   // 03
    "`CUT_SHELFLIFE`,"+       // 04
    "`DEFAULT_TITLE`,"+       // 05
    "`ENFORCE_CART_RANGE`,"+  // 06
    "`REPORT_TFC`,"+          // 07
    "`REPORT_MUS`,"+          // 08
    "`COLOR` "+               // 09
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret+="<group>\n";
    ret+="  "+RDXmlField("name",group_name);
    ret+="  "+RDXmlField("description",q->value(0).toString());
    switch(q->value(1).toUInt()) {
    case RDCart::Audio:
      ret+="  "+RDXmlField("defaultCartType","audio");
      break;

    case RDCart::Macro:
      ret+="  "+RDXmlField("defaultCartType","macro");
      break;

    case RDCart::All:
      break;
    }
    ret+="  "+RDXmlField("defaultLowCart",q->value(2).toUInt());
    ret+="  "+RDXmlField("defaultHighCart",q->value(3).toUInt());
    ret+="  "+RDXmlField("cutShelfLife",q->value(4).toInt());
    ret+="  "+RDXmlField("defaultTitle",q->value(5).toString());
    ret+="  "+RDXmlField("enforceCartRange",RDBool(q->value(6).toString()));
    ret+="  "+RDXmlField("reportTfc",RDBool(q->value(7).toString()));
    ret+="  "+RDXmlField("reportMus",RDBool(q->value(8).toString()));
    ret+="  "+RDXmlField("color",q->value(9).toString());
    ret+="</group>\n";
  }
  delete q;
  return ret;
}


bool RDGroup::create(const QString &name,bool allow_all_users,
		     bool allow_all_services,QString *err_msg)
{
  QString sql;
  RDSqlQuery *q=NULL;

  if(name.length()>10) {
    *err_msg=QObject::tr("Name too long (ten characters max)");
    return false;
  }
  if(name.isEmpty()||(name.toLower()==QObject::tr("all").toLower())) {
    *err_msg=QObject::tr("Invalid group name");
    return false;
  }
  sql=QString("select ")+
    "`NAME` "+
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *err_msg=QObject::tr("group already exists");
    delete q;
    return false;
  }
  delete q;
  sql=QString("insert into `GROUPS` set ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  if(!RDSqlQuery::apply(sql)) {
    *err_msg=QObject::tr("unable to create database record");
    return false;
  }

  //
  // Create Default Users Perms
  //
  if(allow_all_users) {
    sql="select `LOGIN_NAME` from `USERS`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `USER_PERMS` set ")+
	"`USER_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`GROUP_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;
  }

  //
  // Create Default Service Perms
  //
  if(allow_all_services) {
    sql="select `NAME` from `SERVICES`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      sql=QString("insert into `AUDIO_PERMS` set ")+
	"`SERVICE_NAME`='"+RDEscapeString(q->value(0).toString())+"',"+
	"`GROUP_NAME`='"+RDEscapeString(name)+"'";
      RDSqlQuery::apply(sql);
    }
    delete q;
  }

  *err_msg=QObject::tr("OK");

  return true;
}


bool RDGroup::remove(const QString &name,QString *err_msg)
{
  QString sql;
  RDSqlQuery *q=NULL;

  //
  // Check for carts using group
  //
  sql=QString("select ")+
    "`NUMBER` "+  // 00
    "from `CART` where "+
    "`GROUP_NAME`='"+RDEscapeString(name)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    *err_msg=QObject::tr("group name in use");
    delete q;
    return false;
  }

  sql=QString("delete from `USER_PERMS` where ")+
    "`GROUP_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `AUDIO_PERMS` where ")+
    "`GROUP_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `GROUPS` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  sql=QString("delete from `REPLICATOR_MAP` where ")+
    "`GROUP_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  *err_msg=QObject::tr("OK");

  return true;
}


unsigned RDGroup::GetNextFreeCart(unsigned startcart) const
{
  QString sql;
  RDSqlQuery *q;
  unsigned cart_low_limit;
  unsigned cart_high_limit;

  sql=QString("select ")+
    "`DEFAULT_LOW_CART`,"+   // 00
    "`DEFAULT_HIGH_CART` "+  // 01
    "from `GROUPS` where "+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(startcart>q->value(0).toUInt()) {
      cart_low_limit=startcart;
    }
    else {
      cart_low_limit=q->value(0).toUInt();
    }
    cart_high_limit=q->value(1).toUInt();
    delete q;
    if((cart_low_limit<1)||(startcart>cart_high_limit)) {
      return 0;
    }
    sql=QString("select `NUMBER` from `CART` where ")+
      QString::asprintf("(`NUMBER`>=%u)&&(`NUMBER`<=%u) order by `NUMBER`",
			cart_low_limit,cart_high_limit);
    q=new RDSqlQuery(sql);
    if(q->size()<1) {
      delete q;
      return cart_low_limit;
    }
    for(unsigned i=cart_low_limit;i<=cart_high_limit;i++) {
      if(!q->next()) {
	delete q;
	return i;
      }
      if(i!=q->value(0).toUInt()) {
	delete q;
	return i;
      }
    }
    delete q;
  }
  else {
    delete q;
  }
  return 0;
}


bool RDGroup::ReserveCart(const QString &station_name,RDCart::Type type,
			  unsigned cart_num) const
{
  //
  // We use QSqlQuery here, not RDSqlQuery because the insert could
  // fail and we don't want to reset the DB connection if that happens.
  //
  QString sql;
  QSqlQuery *q;
  bool ret=false;

  if((cart_num>=defaultLowCart())&&(cart_num<=defaultHighCart())) {
    sql=QString::asprintf("insert into `CART` set `NUMBER`=%u,",cart_num)+
      "`GROUP_NAME`='"+RDEscapeString(group_name)+"',"+
      QString::asprintf("`TYPE`=%d,",type)+
      "`TITLE`='["+RDEscapeString(QObject::tr("reserved"))+"]',"+
      "`PENDING_STATION`='"+RDEscapeString(station_name)+"',"+
      QString::asprintf("`PENDING_PID`=%d,",getpid())+
      "`PENDING_DATETIME`=now()";
    q=new QSqlQuery(sql);
    ret=q->isActive();
    delete q;
  }
  return ret;
}


void RDGroup::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString::asprintf("update `GROUPS` set `")+
    param+QString::asprintf("`=%d where ",value)+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDGroup::SetRow(const QString &param,unsigned value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update `GROUPS` set `")+
    param+QString::asprintf("`=%u where ",value)+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDGroup::SetRow(const QString &param,const QString &value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString("update `GROUPS` set `")+
    param+"`='"+RDEscapeString(value)+"' where "+
    "`NAME`='"+RDEscapeString(group_name)+"'";
  q=new RDSqlQuery(sql);
  delete q;
}


QString RDGroup::ReportField(ExportType type) const
{
  switch(type) {
      case RDGroup::Traffic:
	return QString("REPORT_TFC");
	break;

      case RDGroup::Music:
	return QString("REPORT_MUS");
	break;

      default:
	break;
  }
  return QString();
}

