// rdlog_loader.h
//
// Log Loading Logic for RDLogPlay
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

#ifndef RDLOG_LOADER_H
#define RDLOG_LOADER_H

#include <QString>
#include <QVector>

#include <rdlogmodel.h>
#include <rdcut_cache.h>

//
// Encapsulates the log loading process:
// 1. Load log lines from database
// 2. Batch-load cuts (Tier 2 optimization)
// 3. Distribute cut cache to log lines
//
// This allows the same logic to be used in both synchronous (main thread)
// and asynchronous (pre-fetch thread) contexts.
//
class RDLogLoader
{
 public:
  RDLogLoader();
  ~RDLogLoader();
  
  //
  // Load a log into the provided RDLogModel
  // Returns: number of lines loaded, or -1 on error
  //
  // Parameters:
  //   log_model - The model to load into (must already have log name set)
  //   enable_timescaling - Whether to enable timescaling on loaded lines
  //   cut_cache_timeout_sec - Cache timeout in seconds (0 = no cache)
  //
  int loadLog(RDLogModel *log_model, 
              bool enable_timescaling = false,
              int cut_cache_timeout_sec = 300);
  
  //
  // Get the cut cache from the last successful load
  // Returns: pointer to cache, or NULL if no cache created
  // Note: Ownership remains with RDLogLoader - do not delete
  //
  RDCutCache *cutCache() const;
  
  //
  // Statistics from last load
  //
  int lastLineCount() const { return last_line_count; }
  int lastCartCount() const { return last_cart_count; }
  QString lastError() const { return last_error; }

 private:
  RDCutCache *loader_cut_cache;
  int last_line_count;
  int last_cart_count;
  QString last_error;
};

#endif  // RDLOG_LOADER_H
