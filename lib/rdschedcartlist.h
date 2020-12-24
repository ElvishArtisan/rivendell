// rdschedcartlist.h
//
// A class for handling carts to be used in scheduler
//
//   Copyright (C) 2005 Stefan Gabriel <stg@st-gabriel.de>
//   Copyright (C) 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSCHEDCARTLIST_H
#define RDSCHEDCARTLIST_H

#include <qlist.h>
#include <qstringlist.h>

class RDSchedCartList
{
  public:
   RDSchedCartList();
   void insertItem(unsigned cartnumber,int cartlength,int stack_id,
		   const QString &stack_artist,
	           const QString &stack_title,
		   const QStringList &stack_schedcodes);
   void removeItem(int itemnumber);
   bool removeIfCode(int itemnumber,const QString &test_code);
   bool itemHasCode(int itemnumber,const QString &test_code);
   bool itemHasCodes(int itemnumber,const QStringList &test_codes);
   unsigned getItemCartNumber(int itemnumber);
   int getItemCartLength(int itemnumber);
   int getItemStackid(int itemnumber);
   QString getItemArtist(int itemnumber);
   QString getItemTitle(int itemnumber);
   QStringList getItemSchedCodes(int itemnumber);
   int getNumberOfItems(void);
   void save(void);
   void restore(void);
   
  private:
   QList<unsigned> list_cartnum;
   QList<unsigned> list_savecartnum;
   QList<int> list_cartlen;
   QList<int> list_savecartlen;
   QList<int> list_stackid;
   QList<int> list_savestackid;
   QStringList list_artist;
   QStringList list_title;
   QStringList list_saveartist;
   QStringList list_savetitle;
   QList<QStringList> list_schedcodes;
   QList<QStringList> list_saveschedcodes;
};


#endif  // RDSCHEDCARTLIST_H
