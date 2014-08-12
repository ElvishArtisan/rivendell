// schedcartlist.cpp
//
// A class for handling carts to be used in scheduler
//
//   Stefan Gabriel <stg@st-gabriel.de>
//
//    $Id: schedcartlist.cpp,v 1.4.8.1 2012/12/13 22:33:45 cvs Exp $   
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

#include <schedcartlist.h>


SchedCartList::SchedCartList(int listsize)
{
	cartnum=new unsigned[listsize];
	cartlen=new int[listsize];
	stackid=new int[listsize];
	artist=new QString[listsize];
	sched_codes=new QString[listsize];
	itemcounter=0;
}

SchedCartList::~SchedCartList()
{
	delete []cartnum;
	delete []cartlen;
	delete []stackid;
	delete []artist;
	delete []sched_codes;
}

void SchedCartList::insertItem(unsigned cartnumber,int cartlength,int stack_id,QString stack_artist,QString stack_schedcodes)
{
	cartnum[itemcounter]=cartnumber;
	cartlen[itemcounter]=cartlength;
	stackid[itemcounter]=stack_id;
	artist[itemcounter]=stack_artist.lower().replace(" ","");
        sched_codes[itemcounter]=stack_schedcodes;	
	itemcounter++;
}


void SchedCartList::removeItem(int itemnumber)
{
	for(int i=itemnumber;i<(itemcounter-1);i++)
	{
		cartnum[i]=cartnum[i+1];
		cartlen[i]=cartlen[i+1];
		stackid[i]=stackid[i+1];
		artist[i]=artist[i+1];
		sched_codes[i]=sched_codes[i+1];
	}
	itemcounter--;
}

bool SchedCartList::removeIfCode(int itemnumber,QString test_code)
{
    QString test = test_code;
    test+="          ";
    test=test.left(11);

    if (sched_codes[itemnumber].find(test)!=-1)
      {
	for(int i=itemnumber;i<(itemcounter-1);i++)
	{
		cartnum[i]=cartnum[i+1];
		cartlen[i]=cartlen[i+1];
		stackid[i]=stackid[i+1];
		artist[i]=artist[i+1];
		sched_codes[i]=sched_codes[i+1];
	}
	itemcounter--;
        return true;
      }
    return false; 
}

bool SchedCartList::itemHasCode(int itemnumber,QString test_code)
{
    QString test=test_code;
    test+="          ";
    test=test.left(11);

    if (sched_codes[itemnumber].find(test)!=-1)
      return true;
    else
      return false;
}


void SchedCartList::save(void)
{
	savecartnum=new unsigned[itemcounter];
	savecartlen=new int[itemcounter];
	savestackid=new int[itemcounter];
	saveartist=new QString[itemcounter];
	save_sched_codes=new QString[itemcounter];

	saveitemcounter=itemcounter;	
	for(int i=0;i<saveitemcounter;i++)
	{
		savecartnum[i]=cartnum[i];
		savecartlen[i]=cartlen[i];
		savestackid[i]=stackid[i];
		saveartist[i]=artist[i];
		save_sched_codes[i]=sched_codes[i];
	}
}


void SchedCartList::restore(void)
{
	if(itemcounter==0)
	{
		for(int i=0;i<saveitemcounter;i++)
		{
			cartnum[i]=savecartnum[i];
			cartlen[i]=savecartlen[i];
			stackid[i]=savestackid[i];
			artist[i]=saveartist[i];
			sched_codes[i]=save_sched_codes[i];
		}
		itemcounter=saveitemcounter;	
	}
	delete []savecartnum;
	delete []savecartlen;
	delete []savestackid;
	delete []saveartist;
	delete []save_sched_codes;
}



unsigned SchedCartList::getItemCartnumber(int itemnumber)
{
	return cartnum[itemnumber];
}

int SchedCartList::getItemStackid(int itemnumber)
{
	return stackid[itemnumber];
}

QString SchedCartList::getItemArtist(int itemnumber)
{
	return artist[itemnumber];
}

QString SchedCartList::getItemSchedCodes(int itemnumber)
{
	return sched_codes[itemnumber];
}

int SchedCartList::getItemCartlength(int itemnumber)
{
	return cartlen[itemnumber];
}


int SchedCartList::getNumberOfItems(void)
{
	return itemcounter;
}
 


