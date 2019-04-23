// rdschedruleslist.h
//
// A class for handling the scheduling rules for rdlogmanager/edit clocks
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSCHEDRULESLIST_H
#define RDSCHEDRULESLIST_H

#include <rdconfig.h>

class RDSchedRulesList
{
 public:
  RDSchedRulesList(QString clockname,RDConfig *config);
  ~RDSchedRulesList();
  void insertItem(int pos,int maxrow,int minwait,QString notafter,
		  QString orafter,QString orafterii);
  QString getItemSchedCode(int pos);
  int getItemMaxRow(int pos);
  int getItemMinWait(int pos);
  int getNumberOfItems(void);
  QString getItemNotAfter(int pos);
  QString getItemOrAfter(int pos);
  QString getItemOrAfterII(int pos);
  QString getItemDescription(int pos);
  void Save(QString clockname);

 private:
  int itemcounter;
  QString* sched_code;
  int* max_row;
  int* min_wait;
  QString* not_after;
  QString* or_after;
  QString* or_after_II;
  QString* description;
};


#endif  // RDSCHEDRULESLIST_H
