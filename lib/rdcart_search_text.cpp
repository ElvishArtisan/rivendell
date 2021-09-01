// rdcart_search_text.cpp
//
// Generates a standardized SQL 'INNER JOIN' and 'WHERE' clause for
// filtering Rivendell carts.
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

#include <rdescape_string.h>
#include <rdcart_search_text.h>
#include <rddb.h>

QString RDBaseSearchText(QString filter,bool incl_cuts)
{
  QString edit_filter=filter;
  QString ret="";
  QString str="";
  int pos=0;
  char find;

  edit_filter=edit_filter.trimmed();
  if(edit_filter.isEmpty()) {
    ret=QString(" ((`CART`.`TITLE` like \"%%\")||")+
      "(`CART`.`ARTIST` like \"%%\")||"+
      "(`CART`.`CLIENT` like \"%%\")||"+
      "(`CART`.`AGENCY` like \"%%\")||"+
      "(`CART`.`ALBUM` like \"%%\")||"+
      "(`CART`.`LABEL` like \"%%\")||"+
      "(`CART`.`NUMBER` like \"%%\")||"+
      "(`CART`.`PUBLISHER` like \"%%\")||"+
      "(`CART`.`COMPOSER` like \"%%\")||"+
      "(`CART`.`CONDUCTOR` like \"%%\")||"+
      "(`CART`.`SONG_ID` like \"%%\")||"+
      "(`CART`.`USER_DEFINED` like \"%%\")";
    if(incl_cuts) {
      ret+=QString("||(`CUTS`.`ISCI` like \"%%\")")+
	"||(`CUTS`.`ISRC` like \"%%\")"+
	"||(`CUTS`.`DESCRIPTION` like \"%%\")"+
	"||(`CUTS`.`OUTCUE` like \"%%\")";
    }
    ret+=")";
  }
  else {
    while(!edit_filter.isEmpty()) {
      if(edit_filter.startsWith("\"") && edit_filter.length()>1) {
	edit_filter=edit_filter.remove(0,1);
	find='\"';
      }
      else {
	find=' '; 
      }
      pos=edit_filter.indexOf(find);
      if(pos>=0) {
	str=edit_filter.left(pos);
	edit_filter=edit_filter.remove(0,pos);
	if(find=='\"') {
	  edit_filter=edit_filter.remove(0,1);
	}
	edit_filter=edit_filter.trimmed();
      }
      else {
	str=edit_filter;
	edit_filter=edit_filter.remove(0,edit_filter.length());
      }
      if(!ret.isEmpty()) {
	ret=ret+" AND ";
      }
      QString search=RDEscapeString(str);
      ret=ret+QString(" ((`CART`.`TITLE` like \"%")+search+"%\")||"+
	"(`CART`.`ARTIST` like \"%"+search+"%\")||"+
	"(`CART`.`CLIENT` like \"%"+search+"%\")||"+
	"(`CART`.`AGENCY` like \"%"+search+"%\")||"+
	"(`CART`.`ALBUM` like \"%"+search+"%\")||"+
	"(`CART`.`LABEL` like \"%"+search+"%\")||"+
	"(`CART`.`NUMBER` like \"%"+search+"%\")||"+
	"(`CART`.`PUBLISHER` like \"%"+search+"%\")||"+
	"(`CART`.`COMPOSER` like \"%"+search+"%\")||"+
	"(`CART`.`CONDUCTOR` like \"%"+search+"%\")||"+
	"(`CART`.`SONG_ID` like \"%"+search+"%\")||"+
	"(`CART`.`USER_DEFINED` like \"%"+search+"%\")";
      if(incl_cuts) {
	ret+=QString("||(`CUTS`.`ISCI` like \"%")+search+"%\")"+
	  "||(`CUTS`.`ISRC` like \"%"+search+"%\")"+
	  "||(`CUTS`.`DESCRIPTION` like \"%"+search+"%\")"+
	  "||(`CUTS`.`OUTCUE` like \"%"+search+"%\")";
      }
      ret+=") ";
    }
   
  }
  return ret;
}

QString RDSchedSearchText(const QString &schedcode)
{
  QString ret="";

  if(!schedcode.isEmpty()) {
    ret+=QString(" inner join `CART_SCHED_CODES` on `CART`.`NUMBER`=`CART_SCHED_CODES`.`CART_NUMBER` and `CART_SCHED_CODES`.`SCHED_CODE`='")+RDEscapeString(schedcode)+"' ";
  }

  return ret;
}


QString RDSchedSearchText(const QStringList &schedcodes)
{
  QString ret="";

  for(int i=0;i<schedcodes.size();i++) {
    ret+=QString::asprintf(" inner join `CART_SCHED_CODES` as S%d on (`CART`.`NUMBER`=S%d.`CART_NUMBER` and S%d.`SCHED_CODE`='%s')",i,i,i,schedcodes.at(i).toUtf8().constData());
  }
  return ret;
}


QString RDCartSearchText(QString filter,const QString &group,
			 const QString &schedcode,bool incl_cuts)
{
  QString ret="";

  ret+=RDSchedSearchText(schedcode);
  ret+=QString(" where ")+RDBaseSearchText(filter,incl_cuts);
  if(!group.isEmpty()) {
    ret+=QString("&&(`CART`.`GROUP_NAME`=\"")+RDEscapeString(group)+"\")";
  }

  return ret;
}

QString RDCartSearchText(QString filter,const QString &group,
			 const QStringList &schedcodes,bool incl_cuts)
{
  QString ret="";

  ret+=RDSchedSearchText(schedcodes);
  ret+=QString(" where ")+RDBaseSearchText(filter,incl_cuts);
  if(!group.isEmpty()) {
    ret+=QString("&&(`CART`.`GROUP_NAME`=\"")+RDEscapeString(group)+"\")";
  }

  return ret;
}


QString RDAllCartSearchText(const QString &filter,const QString &schedcode,
			    const QString &user,bool incl_cuts)
{
  QString sql;
  RDSqlQuery *q;
  QString search="";

  search+=RDSchedSearchText(schedcode);
  search+=" where (";
  sql=QString("select `GROUP_NAME` from `USER_PERMS` where ")+
    "`USER_NAME`='"+RDEscapeString(user)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    search+=QString("(`CART`.`GROUP_NAME`='")+
      RDEscapeString(q->value(0).toString())+"')||";
  }
  delete q;
  search=search.left(search.length()-2)+QString(")");
  search+=QString("&&")+RDBaseSearchText(filter,incl_cuts);

  return search;
}

QString RDAllCartSearchText(const QString &filter,const QStringList &schedcodes,
			    const QString &user,bool incl_cuts)
{
  QString sql;
  RDSqlQuery *q;
  QString search="";

  search+=RDSchedSearchText(schedcodes);
  search+=" where (";
  sql=QString("select `GROUP_NAME` from `USER_PERMS` where ")+
    "`USER_NAME`='"+RDEscapeString(user)+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    search+=QString("(`CART`.`GROUP_NAME`='")+
      RDEscapeString(q->value(0).toString())+"')||";
  }
  delete q;
  search=search.left(search.length()-2)+QString(")");
  search+=QString("&&")+RDBaseSearchText(filter,incl_cuts);
  return search;
}
