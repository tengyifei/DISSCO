[:]                       	return SCOLON;
[\(]                       	return RPAREN;
[\)]                       	return LPAREN;
[\[]                       	return RBRACE;
[\]]                       	return LBRACE;

duration                	return DURTOK;

density                 	return DENSTOK; 
layer						return LAYERTOK; 
filenames               	return FNTOK; 

Bottom                  	return BOTTOMTOK; 
Sound                   	return SOUNDTOK;
frequency               	return FREQTOK; 
loudness                	return LOUDTOK; 
amplitudeEnvelope       	return AMPETOK; 
frequencyEnvelope 			return FREQETOK;
partials					return PARTTOK; 
transients 					return TRANSTOK;
spatialation				return SPATTOK; 
reverberation				return REVERBTOK; 

Note						return NOTETOK; 
index						return INDEXTOK; 
noteType					return NTYPETOK; 
noteStart					return NSTARTTOK;
noteDuration				return NDURTOK;
noteUPS						return UPSTOK;
noteBL						return BLTOK;
noteFreq					return NFREQTOK; 
noteLoud					return NLOUDTOK;
sequence					return SEQTOK;

fixed						return FIXEDTOK; 
random						return RANDOMTOK; 
sieve						return SIEVTOK;

meaningful					return MEANINGTOK; 
modulus						return MODULUSTOK; 
fake						return FAKETOK; 

