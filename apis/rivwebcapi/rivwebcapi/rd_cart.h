/* rd_cart.h
 *
 * Header for the RDCART Structure Rivendell Access Library
 *
 * (C) Copyright 2015 Todd Baker  <bakert@rfa.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
                                                                    */
#ifndef RD_CART_H
#define RD_CART_H

#include <time.h>

enum CART_TYPE {TYPE_ALL,TYPE_AUDIO,TYPE_MACRO};

struct rd_cart {
  unsigned cart_number;
  unsigned cart_type;
  char cart_grp_name[41];
  char cart_title[1021];
  char cart_artist[1021];
  char cart_album[1021];
  int cart_year;
  char cart_label[257];
  char cart_client[257];
  char cart_agency[257];
  char cart_publisher[257];
  char cart_composer[257];
  char cart_conductor[257];
  char cart_user_defined[1021];
  int  cart_usage_code;
  int  cart_forced_length;
  int  cart_average_length;
  int  cart_length_deviation;
  int  cart_average_segue_length;
  int  cart_average_hook_length;
  unsigned  cart_cut_quantity;
  unsigned  cart_last_cut_played;
  unsigned  cart_validity;
  int cart_enforce_length;
  int cart_asyncronous;
  char cart_owner[257];  
  char cart_notes[4096];
  struct tm cart_metadata_datetime;
};                      


#endif   //RD_CART_H
