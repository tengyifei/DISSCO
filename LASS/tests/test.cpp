
#include <iostream.h>
#include "SoundSample.h"
#include "Track.h"
#include "Collection.h"
#include "MultiTrack.h"
#include "Iterator.h"
#include "ParameterLib.h"
#include "Partial.h"
#include "AuWriter.h"
#include "Sound.h"
#include "LinearInterpolator.h"
#include "Loudness.h"
#include "Score.h"
#include "Pan.h"

int main (int argc, const char * argv[]) {
Sound s0(1,42.8081426875887);
s0.get(0).setParam(RELATIVE_AMPLITUDE, 0.239749020935175);
LinearInterpolator shape0;
shape0.addEntry(0.985110968479677,0.797883620703928);
s0.setPartialParam(WAVE_SHAPE, shape0);

s0.setParam(DURATION, 1.99761469908693);

s0.setParam(LOUDNESS, 1.66392266940701);


Sound s1(2,65.5867103076625);
s1.get(0).setParam(RELATIVE_AMPLITUDE, 0.580916134423926);
s1.get(1).setParam(RELATIVE_AMPLITUDE, 0.334360887054096);
LinearInterpolator shape1;
shape1.addEntry(0.0140005376540699,0.733714232992948);
s1.setPartialParam(WAVE_SHAPE, shape1);

s1.setParam(DURATION, 1.45254694469633);

s1.setParam(LOUDNESS, 1.44987229790496);


Sound s2(3,32.3141650165601);
s2.get(0).setParam(RELATIVE_AMPLITUDE, 0.148495846734484);
s2.get(1).setParam(RELATIVE_AMPLITUDE, 0.483569876125234);
s2.get(2).setParam(RELATIVE_AMPLITUDE, 0.653363355948883);
LinearInterpolator shape2;
shape2.addEntry(0.717352414252808,0.0125362287946906);
s2.setPartialParam(WAVE_SHAPE, shape2);

s2.setParam(DURATION, 1.53965293575158);

s2.setParam(LOUDNESS, 1.50296362405798);



// create the score:
Score scor;
       s0.setParam(START_TIME, 184.034966466033);
       scor.add(s0);
    
       s1.setParam(START_TIME, 309.339442017427);
       scor.add(s1);
    
       s2.setParam(START_TIME, 207.806658771155);
       scor.add(s2);
    
    // render in one channel, 44000 Hz
    MultiTrack* renderedScore = scor.render(2,44000);

    // write to file
    AuWriter::write(*renderedScore, "randmess.au");

    delete renderedScore;
}
