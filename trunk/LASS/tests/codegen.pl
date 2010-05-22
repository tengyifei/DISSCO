#!/usr/bin/perl
print '
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
';
# end preprocessing

$NumSounds = 3;
$MaxPartials = 4;
$MaxPitch = 900;
$MaxStartTime = 500;

# create lots of files
for ($i=0; $i< $NumSounds; $i++) {
     $NumPartials = 1 + int(rand($MaxPartials));
     $pitch = 1 + rand($MaxPitch);

     # create a sound with its partials
     print "Sound s".$i."(".$NumPartials.",".$pitch.");\n";
     for ($j = 0; $j< $NumPartials; $j++) {
          $amplitude = rand(1);
          # set the amplitude for each partial
          print "s$i.get($j).setParam(RELATIVE_AMPLITUDE, $amplitude);\n";
          
     }    

     # give the sound its shape (envelope needs >= 1 point)
     print "LinearInterpolator shape$i;\n";
     $NumShapePoints = 1 + int(rand($MaxShapePoints));
     for ($j = 0; $j < $NumShapePoints; $j++) {
          $time = rand(1);
          $level = rand(1);
          print "shape$i.addEntry($time,$level);\n";
     }
     print "s$i.setPartialParam(WAVE_SHAPE, shape$i);\n\n";

     # set the length of the sound
     $length = 1 + rand($MaxDuration);
     print "s$i.setParam(DURATION, $length);\n\n";

     # set the loudness of the sound
     $volume = 1 + rand($MaxLoudness);
     print "s$i.setParam(LOUDNESS, $volume);\n\n";

     print "\n";

}

print '
// create the score:
Score scor;';

for ($i=0; $i< $NumSounds; $i++) {
    $StartTime = rand($MaxStartTime);
    print "
       s$i.setParam(START_TIME, $StartTime);
       scor.add(s$i);
    ";
}

print '
    // render in one channel, 44000 Hz
    MultiTrack* renderedScore = scor.render(2,44000);

    // write to file
    AuWriter::write(*renderedScore, "randmess.au");

    delete renderedScore;
}
'
