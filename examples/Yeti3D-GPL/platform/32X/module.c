/*
  32X MOD support code by Chilly Willy, based on

  Ami-PlayMOD_V1_0_20090417.c   (c) 2009 Massimiliano Scarano   mscarano@libero.it

  which had the following restriction:
    If you use the source code, you should always mention me as the original author.
*/


/* Includes ==================================================================================== */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "32x.h"
#include "hw_32x.h"
#include "module.h"
#include "sound.h"


/* Defines ===================================================================================== */

/* define if handle 6 or 8 channel mods */
#define HANDLE_EXTRA_CHANNELS
/* define if handle panning */
//#define HANDLE_PANNING
/* define if want a little cross-talk between sides (only if no panning) */
//#define CROSS_TALK 3


/* Function prototypes ========================================================================= */

/*
  Called by sound sub-system at 50 Hz
    returns 1 if not playing
*/
uint16_t PlayMOD(struct ModTag* Mod_p);

static void UpdateNote(struct ModTag* Mod_p, uint32_t* Div_p);
static void UpdateEffect(struct ModTag* Mod_p);
static void DoVibrato(struct ModTag* Mod_p, uint8_t Channel);
static void DoTremolo(struct ModTag* Mod_p, uint8_t Channel);
static void DoPorta(struct ModTag* Mod_p, uint8_t Channel);
static uint16_t AmigaPeriod2Hz(int16_t PeriodFrequency);

/*
  System specific functions
*/
static void SetBeat(struct ModTag* Mod_p, uint8_t bpm);
static struct ModTag* SetMOD(struct ModTag* Mod_p);
static void AllocChannel(uint8_t Channel);
static void FreeChannel(uint8_t Channel);
static void SetSample(uint16_t Channel, int8_t* SampleBuff_p,
               uint32_t SampleOffset, uint32_t SampleLength,
               uint32_t LoopStart, uint32_t LoopLength);
static void SetFrequency(uint16_t Channel, uint32_t Freq);
static void SetVolume(uint16_t Channel, uint32_t Volume, int32_t Pan);


/* Global variables ============================================================================ */

/*
  From Amiga ProTracker playroutine source code

  - Amiga PERIODS represent delay times before fetching the next sample (in units of system clock ticks @ ~3.5 MHz)
*/
static const uint16_t AmigaPeriodsTable[ MOD_FINE_TUNE_VALUES ][ MOD_NUMBER_OF_NOTES ] =
{
/* ; Tuning 0, Normal */
  { 856,808,762,720,678,640,604,570,538,508,480,453,   /* ; C-1 to B-1 */
    428,404,381,360,339,320,302,285,269,254,240,226,   /* ; C-2 to B-2 */
    214,202,190,180,170,160,151,143,135,127,120,113 }, /* ; C-3 to B-3 */
/* ; Tuning 1 */
  { 850,802,757,715,674,637,601,567,535,505,477,450,   /* ; same as above */
    425,401,379,357,337,318,300,284,268,253,239,225,   /* ; but with */
    213,201,189,179,169,159,150,142,134,126,119,113 }, /* ; finetune +1 */
/* ; Tuning 2 */
  { 844,796,752,709,670,632,597,563,532,502,474,447,   /* ; etc, */
    422,398,376,355,335,316,298,282,266,251,237,224,   /* ; finetune +2 */
    211,199,188,177,167,158,149,141,133,125,118,112 },
/* ; Tuning 3 */
  { 838,791,746,704,665,628,592,559,528,498,470,444,
    419,395,373,352,332,314,296,280,264,249,235,222,
    209,198,187,176,166,157,148,140,132,125,118,111 },
/* ; Tuning 4 */
  { 832,785,741,699,660,623,588,555,524,495,467,441,
    416,392,370,350,330,312,294,278,262,247,233,220,
    208,196,185,175,165,156,147,139,131,124,117,110 },
/* ; Tuning 5 */
  { 826,779,736,694,655,619,584,551,520,491,463,437,
    413,390,368,347,328,309,292,276,260,245,232,219,
    206,195,184,174,164,155,146,138,130,123,116,109 },
/* ; Tuning 6 */
  { 820,774,730,689,651,614,580,547,516,487,460,434,
    410,387,365,345,325,307,290,274,258,244,230,217,
    205,193,183,172,163,154,145,137,129,122,115,109 },
/* ; Tuning 7 */
  { 814,768,725,684,646,610,575,543,513,484,457,431,
    407,384,363,342,323,305,288,272,256,242,228,216,
    204,192,181,171,161,152,144,136,128,121,114,108 },
/* ; Tuning -8 */
  { 907,856,808,762,720,678,640,604,570,538,508,480,
    453,428,404,381,360,339,320,302,285,269,254,240,
    226,214,202,190,180,170,160,151,143,135,127,120 },
/* ; Tuning -7 */
  { 900,850,802,757,715,675,636,601,567,535,505,477,
    450,425,401,379,357,337,318,300,284,268,253,238,
    225,212,200,189,179,169,159,150,142,134,126,119 },
/* ; Tuning -6 */
  { 894,844,796,752,709,670,632,597,563,532,502,474,
    447,422,398,376,355,335,316,298,282,266,251,237,
    223,211,199,188,177,167,158,149,141,133,125,118 },
/* ; Tuning -5 */
  { 887,838,791,746,704,665,628,592,559,528,498,470,
    444,419,395,373,352,332,314,296,280,264,249,235,
    222,209,198,187,176,166,157,148,140,132,125,118 },
/* ; Tuning -4 */
  { 881,832,785,741,699,660,623,588,555,524,494,467,
    441,416,392,370,350,330,312,294,278,262,247,233,
    220,208,196,185,175,165,156,147,139,131,123,117 },
/* ; Tuning -3 */
  { 875,826,779,736,694,655,619,584,551,520,491,463,
    437,413,390,368,347,328,309,292,276,260,245,232,
    219,206,195,184,174,164,155,146,138,130,123,116 },
/* ; Tuning -2 */
  { 868,820,774,730,689,651,614,580,547,516,487,460,
    434,410,387,365,345,325,307,290,274,258,244,230,
    217,205,193,183,172,163,154,145,137,129,122,115 },
/* ; Tuning -1 */
  { 862,814,768,725,684,646,610,575,543,513,484,457,
    431,407,384,363,342,323,305,288,272,256,242,228,
    216,203,192,181,171,161,152,144,136,128,121,114 }
};


/*
  From Amiga ProTracker playroutine source code

  for Vibrato and Tremolo
*/
static const uint8_t SineTable[ 32 ] =
{
  0, 24, 49, 74, 97, 120, 141, 161,
  180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197,
  180, 161, 141, 120, 97, 74, 49, 24
};


/*
  Global volume for playing Modules
*/
static uint8_t gVolume = 16;


/* Module handler ============================================================================== */

/*
   Initialize Module structure based on data in memory.
*/
uint8_t InitMOD(uint8_t* Data_p, struct ModTag* Mod_p)
{
  const uint8_t ModSignature[ MOD_SIGNATURE_LENGTH ] = {'M', '.', 'K', '.'};
  const uint8_t AltSignature[ MOD_SIGNATURE_LENGTH ] = {'4', 'C', 'H', 'N'};
#ifdef HANDLE_EXTRA_CHANNELS
  const uint8_t Mod6Signature[ MOD_SIGNATURE_LENGTH ] = {'6', 'C', 'H', 'N'};
  const uint8_t Mod8Signature[ MOD_SIGNATURE_LENGTH ] = {'8', 'C', 'H', 'N'};
#endif
  uint8_t Cnt;
  uint8_t Sample;
  uint8_t Order;
  uint32_t Soffset;

  memset( Mod_p, 0, sizeof (struct ModTag) ); /* main data structure set to 0 */

  /* check signature */
  for ( Cnt = 0; Cnt < MOD_SIGNATURE_LENGTH; Cnt++ )
  {
    if ( ModSignature[ Cnt ] != Data_p[ Cnt + MOD_SIGNATURE_OFFSET ] )
    {
      break; /* no match */
    }
  }
  if ( MOD_SIGNATURE_LENGTH == Cnt )
  {
    Mod_p->NumberOfChannels = 4; /* we have a 4 channel mod */
  }

  if ( !Mod_p->NumberOfChannels )
  {
    /* check for alternate 4 channel signature */
    for ( Cnt = 0; Cnt < MOD_SIGNATURE_LENGTH; Cnt++ )
    {
      if ( AltSignature[ Cnt ] != Data_p[ Cnt + MOD_SIGNATURE_OFFSET ] )
      {
        break; /* no match */
      }
    }
    if ( MOD_SIGNATURE_LENGTH == Cnt )
    {
      Mod_p->NumberOfChannels = 4; /* we have a 4 channel mod */
    }
  }

#ifdef HANDLE_EXTRA_CHANNELS
  if ( !Mod_p->NumberOfChannels )
  {
    /* check for 6 channel signature */
    for ( Cnt = 0; Cnt < MOD_SIGNATURE_LENGTH; Cnt++ )
    {
      if ( Mod6Signature[ Cnt ] != Data_p[ Cnt + MOD_SIGNATURE_OFFSET ] )
      {
        break; /* no match */
      }
    }
    if ( MOD_SIGNATURE_LENGTH == Cnt )
    {
      Mod_p->NumberOfChannels = 6; /* we have a 6 channel mod */
    }
  }

  if ( !Mod_p->NumberOfChannels )
  {
    /* check for 8 channel signature */
    for ( Cnt = 0; Cnt < MOD_SIGNATURE_LENGTH; Cnt++ )
    {
      if ( Mod8Signature[ Cnt ] != Data_p[ Cnt + MOD_SIGNATURE_OFFSET ] )
      {
        break; /* no match */
      }
    }
    if ( MOD_SIGNATURE_LENGTH == Cnt )
    {
      Mod_p->NumberOfChannels = 8; /* we have a 8 channel mod */
    }
  }
#endif

  if ( !Mod_p->NumberOfChannels )
  {
    return 1; /* not recognized, or not supported number of channels */
  }

  for ( Sample = 0; Sample < MOD_NUMBER_OF_SAMPLES; Sample++ )
  {
    uint8_t Byte1, Byte2;

    Byte1 = Data_p[ 20 + 22 + 30 * Sample ];
    Byte2 = Data_p[ 20 + 23 + 30 * Sample ];
    Mod_p->Inst[ Sample ].SampleLength = ( (Byte1 * 0x0100) + Byte2 ) * 2;

    Mod_p->Inst[ Sample ].FineTune = Data_p[ 20 + 24 + 30 * Sample ];

    Mod_p->Inst[ Sample ].Volume = Data_p[ 20 + 25 + 30 * Sample ];

    Byte1 = Data_p[ 20 + 26 + 30 * Sample ];
    Byte2 = Data_p[ 20 + 27 + 30 * Sample ];
    Mod_p->Inst[ Sample ].LoopStart = ( (Byte1 * 0x0100) + Byte2 ) * 2;

    Byte1 = Data_p[ 20 + 28 + 30 * Sample ];
    Byte2 = Data_p[ 20 + 29 + 30 * Sample ];
    Mod_p->Inst[ Sample ].LoopLength = ( (Byte1 * 0x0100) + Byte2 ) * 2;
  } /* for Sample */

  Mod_p->SongLength = Data_p[ 950 ]; /* this is the number of orders in a song */

  /* get NumberOfPatterns */
  Mod_p->NumberOfPatterns = 0;
  for ( Order = 0; Order < MOD_NUMBER_OF_ORDERS; Order++ )
  {
    Mod_p->Orders[ Order ] = Data_p[ 952 + Order ];
    if ( Mod_p->Orders[ Order ] > Mod_p->NumberOfPatterns )
    {
      Mod_p->NumberOfPatterns = Mod_p->Orders[ Order ];
    }
  } /* for Order */

  Mod_p->NumberOfPatterns += 1; /* [ 0 ; Mod_p->NumberOfPatterns ] */

  Mod_p->PatternsBuff_p = (uint32_t*)&Data_p[ 1084 ]; /* Pattern data starts here */

  for ( Sample = 0, Soffset = 0; Sample < MOD_NUMBER_OF_SAMPLES; Sample++ )
  {
    if ( Mod_p->Inst[ Sample ].SampleLength > 0 )
    {
      /* pointer to sample data */
      Mod_p->Inst[ Sample ].SampleBuff_p = (int8_t*)&Data_p[ 1084 + Mod_p->NumberOfPatterns * MOD_ROWS_PER_CHANNEL * Mod_p->NumberOfChannels * 4 + Soffset ];
      Soffset += Mod_p->Inst[ Sample ].SampleLength;
    } /* if */
  } /* for Sample */

  Mod_p->Speed = 6; /* default speed */
  Mod_p->Tick = 6;  /* force immediate processing */
  Mod_p->Order = 0; /* default order */
  Mod_p->Row = 0;   /* default row */
  SetBeat(Mod_p, 125);
  Mod_p->IsPlaying = 0;

  /* allocate sound channels with system specific routine */
  for ( Cnt = 0; Cnt < Mod_p->NumberOfChannels; Cnt++ )
    AllocChannel( Cnt );

  return 0; /* no errors */
}


/*
  Cleanup MOD - stop playing and release voices
*/
void ExitMOD(struct ModTag* Mod_p)
{
  uint8_t Channel;

  if (Mod_p)
  {
    if (Mod_p->IsPlaying)
      StopMOD(Mod_p);

    /* release sound channels */
    for ( Channel = 0; Channel < Mod_p->NumberOfChannels; Channel++ )
    {
      FreeChannel( Channel );
    }
    /* make sure struct cannot be reused without init */
    memset(Mod_p, 0, sizeof(struct ModTag));
  }
}


/*
  Start playing Module
    loop = bool = TRUE = loop forever, = FALSE = play once
    returns the previously set Module, if any
*/
struct ModTag* StartMOD(struct ModTag* Mod_p, uint8_t loop)
{
  if (Mod_p)
  {
    Mod_p->IsPlaying = loop ? 0x81 : 1;
    return SetMOD(Mod_p);
  }
  return (struct ModTag*)NULL;
}


/*
  Pause/resume Module
    pause = bool = TRUE = pause, = FALSE = resume
*/
void PauseMOD(struct ModTag* Mod_p, uint8_t pause)
{
  if (Mod_p)
  {
    if ( pause )
      Mod_p->IsPlaying &= 0xFE;
    else
      Mod_p->IsPlaying |= 1;
  }
}


/*
  Stop playing Module
*/
void StopMOD(struct ModTag* Mod_p)
{
  if (Mod_p)
  {
    Mod_p->IsPlaying = 0;
    SetMOD(0);
  }
}


/*
  Wait for Module to play through once
*/
void WaitMOD(struct ModTag* Mod_p)
{
  do
  {
    CacheClearLine(Mod_p);
    if (!Mod_p->IsPlaying)
      break;
    if (Mod_p->IsPlaying & 0x80)
      if (!Mod_p->Row && !Mod_p->Order)
        break;
  } while (1);
}


/*
  Check Module play status
    returns 0 if done playing
*/
uint8_t CheckMOD(struct ModTag* Mod_p)
{
  CacheClearLine(Mod_p);
  if (Mod_p->IsPlaying & 0x80)
    return (!Mod_p->Row && !Mod_p->Order) ? 0 : 1;
  return Mod_p->IsPlaying;
}


/*
  Set global volume
    vol = 0 = off, = 16 = max
    returns the previous volume setting
*/
uint8_t VolumeMOD(uint8_t vol)
{
  uint8_t prev = gVolume;
  gVolume = vol > 16 ? 16: vol;
  return prev;
}


/*
+----------------------------------------------------------------------------+
|                           === Playing the MOD ===                          |
+----------------------------------------------------------------------------+

  timer function, it contains the player logic, it has to be as fast as possible

  The SPEED of a song is the base on how your mod is played.
  Each row of a pattern is updated every SPEED number of clock ticks.
*/
uint16_t PlayMOD(struct ModTag* Mod_p)
{
  Mod_p->Pattern = Mod_p->Orders[ Mod_p->Order ]; /* pattern for current order */

  Mod_p->Tick++;

  if (Mod_p->Tick >= Mod_p->Speed)
  {
    if ( !Mod_p->PatternDelay ) /* if there is no pattern delay */
    {
      UpdateNote( Mod_p, &Mod_p->PatternsBuff_p[ Mod_p->Pattern * MOD_ROWS_PER_CHANNEL * Mod_p->NumberOfChannels + Mod_p->Row * Mod_p->NumberOfChannels ] ); /* process 1 note for every channel */
      Mod_p->Row += 1; /* increment row */
    }
    else
    {
      Mod_p->PatternDelay--; /* else decrement pattern delay */
    }

    Mod_p->Tick = 0;

    if ( Mod_p->Row >= MOD_ROWS_PER_CHANNEL )
    {
      /* process next pattern */
      Mod_p->Order++;
      if (Mod_p->Order >= Mod_p->SongLength)
      {
        /* finished playing the mod */
        if (Mod_p->IsPlaying & 0x80)
        {
          /* loop - reset to defaults */
          Mod_p->Speed = 6;
          Mod_p->Tick = 6;  /* force immediate processing */
          Mod_p->Order = 0;
          Mod_p->Row = 0;
          SetBeat(Mod_p, 125);
        }
        else
        {
          Mod_p->IsPlaying = 0; /* song done playing */
        }
      }
      else
      {
        Mod_p->Pattern = Mod_p->Orders[ Mod_p->Order ];
        Mod_p->Row = 0;
      }
    }
  }
  else
  {
    Mod_p->Row -= 1; /* decrement row */
    /* update the tick based effects */
    UpdateEffect( Mod_p );
    Mod_p->Row += 1; /* increment row */
  }

  return Mod_p->IsPlaying ? 0 : 1;
}


/*
  process division - 1 note for every channel
*/
static void UpdateNote(struct ModTag* Mod_p, uint32_t* Div_p)
{
  uint8_t Channel; /* counter */
  uint8_t EParX; /* Effect Parameter X */
  uint8_t EParY; /* Effect Parameter Y */
  uint8_t JumpFlag, BreakFlag; /* boolean flags for effects JUMP_TO_PATTERN and PATTERN_BREAK on the same row */
  uint8_t Cnt;

  JumpFlag = FALSE;
  BreakFlag = FALSE;

  for ( Channel = 0; Channel < Mod_p->NumberOfChannels; Channel++ )
  {
    uint32_t division;

    /* Amiga-fied mess ;-) */
    division = Div_p[ Channel ];
/*
+-------------------------------------+
| Byte 0    Byte 1   Byte 2   Byte 3  |
|-------------------------------------|
|aaaaBBBB CCCCCCCCC DDDDeeee FFFFFFFFF|
+-------------------------------------+
aaaaDDDD     = sample number
BBBBCCCCCCCC = sample period value
eeee         = effect number
FFFFFFFF     = effect parameters
*/
    Mod_p->Notes[ Channel ].SampleNumber = ((division >> 24) & 0x00F0) | ((division >> 12) & 0x000F);
    Mod_p->Notes[ Channel ].PeriodFrequency = ((division >> 16) & 0x0FFF);
    Mod_p->Notes[ Channel ].EffectNumber = (division >> 8) & 0x000F;
    Mod_p->Notes[ Channel ].EffectParameter = division & 0x00FF;
    Mod_p->Notes[ Channel ].NoteExists = FALSE;

    /* look up table here */
    for ( Cnt = 0; Cnt < MOD_NUMBER_OF_NOTES; Cnt++ )
    {
      if ( AmigaPeriodsTable[ 0 ][ Cnt ] == Mod_p->Notes[ Channel ].PeriodFrequency )
      {
        Mod_p->Notes[ Channel ].NoteExists = TRUE;
        Mod_p->Notes[ Channel ].PeriodFrequencyCol = Cnt;
        break;
      }
    } /* for Cnt */

    EParX = Mod_p->Notes[ Channel ].EffectParameter >> 4;
    EParY = Mod_p->Notes[ Channel ].EffectParameter & 0xF;

    Mod_p->Channels[ Channel ].SampleOffset = 2; // first word overwritten by many trackers

    if ( Mod_p->Notes[ Channel ].SampleNumber > 0 )
    {
      Mod_p->Channels[ Channel ].LastInstrument = Mod_p->Notes[ Channel ].SampleNumber - 1;

      if ( !( ( Mod_p->Notes[ Channel ].EffectNumber == EFFECT_EXTENDED_EFFECTS ) && ( EParX == EFFECT_DELAY_NOTE ) ) )
      {
        Mod_p->Channels[ Channel ].Volume = Mod_p->Inst[ Mod_p->Notes[ Channel ].SampleNumber - 1 ].Volume; /* default volume of sample */
      }
    }

    if ( Mod_p->Notes[ Channel ].NoteExists == TRUE )
    {
      if ( ( Mod_p->Channels[ Channel ].WaveformControl & 0x0F ) < 4 )
      {
        Mod_p->Channels[ Channel ].VibratoPosition = 0;
      }
      if ( ( Mod_p->Channels[ Channel ].WaveformControl >> 4 ) < 4 )
      {
        Mod_p->Channels[ Channel ].TremoloPosition = 0;
      }

      /* if not a porta effect, then set the channels frequency to the looked up amiga value + or - any finetune */
      if ( (Mod_p->Notes[ Channel ].EffectNumber != EFFECT_PORTA_TO_NOTE) &&
           (Mod_p->Notes[ Channel ].EffectNumber != EFFECT_PORTA_PLUS_VOL_SLIDE) )
      {
        uint8_t FineTune;

        FineTune = Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].FineTune;
        Mod_p->Channels[ Channel ].PeriodFrequency = AmigaPeriodsTable[ FineTune ][ Mod_p->Notes[ Channel ].PeriodFrequencyCol ];
      } /* if */

      //Mod_p->Channels[ Channel ].SampleOffset = 2;
    } /* if */

    /* skip effects ? */
    if ( !( ( Mod_p->Notes[ Channel ].EffectNumber == 0 ) && ( Mod_p->Notes[ Channel ].EffectParameter == 0 ) ) )
    {
      /* process the non tick based effects also grab parameters for tick based effects */
      switch ( Mod_p->Notes[ Channel ].EffectNumber )
      {
        case EFFECT_ARPEGGIO:   /* not processed on tick 0 */
        case EFFECT_PORTA_UP:   /* not processed on tick 0 */
        case EFFECT_PORTA_DOWN: /* not processed on tick 0 */
          break;

        case EFFECT_PORTA_TO_NOTE: /* reviewed OK */
          if ( Mod_p->Notes[ Channel ].EffectParameter > 0 )
          {
            Mod_p->Channels[ Channel ].PortaSpeed = Mod_p->Notes[ Channel ].EffectParameter;
          }
          /* no break statement here */

        case EFFECT_PORTA_PLUS_VOL_SLIDE: /* reviewed OK */
          if ( Mod_p->Notes[ Channel ].NoteExists == TRUE )
          {
            Mod_p->Channels[ Channel ].PortaTo = Mod_p->Notes[ Channel ].PeriodFrequency;
          }
          break;

        case EFFECT_VIBRATO: /* reviewed OK */
          if ( EParX > 0 )
          {
            Mod_p->Channels[ Channel ].VibratoSpeed = EParX;
          }
          if ( EParY > 0 )
          {
            Mod_p->Channels[ Channel ].VibratoDepth = EParY;
          }
          break;

        case EFFECT_VIBRATO_PLUS_VOL_SLIDE: /* not processed on tick 0 */
          break;

        case EFFECT_TREMOLO: /* reviewed OK */
          if ( EParX > 0 )
          {
            Mod_p->Channels[ Channel ].TremoloSpeed = EParX;
          }
          if ( EParY > 0 )
          {
            Mod_p->Channels[ Channel ].TremoloDepth = EParY;
          }
          break;

        case EFFECT_PAN: /* reviewed OK */ /* not a ProTracker effect */
          Mod_p->Channels[ Channel ].PanValue = Mod_p->Notes[ Channel ].EffectParameter;
          break;

        case EFFECT_SAMPLE_OFFSET: /* reviewed OK */
          if ( Mod_p->Notes[ Channel ].EffectParameter > 0 )
          {
            Mod_p->Channels[ Channel ].SampleOffset = ( Mod_p->Notes[ Channel ].EffectParameter << 8 );
          }
          if ( Mod_p->Channels[ Channel ].SampleOffset > Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleLength )
          {
            Mod_p->Channels[ Channel ].SampleOffset = Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleLength;
            /* this will result in silence */
          }
          break;

       case EFFECT_VOLUME_SLIDE: /* not processed on tick 0 */
         break;

        case EFFECT_JUMP_TO_PATTERN: /* reviewed OK */
          Mod_p->Order = Mod_p->Notes[ Channel ].EffectParameter;
          Mod_p->Row = -1;
          if ( Mod_p->Order >= Mod_p->SongLength )
          {
            Mod_p->Order = 0;
          }
//        if ( Mod_p->Order == 0 )
//        {
            /* loop the mod */
            if (!(Mod_p->IsPlaying & 0x80))
            {
              Mod_p->IsPlaying = 0; /* song done playing */
              return;
            }
//        }
          JumpFlag = TRUE;
          break;

        case EFFECT_SET_VOLUME: /* reviewed OK */
          Mod_p->Channels[ Channel ].Volume = Mod_p->Notes[ Channel ].EffectParameter;
          if ( Mod_p->Channels[ Channel ].Volume < 0 )
          {
            Mod_p->Channels[ Channel ].Volume = 0;
          }
          if ( Mod_p->Channels[ Channel ].Volume > 64 )
          {
            Mod_p->Channels[ Channel ].Volume = 64;
          }
          break;

        case EFFECT_PATTERN_BREAK: /* reviewed OK */
          Mod_p->Row = ( EParX * 10 ) + EParY; /* get row number */
          if ( Mod_p->Row == 0 )
          {
            Mod_p->Row = -1;
          }
          else if ( Mod_p->Row >= MOD_ROWS_PER_CHANNEL )
          {
            Mod_p->Row = -1;
          }
          else
          {
            Mod_p->Row -= 1;
          }

          if ( ! JumpFlag && ! BreakFlag )
          {
            Mod_p->Order++;

            if ( Mod_p->Order >= Mod_p->SongLength )
            {
              Mod_p->Order = 0;
              /* repeat the mod */
              if (!(Mod_p->IsPlaying & 0x80))
              {
                Mod_p->IsPlaying = 0; /* song done playing */
                return;
              }
            }
          }
          BreakFlag = TRUE;
          break;

        case EFFECT_EXTENDED_EFFECTS:
          switch (EParX)
          {
            case EFFECT_FINE_PORTA_UP: /* reviewed OK */
              Mod_p->Channels[ Channel ].PeriodFrequency -= EParY;
              break;

            case EFFECT_FINE_PORTA_DOWN: /* reviewed OK */
              Mod_p->Channels[ Channel ].PeriodFrequency += EParY;
              break;

            case EFFECT_GLISSANDO_CONTROL: /* not implemented not even in fmoddoc2 */
              break;

            case EFFECT_SET_VIBRATO_WAVEFORM: /* reviewed OK */
              Mod_p->Channels[ Channel ].WaveformControl &= 0xF0;
              Mod_p->Channels[ Channel ].WaveformControl |= EParY;
              break;

            case EFFECT_SET_FINETUNE: /* reviewed OK */
              Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].FineTune = EParY;
              break;

            case EFFECT_PATTERN_LOOP: /* reviewed OK */
              if ( EParY == 0 )
              {
                Mod_p->Channels[ Channel ].PatLoopRow = Mod_p->Row;
              }
              else
              {
                if ( ! Mod_p->Channels[ Channel ].PatLoopNo )
                {
                  Mod_p->Channels[ Channel ].PatLoopNo = EParY;
                }
                else
                {
                  Mod_p->Channels[ Channel ].PatLoopNo--;
                }
                if ( Mod_p->Channels[ Channel ].PatLoopNo )
                {
                  Mod_p->Row = Mod_p->Channels[ Channel ].PatLoopRow - 1;
                }
              }
              break;

            case EFFECT_SET_TREMOLO_WAVEFORM: /* reviewed OK */
              Mod_p->Channels[ Channel ].WaveformControl &= 0x0F;
              Mod_p->Channels[ Channel ].WaveformControl |= (EParY << 4);
              break;

            case EFFECT_POSITION_PANNING: /* reviewed OK */
              Mod_p->Channels[ Channel ].PanValue = EParY;
              break;

            case EFFECT_RETRIG_NOTE: /* not processed on tick 0 */
              break;

            case EFFECT_FINE_VOLUME_SLIDE_UP: /* reviewed OK */
              Mod_p->Channels[ Channel ].Volume += EParY;
              if ( Mod_p->Channels[ Channel ].Volume > 64 )
              {
                Mod_p->Channels[ Channel ].Volume = 64;
              }
              break;

            case EFFECT_FINE_VOLUME_SLIDE_DOWN: /* reviewed OK */
              Mod_p->Channels[ Channel ].Volume -= EParY;
              if ( Mod_p->Channels[ Channel ].Volume < 0 )
              {
                Mod_p->Channels[ Channel ].Volume = 0;
              }
              break;

            case EFFECT_CUT_NOTE: /* not processed on tick 0 */
              break;

            case EFFECT_DELAY_NOTE: /* not processed on tick 0 */ /* reviewed OK */
              continue; /* process next note */
              break;

            case EFFECT_PATTERN_DELAY: /* reviewed OK */
              Mod_p->PatternDelay = EParY;
              break;

            default:
              break;
          }
          break;

        case EFFECT_SET_SPEED: /* cw - update for bpm  */
          if ( Mod_p->Notes[ Channel ].EffectParameter <= 0x20 )
          {
            if ( Mod_p->Notes[ Channel ].EffectParameter )
            {
              Mod_p->Speed = Mod_p->Notes[ Channel ].EffectParameter;
            }
          }
          else
          {
            SetBeat(Mod_p, Mod_p->Notes[ Channel ].EffectParameter);
          }
          break;

        default:
          /* effect not defined */
          break;
      } /* switch */
    } /* if */

    if ( Mod_p->Notes[ Channel ].EffectNumber != EFFECT_SET_TREMOLO_WAVEFORM )
    {
      /* set volume */
      SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
    }

    if ( Mod_p->Channels[ Channel ].PeriodFrequency > 0 )
    {
      /* set frequency */
      SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );
    }

    if ( Mod_p->Notes[ Channel ].NoteExists == TRUE )
    {
      if (   ( Mod_p->Notes[ Channel ].EffectNumber != EFFECT_PORTA_TO_NOTE ) &&
             ( Mod_p->Notes[ Channel ].EffectNumber != EFFECT_PORTA_PLUS_VOL_SLIDE )   )
      {
        /* set sample */
        SetSample( Channel,
                   Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleBuff_p,
                   Mod_p->Channels[ Channel ].SampleOffset,
                   Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleLength,
                   Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopStart,
                   Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopLength );
      }
    } /* if */
  } /* for Channel */

  return;
}


/*
  update tick based effects
*/
static void UpdateEffect(struct ModTag* Mod_p)
{
  uint8_t Channel; /* counter */
  uint8_t EParX; /* Effect Parameter X */
  uint8_t EParY; /* Effect Parameter Y */
  uint8_t Tick = Mod_p->Tick; /* current music tick */

  for ( Channel = 0; Channel < Mod_p->NumberOfChannels; Channel++ )
  {
    EParX = Mod_p->Notes[ Channel ].EffectParameter >> 4;
    EParY = Mod_p->Notes[ Channel ].EffectParameter & 0xF;

    switch ( Mod_p->Notes[ Channel ].EffectNumber )
    {

      case EFFECT_ARPEGGIO: /* effect used in chip tunes */ /* reviewed OK */
        if ( Mod_p->Notes[ Channel ].EffectParameter > 0 )
        {
          switch ( Tick % 3 )
          {
            case 0:
              SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );
              break;

            case 1:
              SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency + EParX ) );
              break;

            case 2:
              SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency + EParY ) );
              break;
          } /* switch */
        } /* if */
        break;

      case EFFECT_PORTA_UP: /* reviewed OK */
        Mod_p->Channels[ Channel ].PeriodFrequency -= Mod_p->Notes[ Channel ].EffectParameter;
        if ( Mod_p->Channels[ Channel ].PeriodFrequency < 113 )
        {
          Mod_p->Channels[ Channel ].PeriodFrequency = 113; /* stop at B-3 */
        }
        SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );
        break;

      case EFFECT_PORTA_DOWN: /* reviewed OK */
        Mod_p->Channels[ Channel ].PeriodFrequency += Mod_p->Notes[ Channel ].EffectParameter;
        if ( Mod_p->Channels[ Channel ].PeriodFrequency > 856 )
        {
          Mod_p->Channels[ Channel ].PeriodFrequency = 856; /* stop at C-1 */
        }
        SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );
        break;

      case EFFECT_PORTA_TO_NOTE: /* reviewed OK */
        DoPorta( Mod_p, Channel );
        break;

      case EFFECT_VIBRATO: /* reviewed OK */
        DoVibrato( Mod_p, Channel );
        break;

      case EFFECT_PORTA_PLUS_VOL_SLIDE: /* reviewed OK */
        DoPorta( Mod_p, Channel );

        if ( EParX > 0 )
        {
          Mod_p->Channels[ Channel ].Volume += EParX;
        }
        if ( EParY > 0 )
        {
          Mod_p->Channels[ Channel ].Volume -= EParY;
        }
        if ( Mod_p->Channels[ Channel ].Volume < 0 )
        {
          Mod_p->Channels[ Channel ].Volume = 0;
        }
        if ( Mod_p->Channels[ Channel ].Volume > 64 )
        {
          Mod_p->Channels[ Channel ].Volume = 64;
        }
        SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
        break;

      case EFFECT_VIBRATO_PLUS_VOL_SLIDE: /* reviewed OK */
        DoVibrato( Mod_p, Channel );

        if ( EParX > 0 )
        {
          Mod_p->Channels[ Channel ].Volume += EParX;
        }
        if ( EParY > 0 )
        {
          Mod_p->Channels[ Channel ].Volume -= EParY;
        }
        if ( Mod_p->Channels[ Channel ].Volume < 0 )
        {
          Mod_p->Channels[ Channel ].Volume = 0;
        }
        if ( Mod_p->Channels[ Channel ].Volume > 64 )
        {
          Mod_p->Channels[ Channel ].Volume = 64;
        }
        SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
        break;

      case EFFECT_TREMOLO: /* reviewed OK */
        DoTremolo( Mod_p, Channel );
        break;

      case EFFECT_VOLUME_SLIDE: /* reviewed OK */
        if ( EParX > 0 )
        {
          Mod_p->Channels[ Channel ].Volume += EParX;
        }
        if ( EParY > 0 )
        {
          Mod_p->Channels[ Channel ].Volume -= EParY;
        }
        if ( Mod_p->Channels[ Channel ].Volume < 0 )
        {
          Mod_p->Channels[ Channel ].Volume = 0;
        }
        if ( Mod_p->Channels[ Channel ].Volume > 64 )
        {
          Mod_p->Channels[ Channel ].Volume = 64;
        }
        SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
        break;

      case EFFECT_EXTENDED_EFFECTS:
        switch (EParX)
        {
          case EFFECT_RETRIG_NOTE: /* I am not 100% sure about this effect */
            if ( !EParY )
            {
              break;
            }
            if ( !( Tick % EParY ) )
            {
              SetSample( Channel,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleBuff_p,
                         Mod_p->Channels[ Channel ].SampleOffset,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleLength,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopStart,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopLength );
            }
            break;

          case EFFECT_CUT_NOTE: /* reviewed OK */
            if ( Tick == EParY )
            {
              Mod_p->Channels[ Channel ].Volume = 0;
              SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
            }
            break;

          case EFFECT_DELAY_NOTE: /* I am not 100% sure about this effect */
            if ( Tick == EParY )
            {
              if ( Mod_p->Notes[ Channel ].SampleNumber > 0 )
              {
                Mod_p->Channels[ Channel ].Volume = Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].Volume;
              }
              if ( Mod_p->Inst[ Mod_p->Notes[ Channel ].SampleNumber - 1 ].Volume <= 0x40 )
              {
                Mod_p->Channels[ Channel ].Volume = Mod_p->Inst[ Mod_p->Notes[ Channel ].SampleNumber - 1 ].Volume;
              }

              SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );
              SetVolume( Channel, (Mod_p->Channels[ Channel ].Volume << 10), Mod_p->Channels[ Channel ].PanValue );
              SetSample( Channel,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleBuff_p,
                         Mod_p->Channels[ Channel ].SampleOffset,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].SampleLength,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopStart,
                         Mod_p->Inst[ Mod_p->Channels[ Channel ].LastInstrument ].LoopLength );
            } /* if */
            break;

          default:
            break;
        }
        break;

      default:
        break;
    } /* switch */
  } /* for Channel */

  return;
}


/*
*/
static void DoVibrato(struct ModTag* Mod_p, uint8_t Channel)
{
  uint16_t Delta = 0;
  uint8_t Temp;

  Temp = ( Mod_p->Channels[ Channel ].VibratoPosition & 31 ); /* Temp will be the index */

  switch ( Mod_p->Channels[ Channel ].WaveformControl & 3 )
  {
    case 0:
      Delta = SineTable[ Temp ]; /* look up sine table */
      break;

    case 1:
      Temp <<= 3; /* ramp down */
      if ( Mod_p->Channels[ Channel ].VibratoPosition < 0 )
      {
        Temp = 255 - Temp;
      }
      Delta = Temp;
      break;

    case 2:
      Delta = 255; /* square */
      break;

    case 3:
      Delta = rand() & 255; /* random */
      break;
  } /* switch */

  Delta *= Mod_p->Channels[ Channel ].VibratoDepth;
  Delta >>= 7;
  //Delta <<= 2; /* we use 4*periods so make vibrato 4 times bigger */

  if ( Mod_p->Channels[ Channel ].VibratoPosition >= 0 )
  {
    SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency + Delta ) );
  }
  else
  {
    SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency - Delta ) );
  }

  Mod_p->Channels[ Channel ].VibratoPosition += Mod_p->Channels[ Channel ].VibratoSpeed;
  if ( Mod_p->Channels[ Channel ].VibratoPosition > 31 )
  {
    Mod_p->Channels[ Channel ].VibratoPosition -= 64;
  }

  return;
}


/*
*/
static void DoTremolo(struct ModTag* Mod_p, uint8_t Channel)
{
  uint16_t Delta;
  uint8_t Temp;

  Temp = ( Mod_p->Channels[ Channel ].TremoloPosition & 31 ); /* Temp will be the index */

  switch ( ( Mod_p->Channels[ Channel ].WaveformControl >> 4 ) & 3 )
  {
    case 0:
      Delta = SineTable[ Temp ]; /* look up sine table */
      break;

    case 1:
      Temp <<= 3; /* ramp down */
      if ( Mod_p->Channels[ Channel ].TremoloPosition < 0 )
      {
        Temp = 255 - Temp;
      }
      Delta = Temp;
      break;

    case 2:
      Delta = 255; /* square */
      break;

    case 3:
      Delta = rand() & 255; /* random */
      break;
  } /* switch */

  Delta *= Mod_p->Channels[ Channel ].TremoloDepth;
  Delta >>= 6;

  if ( Mod_p->Channels[ Channel ].TremoloPosition >= 0 )
  {
    if ( Mod_p->Channels[ Channel ].Volume + Delta > 64 )
    {
      Delta = 64 - Mod_p->Channels[ Channel ].Volume;
    }

    SetVolume( Channel, ( ( Mod_p->Channels[ Channel ].Volume + Delta ) << 10 ), Mod_p->Channels[ Channel ].PanValue );
  }
  else
  {
    if ( (int16_t) ( Mod_p->Channels[ Channel ].Volume - Delta < 0 ) )
    {
      Delta = Mod_p->Channels[ Channel ].Volume;
    }

    SetVolume( Channel, ( ( Mod_p->Channels[ Channel ].Volume - Delta ) << 10 ), Mod_p->Channels[ Channel ].PanValue );
  }

  Mod_p->Channels[ Channel ].TremoloPosition += Mod_p->Channels[ Channel ].TremoloSpeed;
  if ( Mod_p->Channels[ Channel ].TremoloPosition > 31 )
  {
    Mod_p->Channels[ Channel ].TremoloPosition -= 64;
  }

  return;
}


/*
*/
static void DoPorta(struct ModTag* Mod_p, uint8_t Channel)
{
  /* slide pitch down if it needs too */
  if ( Mod_p->Channels[ Channel ].PeriodFrequency < Mod_p->Channels[ Channel ].PortaTo )
  {
    Mod_p->Channels[ Channel ].PeriodFrequency += Mod_p->Channels[ Channel ].PortaSpeed;
    if ( Mod_p->Channels[ Channel ].PeriodFrequency > Mod_p->Channels[ Channel ].PortaTo )
    {
      Mod_p->Channels[ Channel ].PeriodFrequency = Mod_p->Channels[ Channel ].PortaTo;
    }
  }

  /* slide pitch up if it needs too */
  if ( Mod_p->Channels[ Channel ].PeriodFrequency > Mod_p->Channels[ Channel ].PortaTo )
  {
    Mod_p->Channels[ Channel ].PeriodFrequency -= Mod_p->Channels[ Channel ].PortaSpeed;
    if ( Mod_p->Channels[ Channel ].PeriodFrequency < Mod_p->Channels[ Channel ].PortaTo )
    {
      Mod_p->Channels[ Channel ].PeriodFrequency = Mod_p->Channels[ Channel ].PortaTo;
    }
  }

  SetFrequency( Channel, AmigaPeriod2Hz( Mod_p->Channels[ Channel ].PeriodFrequency ) );

  return;
}


static uint16_t AmigaPeriod2Hz(int16_t PeriodFrequency)
{
  uint16_t Frequency = 65535; /* SampleRate, the rate to send data to the audio hardware (Hz unit) */

  if ( PeriodFrequency > 54 )
    Frequency = (uint16_t) ( AMIGA_PAL_CPU_CLOCK / (PeriodFrequency * 2) );

  return Frequency;
}


/* System specific functions =================================================================== */

static void SetBeat(struct ModTag* Mod_p, uint8_t bpm)
{
    uint16_t nsmp = (SAMPLE_RATE * 5)/(bpm * 2);
    Mod_p->NumSamples = (nsmp > MAX_NUM_SAMPLES) ? MAX_NUM_SAMPLES : nsmp;
}


static struct ModTag* SetMOD(struct ModTag* Mod_p)
{
  /* once set, PlayMOD() starts being called */
  return (struct ModTag*)SVC_SetMOD(Mod_p);
}


static void AllocChannel(uint8_t Channel)
{
  voice_t smp;
  memset(&smp, 0xFF, sizeof(voice_t));
  smp.flags = VF_ENABLED | VF_MODULE;
  /* use SVC_Update() to force voice allocated to the module */
  SVC_Update(Channel, &smp);
}


static void FreeChannel(uint8_t Channel)
{
  voice_t smp;
  memset(&smp, 0xFF, sizeof(voice_t));
  smp.flags = 0;
  /* use SVC_Update() to force voice deallocated */
  SVC_Update(Channel, &smp);
}


static void SetSample(uint16_t Channel, int8_t* SampleBuff_p,
               uint32_t SampleOffset, uint32_t SampleLength,
               uint32_t LoopStart, uint32_t LoopLength)
{
  voice_t smp;
  smp.wave = SampleBuff_p;
  smp.index = SampleOffset << 14;
  smp.step = 0xFFFFFFFF;
  smp.loop = LoopStart << 14;
  smp.length = (LoopLength > 2) ? (LoopStart + LoopLength) << 14 : SampleLength << 14;
  smp.left = 0xFFFF;
  smp.right = 0xFFFF;
  smp.flags = VF_ENABLED | ((LoopLength > 2) ? VF_LOOPS : 0) | VF_MODULE | VF_PLAYING;
  SSVC_Update(Channel, &smp);
}


static void SetFrequency(uint16_t Channel, uint32_t Freq)
{
  SSVC_UpdateStep(Channel, (Freq << 14) / SAMPLE_RATE);
}


static void SetVolume(uint16_t Channel, uint32_t Volume, int32_t Pan)
{
  uint32_t left, right;
  uint32_t vol;
  uint16_t chan = Channel & 3;

  vol = (Volume >= (64<<10)) ? (0x7FFF * gVolume) >> 4 : ((Volume >> 1) * gVolume) >> 4;

#ifdef HANDLE_PANNING
  right = (vol * Pan * Pan) / 16384;
  left = (vol * (128 - Pan) * (128 - Pan)) / 16384;
#else
  if ( (chan == 0) || (chan == 3) )
  {
#ifdef CROSS_TALK
    right = vol >> CROSS_TALK;
#else
    right = 0;
#endif
    left = vol;
  }
  else
  {
    right = vol;
#ifdef CROSS_TALK
    left = vol >> CROSS_TALK;
#else
    left = 0;
#endif
  }
#endif

  SSVC_UpdateVol(Channel, ((uint32_t)left << 16) | right);
}

