// rdcart_search_text.cpp
//
// Generates a standardized SQL 'WHERE' clause for filtering Rivendell carts.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcart_search_text.cpp,v 1.21.4.2 2013/12/11 20:54:15 cvs Exp $
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
QString return_string="";
QString search_string="";
int pos=0;
char find;

edit_filter=edit_filter.stripWhiteSpace();
if(edit_filter.isEmpty()) {
      return_string=QString().sprintf(" ((CART.TITLE like \"%%%s%%\")||\
      (CART.ARTIST like \"%%%s%%\")||(CART.CLIENT like \"%%%s%%\")||\
      (CART.AGENCY like \"%%%s%%\")||(CART.ALBUM like \"%%%s%%\")||\
      (CART.LABEL like \"%%%s%%\")||(CART.NUMBER like \"%%%s%%\")||\
      (CART.PUBLISHER like \"%%%s%%\")||(CART.COMPOSER like \"%%%s%%\")||\
      (CART.CONDUCTOR like \"%%%s%%\")||(CART.SONG_ID like \"%%%s%%\")||\
      (CART.USER_DEFINED like \"%%%s%%\")",
  			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8(),
			   (const char *)search_string.utf8());
    if(incl_cuts) {
      return_string+=QString().sprintf("||(CUTS.ISCI like \"%%%s%%\")\
                                        ||(CUTS.ISRC like \"%%%s%%\")\
                                        ||(CUTS.DESCRIPTION like \"%%%s%%\")\
                                        ||(CUTS.OUTCUE like \"%%%s%%\")",
				       (const char *)search_string.utf8(),
				       (const char *)search_string.utf8(),
				       (const char *)search_string.utf8(),
				       (const char *)search_string.utf8());
    }
    return_string+=")";
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
    pos=edit_filter.find(find);
    if(pos>=0) {
      search_string=edit_filter.left(pos);
      edit_filter=edit_filter.remove(0,pos);
      if(find=='\"') {
	edit_filter=edit_filter.remove(0,1);
      }
      edit_filter=edit_filter.stripWhiteSpace();
    }
    else {
      search_string=edit_filter;
      edit_filter=edit_filter.remove(0,edit_filter.length());
    }
    if(!return_string.isEmpty()) {
      return_string=return_string+" AND ";
    }
    QString search=RDEscapeString(search_string);
    return_string=return_string+QString().sprintf(" ((CART.TITLE like \"%%%s%%\")||\
      (CART.ARTIST like \"%%%s%%\")||(CART.CLIENT like \"%%%s%%\")||	\
      (CART.AGENCY like \"%%%s%%\")||(CART.ALBUM like \"%%%s%%\")||	\
      (CART.LABEL like \"%%%s%%\")||(CART.NUMBER like \"%%%s%%\")||	\
      (CART.PUBLISHER like \"%%%s%%\")||(CART.COMPOSER like \"%%%s%%\")|| \
      (CART.CONDUCTOR like \"%%%s%%\")||(CART.SONG_ID like \"%%%s%%\")|| \
      (CART.USER_DEFINED like \"%%%s%%\")",
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8(),
				    (const char *)search.utf8());
    if(incl_cuts) {
      return_string+=QString().sprintf("||(CUTS.ISCI like \"%%%s%%\")\
                                        ||(CUTS.ISRC like \"%%%s%%\")\
                                        ||(CUTS.DESCRIPTION like \"%%%s%%\")\
                                        ||(CUTS.OUTCUE like \"%%%s%%\")",
				       (const char *)search.utf8(),
				       (const char *)search.utf8(),
				       (const char *)search.utf8(),
				       (const char *)search.utf8());
    }
    return_string+=")";
  }
  
 }
 return return_string;
}


QString RDCartSearchText(QString filter,const QString &group,
			 const QString &schedcode,bool incl_cuts)
{
  QString ret=QString(" ")+RDBaseSearchText(filter,incl_cuts);
  if(!group.isEmpty()) {
    ret+=QString("&&(CART.GROUP_NAME=\"")+RDEscapeString(group)+"\")";
  }

  if(!schedcode.isEmpty()) {
    QString code=schedcode+"          ";
    code=code.left(11);
    ret+=QString().sprintf("&&(SCHED_CODES like \"%%%s%%\")",
			   (const char *)code);
  }
  return ret.utf8();
}


QString RDAllCartSearchText(const QString &filter,const QString &schedcode,
			    const QString &user,bool incl_cuts)
{
  QString sql;
  RDSqlQuery *q;
  QString search="(";

  sql=QString().sprintf("select GROUP_NAME from USER_PERMS\
                         where USER_NAME=\"%s\"",
			(const char *)user);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    search+=QString().sprintf("(CART.GROUP_NAME=\"%s\")||",
			      (const char *)q->value(0).toString());
  }
  delete q;
  search=search.left(search.length()-2)+QString(")");
  search+=QString("&&")+RDBaseSearchText(filter,incl_cuts);

  if(!schedcode.isEmpty()) {
    QString code=schedcode+"          ";
    code=code.left(11);
    search+=QString().sprintf("&&(SCHED_CODES like \"%%%s%%\")",
			      (const char *)code);
  }

  return search;
}
