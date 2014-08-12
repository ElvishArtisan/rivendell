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


#ifndef SCHEDCARTLIST_H
#define SCHEDCARTLIST_H


class SchedCartList
{
  public:
   SchedCartList(int listsize);
   ~SchedCartList();
   void insertItem(unsigned cartnumber,int cartlength,int stack_id,QString stack_artist,QString stack_schedcodes);
   void removeItem(int itemnumber);
   bool removeIfCode(int itemnumber,QString test_code);
   bool itemHasCode(int itemnumber,QString test_code);
   unsigned getItemCartnumber(int itemnumber);
   int getItemCartlength(int itemnumber);
   int getItemStackid(int itemnumber);
   QString getItemArtist(int itemnumber);
   QString getItemSchedCodes(int itemnumber);
   int getNumberOfItems(void);
   void save(void);
   void restore(void);
   
  private:
   int itemcounter;
   int saveitemcounter;
   unsigned* cartnum;
   unsigned* savecartnum;
   int* cartlen;
   int* savecartlen;
   int* stackid;
   int* savestackid;
   QString* saveartist;
   QString* artist;
   QString* sched_codes;
   QString* save_sched_codes;
};


#endif 
