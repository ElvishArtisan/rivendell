//   rdmp4.h
//
//   Helpers for dealing with MP4 files.
//
//   (C) Copyright 2014 Christopher Smowton <chris@smowton.net>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDMP4_H
#define RDMP4_H

#ifdef HAVE_MP4_LIBS

#include <mp4v2/mp4v2.h>
#include <neaacdec.h>
// neaacdec.h defines "LC", as in "low-complexity AAC", which clashes with a Rivendell Command.
#undef LC

struct DLMP4 {

DLMP4() : loadSuccess(false) {}

  void *neaac_handle;
  void *mp4_handle;
  bool loadSuccess;

  // MP4v2 Functions
  MP4FileHandle (*MP4Read) (const char* fileName);
  MP4SampleId (*MP4GetTrackNumberOfSamples) (MP4FileHandle hFile, MP4TrackId trackId);
  MP4SampleId (*MP4GetSampleIdFromTime) (MP4FileHandle hFile, MP4TrackId trackId, MP4Timestamp when, bool wantSyncSample);
  uint32_t (*MP4GetTrackMaxSampleSize) (MP4FileHandle hFile, MP4TrackId trackId);
  bool (*MP4GetTrackESConfiguration) (MP4FileHandle hFile, MP4TrackId trackId, uint8_t** ppConfig, uint32_t* pConfigSize);
  bool (*MP4ReadSample) (
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    MP4SampleId   sampleId,
    uint8_t** ppBytes,
    uint32_t* pNumBytes,
    MP4Timestamp* pStartTime,
    MP4Duration*  pDuration,
    MP4Duration*  pRenderingOffset,
    bool*         pIsSyncSample);
  uint32_t (*MP4GetTrackBitRate) (MP4FileHandle hFile, MP4TrackId trackId);
  int (*MP4GetTrackAudioChannels) (MP4FileHandle hFile, MP4TrackId trackId);
  MP4Duration (*MP4GetTrackDuration) (MP4FileHandle hFile, MP4TrackId trackId);
  uint64_t (*MP4ConvertFromTrackDuration) (
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    MP4Duration   duration,
    uint32_t      timeScale);
  uint32_t (*MP4GetTrackTimeScale) (MP4FileHandle hFile, MP4TrackId trackId);
  uint32_t (*MP4GetNumberOfTracks) (MP4FileHandle hFile, const char* type, uint8_t subType);
  MP4TrackId (*MP4FindTrackId) (MP4FileHandle hFile, uint32_t trackIdx);
  const char* (*MP4GetTrackType) (MP4FileHandle hFile, MP4TrackId);
  const char* (*MP4GetTrackMediaDataName) (MP4FileHandle hFile, MP4TrackId);
  uint8_t (*MP4GetTrackEsdsObjectTypeId) (MP4FileHandle hFile, MP4TrackId);
  const MP4Tags* (*MP4TagsAlloc) (void);
  bool (*MP4TagsFetch) (const MP4Tags* tags, MP4FileHandle hFile);
  void (*MP4TagsFree) (const MP4Tags* tags);
  void (*MP4Close) (MP4FileHandle hFile, uint32_t flags);

  // libfaad / NeAACDec functions
  NeAACDecHandle (*NeAACDecOpen) (void);
  NeAACDecConfigurationPtr (*NeAACDecGetCurrentConfiguration) (NeAACDecHandle hDecoder);
  unsigned char (*NeAACDecSetConfiguration) (NeAACDecHandle hDecoder, NeAACDecConfigurationPtr config);
  char (*NeAACDecInit2)(NeAACDecHandle hDecoder,
			unsigned char *pBuffer,
			unsigned long SizeOfDecoderSpecificInfo,
			unsigned long *samplerate,
			unsigned char *channels);
  void* (*NeAACDecDecode) (NeAACDecHandle hDecoder,
			   NeAACDecFrameInfo *hInfo,
			   unsigned char *buffer,
			   unsigned long buffer_size);
  void (*NeAACDecClose) (NeAACDecHandle hDecoder);

  // Helper functions:
  MP4TrackId getMP4AACTrack(MP4FileHandle f);

  bool load();

};

#endif // HAVE_MP4_LIBS
#endif // RDMP4_H
