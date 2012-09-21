#ifndef _MODULE_H_
#define _MODULE_H_

#include <stdint.h>


#define AMIGA_PAL_CPU_CLOCK ( 7093789 ) /* Hz unit (~7 MHz) */

#ifndef FALSE
#define FALSE ( (uint8_t) 0 )
#endif
#ifndef TRUE
#define TRUE  ( (uint8_t) 1 )
#endif

/* MOD enum Constants */
#define MOD_SIGNATURE_LENGTH   ( (uint8_t) 4 )
#define MOD_NAME_LENGTH        ( (uint8_t) 20 )
#define MOD_SAMPLE_NAME_LENGTH ( (uint8_t) 22 )
#define MOD_NUMBER_OF_SAMPLES  ( (uint8_t) 31 )
#define MOD_ROWS_PER_CHANNEL   ( (uint8_t) 64 )
#define MOD_NUMBER_OF_ORDERS   ( (uint8_t) 128 )
#define MOD_SIGNATURE_OFFSET   ( (int16_t) 1080 ) /* 0x0438 */
#define MOD_FINE_TUNE_VALUES   ( (uint8_t) 16 )
#define MOD_NUMBER_OF_NOTES    ( (uint8_t) 36 )
#define MOD_NUMBER_OF_CHANNELS ( (uint8_t) 8 )
#define MOD_TIMER_SPEED        ( (uint8_t) 50 ) /* timer speed, we need a speed of 50 Hz = 50 ticks per second */

/* EFFECTS */
#define EFFECT_ARPEGGIO               ( (uint8_t) 0x00 )   /* Effect 0xy (Arpeggio) */
#define EFFECT_PORTA_UP               ( (uint8_t) 0x01 )   /* Effect 1xy (Porta Up) */
#define EFFECT_PORTA_DOWN             ( (uint8_t) 0x02 )   /* Effect 2xy (Porta Down) */
#define EFFECT_PORTA_TO_NOTE          ( (uint8_t) 0x03 )   /* Effect 3xy (Porta To Note) */
#define EFFECT_VIBRATO                ( (uint8_t) 0x04 )   /* Effect 4xy (Vibrato) */
#define EFFECT_PORTA_PLUS_VOL_SLIDE   ( (uint8_t) 0x05 )   /* Effect 5xy (Porta + Vol Slide) */
#define EFFECT_VIBRATO_PLUS_VOL_SLIDE ( (uint8_t) 0x06 )   /* Effect 6xy (Vibrato + Vol Slide) */
#define EFFECT_TREMOLO                ( (uint8_t) 0x07 )   /* Effect 7xy (Tremolo) */
#define EFFECT_PAN                    ( (uint8_t) 0x08 )   /* Effect 8xy (Pan) */
#define EFFECT_SAMPLE_OFFSET          ( (uint8_t) 0x09 )   /* Effect 9xy (Sample Offset) */
#define EFFECT_VOLUME_SLIDE           ( (uint8_t) 0x0A )   /* Effect Axy (Volume Slide) */
#define EFFECT_JUMP_TO_PATTERN        ( (uint8_t) 0x0B )   /* Effect Bxy (Jump To Pattern) */
#define EFFECT_SET_VOLUME             ( (uint8_t) 0x0C )   /* Effect Cxy (Set Volume) */
#define EFFECT_PATTERN_BREAK          ( (uint8_t) 0x0D )   /* Effect Dxy (Pattern Break) */
#define EFFECT_EXTENDED_EFFECTS       ( (uint8_t) 0x0E )   /* Extended Effects */
#define EFFECT_SET_SPEED              ( (uint8_t) 0x0F )   /* Effect Fxy (Set Speed) */

#define EFFECT_SET_FILTER             ( (uint8_t) 0x00 )   /* Effect E0x (Set Filter) */
#define EFFECT_FINE_PORTA_UP          ( (uint8_t) 0x01 )   /* Effect E1x (Fine Porta Up) */
#define EFFECT_FINE_PORTA_DOWN        ( (uint8_t) 0x02 )   /* Effect E2x (Fine Porta Down) */
#define EFFECT_GLISSANDO_CONTROL      ( (uint8_t) 0x03 )   /* Effect E3x (Glissando Control) */
#define EFFECT_SET_VIBRATO_WAVEFORM   ( (uint8_t) 0x04 )   /* Effect E4x (Set Vibrato Waveform) */
#define EFFECT_SET_FINETUNE           ( (uint8_t) 0x05 )   /* Effect E5x (Set Finetune) */
#define EFFECT_PATTERN_LOOP           ( (uint8_t) 0x06 )   /* Effect E6x (Pattern Loop) */
#define EFFECT_SET_TREMOLO_WAVEFORM   ( (uint8_t) 0x07 )   /* Effect E7x (Set Tremolo WaveForm) */
#define EFFECT_POSITION_PANNING       ( (uint8_t) 0x08 )   /* Effect E8x (Position Panning) */
#define EFFECT_RETRIG_NOTE            ( (uint8_t) 0x09 )   /* Effect E9x (Retrig Note) */
#define EFFECT_FINE_VOLUME_SLIDE_UP   ( (uint8_t) 0x0A )   /* Effect EAx (Fine Volume Slide Up) */
#define EFFECT_FINE_VOLUME_SLIDE_DOWN ( (uint8_t) 0x0B )   /* Effect EBx (Fine Volume Slide Down) */
#define EFFECT_CUT_NOTE               ( (uint8_t) 0x0C )   /* Effect ECx (Cut Note) */
#define EFFECT_DELAY_NOTE             ( (uint8_t) 0x0D )   /* Effect EDx (Delay Note) */
#define EFFECT_PATTERN_DELAY          ( (uint8_t) 0x0E )   /* Effect EEx (Pattern Delay) */
#define EFFECT_INVERT_LOOP            ( (uint8_t) 0x0F )   /* Effect EFx (Invert Loop) */


struct NoteTag
{
  uint8_t SampleNumber;
  uint8_t EffectNumber;
  uint8_t EffectParameter;
  uint8_t NoteExists;           /* boolean flag */
  uint8_t PeriodFrequencyCol;   /* column position in AmigaPeriodsTable[][] */
  uint16_t PeriodFrequency;
};

struct ChannelTag
{
  uint8_t LastInstrument;
  int8_t Volume;                /* default volume of sample */
  uint8_t PortaSpeed;
  uint8_t VibratoSpeed;
  uint8_t VibratoDepth;
  uint8_t TremoloSpeed;
  uint8_t TremoloDepth;
  uint8_t WaveformControl;      /* upper 4 bits for the tremolo wavecontrol, and the lower 4 bits for the vibrato wavecontrol */
  int8_t VibratoPosition;
  int8_t TremoloPosition;
  uint8_t PatLoopRow;
  uint8_t PatLoopNo;
  int16_t PanValue;             /* not used */
  int16_t PeriodFrequency;
  int16_t PortaTo;              /* note to port to value */
  uint32_t SampleOffset;
};

struct SampleTag
{
  uint8_t FineTune;
  uint8_t Volume;

  int8_t* SampleBuff_p;         /* pointer to the actual physical data in memory */
  uint32_t LoopStart;
  uint32_t LoopLength;
  uint32_t SampleLength;        /* The maximum size for a sample on the Amiga is 128K. */
};

/* main data structure */
struct ModTag
{
  uint8_t IsPlaying;                /* flags - b0 = 1 = playing, 0 = stopped; b7 = 1 = loop at end */
  uint8_t SongLength;               /* song length (1 to 128) */
  uint8_t NumberOfPatterns;         /* number of physical patterns (1 to 64) */
  uint8_t NumberOfChannels;         /* number of channels (4, 6, or 8) */
  uint8_t Speed;                    /* number of ticks (or times the function is called) between each time a new row is processed */
  uint8_t Tick;                     /* current music tick */
  int8_t Row;                       /* current row number value 0-63 */
  uint8_t Order;                    /* current value */
  uint8_t Pattern;                  /* current value */
  uint8_t PatternDelay;             /* number of notes to delay pattern for */

  uint16_t NumSamples;              /* sets timing for beats per minute */

  uint32_t* PatternsBuff_p;         /* start of pattern data (1 to 64 * 1024) */

  uint8_t Orders[ MOD_NUMBER_OF_ORDERS ]; /* pattern playing orders (128 entries of 0 to 63) */

  struct SampleTag Inst[ MOD_NUMBER_OF_SAMPLES ]; /* instrument headers */
  struct ChannelTag Channels[ MOD_NUMBER_OF_CHANNELS ]; /* run time channel info */
  struct NoteTag Notes[ MOD_NUMBER_OF_CHANNELS ]; /* run time note info */
};


/*
  Initialize Module struct for playing
    returns 0 if no error
*/
uint8_t InitMOD(uint8_t* Data_p, struct ModTag* Mod_p);

/*
  Cleanup MOD - stop playing and release voices
*/
void ExitMOD(struct ModTag* Mod_p);

/*
  Start playing Module
    loop = bool = TRUE = loop forever, = FALSE = play once
    returns the previously playing Module, if any
*/
struct ModTag* StartMOD(struct ModTag* Mod_p, uint8_t loop);

/*
  Pause/resume Module
    pause = bool = TRUE = pause, = FALSE = resume
*/
void PauseMOD(struct ModTag* Mod_p, uint8_t pause);

/*
  Stop playing Module
*/
void StopMOD(struct ModTag* Mod_p);

/*
  Wait for Module to play through once
*/
void WaitMOD(struct ModTag* Mod_p);

/*
  Check Module play status
    returns 0 if done playing
*/
uint8_t CheckMOD(struct ModTag* Mod_p);

/*
  Set global volume
    vol = 0 = off, = 16 = max
    returns the previous volume setting
*/
uint8_t VolumeMOD(uint8_t vol);


#endif
