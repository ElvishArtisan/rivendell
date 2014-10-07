// schedcartlist.h
//
// A class for handling carts to be used in scheduler
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//   
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

#include <qsqldatabase.h>
#include <qdatetime.h>
#include <schedcart.h>

#ifndef SCHEDCARTLIST_H
#define SCHEDCARTLIST_H

class SchedCartList
{
  public:
   SchedCartList();
   ~SchedCartList();

   void insert(unsigned cartnumber,QString stack_artist,QString stack_schedcodes);
   SchedCart *first();
   void excludeIfCode(QString code);
   void saveOrBreakRule(QString ruleName, QTime time, QString *errors);
   SchedCart *sample();
   
 private:
   SchedCart *first_sched_cart;

   bool allExcluded();
   void clearExclusions();
   void save();
   
   int size();
};


#endif 
