// schedcart.h
//
// A class for handling cart to be used in scheduler
//
//   Alban Peignier <alban@tryphon.eu>
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

#include <qstring.h>

#ifndef SCHEDCART_H
#define SCHEDCART_H

class SchedCart
{
  public:
   SchedCart(unsigned cartnumber, QString artist,QString schedcodes);
   ~SchedCart();

   void exclude();
   bool hasBeenExcluded();
   void clearExclusion();

   unsigned getCartNumber();
   QString getArtist();
   QString getSchedCodes();

   bool hasSchedulerCode(QString code);

   SchedCart *previous();
   void setPrevious(SchedCart *previous);

   SchedCart *next();
   void setNext(SchedCart *next);
   
  private:
   unsigned cart_number;
   QString artist;
   QString sched_codes;
   bool excluded;

   SchedCart *next_sched_cart;
   SchedCart *previous_sched_cart;
};

#endif 
