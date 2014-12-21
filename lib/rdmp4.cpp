//   rdmp4.cpp
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

#ifdef HAVE_MP4_LIBS

#include <rdmp4.h>
#include <string.h>
#include <dlfcn.h>

MP4TrackId DLMP4::getMP4AACTrack(MP4FileHandle f)
{
  
  uint32_t nTracks = this->MP4GetNumberOfTracks(f);

  for(uint32_t trackIndex = 0; trackIndex < nTracks; ++trackIndex) {

    MP4TrackId thisTrack = this->MP4FindTrackId(f, trackIndex);
    const char* trackType = this->MP4GetTrackType(f, thisTrack);
    if(trackType && !strcmp(trackType, MP4_AUDIO_TRACK_TYPE)) {
   
      const char* dataName = this->MP4GetTrackMediaDataName(f, thisTrack);
      // The M4A format is only currently useful for AAC in an M4A container:
      if(dataName && 
	 (!strcasecmp(dataName, "mp4a")) && 
	 this->MP4GetTrackEsdsObjectTypeId(f, thisTrack) == MP4_MPEG4_AUDIO_TYPE) {

	return thisTrack;

      }

    }

  }

  return MP4_INVALID_TRACK_ID;

}

#define check_dlsym(lval, libhandle, symname) \
  *(void**)(&lval) = dlsym(libhandle, symname); \
  if(!lval) return false;

bool DLMP4::load()
{

  if(loadSuccess)
    return true;

  neaac_handle = dlopen("libfaad.so",RTLD_LAZY);
  mp4_handle = dlopen("libmp4v2.so",RTLD_LAZY);

  if(!neaac_handle || !mp4_handle)
    return false;

  check_dlsym(this->MP4Read, mp4_handle, "MP4Read");
  check_dlsym(this->MP4GetTrackNumberOfSamples, mp4_handle, "MP4GetTrackNumberOfSamples");
  check_dlsym(this->MP4GetSampleIdFromTime, mp4_handle, "MP4GetSampleIdFromTime");
  check_dlsym(this->MP4GetTrackMaxSampleSize, mp4_handle, "MP4GetTrackMaxSampleSize");
  check_dlsym(this->MP4GetTrackESConfiguration, mp4_handle, "MP4GetTrackESConfiguration");
  check_dlsym(this->MP4ReadSample, mp4_handle, "MP4ReadSample");
  check_dlsym(this->MP4GetTrackBitRate, mp4_handle, "MP4GetTrackBitRate");
  check_dlsym(this->MP4GetTrackAudioChannels, mp4_handle, "MP4GetTrackAudioChannels");
  check_dlsym(this->MP4GetTrackDuration, mp4_handle, "MP4GetTrackDuration");
  check_dlsym(this->MP4ConvertFromTrackDuration, mp4_handle, "MP4ConvertFromTrackDuration");
  check_dlsym(this->MP4GetTrackTimeScale, mp4_handle, "MP4GetTrackTimeScale");
  check_dlsym(this->MP4GetNumberOfTracks, mp4_handle, "MP4GetNumberOfTracks");
  check_dlsym(this->MP4FindTrackId, mp4_handle, "MP4FindTrackId");
  check_dlsym(this->MP4GetTrackType, mp4_handle, "MP4GetTrackType");
  check_dlsym(this->MP4GetTrackMediaDataName, mp4_handle, "MP4GetTrackMediaDataName");
  check_dlsym(this->MP4GetTrackEsdsObjectTypeId, mp4_handle, "MP4GetTrackEsdsObjectTypeId");
  check_dlsym(this->MP4TagsAlloc, mp4_handle, "MP4TagsAlloc");
  check_dlsym(this->MP4TagsFetch, mp4_handle, "MP4TagsFetch");
  check_dlsym(this->MP4TagsFree, mp4_handle, "MP4TagsFree");
  check_dlsym(this->MP4Close, mp4_handle, "MP4Close");

  check_dlsym(this->NeAACDecOpen, neaac_handle, "NeAACDecOpen");
  check_dlsym(this->NeAACDecGetCurrentConfiguration, neaac_handle, "NeAACDecGetCurrentConfiguration");
  check_dlsym(this->NeAACDecSetConfiguration, neaac_handle, "NeAACDecSetConfiguration");
  check_dlsym(this->NeAACDecInit2, neaac_handle, "NeAACDecInit2");
  check_dlsym(this->NeAACDecDecode, neaac_handle, "NeAACDecDecode");
  check_dlsym(this->NeAACDecClose, neaac_handle, "NeAACDecClose");

  loadSuccess = true;
  return true;

}

#undef check_dlsym

#endif // HAVE_MP4_LIBS
