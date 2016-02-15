// schedcartlist.cpp
//
// A class for handling cart to be used in scheduler
//
//   Alban Peignier <alban@tryphon.eu>
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

#include <schedcart.h>

SchedCart::SchedCart(unsigned cartNumber, QString cartArtist,QString cartSchedCodes)
{
       cart_number=cartNumber;
       artist=cartArtist;
       sched_codes=cartSchedCodes;
  
  excluded=false;
  next_sched_cart=NULL;
  previous_sched_cart=NULL;
}

SchedCart::~SchedCart()
{
}

unsigned SchedCart::getCartNumber()
{
       return cart_number;
}

QString SchedCart::getArtist()
{
       return artist;
}

QString SchedCart::getSchedCodes()
{
       return sched_codes;
}

bool SchedCart::hasSchedulerCode(QString code) {
  QString test = (code + "          ").left(11);
  return sched_codes.find(test) >= 0;
}

SchedCart* SchedCart::previous() {
  return previous_sched_cart;
}

void SchedCart::setPrevious(SchedCart *schedCart) {
  previous_sched_cart = schedCart;
}

SchedCart* SchedCart::next() {
  return next_sched_cart;
}

void SchedCart::setNext(SchedCart *schedCart) {
  next_sched_cart = schedCart;
}

void SchedCart::exclude() {
  excluded = true;
}

bool SchedCart::hasBeenExcluded() {
  return excluded;
}

void SchedCart::clearExclusion() {
  excluded = false;
}
