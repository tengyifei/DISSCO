/*
LASS (additive sound synthesis library)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//	Score.h	
//
//----------------------------------------------------------------------------//

#ifndef __SCORE_H
#define __SCORE_H

//----------------------------------------------------------------------------//
#include "XmlReader.h"
#include "Types.h"
#include "Collection.h"
#include "MultiTrack.h"
#include "Sound.h"
#include "Reverb.h"
#include "XmlReader.h"
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------//

// This struct passes data between the main thread and the worker threads
struct threadlist_entry
{
	// The sound object to render
	Sound *snd;
	// A pointer to the result of the rendering
	MultiTrack *resultTrack;
	// A flag to indicate we are finished
	int done;
	// A pointer to the worker thread
	pthread_t *thread;
	// A pointer to the mutex to use
	pthread_mutex_t *listMutex;
	// A pointer to the condition to signal when done
	pthread_cond_t *finishCondition;

	// Rendering parameters
	int numChannels;
	m_rate_type samplingRate;
};

//----------------------------------------------------------------------------//

/**
*	A Score simply consists of a collection of Sounds.
*	In addition to this, it provides functionality for 
*	managing clipping in a piece.
*	\author Braden Kowitz
**/
class Score : public Collection<Sound>
{
public:

    /**
    *	This is the default constructor.  It sets the ClippingManagementMode 
    *	to NONE.
    **/
    Score();

    /**
    *	\enum ClippingManagementMode
    *	This sets the clipping management mode for this score.
    *	This post-process is run after the score is rendered.
    **/

    /**
    *	\var ClippingManagementMode NONE
    *		- No clipping management is taken at all.
    *		- This lets the composer render once,
    *		  then try different post-processes, saving some time.
    **/

    /**	
    *	\var ClippingManagementMode CLIP
    *		- Any value over 1 or under -1 is clipped to these limits.
    **/

    /**
    *	\var ClippingManagementMode SCALE
    *		- The max amplitude value is found in the entire score
    *             (all tracks).
    *		- Each track is then scaled by 1/maxAmplitude.
    **/

    /**
    *	\var ClippingManagementMode CHANNEL_SCALE
    *		- For each track, a max amplitude value if found.
    *		- This track is then scaled by 1/maxAmplitude
    **/

    /**
    *	\var ClippingManagementMode ANTICLIP
    *		- If the total amplitude accross tracks at any given
    *		  time is greater than 1, then that sample is scaled
    *		  on all tracks by 1/totalAmplitude.
    **/

    /**
    *	\var ClippingManagementMode CHANNEL_ANTICLIP
    *		- For each channel, and each sample
    *		- if a sample has an amplitude greater than 1,
    *		  then that sample is scaled by 1/amplitude.
    **/	
    enum ClippingManagementMode
    {
        NONE,
        CLIP,
        SCALE,
        CHANNEL_SCALE,
        ANTICLIP,
        CHANNEL_ANTICLIP
    };
    
    /**
    *	This function: 
    *	- Renders each sound in this Score.
    *	- Composites the sounds into a MultiTrack object
    *	- Post-Processes the sound for clipping management
    *	
    *	\note The caller must delete this object.
    *	\param numChannels The number of channels to render
    *	\param samplingRate The sampling rate which defaults to 
    *		DEFAULT_SAMPLING_RATE
    *	\return A MultiTrack object
    **/
    MultiTrack* render(
        int numChannels,
        m_rate_type samplingRate = DEFAULT_SAMPLING_RATE);

    /**
    *	This function:
    *	- Renders each sound in this Score using a specified
    *		number of threads for parallel rendering
    *	- Composites the sounds into a MultiTrack object
    *	- Post-Processes the sound for clipping management
    *
    *	\note The caller must delete this object.  Also, the caller must
    *		specify a value for samplingRate (unlike single-threaded render)
    *	\param numChannels The number of channels to render
    *	\param samplingRate The sampling rate
    *	\param nThreads The number of threads to use when rendering
    *	\return a MultiTrack object
    **/
    MultiTrack* render(
        int numChannels,
        m_rate_type samplingRate,
	int nThreads);

    /**
    *	This function sets the ClippingManagementMode for this score.
    *	\param mode The ClippingManagementMode to set
    **/
    void setClippingManagementMode(ClippingManagementMode mode);

    /**
    *	This function gets the current ClippingManagementMode for this score.
    *	\return The ClippingManagementMode
    **/
    ClippingManagementMode getClippingManagementMode();

    /**
    *	This function manages clipping on a MultiTrack object with the 
    *	specified mode. This is performed automatically when a score is 
    *	rendered, but is available for the user to render once, then 
    *	post-process many times.
    *	\param mt The MultiTrack to clip
    *	\param mode The ClippingManagementMode
    **/
    static void manageClipping(MultiTrack* mt, ClippingManagementMode mode);

    /**
    *   This function performs reverb in the render() method.
    *	\param newReverbObj The Reverb object
    **/
	void use_reverb(Reverb *newReverbObj);

    /** 
    *	\deprecated
    *	This outputs an XML representation of the object to STDOUT
    *
    **/
    void xml_print( );
    /**
    *	\deprecated
    **/
    void xml_print( ofstream& xmlOutput );
    /**
    *	\deprecated
    **/
    void xml_print( const char * xmlOutputPath );
    /**
    *	\deprecated
    **/
    void xml_read( XmlReader::xmltag *scoretag);
    
    unordered_map<long, Reverb *>* reverbHash;
    unordered_map<long, DynamicVariable *>* dvHash;

private:
    ClippingManagementMode cmm_;
	
    Reverb *reverbObj;
   
    /**
    * This private function clips the MultiTrack.
    * \param mt The MultiTrack to clip
    **/
    static void clip(MultiTrack* mt);
   
    /**
    * This private function scales the MultiTrack.
    * \param mt The MultiTrack to scale
    **/
    static void scale(MultiTrack* mt);

    /**
    * This private function scales the channels in the MultiTrack.
    * \param mt The MultiTrack to scale
    **/
    static void channelScale(MultiTrack* mt);

    /**
    * This private function unclips the MultiTrack.
    * \param mt The MultiTrack to unclip
    **/
    static void anticlip(MultiTrack* mt);

    /**
    * This private function unclips the channels in a MultiTrack.
    * \param mt The MultiTrack to unclip
    **/
    static void channelAnticlip(MultiTrack* mt);
};


//----------------------------------------------------------------------------//
#endif //__SCORE_H


