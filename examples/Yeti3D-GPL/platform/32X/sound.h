#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <stdint.h>


extern void fill_buffer(unsigned long buff);
extern char snd_buffer[];
extern uint16_t svc_num_samples;

#define NUM_SAMPLES svc_num_samples

#ifdef CD_SAMPLE_RATE
#define NUM_VOICES      8
#define MAX_NUM_SAMPLES 1024
#define SAMPLE_RATE     44100
#define SAMPLE_CENTER   258
#else
#define NUM_VOICES      8
#define MAX_NUM_SAMPLES 512
#define SAMPLE_RATE     22050
#define SAMPLE_CENTER   516
#endif


/* Sample Voice Flags */
#define VF_ENABLED  0x4000      /* voice is allocated and ready to use */
#define VF_ONESHOT  0x2000      /* automatically disable after playing (for sound effects mainly) */
#define VF_LOOPS    0x1000      /* voice loops when it reaches the end */
#define VF_MODULE   0x0800      /* voice belongs to module */
#define VF_PLAYING  0x0001      /* voice is playing */


/* Sample Voice Structure */
typedef struct voice {
    int8_t*     wave;
    uint32_t    index;
    uint32_t    step;
    uint32_t    loop;
    uint32_t    length;
    uint16_t    left;
    uint16_t    right;
    uint16_t    flags;
    uint8_t     pad[6];         /* padded to take exactly two cache lines */
} voice_t;


/*
 * Lock the voices to allow access by only one SH2
 *   id = 2 = locked by Master SH2
 */
void SVC_Lock(int16_t id);

/*
 * Unlock the voices
 */
void SVC_Unlock(void);

/*
 * Pause/Resume all sound output
 */
void SVC_Pause(uint16_t pause);

/*
 * Allocate a voice and play a sample
 *   svc = sample to play (in voice format - is copied to allocated voice)
 *
 *   returns voice allocated, 0 to 7, 0xFFFF = no available voices
 */
uint16_t SVC_Start(voice_t* svc);

/*
 * Update sample on a voice
 *   vc = index of voice to update (0 to 7)
 *   svc = sample to play (in voice format - fields copied if not -1)
 */
void SVC_Update(uint16_t vc, voice_t* svc);

/*
 * Return voice flags field
 *   vc = index of voice (0 to 7)
 *
 *   returns voice flags
 */
uint16_t SVC_Status(uint16_t vc);

/*
 * Set pointer to module to call during the sample buffer update
 *   returns previously set module
 */
void* SVC_SetMOD(void* Mod_p);

/*
 * Update sample on a voice
 *   vc = index of voice to update (0 to 7)
 *   svc = sample to play (in voice format - fields copied if not -1)
 *
 * This is a special update for calling from the Slave SH2
 */
void SSVC_Update(uint16_t vc, voice_t* svc);

/*
 * Update step on a voice
 *   vc = index of voice to update (0 to 7)
 *   step = new step
 *
 * This is a special update for calling from the Slave SH2
 */
void SSVC_UpdateStep(uint16_t vc, uint32_t step);

/*
 * Update volume on a voice
 *   vc = index of voice to update (0 to 7)
 *   vol = new volume
 *
 * This is a special update for calling from the Slave SH2
 */
void SSVC_UpdateVol(uint16_t vc, uint32_t vol);

#endif
