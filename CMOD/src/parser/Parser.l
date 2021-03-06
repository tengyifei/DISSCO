%{
#include "stdio.h"
#include "string.h"
#include "Parser.h"

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

%}

DIGIT [0-9]
float "-"?{DIGIT}*"."?{DIGIT}*
emptystring ["]["]
emptylassiestring [`][`]

%x COMMENT STRING LASSIESTRING
%%

[=] 				{ 
						#ifdef PARSER_DEBUG
						printf("saw equal\n"); 
						#endif
						return EQUALTOK; }
[<]					{ return LBRAC; }
[>]					{ return RBRAC; }
[,]					{ return COMMA; }
{float}				{ 
						#ifdef PARSER_DEBUG
						printf("Saw float\n"); 
						#endif
                                                yylval=(long int)strdup(yytext); return NUMBERTOK; }

{emptystring} { yylval= (long int)strdup(""); return STRTOK; }
{emptylassiestring} { yylval= (long int)strdup(""); return STRTOK; }
["]					BEGIN(STRING);
<STRING>[^"]*		{ 
						#ifdef PARSER_DEBUG
						printf("Saw a string %s\n", yytext);
						#endif
						yylval= (long int)strdup(yytext); return STRTOK; }
<STRING>["]			BEGIN(INITIAL);

[`]					BEGIN(LASSIESTRING);
<LASSIESTRING>[^`]*		{ 
						#ifdef PARSER_DEBUG
						printf("Saw a lassiestring %s\n", yytext);
						#endif
						yylval= (long int)strdup(yytext); return STRTOK; }
<LASSIESTRING>[`]			BEGIN(INITIAL);



[;]					{
						#ifdef PARSER_DEBUG
						printf("Line over\n"); 
						#endif
						return SEMITOK; }
[:]                    	return SCOLON;
[\(]                   	return LPAREN;
[\)]                   	return RPAREN;
[\[]                   	return RBRACE;
[\]]                   	return LBRACE;

title				|
fileFlags			|
fileList			|
soundSynthesis			|
numChannels			|
scoreFlag                       |
sampleRate			|
sampleSize			|
numThreads			|
pieceStartTime			|
pieceDuration  |
LASSIENOTEDEFAULTMODIFIER |
LASSIENOTECUSTOMMODIFIER |
LASSIETITLE |
LASSIEFILEFLAGS |
LASSIEFILELIST |
LASSIEPIECESTARTTIME |
LASSIEPIECEDURATION |
LASSIESOUNDSYNTHESIS |
LASSIENUMCHANNELS |
LASSIESAMPLERITE |
LASSIESAMPLESIZE |
LASSIENUMTHREADS			{ yylval= (long int)strdup(yytext); return GLOBALKEYWORDTOK; 
				  /* Adding Tokens - keywords:
				     To add another keyword insert it above this line.
				     The format is "keyword |" which is a keyword followed by
				     the '|' symbol.  Just follow the format above and you
				     should be fine.  Do NOT insert keywords after the 
				     "  { yylval= ... GLOBALKEYWORDTOK}". This bracketed area must 
				     come after the last keyword in the list. If you wish to remove 
				     the last keyword, change it to some other keyword you are using.
				  */ }

tempo                           |
timeSignature                   |
EDUPerBeat                      |
maxChildDur         |
childNames			|
numChildren			|
childEventDef			|
frequency			|
loudness			|
spatialization			|
reverberation			|
modifiers			|
numPartials			|
deviation			|
spectrum			|
notePitchClass			|
noteDynamicMark			|
noteModifiers         		|
envelopeBuilder        		|
sieveBuilder         		|
patternBuilder    |   
LASSIEeventType |  
LASSIEmaxChildDur |
LASSIEEDUPerBeat |
LASSIEeventName |
LASSIEtimeSignatureEntry1 |
LASSIEtimeSignatureEntry2 |
LASSIEtempoMethodFlag |
LASSIEtempoPrefix |
LASSIEtempoNoteValue |
LASSIEtempoFractionEntry1 |
LASSIEtempoFractionEntry2 | 
LASSIEtempoValueEntry |
LASSIEnumChildrenEntry1 |
LASSIEnumChildrenEntry2 |
LASSIEnumChildrenEntry3 |
LASSIEchildEventDefEntry1 |
LASSIEchildEventDefEntry2 |
LASSIEchildEventDefEntry3 |
LASSIEchildEventDefAttackSieve |
LASSIEchildEventDefDurationSieve |
LASSIEflagNumChildren |
LASSIEflagChildEventDef |
LASSIEflagChildEventDefStartType |
LASSIEflagChildEventDefDurationType |
LASSIEeventLayers |
LASSIEBOTTOMfrequencyFlag |
LASSIEBOTTOMfrequencyContinuumFlag |
LASSIEBOTTOMfrequencyEntry1 |
LASSIEBOTTOMfrequencyEntry2 |
LASSIEBOTTOMloudness |
LASSIEBOTTOMspatialization |
LASSIEBOTTOMreverb |
LASSIEBOTTOMmodifiers |
LASSIESOUNDnumPartials |
LASSIESOUNDdeviation |
LASSIESOUNDspectrum |
LASSIEENV |
LASSIESIV |
LASSIESPA |
LASSIEPAT |
LASSIEREV |
LASSIEFUNCTION { yylval= (long int)strdup(yytext); return EVENTKEYWORDTOK; }

EnvLib				|
ValuePick           		|
ChooseL				|
GetPattern			|
ExpandPattern			|
AdjustPattern			|
Stochos				|
MakeEnvelope			|
MakeSieve			|
MakePattern			|
MakeList			|
ReadENVFile			|
ReadSIVFile			|
ReadPATFile			|
ReadSPAFile			|
ReadREVFile			|
Select				|
Inverse				|
LN				|
Fibonacci			|
Decay				|
Randomizer			|
RandomInt			|
RandomSeed			|
MathExpr |
Random				{ 
						#ifdef PARSER_DEBUG
						printf("saw %s\n", yytext); 
						#endif
						yylval= (long int)strdup(yytext); return FUNCTOK;
				  /* Adding Tokens - functions:
				     See the "Adding Tokens - keywords" help above.
				     This section adds function keywords.
				  */ }

DURATION_UNITS			| /*Deprecated -- use DURATION_EDU*/
DURATION_EDU      |
AVAILABLE_EDU |
CURRENT_LAYER			|
CURRENT_SEGMENT			|
CURRENT_TYPE			|
CURRENT_CHILD_NUM		|
PREVIOUS_CHILD_DURATION |
CURRENT_PARTIAL_NUM		|
CURRENT_DENSITY			{ 
							#ifdef PARSER_DEBUG
							printf("saw %s\n", yytext); 
							#endif
							yylval= (long int)strdup(yytext); return SFUNCTOK;
				  /* These are "static functions" which take no parameters but are
				     evaluated just like a function.  They exists to make the configs
				     look better since these should act like variables.
				  */ }

TRUE					{ yylval=(long int)strdup("1"); return NUMBERTOK; }
FALSE					{ yylval=(long int)strdup("0"); return NUMBERTOK; }

"/*"					BEGIN(COMMENT);
<COMMENT>[^*]*			/* ignore this */;
<COMMENT>"*"+[^*/]		/* ignore this also */;
<COMMENT>"*/"			BEGIN(INITIAL) /* exit comment */;
[\n]				yylloc.first_line++/* ignore me */;
[\t ]+   			/* ignore me */;
%%
