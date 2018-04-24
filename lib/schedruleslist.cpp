// schedruleslist.cpp
//
// A class for handling the scheduling rules for rdlogmanager/edit clocks
//
//   Stefan Gabriel <stg@st-gabriel.de>
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

#include <QMessageBox>
#include <QVariant>

#include <rddb.h>
#include <rdescape_string.h>
#include <schedruleslist.h>

SchedRulesList::SchedRulesList(QString clockname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  sql=QString("create table if not exists `")+clockname.replace(" ","_")+
    "_RULES` ("+
    "CODE varchar(10) not null primary key,"+
    "MAX_ROW int unsigned,"+
    "MIN_WAIT int unsigned,"+
    "NOT_AFTER varchar(10),"+
    "OR_AFTER varchar(10),"+
    "OR_AFTER_II varchar(10))";
  q=new RDSqlQuery(sql);
  delete q;

  sql=QString("select CODE,DESCRIPTION from SCHED_CODES order by CODE asc"); 

  q=new RDSqlQuery(sql);

  itemcounter=q->size();  
  sched_code = new QString[itemcounter];
  max_row = new int[itemcounter];
  min_wait = new int[itemcounter];
  not_after = new QString[itemcounter];
  or_after = new QString[itemcounter];
  or_after_II = new QString[itemcounter];
  description = new QString[itemcounter];
    
  for (int i=0; i<itemcounter; i++){
    q->next();
    sched_code[i] = q->value(0).toString();
    description[i] = q->value(1).toString();
    sql=QString("select ")+
      "MAX_ROW,"+
      "MIN_WAIT,"+
      "NOT_AFTER,"+
      "OR_AFTER,"+
      "OR_AFTER_II "+
      "from "+clockname.replace(" ","_")+"_RULES where "+
      "CODE=\""+sched_code[i]+"\"";
    q1=new RDSqlQuery(sql);
    if(q1->first())
      {
      max_row[i] = q1->value(0).toInt();
      min_wait[i] = q1->value(1).toInt();
      not_after[i] = q1->value(2).toString();
      or_after[i] = q1->value(3).toString();
      or_after_II[i] = q1->value(3).toString();
      }
    else
      {
      max_row[i] = 1;
      min_wait[i] = 0;
      not_after[i] = "";
      or_after[i] = "";
      or_after_II[i] = "";
      }
    delete q1;
    }
  delete q;
}

SchedRulesList::~SchedRulesList()
{
  delete []sched_code;
  delete []max_row;
  delete []min_wait;
  delete []not_after;
  delete []or_after;
  delete []or_after_II;
  delete []description;
}

void SchedRulesList::insertItem(int pos,int maxrow,int minwait,QString notafter,QString orafter,QString orafterii)
{
  max_row[pos] = maxrow;
  min_wait[pos] = minwait;
  not_after[pos] = notafter;
  or_after[pos] = orafter;
  or_after_II[pos] = orafterii;
}

QString SchedRulesList::getItemSchedCode(int pos)
{
  return sched_code[pos];
}

int SchedRulesList::getItemMaxRow(int pos)
{
  return max_row[pos];
}

int SchedRulesList::getItemMinWait(int pos)
{
  return min_wait[pos];
}

QString SchedRulesList::getItemNotAfter(int pos)
{
  return not_after[pos];
}

QString SchedRulesList::getItemOrAfter(int pos)
{
  return or_after[pos];
}

QString SchedRulesList::getItemOrAfterII(int pos)
{
  return or_after_II[pos];
}

QString SchedRulesList::getItemDescription(int pos)
{
  return description[pos];
}

int SchedRulesList::getNumberOfItems(void)
{
  return itemcounter;
}

void SchedRulesList::Save(QString clockname)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from `")+clockname.replace(" ","_")+"_RULES`";
  q=new RDSqlQuery(sql);
  delete q;
  for (int i=0;i<itemcounter;i++) 
   {
     sql=QString("insert into ")+clockname.replace(" ","_")+"_RULES set "+
       "CODE=\""+sched_code[i]+"\","+
       QString().sprintf("MAX_ROW=%d,",max_row[i])+
       QString().sprintf("MIN_WAIT=%d,",min_wait[i])+
       "NOT_AFTER=\""+RDEscapeString(not_after[i])+"\","+
       "OR_AFTER=\""+RDEscapeString(or_after[i])+"\","+
       "OR_AFTER_II=\""+RDEscapeString(or_after_II[i])+"\"";
   q=new RDSqlQuery(sql);
   delete q;
   }
}




