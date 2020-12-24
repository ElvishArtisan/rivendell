// rdschedcartlist.cpp
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

#include <stdio.h>
#include "rdschedcartlist.h"

RDSchedCartList::RDSchedCartList()
{
}


void RDSchedCartList::insertItem(unsigned cartnumber,int cartlength,int stack_id,
			       const QString &stack_artist,
			       const QString &stack_title,
			       const QStringList &stack_schedcodes)
{
  list_cartnum.push_back(cartnumber);
  list_cartlen.push_back(cartlength);
  list_stackid.push_back(stack_id);
  list_artist.push_back(stack_artist.lower().replace(" ",""));
  list_title.push_back(stack_title.lower().replace(" ",""));
  list_schedcodes.push_back(stack_schedcodes);  
}


void RDSchedCartList::removeItem(int itemnumber)
{
  list_cartnum.removeAt(itemnumber);
  list_cartlen.removeAt(itemnumber);
  list_stackid.removeAt(itemnumber);
  list_artist.removeAt(itemnumber);
  list_title.removeAt(itemnumber);
  list_schedcodes.removeAt(itemnumber);
}

bool RDSchedCartList::removeIfCode(int itemnumber,const QString &test_code)
{
  bool matched=false;

  for(int i=list_schedcodes.size()-1;i>=0;i--) {
    if(list_schedcodes.at(i).contains(test_code)) {
      list_cartnum.removeAt(i);
      list_cartlen.removeAt(i);
      list_stackid.removeAt(i);
      list_artist.removeAt(i);
      list_title.removeAt(i);
      list_schedcodes.removeAt(i);
      matched=true;
    }
  }

  return matched;
}

bool RDSchedCartList::itemHasCode(int itemnumber,const QString &test_code)
{
  return list_schedcodes.at(itemnumber).contains(test_code);
}


bool RDSchedCartList::itemHasCodes(int itemnumber,const QStringList &test_codes)
{
  int matches=0;

  for(int i=0;i<test_codes.size();i++) {
    if(itemHasCode(itemnumber,test_codes.at(i))) {
      matches++;
    }
  }
  return(matches==test_codes.size());
}


void RDSchedCartList::save(void)
{
  list_savecartnum=list_cartnum;
  list_savecartlen=list_cartlen;
  list_savestackid=list_stackid;
  list_saveartist=list_artist;
  list_savetitle=list_title;
  list_saveschedcodes=list_schedcodes;
}


void RDSchedCartList::restore(void)
{
  list_cartnum=list_savecartnum;
  list_cartlen=list_savecartlen;
  list_stackid=list_savestackid;
  list_artist=list_saveartist;
  list_title=list_savetitle;
  list_schedcodes=list_saveschedcodes;
}


unsigned RDSchedCartList::getItemCartNumber(int itemnumber)
{
  return list_cartnum.at(itemnumber);
}


int RDSchedCartList::getItemStackid(int itemnumber)
{
  return list_stackid.at(itemnumber);
}


QString RDSchedCartList::getItemArtist(int itemnumber)
{
  return list_artist.at(itemnumber);
}


QString RDSchedCartList::getItemTitle(int itemnumber)
{
  return list_title.at(itemnumber);
}


QStringList RDSchedCartList::getItemSchedCodes(int itemnumber)
{
  return list_schedcodes.at(itemnumber);
}


int RDSchedCartList::getItemCartLength(int itemnumber)
{
  return list_cartlen.at(itemnumber);
}


int RDSchedCartList::getNumberOfItems(void)
{
  return list_cartnum.size();
}
