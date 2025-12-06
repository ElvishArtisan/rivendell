// rdcut_cache.h
//
// Cut data cache for batch loading 
//
// Eliminates ~800 database queries per log load by:
// 1. Batch-loading all cuts for all carts with single query
// 2. Implementing cut rotation logic (Sequential/Random/Weighted) using cached data
// 3. Providing fast O(1) lookup by cut name and cart number
//
// Cache expires after configurable timeout (default 5 min) to ensure
// fresh data when carts are added/modified during playback.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCUT_CACHE_H
#define RDCUT_CACHE_H

#include <QDateTime>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QVector>

#include <rdcart.h>

// Container for cut metadata loaded from CUTS table
// Includes all fields needed for playback and rotation logic
class RDCutData
{
 public:
  RDCutData();
  QString cut_name;
  int cut_number;
  unsigned length;
  int start_point;
  int end_point;
  int segue_start_point;
  int segue_end_point;
  int segue_gain;
  int talk_start_point;
  int talk_end_point;
  int hook_start_point;
  int hook_end_point;
  QString outcue;
  QString isrc;
  QString isci;
  QString description;
  QString recording_mbid;
  QString release_mbid;
  QDateTime start_datetime;
  QDateTime end_datetime;
  bool mon;
  bool tue;
  bool wed;
  bool thu;
  bool fri;
  bool sat;
  bool sun;
  int weight;
  int local_counter;
  QDateTime last_play_datetime;
  int play_order;
  bool evergreen;
  
  void clear();
};


class RDCutCache
{
 public:
  RDCutCache();
  ~RDCutCache();
  
  void clear();
  int batchLoadCuts(const QVector<unsigned> &cart_numbers);
  QVector<RDCutData> getCutsForCart(unsigned cart_number) const;
  RDCutData getCut(const QString &cut_name) const;
  bool getCutByName(const QString &cut_name, RDCutData *cut_data) const;
  bool hasCut(const QString &cut_name) const;
  int cutCount() const;
  
  // Cut selection/rotation using cached data
  QString selectCut(unsigned cart_number, RDCart::PlayOrder play_order,
                    bool use_weighting, const QTime &time=QTime::currentTime()) const;
  
  // Cache lifecycle management
  void setCacheTimeout(int msecs);  // Set cache validity period (0=no timeout)
  bool isValid() const;              // Check if cache is still valid
  void touch();                      // Update last access time
  QDateTime loadTime() const;        // When cache was loaded
  
 private:
  // Dual indexing for fast lookups:
  // - by cart number: for rotation (get all cuts for a cart)
  // - by cut name: for direct retrieval (get specific cut data)
  QMap<unsigned, QVector<RDCutData> > cuts_by_cart;
  QMap<QString, RDCutData> cuts_by_name;
  QDateTime cache_load_time;
  QDateTime cache_last_access;
  int cache_timeout_msecs;  // Cache validity period in milliseconds (0=no timeout)
};

#endif  // RDCUT_CACHE_H
