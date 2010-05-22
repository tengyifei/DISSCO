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
//	AuWriter.cpp
//
//----------------------------------------------------------------------------//

#ifndef __AU_WRITER_CPP
#define __AU_WRITER_CPP

//----------------------------------------------------------------------------//

#include <cstdarg>
#include "AuWriter.h"

//----------------------------------------------------------------------------//
bool AuWriter::write(SoundSample& ss, string filename, int bits)
{
    vector<SoundSample*> channels;
    channels.push_back(&ss);
    return write(channels, filename, bits);
}

//----------------------------------------------------------------------------//
bool AuWriter::write(Track& t, string filename, int bits)
{
    vector<SoundSample*> channels;
    channels.push_back(&t.getWave());
    return write(channels, filename, bits);
}


//----------------------------------------------------------------------------//
bool AuWriter::write(MultiTrack& mt, string filename, int bits)
{
    vector<SoundSample*> channels;

    Iterator<Track*> it = mt.iterator();
    while (it.hasNext())
    {
        channels.push_back(& it.next()->getWave());
    }

    return write(channels, filename, bits);
}

//----------------------------------------------------------------------------//
bool AuWriter::write_one_per_track(MultiTrack& mt, char *filename, ...)
{
	vector<SoundSample*> channels;
	va_list marker;

	bool ret_val = true;
	char *cur_filename = filename;
	Iterator<Track*> it = mt.iterator();

	va_start(marker, filename);
	while (it.hasNext())
	{
		channels.push_back(& it.next()->getWave());
		ret_val = ret_val & write(channels, string(cur_filename));
		channels.pop_back();
		cur_filename = va_arg(marker, char *);
	}
	va_end(marker);

	return ret_val;
}

//----------------------------------------------------------------------------//
bool AuWriter::write(vector<SoundSample*>& channels, string filename, 
						int bits)
{
    // ensure that all of the SoundSamples, have the same samplingRate.
    // Also, find the lowest sampleCount value of all the SoundSamples.
    // warn if sections of other SoundSamples are going to be clipped
    // because of this.
    
    int numChannels = channels.size();
    
    if (numChannels == 0)
    {
        cout << "ERROR: AuWriter: numChannels = 0" << endl;
        return false;
    }
    
    m_rate_type samplingRate = channels[0]->getSamplingRate();
    m_sample_count_type numSamples = channels[0]->getSampleCount();
    m_sample_count_type maxSamples = numSamples;
    
    for (int c=0; c<numChannels; c++)
    {
        // make sure every channel has the same sampling rate
        if (channels[c]->getSamplingRate() != samplingRate)
        {
            cerr << "ERROR: AuWriter: not all channels "
                 << "have the same sampling rate" << endl;
            return false;
        }
        
        // check the number of samples
        m_sample_count_type count = channels[0]->getSampleCount();
        if (count < numSamples) numSamples = count;
        if (count > maxSamples) maxSamples = count;
    }
    
    if (maxSamples > numSamples)
    {
        m_time_type secondsClipped =
         ((m_time_type)(maxSamples - numSamples)) /
         ((m_time_type)samplingRate);
         
        cerr << "WARNING: AuWriter: " 
             << "Because not all SoundSamples were of the same length, " << endl
             << secondsClipped << " seconds will be clipped off of the "
             << "end of one or more channels." << endl;
    }
    
    
    cout << "Writing " << numSamples << " samples at " << samplingRate 
         << " Hz to file " << filename
         << " (" << numChannels << " channels, " 
         << (numSamples / samplingRate) << " sec)" << endl;
    
    // now, we are ready to output the file.
    
    // try to open an output stream to filename
    ofstream file;
    file.open(filename.c_str());
    
    //--------------------
    // start of HEADER
    
    file.write(".snd",4); // magic string
    
    WriteIntMsb(file, 28L, 4); // Length of header

    // length of sound (just really long, players stop at end of file)
    WriteIntMsb(file, 0x7FFFFFFFL, 4);
    
    // file format : 16 bit linear, 24 bit linear, 32 bit linear
    WriteIntMsb(file, bits, 4);  
    
    // sampling rate
    WriteIntMsb(file, samplingRate, 4);

    // number of channels
    WriteIntMsb(file, numChannels, 4);

    // padding before the data:
    WriteIntMsb(file, 0, 4); 
    
    // end of HEADER
    //--------------------

    m_sample_type sample;
    m_sample_count_type outOfBounds = 0;
    int value;

    for (m_sample_count_type s=0; s<numSamples; s++)
    {
        for (int c=0; c<numChannels; c++)
        {
            // get the sample:
            sample = (*channels[c])[s];
        
            // check bounds:
            if (sample > 1.0) {sample = 1.0; outOfBounds++;}
            if (sample < -1.0) {sample = -1.0; outOfBounds++;}
            
            // convert the doubles to integer values
			switch (bits) {
			case _16_BIT_LINEAR:
			default:
	            // 16 bit sound (this number is 2^16/2 - 1)
				value = (int) ((sample) * 32767.0);
	            WriteIntMsb(file, value, 2);
				break;
			case _24_BIT_LINEAR:
				value = (int) ((sample) * (double) (1 << 23 - 1));
	            WriteIntMsb(file, value, 3);
				break;
			case _32_BIT_LINEAR:
				value = (int) ((sample) * (double) (2 << 31 - 1));
	            WriteIntMsb(file, value, 4);
				break;
			};

        }
    }

    
    // warn on outOfBounds
    if (outOfBounds > 0)
    {
        m_time_type secondsClipped =
         ((m_time_type)outOfBounds) /
         ((m_time_type)samplingRate);
         
        cerr << "WARNING: AuWriter: "
             << outOfBounds << " samples "
             << "(" << secondsClipped << " seconds) "
             << "contained values that clipped." << endl;
    }
    
    file.close();
    
    return true;
}


//----------------------------------------------------------------------------//
void AuWriter::WriteIntMsb(ostream &out, long l, int size) {
    if (size <= 0) return;
    WriteIntMsb(out, l>>8, size-1); // Write MS Bytes
    out.put(l&255); // Write LS Byte
}
    
//----------------------------------------------------------------------------//
#endif //__AU_WRITER_CPP
