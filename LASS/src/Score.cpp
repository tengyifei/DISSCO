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
//	Score.cpp
//
//----------------------------------------------------------------------------//

#ifndef __SCORE_CPP
#define __SCORE_CPP

//----------------------------------------------------------------------------//

#include "Score.h"
#include "Types.h"

//----------------------------------------------------------------------------//

// This function is the worker function for the rendering threads
void* multithreaded_render_worker(void *vtle)
{
	// Cast the argument to correct type
	threadlist_entry *tle=(threadlist_entry*)vtle;

	// Do the rendering
	MultiTrack* renderedSound = tle->snd->render(tle->numChannels, tle->samplingRate);

	// Obtain the lock so we can modify the list entry
	pthread_mutex_lock(tle->listMutex);
		tle->resultTrack=renderedSound;
		tle->done=1;
	pthread_mutex_unlock(tle->listMutex);

	// Signal that we are done
	pthread_cond_signal(tle->finishCondition);

	return NULL;
}

//----------------------------------------------------------------------------//
Score::Score()
    : cmm_(NONE)
{
    reverbObj = NULL;
}

//----------------------------------------------------------------------------//
MultiTrack* Score::render(int numChannels, m_rate_type samplingRate)
{

    // figure out how long the score will run:
    Iterator<Sound> it = iterator();
    m_time_type scoreEndTime = 0.0;
    int numSounds = 0;
    while (it.hasNext())
    {
        Sound& snd = it.next();
        m_time_type soundEndTime = snd.getParam(START_TIME) +
                                   snd.getTotalDuration();
        if (soundEndTime > scoreEndTime) scoreEndTime = soundEndTime;
        numSounds++;
    }

    // figure in the reverb die-out period
    if(reverbObj != NULL)
        scoreEndTime += reverbObj->getDecay();

    // how many samples we will need:
    m_sample_count_type numSamples =
        (m_sample_count_type) (scoreEndTime * float(samplingRate));

    // now, create an empty multi-track object of the proper length and channels:
    MultiTrack* score = new MultiTrack(numChannels, numSamples, samplingRate);

    // for each sound in this score:
    int num=0;
    it = iterator();
    while (it.hasNext())
    {

        num++;
        cout << "Sound #" << num << " of " << numSounds << ":" << endl;

        // render the sound:
        Sound& snd = it.next();
        MultiTrack* renderedSound = snd.render(numChannels, samplingRate);

        // composite the rendered sound:
        score->composite(*renderedSound, snd.getParam(START_TIME));
        
        // delete the rendered sound:
        delete renderedSound;
    }
    
    // do the reverb
    if(reverbObj != NULL)
    {
        cout << "Applying reverb to the score..." << endl;
        MultiTrack *tmp = & reverbObj->do_reverb_MultiTrack(*score);
        delete score;
        score = tmp;
    }

    // perform Clipping management on the composite:
    cout << "Managing Clipping for the score..." << endl;
    manageClipping(score, cmm_);
    
    // return the composite:
    return score;
}

//----------------------------------------------------------------------------//

// This is the multithreaded version of render
MultiTrack* Score::render(int numChannels, m_rate_type samplingRate, int nThreads)
{
    // figure out how long the score will run:
    Iterator<Sound> it = iterator();
    m_time_type scoreEndTime = 0.0;
    while (it.hasNext())
    {
        Sound& snd = it.next();
        m_time_type soundEndTime = snd.getParam(START_TIME) +
                                   snd.getTotalDuration();
        if (soundEndTime > scoreEndTime) scoreEndTime = soundEndTime;
    }

    // figure in the reverb die-out period
    if(reverbObj != NULL)
        scoreEndTime += reverbObj->getDecay();

    // how many samples we will need:
    m_sample_count_type numSamples =
        (m_sample_count_type) (scoreEndTime * float(samplingRate));

    // now, create an empty multi-track object of the proper length and channels:
    MultiTrack* score = new MultiTrack(numChannels, numSamples, samplingRate);

    // for each sound in this score:
    int num=0;
    it = iterator();

    //Set up for multithreading the rendering
    pthread_cond_t finishCondition=PTHREAD_COND_INITIALIZER;
    pthread_mutex_t listMutex=PTHREAD_MUTEX_INITIALIZER;

    //Begin owning the list Mutex
    pthread_mutex_lock(&listMutex);

	// This is a list of the data passed to and from the threads
    Collection<threadlist_entry*> threadList;

	// Populate the list with some initial data
    for(int th=0;(th<nThreads)&&(it.hasNext());th++)
    {
	   threadlist_entry *tle=new threadlist_entry;
	   tle->snd=&(it.next());
	   tle->done=0;
	   tle->thread=new pthread_t;
	   tle->listMutex=&listMutex;
	   tle->finishCondition=&finishCondition;
	   tle->numChannels=numChannels;
	   tle->samplingRate=samplingRate;

	   // Add to the collection
	   threadList.add(tle);

	   num++;
       cout << "Sound #" << num << ":" << endl;

	   // Begin the thread to render this sound
	   pthread_create(tle->thread,NULL,multithreaded_render_worker,(void*)tle);
    }

    //MultiTrack *thisTrack;
    int finishedCount=0;
    while (finishedCount<size())
    {
		// Look thru the list to find done threads
	    for(int lm=0;lm<nThreads;lm++)
	    {
		    threadlist_entry *tle=threadList.get(lm);
		    if(tle->done)
		    {
				// Do the composite into the final result
				score->composite(*(tle->resultTrack), tle->snd->getParam(START_TIME));
				delete tle->resultTrack;

				// Reset values in the thread list
				tle->resultTrack=NULL;
				tle->done=0;
				finishedCount++;
			
				// If there is another sound still to render, lets do it
				if(it.hasNext())
				{
					num++;
        			cout << "Sound #" << num << ":" << endl;
					tle->snd=&(it.next());
	   				pthread_create(tle->thread,NULL,multithreaded_render_worker,(void*)tle);
				}
		    }
	    }

		// If we haven't rendered everything yet, lets wait for a thread to signal us.
	    if(finishedCount!=size())
	    	pthread_cond_wait(&finishCondition,&listMutex);
    }
    
    // do the reverb
    if(reverbObj != NULL)
    {
        cout << "Applying reverb to the score..." << endl;
        MultiTrack *tmp = & reverbObj->do_reverb_MultiTrack(*score);
        delete score;
        score = tmp;
    }

    // perform Clipping management on the composite:
    cout << "Managing Clipping for the score..." << endl;
    manageClipping(score, cmm_);
    
    // return the composite:
    return score;
}

//----------------------------------------------------------------------------//
void Score::setClippingManagementMode(ClippingManagementMode mode)
{
    cmm_ = mode;
}

//----------------------------------------------------------------------------//
Score::ClippingManagementMode Score::getClippingManagementMode()
{
    return cmm_;
}


//----------------------------------------------------------------------------//
void Score::manageClipping(MultiTrack* mt, ClippingManagementMode mode)
{
    switch (mode)
    {
        case NONE:		break;
        case CLIP: 		clip(mt); break;
        case SCALE : 		scale(mt); break;
        case CHANNEL_SCALE : 	channelScale(mt); break;
        case ANTICLIP :		anticlip(mt); break;
        case CHANNEL_ANTICLIP :	channelAnticlip(mt); break;
        default : 		break;
    }
}

//----------------------------------------------------------------------------//
void Score::use_reverb(Reverb *newReverbObj)
{
	reverbObj = newReverbObj;
}

//----------------------------------------------------------------------------//
// 	PRIVATE FUNCTIONS:
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
void Score::clip(MultiTrack* mt)
{
    cout << "Performing CLIP" << endl;

    // for each track
    for (int t=0; t<mt->size(); t++)
    {
        SoundSample& wave = mt->get(t)->getWave();
        SoundSample& amp = mt->get(t)->getAmp();

        // for each sample
        m_sample_count_type numSamples = wave.getSampleCount();
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            if (wave[s] >  1.0) wave[s] =  1.0;
            if (wave[s] < -1.0) wave[s] = -1.0;
            if (amp[s] >  1.0) amp[s] =  1.0;
            if (amp[s] < -1.0) amp[s] = -1.0;
        }
        
    }
}

//----------------------------------------------------------------------------//
void Score::scale(MultiTrack* mt)
{
    cout << "Performing SCALE" << endl;

    // -----
    // first, find the maximum amplitude:

    m_sample_type maxAmp = 0;
    // for each track
    for (int t=0; t<mt->size(); t++)
    {
        SoundSample& amp = mt->get(t)->getAmp();
        // for each sample
        m_sample_count_type numSamples = amp.getSampleCount();
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            if (amp[s] > maxAmp) maxAmp = amp[s];
        }
    }
    
    // -----
    // create a scaling factor:
    m_sample_type scalingFactor = 1.0 / maxAmp;
    
    // -----
    // scale every value by this factor
    
    // for each track
    for (int t=0; t<mt->size(); t++)
    {
        SoundSample& wave = mt->get(t)->getWave();
        SoundSample& amp = mt->get(t)->getAmp();

        // for each sample
        m_sample_count_type numSamples = wave.getSampleCount();
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            wave[s] *= scalingFactor;
            amp[s] *= scalingFactor;
        }
    }
}

//----------------------------------------------------------------------------//
void Score::channelScale(MultiTrack* mt)
{
    cout << "Performing CHANNEL_SCALE" << endl;

    // -----
    // for each track
    for (int t=0; t<mt->size(); t++)
    {
        // -----
        // find the maximum amplitude:
        SoundSample& wave = mt->get(t)->getWave();
        SoundSample& amp = mt->get(t)->getAmp();
        
        m_sample_type maxAmp = 0;

        // for each sample
        m_sample_count_type numSamples = wave.getSampleCount();
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            if (amp[s] > maxAmp) maxAmp = amp[s];
        }

        // -----
        // create a scaling factor:
        m_sample_type scalingFactor = 1.0 / maxAmp;
    
        // -----
        // scale every value by this factor
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            wave[s] *= scalingFactor;
            amp[s] *= scalingFactor;
        }
    
    }
}

//----------------------------------------------------------------------------//
void Score::anticlip(MultiTrack* mt)
{ 
    cout << "Performing ANTICLIP" << endl;

    // this is a dificult one, because we need fast cros-track access.
    // make our own data-structure for this one.
    vector<SoundSample*> wave;
    vector<SoundSample*> amp;
    int numTracks = mt->size();
    for (int i=0; i<numTracks; i++)
    {
        wave.push_back(& mt->get(i)->getWave());
        amp.push_back(& mt->get(i)->getAmp());
    }

    // now, for each sample:
    m_sample_count_type numSamples = wave[0]->getSampleCount();
    for (m_sample_count_type s=0; s<numSamples; s++)
    {
        // find the total amplitude across all tracks.
        m_sample_type totalAmp = 0.0;
        for (int t=0; t<numTracks; t++)
        {
            totalAmp += (*amp[t])[s];
        }
        
        // scale if necessary
        if (totalAmp > 1.0)
        {
            m_sample_type scalingFactor = 1.0 / totalAmp;
            for (int t=0; t<numTracks; t++)
            {
                (*amp[t] )[s] *= scalingFactor;
                (*wave[t])[s] *= scalingFactor;
            }
        }
    }
}

//----------------------------------------------------------------------------//
m_sample_type todB(m_sample_type x)
{
  return (m_sample_type)(log10((double)x) * 20.0);
}

m_sample_type fromdB(m_sample_type x)
{
  return (m_sample_type)pow(10.0, ((double)x * 0.05));
}

m_sample_type compressSound(m_sample_type x, m_sample_type peak,
  m_sample_type dBCompressionPoint)
{
  m_sample_type xdB = todB(x);
  m_sample_type cdB = dBCompressionPoint;
  m_sample_type pdB = todB(peak);
  
  if(x < fromdB(dBCompressionPoint))
    return x;

  return fromdB((pdB - xdB) * (pdB * xdB - cdB * cdB) /
    ((cdB - pdB) * (cdB - pdB)));
}

void Score::channelAnticlip(MultiTrack* mt)
{
    cout << "Performing CHANNEL_ANTICLIP" << endl;

    // for each track
    m_sample_type maxAmplitude = 0.0;
    int peakPlace = 0;
    for (int t=0; t<mt->size(); t++)
    {
        SoundSample& wave = mt->get(t)->getWave();
        SoundSample& amp = mt->get(t)->getAmp();
        
        // for each sample
        m_sample_count_type numSamples = wave.getSampleCount();
        for (m_sample_count_type s=0; s<numSamples; s++)
        {
            if (amp[s] > 1.0)
            {
                // scale this sample:
                //wave[s] *= 1.0 / amp[s];
                //amp[s] = 1.0;
            }
            m_sample_type cur = wave[s];
            if(cur < 0) cur = -cur;
            if(cur > maxAmplitude)
            {
              maxAmplitude = cur;
              peakPlace = s;
            } 
        }
    }
    if(maxAmplitude < 0.99)
    {
      cout << "Peak at " << maxAmplitude << endl;
      //cout << "Normalizing to achieve better signal-to-noise ratio.";
    }
    else
    {
      cout << "Warning: peak is " << todB(maxAmplitude) << " dB at " <<
        ((double)peakPlace / (double)mt->get(0)->getWave().getSamplingRate()) <<
        " seconds. Compressing [-6, " << todB(maxAmplitude) << ") to [-6, 0) dB";
      maxAmplitude /= 0.99; //Never actually allow it to hit 0dB.
      //m_sample_type normalizeValue = maxAmplitude;
      for (int t=0; t<mt->size(); t++)
      {
          SoundSample& wave = mt->get(t)->getWave();
          SoundSample& amp = mt->get(t)->getAmp();
          
          // for each sample
          m_sample_count_type numSamples = wave.getSampleCount();
          for (m_sample_count_type s=0; s<numSamples; s++)
          {
              amp[s] = 1.0;
              wave[s] = compressSound(wave[s], maxAmplitude, -6.0);
              //Test showing compression curve used:
              //wave[s] = compressSound((float)s / (float)numSamples * 2.f, 2.f, -6.0);
          }
      }
    }
}

//XML FUNCTIONS---------------------------------------------------------------//
//----------------------------------------------------------------------------//
void Score::xml_read(XmlReader::xmltag *scoretag)
{
	
	// Sanity Check
	if(strcmp("score",scoretag->name))
	{
		printf("Not a score tag!  This is a %s tag!\n",scoretag->name);
		return;
	}
	
	
	

	// Perhaps that an error should be thrown if the following  are *not*
	// found...  But i'm just coding now for sake of demonstration.
	XmlReader::xmltag *childtag;
	char *value;
	reverbHash = new DISSCO_HASHMAP<long, Reverb *>;
	dvHash = new DISSCO_HASHMAP<long, DynamicVariable *>;
	
	while((childtag = scoretag->children->find("reverb")) != 0)
	{
		if((value = childtag->getParamValue("id")) != 0)
		{
			int id = atoi(value);
			Reverb* tempRev = new Reverb(0);
			tempRev->xml_read(childtag);
			(*reverbHash)[id] = tempRev;
		}
	}
	
	while((childtag = scoretag->children->find("dv")) != 0)
	{
		if((value = childtag->getParamValue("id")) != 0)
		{
			int id = atoi(value);
			DynamicVariable* tempDV = DynamicVariable::create_dv_from_xml(childtag);
			(*dvHash)[id] = tempDV;
		}
	}
	
	if((value = scoretag->findChildParamValue("reverb_ptr","id")) != 0)
	{
		long id = atoi(value);
		Reverb * temp;
		temp = (*reverbHash)[id];
		
		if(temp != NULL)
			use_reverb(temp);
	}

	if((value = scoretag->findChildParamValue("cmm_","value")) != 0)
		setClippingManagementMode((Score::ClippingManagementMode)atoi(value));
	
	while((childtag = scoretag->children->find("sound")) != 0)
	{
		Sound s;
		s.xml_read(childtag, reverbHash, dvHash);
		add(s);	
	}
}


//----------------------------------------------------------------------------//
void Score::xml_print( ofstream& xmlOutput )
{
	// revObjs is a list of the distinct reverb objects in this
	// score and its associated sounds, partials, etc.
	//  Inline in the score's xml, reverb objects are denoted by unique
	//  id's (their pointers), then these objects are displayed fully at
	//  the end of the file
	list<Reverb*> revObjs;
	revObjs.push_back( reverbObj );
	
	// dynObjs is a list of Dynamic Variables, used for the same kind of
	//  thing as reverb
	list<DynamicVariable*> dynObjs;
	xmlOutput << "<score>" << endl;
	xmlOutput << "\t<cmm_ value=\"" << getClippingManagementMode() << "\" />" << endl;
	xmlOutput << "\t<reverb_ptr id=\"" << (long)reverbObj << "\" />" << endl;
		// XML for this score's sounds
	Iterator<Sound> it = iterator();
	while (it.hasNext())
	{
		Sound& snd = it.next();
		snd.xml_print( xmlOutput, revObjs, dynObjs );
	}
	
	// XML for this score's reverb objects
	list<Reverb*>::const_iterator revit;
	for( revit=revObjs.begin(); revit != revObjs.end(); revit++ )
	{
	    if( (*revit) != NULL )
		(*revit)->xml_print( xmlOutput );
	}
		// XML for this score's dynamic variables
	list<DynamicVariable*>::const_iterator dynit;
	for( dynit=dynObjs.begin(); dynit != dynObjs.end(); dynit++ )
	{
	    if( (*dynit) != NULL )
		(*dynit)->xml_print( xmlOutput );
	}

	xmlOutput << "</score>" << endl;
	xmlOutput.close();
}

void Score::xml_print(const char * xmlOutputPath)
{
	ofstream xmlOutput(xmlOutputPath);
	if(!xmlOutput)
	{
		cout << "Error Opening file for XML output!" << endl;
	}
	else
	{
		xml_print(xmlOutput);
	}
}

void Score::xml_print()
{
	ofstream xmlOutput("lass-output.xml");
	if(!xmlOutput)
	{
		cout << "Error Opening file for XML output!" << endl;
	}
	else
	{
		xml_print(xmlOutput);
	}
}



//----------------------------------------------------------------------------//
#endif //__SCORE_CPP

