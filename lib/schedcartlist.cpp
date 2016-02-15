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

#include <stdlib.h>
 
SchedCartList::SchedCartList()
{
  first_sched_cart = NULL;
}

SchedCartList::~SchedCartList()
{
  if (first_sched_cart == NULL) {
    return;
  }
  SchedCart *current = first_sched_cart;
  SchedCart *next = NULL;
  
  while (current != NULL) {
    next = current->next();
    delete current;
    current = next;
  }
}

SchedCart* SchedCartList::first()
{
  return first_sched_cart;
}


void SchedCartList::insert(unsigned cartnumber,QString stack_artist,QString stack_schedcodes)
{
  SchedCart *created = new SchedCart(cartnumber, stack_artist, stack_schedcodes);

  if (first_sched_cart != NULL) {
    first_sched_cart->setPrevious(created);
    created->setNext(first_sched_cart);
  } 
  first_sched_cart = created;
}


void SchedCartList::excludeIfCode(QString code) {
  SchedCart *current = NULL;
  for (current = first(); current != NULL; current = current->next()) {
    if (current->hasSchedulerCode(code)) {
      current->exclude();
    }
  }
}


bool SchedCartList::allExcluded() {
  SchedCart *current = NULL;
  for (current = first_sched_cart; current != NULL; current = current->next()) {
    if (!current->hasBeenExcluded()) {
      return false;
    }
  }
  return true;
}


void SchedCartList::clearExclusions() {
  SchedCart *current = NULL;
  for (current = first_sched_cart; current != NULL; current = current->next()) {
    current->clearExclusion();
  }
}

void SchedCartList::save() {
  SchedCart *current = first_sched_cart;
  SchedCart *excluded = NULL;
 
  while (current != NULL) {
    if (current->hasBeenExcluded()) {
      excluded = current;
      current = excluded->next();

      // Remove excluded from the list
      if (excluded->previous() != NULL) {
        excluded->previous()->setNext(excluded->next());
      } else {
        first_sched_cart = excluded->next();
      }

      if (excluded->next() != NULL) {
        excluded->next()->setPrevious(excluded->previous());
      }

      // Delete excluded
      delete excluded;
    } else {
      current = current->next();
    }
  }
}


void SchedCartList::saveOrBreakRule(QString ruleName, QTime time, QString *errors) {
  if (!allExcluded()) {
    save();
  } else {
    *errors += time.toString("hh:mm:ss") + QString(" Rule broken: ") + ruleName + QString("\n");
    clearExclusions();
  }
}

int SchedCartList::size() {
  int size = 0;
  SchedCart *current = NULL;
  for (current = first_sched_cart; current != NULL; current = current->next()) {
    size++;
  }
  return size;
}


SchedCart* SchedCartList::sample() {
  int randomPosition = rand() % size();
  int position = 0;
  SchedCart *current = NULL;
  for (current = first_sched_cart; current != NULL; current = current->next()) {
    if (position == randomPosition) {
      return current;
    }
    position ++;
  }
  return NULL;
}
 


