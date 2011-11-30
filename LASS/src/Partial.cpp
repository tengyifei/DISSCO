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
//	Partial.cpp
//
//----------------------------------------------------------------------------//

#ifndef __PARTIAL_CPP
#define __PARTIAL_CPP

//----------------------------------------------------------------------------//
#include "Partial.h"
#include "SoundSample.h"
#include "Constant.h"
#include "Track.h"
#include "Iterator.h"

#define PRIM_THREADS_USE_PTHREAD
#include "Threads.h"
using namespace prim;

extern Mutex renderLock;

//----------------------------------------------------------------------------//
Partial::Partial()
{
    // set some default parameters:
    setParam(WAVE_TYPE, 0);
    setParam(RELATIVE_AMPLITUDE, 1.0);
    setParam(PARTIAL_NUM, 1.0);
    setParam(WAVE_SHAPE, 1.0);
    setParam(FREQUENCY, 440.0);
    setParam(LOUDNESS_SCALAR, 1.0);
    //setParam(GLISSANDO_ENV, 1.0); // (no gliss)
    setParam(FREQ_ENV, 1.0);  // nothing applied to change frequency
    //setParam(DETUNING_ENV, 1.0); // (no detuning)
    setParam(AMPTRANS_AMP_ENV, 0 ); //no transients
    setParam(AMPTRANS_RATE_ENV, 0 );
    setParam(FREQTRANS_AMP_ENV, 0 );
    setParam(FREQTRANS_RATE_ENV, 0 );
    setParam(AMPTRANS_WIDTH, 1103); 
    setParam(FREQTRANS_WIDTH, 1103);

    reverbObj = NULL;
}

//----------------------------------------------------------------------------//
Track* Partial::render(m_sample_count_type sampleCount,
                       m_time_type duration,
                       m_rate_type samplingRate)
{
    //ScopedLock pl(PartialLock);
    // calculate the number of samples needed:
    m_sample_count_type numSamplesToRender = (m_sample_count_type) (duration * (m_time_type)samplingRate);
    m_sample_count_type numSamplesTotal;
    if(reverbObj != NULL)
        numSamplesTotal = (m_sample_count_type) ((duration + reverbObj->getDecay()) * (m_time_type)samplingRate);
    else
        numSamplesTotal = (m_sample_count_type) (duration * (m_time_type)samplingRate);

    //Create two SoundSample objects to write to:
    //One for the actual sound:
    SoundSample* waveSample = new SoundSample(sampleCount, samplingRate);
    //And one to store amplitude data:
    SoundSample* ampSample = new SoundSample(sampleCount, samplingRate);

    renderLock.Lock();
    DynamicVariable* copy_FREQUENCY = getParam(FREQUENCY).clone();
    DynamicVariable* copy_WAVE_SHAPE = getParam(WAVE_SHAPE).clone();
    DynamicVariable* copy_TREMOLO_AMP = getParam(TREMOLO_AMP).clone();
    DynamicVariable* copy_TREMOLO_RATE = getParam(TREMOLO_RATE).clone();
    DynamicVariable* copy_VIBRATO_AMP = getParam(VIBRATO_AMP).clone();
    DynamicVariable* copy_VIBRATO_RATE = getParam(VIBRATO_RATE).clone();
    DynamicVariable* copy_PHASE = getParam(PHASE).clone();
    DynamicVariable* copy_LOUDNESS_SCALAR = getParam(LOUDNESS_SCALAR).clone();
    //DynamicVariable* copy_FREQUENCY_DEVIATION = getParam(FREQUENCY_DEVIATION).clone();
    //DynamicVariable* copy_GLISSANDO_ENV = getParam(GLISSANDO_ENV).clone();
    DynamicVariable* copy_FREQ_ENV = getParam(FREQ_ENV).clone();
    //DynamicVariable* copy_DETUNING_ENV = getParam(DETUNING_ENV).clone();
    DynamicVariable* copy_AMPTRANS_WIDTH = getParam(AMPTRANS_WIDTH).clone();
    DynamicVariable* copy_FREQTRANS_WIDTH = getParam(FREQTRANS_WIDTH).clone();
    DynamicVariable* copy_AMPTRANS_AMP_ENV = getParam(AMPTRANS_AMP_ENV).clone();
    DynamicVariable* copy_AMPTRANS_RATE_ENV = getParam(AMPTRANS_RATE_ENV).clone();
    DynamicVariable* copy_FREQTRANS_AMP_ENV = getParam(FREQTRANS_AMP_ENV).clone();
    DynamicVariable* copy_FREQTRANS_RATE_ENV = getParam(FREQTRANS_RATE_ENV).clone();
    
    //Tell each dynamic variable what the DURATION will be:
    copy_FREQUENCY->setDuration(duration);
    copy_WAVE_SHAPE->setDuration(duration);
    copy_TREMOLO_AMP->setDuration(duration);
    copy_TREMOLO_RATE->setDuration(duration);
    copy_VIBRATO_AMP->setDuration(duration);
    copy_VIBRATO_RATE->setDuration(duration);
    copy_PHASE->setDuration(duration);
    copy_LOUDNESS_SCALAR->setDuration(duration);
    //copy_FREQUENCY_DEVIATION->setDuration(duration);
    //copy_GLISSANDO_ENV->setDuration(duration);
    copy_FREQ_ENV->setDuration(duration);
    //copy_DETUNING_ENV->setDuration(duration);
    copy_AMPTRANS_WIDTH->setDuration(duration);
    copy_FREQTRANS_WIDTH->setDuration(duration);
    copy_AMPTRANS_AMP_ENV->setDuration(duration);
    copy_AMPTRANS_RATE_ENV->setDuration(duration);
    copy_FREQTRANS_AMP_ENV->setDuration(duration);
    copy_FREQTRANS_RATE_ENV->setDuration(duration);
    
    //Tell each dynamic variable what the SAMPLING_RATE will be:
    copy_FREQUENCY->setSamplingRate(samplingRate);
    copy_WAVE_SHAPE->setSamplingRate(samplingRate);
    copy_TREMOLO_AMP->setSamplingRate(samplingRate);
    copy_TREMOLO_RATE->setSamplingRate(samplingRate);
    copy_VIBRATO_AMP->setSamplingRate(samplingRate);
    copy_VIBRATO_RATE->setSamplingRate(samplingRate);
    copy_PHASE->setSamplingRate(samplingRate);
    copy_LOUDNESS_SCALAR->setSamplingRate(samplingRate);
    //copy_FREQUENCY_DEVIATION->setSamplingRate(samplingRate);
    //copy_GLISSANDO_ENV->setSamplingRate(samplingRate);
    copy_FREQ_ENV->setSamplingRate(samplingRate);
    //copy_DETUNING_ENV->setSamplingRate(samplingRate);
    copy_AMPTRANS_WIDTH->setSamplingRate(samplingRate);
    copy_FREQTRANS_WIDTH->setSamplingRate(samplingRate);
    copy_AMPTRANS_AMP_ENV->setSamplingRate(samplingRate);
    copy_AMPTRANS_RATE_ENV->setSamplingRate(samplingRate);
    copy_FREQTRANS_AMP_ENV->setSamplingRate(samplingRate);
    copy_FREQTRANS_RATE_ENV->setSamplingRate(samplingRate);
    
    // grab iterators for each DynamicVariable:
    typedef Iterator<m_value_type> ValIter;

    ValIter frequency_it = copy_FREQUENCY->valueIterator();
    ValIter wave_shape_it = copy_WAVE_SHAPE->valueIterator();
    ValIter tremolo_amp_it = copy_TREMOLO_AMP->valueIterator();
    ValIter tremolo_rate_it = copy_TREMOLO_RATE->valueIterator();
    ValIter vibrato_amp_it = copy_VIBRATO_AMP->valueIterator();
    ValIter vibrato_rate_it = copy_VIBRATO_RATE->valueIterator();
    ValIter phase_it = copy_PHASE->valueIterator();
    ValIter loudness_scalar_it = copy_LOUDNESS_SCALAR->valueIterator();
    ValIter amptrans_width_it = copy_AMPTRANS_WIDTH->valueIterator();
    ValIter freqtrans_width_it = copy_FREQTRANS_WIDTH->valueIterator();
    //ValIter freq_deviation_it = copy_FREQUENCY_DEVIATION->valueIterator();
    //ValIter gliss_it = copy_GLISSANDO_ENV->valueIterator();
    ValIter freq_it = copy_FREQ_ENV->valueIterator();
    //ValIter detuning_it = copy_DETUNING_ENV->valueIterator();
    ValIter amptrans_amp_it = copy_AMPTRANS_AMP_ENV->valueIterator();
    ValIter amptrans_rate_it = copy_AMPTRANS_RATE_ENV->valueIterator();
    ValIter freqtrans_amp_it = copy_FREQTRANS_AMP_ENV->valueIterator();
    ValIter freqtrans_rate_it = copy_FREQTRANS_RATE_ENV->valueIterator();
    renderLock.Unlock();
    
    m_value_type* frequency_it_ptr = new m_value_type[numSamplesToRender],
      *frequency_it_start = frequency_it_ptr;
    m_value_type* wave_shape_it_ptr = new m_value_type[numSamplesToRender],
      *wave_shape_it_start = wave_shape_it_ptr;
    m_value_type* tremolo_amp_it_ptr = new m_value_type[numSamplesToRender],
      *tremolo_amp_it_start = tremolo_amp_it_ptr;
    m_value_type* tremolo_rate_it_ptr = new m_value_type[numSamplesToRender],
      *tremolo_rate_it_start = tremolo_rate_it_ptr;
    m_value_type* vibrato_amp_it_ptr = new m_value_type[numSamplesToRender],
      *vibrato_amp_it_start = vibrato_amp_it_ptr;
    m_value_type* vibrato_rate_it_ptr = new m_value_type[numSamplesToRender],
      *vibrato_rate_it_start = vibrato_rate_it_ptr;
    m_value_type* phase_it_ptr = new m_value_type[numSamplesToRender],
      *phase_it_start = phase_it_ptr;
    m_value_type* loudness_scalar_it_ptr = new m_value_type[numSamplesToRender],
      *loudness_scalar_it_start = loudness_scalar_it_ptr;
    m_value_type* amptrans_width_it_ptr = new m_value_type[numSamplesToRender],
      *amptrans_width_it_start = amptrans_width_it_ptr;
    m_value_type* freqtrans_width_it_ptr = new m_value_type[numSamplesToRender],
      *freqtrans_width_it_start = freqtrans_width_it_ptr;
    //m_value_type* freq_deviation_it_ptr = new m_value_type[numSamplesToRender],
    //  *freq_deviation_it_start = freq_deviation_it_ptr;
    //m_value_type* gliss_it_ptr = new m_value_type[numSamplesToRender],
    //  *gliss_it_start = gliss_it_ptr;
    m_value_type* freq_it_ptr = new m_value_type[numSamplesToRender],
      *freq_it_start = freq_it_ptr;
    //m_value_type* detuning_it_ptr = new m_value_type[numSamplesToRender],
    //  *detuning_it_start = detuning_it_ptr;
    m_value_type* amptrans_amp_it_ptr = new m_value_type[numSamplesToRender],
      *amptrans_amp_it_start = amptrans_amp_it_ptr;
    m_value_type* amptrans_rate_it_ptr = new m_value_type[numSamplesToRender],
      *amptrans_rate_it_start = amptrans_rate_it_ptr;
    m_value_type* freqtrans_amp_it_ptr = new m_value_type[numSamplesToRender],
      *freqtrans_amp_it_start = freqtrans_amp_it_ptr;
    m_value_type* freqtrans_rate_it_ptr = new m_value_type[numSamplesToRender],
      *freqtrans_rate_it_start = freqtrans_rate_it_ptr;
    
    renderLock.Lock();
    for(m_sample_count_type s = 0; s < numSamplesToRender; s++)
    {
      frequency_it_ptr[s] = frequency_it.next();
      wave_shape_it_ptr[s] = wave_shape_it.next();
      tremolo_amp_it_ptr[s] = tremolo_amp_it.next();
      tremolo_rate_it_ptr[s] = tremolo_rate_it.next();
      vibrato_amp_it_ptr[s] = vibrato_amp_it.next();
      vibrato_rate_it_ptr[s] = vibrato_rate_it.next();
      phase_it_ptr[s] = phase_it.next();
      loudness_scalar_it_ptr[s] = loudness_scalar_it.next();
      amptrans_width_it_ptr[s] = amptrans_width_it.next();
      freqtrans_width_it_ptr[s] = freqtrans_width_it.next();
      //freq_deviation_it_ptr[s] = freq_deviation_it.next();
      //gliss_it_ptr[s] = gliss_it.next();
      freq_it_ptr[s] = freq_it.next();
      //detuning_it_ptr[s] = detuning_it.next();
      amptrans_amp_it_ptr[s] = amptrans_amp_it.next();
      amptrans_rate_it_ptr[s] = amptrans_rate_it.next();
      freqtrans_amp_it_ptr[s] = freqtrans_amp_it.next();
      freqtrans_rate_it_ptr[s] = freqtrans_rate_it.next();
    }
    renderLock.Unlock();

    // randomize the frequency deviation:
    //
    // calculate the variance:
    // grab the frequency at the start of the partial:
    //m_value_type base_freq = getParam(FREQUENCY).valueIterator().next();
    //m_value_type freq_variance=(base_freq/(getParam(PARTIAL_NUM)+1.0))/2.0;
    //
    // get a random nummber between -1 and 1:
    //m_value_type rand = (((double)random())/((double)RAND_MAX)*2.0)-1.0;
    //
    // vary the freq_dev by this value:
    // make a copy of the frequency deviation dynamic variable
    //DynamicVariable* freq_dev = getParam(FREQUENCY_DEVIATION).clone();
    // scale the copy:
    //freq_dev->scale(rand * freq_variance);
    //
    // make a copy of the glissando dynamic variable

    // Phase values start off at zero for now:
    m_value_type tremolo_phase = 0.0;
    m_value_type vibrato_phase = 0.0;
    m_value_type freq_phase = 0.0;
    
    // values used in the loop:
    m_value_type tremolo;
    m_value_type amplitude;
    m_value_type vibrato;
    m_value_type frequency;
    m_value_type phase;
    m_value_type sample;
    m_value_type amptransient;
    m_value_type freqtransient;
    m_value_type amptrans_width = 0.0;
    m_value_type freqtrans_width = 0.0;
    m_time_type amptransprob;
    m_time_type freqtransprob;
    
    //srand(time(0));

    //flags to tell if we are in a transient
    int amptransflag = 0;
    int freqtransflag = 0;
    
    //placemarkers once in a transient
    int amptransplace = 0;
    int freqtransplace = 0;
    
    //counters to see if we should check for a transient
    int amptranscheck = (int)amptrans_width;
    int freqtranscheck = (int)freqtrans_width;
    float random;

    m_value_type freqmod = 0.0, trans_freqmod = 0;
    m_value_type amplifier = 0.0, trans_amplifier = 0;
    
    //Loop over every sample:
    for(m_sample_count_type s = 0; s < numSamplesToRender; s++)
    {  
      //---------//
      //AMPLITUDE//
      //---------//
      
      //Advance transient related iterators
      amptrans_width = *amptrans_width_it_ptr++;
      trans_amplifier = *amptrans_amp_it_ptr++;
      amptransprob = *amptrans_rate_it_ptr++;
      
      //Grab the tremolo value
      tremolo = *tremolo_amp_it_ptr++ * sin(2.0 * M_PI * tremolo_phase);
      
      //Increment the tremolo phase
      tremolo_phase = pmod(tremolo_phase +
        (*tremolo_rate_it_ptr++ / samplingRate));
        
	    //Once counter reaches 0, check for transient
	    if(amptranscheck <= 0 && s + amptrans_width < numSamplesToRender)
      {
        random = ((float)std::rand()/(float)RAND_MAX);
        if(random <= amptransprob)
	      {		
          trans_amplifier *= (float)std::rand()/(float)RAND_MAX;
          if(((float)std::rand()/(float)RAND_MAX) <= 0.5)
            trans_amplifier *= -1;
          amplifier = trans_amplifier;
          amptransflag = 1;
          //We are at beginning of transient
          amptransplace = 0;
	      }
        //Reset check counter
        amptranscheck = (int)amptrans_width;
      }
	
      //Augment the amplitude value by the tremolo value and the loudness scale.
      if(amptransflag)	 
      {
        //The following decides where we are inside the transient.
        //Used to scale the transient multiplier effect.
        if(amptransplace < amptrans_width / 2)
          amptransient = amplifier * amptransplace / (amptrans_width / 2);
        else
          amptransient = amplifier * (amptrans_width - amptransplace) /
            (amptrans_width / 2);
        amptransplace++;
        if(amptransplace >= amptrans_width)
          amptransflag = 0;
      }
      else
        amptransient = 0;
	
      //Decrease the check counter.
      amptranscheck--;
      amplitude = *loudness_scalar_it_ptr++ * *wave_shape_it_ptr++ *
        (1.0 + tremolo);

      //Apply transient modifier to amplitude
      amplitude = amplitude + amptransient*amplitude;

      //---------//
      //FREQUENCY//
      //---------//

      //Advance frequency transient related iterators.
      freqtrans_width = *freqtrans_width_it_ptr++;
      trans_freqmod = *freqtrans_amp_it_ptr++;
      freqtransprob = *freqtrans_rate_it_ptr++;
	
      //If we should check, check.
      if(freqtranscheck <= 0 && s+freqtrans_width < numSamplesToRender)
      {
        random = ((float)std::rand() / (float)RAND_MAX);
        if(random <= freqtransprob)
        {		
          trans_freqmod *= (float)std::rand() / (float)RAND_MAX;
          if(((float)std::rand() / (float)RAND_MAX) <= 0.5)
            trans_freqmod *= -1;
          freqmod = trans_freqmod;
          freqtransflag = 1;
          freqtransplace = 0;
        }
        freqtranscheck = (int)freqtrans_width;
      }

      //If we are inside a transient, make appropriate changes.
      if(freqtransflag)	 
      {
        if(freqtransplace < freqtrans_width / 2)
          freqtransient = freqmod * freqtransplace / (freqtrans_width / 2);
        else
          freqtransient = freqmod * (freqtrans_width - freqtransplace) /
            (freqtrans_width/2);
        freqtransplace++;
        if(freqtransplace >= freqtrans_width)
          freqtransflag = 0;
      }
      else
        freqtransient = 0;
      freqtranscheck--;

      //Perhaps the vibrato should be keyed to PartialNumber.

      //Grab the vibrato value.
      vibrato =  *vibrato_amp_it_ptr++ * sin(2.0 * M_PI * vibrato_phase);
      
      //Increment the vibrato phase.
      vibrato_phase = pmod(
        vibrato_phase + (*vibrato_rate_it_ptr++ / samplingRate));

      //Augment the frequency value by the freq-deviation and vibrato and
      //detuning and freq_env and transients.
      frequency  = *frequency_it_ptr++ * *freq_it_ptr++; // * *detuning_it_ptr++
      
      //frequency += *freq_deviation_it_ptr++;
      frequency *= 1.0 + vibrato;

      //Apply frequency transient adjustments.
      frequency += frequency*freqtransient;
      
      //Increment the frequency phase:
      freq_phase = pmod (freq_phase + (frequency / samplingRate));
        
      //Calculate the actual phase, augmenting freq_phase with they dynamic
      //variable phase (phase offset).
      phase = freq_phase + *phase_it_ptr++;

      //------//
      //SAMPLE//
      //------//
        
      switch( (int) getParam(WAVE_TYPE))
      {
        case 1: //Create a random sample.
        sample = amplitude * 2 * ((((double) std::rand()) / ((double) RAND_MAX)) - 0.5);
        break;
        
        default: //Calculate the sinusoidal sample.
        sample = amplitude * (sin(2.0 * M_PI * phase));
        break;
      }
      
      //Assign it.
      (*waveSample)[s] = sample;
      (*ampSample)[s] = amplitude;
    }

    //Create a TrackObject.
    Track *returnTrack = new Track(waveSample, ampSample);

    //Calculate reverb if necessary.
    renderLock.Lock();
    if(reverbObj != NULL)
    {
      Track &tmp = reverbObj->do_reverb_Track(*returnTrack);
      delete returnTrack;
      returnTrack = new Track(tmp);

      //Delete the temporary track object.
      delete &tmp;
    }
    renderLock.Unlock();
    
    //Delete the envelope calculations.
    delete [] frequency_it_start;
    delete [] wave_shape_it_start;
    delete [] tremolo_amp_it_start;
    delete [] tremolo_rate_it_start;
    delete [] vibrato_amp_it_start;
    delete [] vibrato_rate_it_start;
    delete [] phase_it_start;
    delete [] loudness_scalar_it_start;
    delete [] amptrans_width_it_start;
    delete [] freqtrans_width_it_start;
    //delete [] freq_deviation_it_start;
    //delete [] gliss_it_start;
    delete [] freq_it_start;
    //delete [] detuning_it_start;
    delete [] amptrans_amp_it_start;
    delete [] amptrans_rate_it_start;
    delete [] freqtrans_amp_it_start;
    delete [] freqtrans_rate_it_start;
    
    return returnTrack;
}

//----------------------------------------------------------------------------//
inline m_value_type Partial::pmod(m_value_type num)
{
    while (num > 1.0)
        num -= 1.0;
    return num;
}


//----------------------------------------------------------------------------//
void Partial::use_reverb(Reverb *newReverbObj)
{
	reverbObj = newReverbObj;
}

//----------------------------------------------------------------------------//
m_time_type Partial::getTotalDuration(m_time_type dryDuration)
{
	return dryDuration + ( (reverbObj != 0) ? reverbObj->getDecay() : 0 );
}

//----------------------------------------------------------------------------//
void Partial::xml_print( ofstream& xmlOutput, list<Reverb*>& revObjs, list<DynamicVariable*>& dynObjs )
{

	xmlOutput << "\t\t<partial>" << endl;

        // Output reverb ID and update reverb collection if necessary
        xmlOutput << "\t\t\t<reverb_ptr id=\"" << (long)reverbObj << "\" />" << endl;
        list<Reverb*>::const_iterator revit;
        for( revit=revObjs.begin(); revit != revObjs.end(); revit++ )
        {
                if( (*revit) == reverbObj )
                        break;
        }
        if( revit == revObjs.end() ){
                revObjs.push_back( reverbObj );
        }


	// Static Parameters
	xmlOutput << "\t\t\t<relative_amplitude value=\"" << getParam(RELATIVE_AMPLITUDE) << "\" />" << endl;
	xmlOutput << "\t\t\t<partial_num value=\"" << getParam(PARTIAL_NUM) << "\" />" << endl;

	// Dynamic Paramters
	// Call the xml_print for each Dynamic Var

	xmlOutput << "\t\t\t<frequency>" << endl;
	getParam(FREQUENCY).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</frequency>" << endl;
       
	xmlOutput << "\t\t\t<wave_shape>" << endl;
	getParam(WAVE_SHAPE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</wave_shape>" << endl;
	
	xmlOutput << "\t\t\t<loudness_scalar>" << endl;
	getParam(LOUDNESS_SCALAR).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</loudness_scalar>" << endl;

	xmlOutput << "\t\t\t<tremolo_amp>" << endl;
	getParam(TREMOLO_AMP).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</tremolo_amp>" << endl;
	
	xmlOutput << "\t\t\t<tremolo_rate>" << endl;
	getParam(TREMOLO_RATE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</tremolo_rate>" << endl;
	
	xmlOutput << "\t\t\t<vibrato_amp>" << endl;
	getParam(VIBRATO_AMP).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</vibrato_amp>" << endl;
	
	xmlOutput << "\t\t\t<vibrato_rate>" << endl;
	getParam(VIBRATO_RATE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</vibrato_rate>" << endl;
	
	xmlOutput << "\t\t\t<phase>" << endl;
	getParam(PHASE).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</phase>" << endl;

	//xmlOutput << "\t\t\t<frequency_deviation>" << endl;
	//getParam(FREQUENCY_DEVIATION).xml_print( xmlOutput, dynObjs );
	//xmlOutput << "\t\t\t</frequency_deviation>" << endl;
	
	/*xmlOutput << "\t\t\t<glissando_env>" << endl;
	getParam(GLISSANDO_ENV).xml_print( xmlOutput, dynObjs );
	xmlOutput << "\t\t\t</glissando_env>" << endl;
	*/
	

	xmlOutput << "\t\t\t<freq_env>" << endl;
	getParam(FREQ_ENV).xml_print(xmlOutput, dynObjs);
	xmlOutput << "\t\t\t</freq_env>" << endl;


	//xmlOutput << "\t\t\t<detuning_env>" << endl;
	//getParam(DETUNING_ENV).xml_print( xmlOutput, dynObjs );
	//xmlOutput << "\t\t\t</detuning_env>" << endl;

	xmlOutput << "\t\t</partial>" << endl;
}

void Partial::xml_read(XmlReader::xmltag* partialtag, DISSCO_HASHMAP<long, Reverb *>* reverbHash, DISSCO_HASHMAP<long, DynamicVariable *> *dvHash)
{
	if(strcmp("partial",partialtag->name))
	{
		printf("Not a partial tag!  This is a %s tag.\n",partialtag->name);
		return;
	}

	char *value;

	if((value = partialtag->findChildParamValue("reverb_ptr", "id")) != 0)
	{
		long id=atoi(value);
		Reverb * temp;
		temp = (*reverbHash)[id];
		
		if(temp != NULL)
		{
			use_reverb(temp);
		}
	}

	if((value = partialtag->findChildParamValue(
	  "relative_amplitude", "value")) != 0)
	  	setParam(RELATIVE_AMPLITUDE, atof(value));

	if((value = partialtag->findChildParamValue("partial_num","value")) != 0)
		setParam(PARTIAL_NUM, atoi(value));
	
	// For sake of ease, I am going to forego searching for each item and instead
	// iterate thru the list of child tags.

	XmlReader::xmltag *dvtag;	XmlReader::xmltagset *child=partialtag->children;

	while(child)
	{
		dvtag=child->tag;

		if(strcmp(dvtag->name,"frequency") == 0)
			auxLoadParam(FREQUENCY,dvtag,dvHash);
		else if(strcmp(dvtag->name,"wave_shape") == 0)
			auxLoadParam(WAVE_SHAPE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"loudness_scalar") == 0)
			auxLoadParam(LOUDNESS_SCALAR,dvtag,dvHash);
		else if(strcmp(dvtag->name,"tremolo_amp") == 0)
			auxLoadParam(TREMOLO_AMP,dvtag,dvHash);
		else if(strcmp(dvtag->name,"tremolo_rate") == 0)
			auxLoadParam(TREMOLO_RATE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"vibrato_amp") == 0)
			auxLoadParam(VIBRATO_AMP,dvtag,dvHash);
		else if(strcmp(dvtag->name,"vibrato_rate") == 0)
			auxLoadParam(VIBRATO_RATE,dvtag,dvHash);
		else if(strcmp(dvtag->name,"phase") == 0)
			auxLoadParam(PHASE,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"frequency_deviation") == 0)
		//auxLoadParam(FREQUENCY_DEVIATION,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"glissando_env") == 0)
		//auxLoadParam(GLISSANDO_ENV,dvtag,dvHash);
		else if(strcmp(dvtag->name,"freq_env") == 0)
		  auxLoadParam(FREQ_ENV,dvtag,dvHash);
		//else if(strcmp(dvtag->name,"detuning_env") == 0)
		//auxLoadParam(DETUNING_ENV,dvtag,dvHash);

		child=child->next;
	}
}

void Partial::auxLoadParam(enum PartialDynamicParam param,XmlReader::xmltag *tag, DISSCO_HASHMAP<long, DynamicVariable *> *dvHash)
{
	char *value;

	// Try and do a lookup
	if((value = tag->findChildParamValue("dv_ptr", "id")) != 0)
	{
		long id=atol(value);
		DynamicVariable *dv=(*dvHash)[id];

		if(dv)
			setParam(param,*dv);
		else
			//printf("[%s:%d] PartialParam refrences non-existant dv %d!\n",__FILE__,__LINE__,id);
			cout<<"["<<__FILE__<<":"<<__LINE__<<"] PartialParam references non-existant dv "<<id<<"!"<<endl;
	}

	//Else read in the tag
	else
	{
		DynamicVariable *dv=DynamicVariable::create_dv_from_xml(tag);
		setParam(param,*dv);
	}
}

#endif //__PARTIAL_CPP
