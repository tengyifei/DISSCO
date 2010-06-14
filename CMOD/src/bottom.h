/*
CMOD (composition module)
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
//   Bottom.h  This class is used to store and manage the details of the
//   bottom level objects.  Unlike the other events, bottom does not create 
//   subevents.  In stead it creates sounds for synthesis and notes for 
//   traditional notated scores are created.  Visual events might be added
//   later.
//
//----------------------------------------------------------------------------//

#ifndef BOTTOM_H
#define BOTTOM_H

// CMOD includes
#include "libraries.h"

#include "define.h"
#include "filevalue.h"
#include "modifier.h"
#include "event.h"
#include "note.h"

//----------------------------------------------------------------------------//
class Bottom : public Event {

  private:

    /* We have to hold onto FileValues here, because these vars need to be 
       recomputed for EVERY sound/note/visual, since there could be some 
       randomness built in. */
    FileValue* frequencyFV;
    FileValue* loudnessFV;
    FileValue* spatializationFV;
    FileValue* reverberationFV;
    FileValue* modifiersFV;

    /* SOUND EVENT */
    FileValue* numPartialsFV;
    FileValue* deviationFV;
    FileValue* spectrumFV;

    /* NOTE EVENT */
    FileValue* notePitchClassFV;
    FileValue* noteDynamicMarkFV;
    FileValue* noteModifiersFV;

    int currPartialNum; // current partial as we're processing the event
    map<string, double> mod_used; // maps each mod name to an indicator if it's being used - -1=initVal, 0=notUsed, 1=used
    int wellTempPitch;  // pitch number for a well-tempered frequency (used to create notes)

    vector<Sound*> childSounds;
    vector<Note*> childNotes;

    static int sndcount; // used to count the total number of sounds produced for a piece

//----------------------------------------------------------------------------//

  public:

    /**
    * Default constructor for a bottom event
    **/
    Bottom(){}

    /**
    *   Normal Bottom constructor
    *   \param astartTime The start time of the event
    *   \param aDuration The duration of the event
    *   \param aType The type of the event
    *   \param aName A name for the event
    *   \param level The number of parents to this event
    **/ 
    Bottom(float aStrtTime, float aDuration, int aType, string aName, int level);

    /**
    *   Bottom copy constructor
    *   \param origBottom The Bottom to copy
    *   \return A copy of origBottom
    **/
    Bottom(const Bottom &origBottom);

    /**
     *   Bottom assignment operator
     *   \param origBottom The Bottom to copy
     **/
    Bottom& operator=(const Bottom& origBottom);

    /**
    *   Destructor.
    **/
    ~Bottom();

    //--------------------- Initialization functions  -----------------------//
    /**
     *  Initialize the Bottom event variables
     **/
    void initBottomVars( FileValue* frequency, FileValue* loudness,
                         FileValue* spatialization, FileValue* reverberation,
                         FileValue* modifiers );

    /**
     *  Initialize variables for a sound
     **/
    void initSoundVars( FileValue* numPartials, FileValue* deviation,
                        FileValue* spectrum );

    /**
     *  Initialize variables for a note
     **/
    void initNoteVars( FileValue* note_pitchClass, FileValue* note_dynamicMark,
                        FileValue* note_modifiers );

    //--------------------- Build Methods  -----------------------//
    /**
     *  Starting point for building children. Overrides the usual
     *  method in Event, allowing the creation of Sounds, Notes,
     *  and Visuals instead of child Events.
     **/
    void constructChild(float stime, float dur, int type, string name, int level);

    /**
     *  Returns the number of current partial -- overrides Event
     **/
    int getCurrPartialNum() { return currPartialNum; };


    /**
     * Creates a sound and adds the sound to the Score.
     **/
    void buildSound(float stime, float dur, int type, string name);

    /**
     *  Creates a note (traditional notation) with all its attributes.
     **/
    void buildNote(float stime, float dur, int type, string name);

    /**
     *  Creates a visual.
     **/
    void buildVisual();

    /**
     *  Prints Bottom ID, name, start time, duration, and other parameters 
     *  to the outputFile
     **/
    void print();

    /**
     * Prints the sound ID, start time, duration, type, frequency, 
     * and sones of a sound.
     * \param stime start time for the sound
     * \param dur duration of the sound
     * \param type type of the sound
     * \param name filename of the sound
     * \param numPartials integer containing the number of partials
     * \param freq float containing the frequency value
     * \param sones float containing the sones value
     **/
    void printSound( float stime, float dur, int type, string name,
                     int numPartials, float freq, float sones);

    /**
     * Prints a note
     * \param n the note to print
     * \param type type of the note
     * \param name filename of the note
     **/
    void printNote(Note& n, int type, string name);

    /**
     * Adds pointers to any notes in this Bottom event to a vector
     * \param noteVect a reference to a vector of notes
    **/
    list<Note> getNotes();

    //--------------------- Private helper methods  -----------------------//
  private:
    /**
     *  Computes a base frequency for the bottom event
     **/
    float computeBaseFreq();

    /**
     *  Computes a loudness value for the bottom event
     **/
    float computeLoudness();

    /**
     *  Computes the number of partials to create
     **/
    int computeNumPartials(float baseFreq);

    /**
     *  Computes a deviation value for the bottom event
     **/
    float computeDeviation();

    /**
     *   Assigns a frequency to a partial according to baseFrequency and the
     *   deviation (which is randomly selected as positive or negative).
     *   Frequencies are checked against the MINFREQ and CEILING 
     *   (see define.h) and set (applied to the sound object) with setParam
     *   \param part reference to a partial
     *   \param deviation the deviation
     *   \param baseFrequency the fundamental frequency
     *   \param partNum the number of the partial
     **/
    void setPartialFreq(Partial& part, float deviation, float baseFrequency, int partNum);

    /**
     *  Chooses an envelope number for each partial then a scaling factor and 
     *  then assigns the scaled envelope.  There are different methods for
     *  choosing the envelope and its scaling factor.
     * \param part reference to a partial
     * \param partNum the number of the partial
     **/
    void setPartialSpectrum(Partial& part, int partNum);

    /**
     *  Assigns values to the array of ampscales based on a randomly selected rule
     *  ==== not yet implemented ====
     * \param numPartials The number of partials
     * \param ampScale An array of floats for determining the amplitude scale
     **/
    void rules(int numPartials, float ampScale[]);

//----------------------------------------------------------------------------//
    /**
     *  Applies spatialization to a sound
     *  \param s a pointer to the sound being created
     *  \param numPartials the number of partials in the sound
     **/

    void applySpatialization(Sound* s, int numPartials);
    /**
     *  Sets the spatialization of a sound according to a stereo environment
     *  \param s pointer to a sound
     *  \param envList List of FileValues (envelopes)
     *  \param applyHow string containing info if it applies to sound or 
     *     individual partials 
     *  \param numParts the number of partials in this sound
     **/
    void spatializationStereo(Sound *s, 
                              list<FileValue>* envList, 
                              string applyHow,
                              int numParts);

    /**
     *  Sets the spatialization of a sound by assigning each speaker in an 
     *     array its own envelope
     *  \param s pointer to a sound
     *  \param outerList List of Lists of FileValues (envelopes)
     *  \param applyHow string containing info if it applies to sound or 
     *     individual partials 
     *  \param numParts the number of partials in this sound
     **/
    void spatializationMultiPan(Sound *s, 
                                list<FileValue>* outerList, 
                                string applyHow,
                                int numParts);

    /**
     *  Sets the spatialization of a sound assuming a speaker array 
     *     forming a circle and uses polar coordinates to spread 
     *     the sound between them. (see LASS).
     *  \param s pointer to a sound
     *  \param thetaList List of theta FileValues (envelopes)
     *  \param radiusList List of radius FileValues (envelopes)
     *  \param applyHow string containing info if it applies to sound or 
     *     individual partials 
     *  \param numParts the number of partials in this sound
     **/
    void spatializationPolar(Sound *s, 
                             list<FileValue>* thetaList,
                             list<FileValue>* radiusList, 
                             string applyHow,
                             int numParts);

    /**
     *  Applies reverberation to a sound
     *  \param s a pointer to the sound being created
     **/
    void applyReverberation(Sound* s);

    /**
    *  Use of modifiers: tremolo, vibrato, transients. Makes 3 lists/maps - 
    *  one for modifiers with no dependencies, one for modifiers grouped 
    *  together, and one for modifiers with direct dependencies on other 
    *  modifiers. It goes through each list (in the order mentioned) to 
    *  find which modifiers to use and their respective values and applies 
    *  each of them.
    **/
    void applyModifiers(Sound *s, int numPartials);

    /**
     *  Apply modifiers for a note.
     **/
    vector<string> applyNoteModifiers();

};
#endif
