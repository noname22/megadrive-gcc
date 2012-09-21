! 32X Sample Voice Handler - by Chilly Willy

        .equ    MARS_SYS_COMM4, 0x20004024
        .equ    MARS_SYS_COMM6, 0x20004026

        .ifdef  CD_SAMPLE_RATE
        .equ    NUM_VOICES,      8
        .equ    MAX_NUM_SAMPLES, 1024
        .equ    SAMPLE_RATE,     44100
        .equ    SAMPLE_CENTER,   258
        .equ    SAMPLE_MIN,     -256
        .equ    SAMPLE_MAX,      256
        .else
        .equ    NUM_VOICES,      8
        .equ    MAX_NUM_SAMPLES, 512
        .equ    SAMPLE_RATE,     22050
        .equ    SAMPLE_CENTER,   516
        .equ    SAMPLE_MIN,     -512
        .equ    SAMPLE_MAX,      512
        .endif

        /* MSB of vc_flags */
        .equ    VF_ENABLED, 0x40
        .equ    VF_ONESHOT, 0x20    /* automatically disable after playing (for sound effects mainly) */
        .equ    VF_LOOPS,   0x10
        .equ    VF_MODULE,  0x08    /* voice belongs to module */

        /* LSB of vc_flags */
        .equ    VF_PLAYING, 0x01    /* voice is playing */

! voice structure
        .equ    vc_wave,   0
        .equ    vc_index,  4
        .equ    vc_step,   8
        .equ    vc_loop,   12
        .equ    vc_length, 16
        .equ    vc_left,   20
        .equ    vc_right,  22
        .equ    vc_flags,  24
        .equ    vc_sizeof, 32       /* padded to take exactly two cache lines */


        .data

        .align  4

! handler variables (some accessed uncached)

module:
        .long   0
mod_pause:
        .word   0
svc_pause:
        .word   0
svc_purge:
        .word   0

        .global _svc_num_samples
_svc_num_samples:
svc_num_samples:
        .word   0

        .align  4

! sample buffer - two buffers of NUM_SAMPLES stereo PWM samples
        .global _snd_buffer
_snd_buffer:
        .space  MAX_NUM_SAMPLES*2*2*2

        .align  4

! voices
voices:
        .space  vc_sizeof*NUM_VOICES

        .align  2

! uncomment the section declaration below to move code to rom
!
!        .text
!
!        .align  2

!-----------------------------------------------------------------------
! Voice Support Functions
!-----------------------------------------------------------------------

! void SVC_Lock(int16_t id)
! Entry: r4 = id

        .global _SVC_Lock
_SVC_Lock:
        exts.w  r4,r4
        mov.l   ss_svc_state,r1
0:
        mov.w   @r1,r0
        cmp/eq  #1,r0                   /* loop until unlocked */
        bf      0b

        mov.w   r4,@r1
        mov.w   @r1,r0
        cmp/eq  r4,r0
        bf      0b                      /* race condition - we lost */

        rts
        nop

! void SVC_Unlock(void)

        .global _SVC_Unlock
_SVC_Unlock:
        mov     #1,r0
        mov.l   ss_svc_state,r1
        rts
        mov.w   r0,@r1

! void SVC_Pause(uint16_t pause)
! Entry: r4 = pause (0 = play, ~0 = pause)

        .global _SVC_Pause
_SVC_Pause:
        mov.l   ss_svc_pause,r1
        rts
        mov.w   r4,@r1

! uint16_t SVC_Start(voice_t* svc)
! Entry: r4 = pointer to SVC voice to play
! Exit:  r0 = SVC voice, 0xFFFF = no free voices, 0 to NUM_VOICES - 1 = SVC voice

        .global _SVC_Start
_SVC_Start:
        sts.l   pr,@-r15                /* save return address */

        mov     r4,r3
        bsr     _SVC_Lock
        mov     #2,r4                   /* locked by MSH2 */

        mov.l   ss_flush_voices,r1
        mov.l   ss_voices,r2
        mov     #NUM_VOICES,r4
1:
        /* flush SVC voice */
        mov     #0,r0
        mov.l   r0,@(0x00,r1)
        mov.l   r0,@(0x10,r1)
        /* check flags if free */
        mov.w   @(vc_flags,r2),r0
        shlr8   r0
        tst     #VF_ENABLED,r0
        bt      2f                      /* voice disabled, use it */
        /* next voice */
        dt      r4
        add     #vc_sizeof,r1
        bf/s    1b
        add     #vc_sizeof,r2

        /* no free voices */
        bsr     _SVC_Unlock
        nop
        lds.l   @r15+,pr                /* restore return address */
        rts
        mov     #-1,r0
2:
        /* copy SVC voice */
        mov.l   @(vc_wave,r3),r0
        mov.l   r0,@(vc_wave,r2)
        mov.l   @(vc_index,r3),r0
        mov.l   r0,@(vc_index,r2)
        mov.l   @(vc_step,r3),r0
        mov.l   r0,@(vc_step,r2)
        mov.l   @(vc_loop,r3),r0
        mov.l   r0,@(vc_loop,r2)
        mov.l   @(vc_length,r3),r0
        mov.l   r0,@(vc_length,r2)
        mov.l   @(vc_left,r3),r0
        mov.l   r0,@(vc_left,r2)
        mov.w   @(vc_flags,r3),r0
        mov.w   r0,@(vc_flags,r2)

        mov.l   ss_svc_purge,r1
        mov     #1,r0
        bsr     _SVC_Unlock
        mov.w   r0,@r1                  /* SSH2 needs to flush SVC voices */
        lds.l   @r15+,pr                /* restore return address */
        mov     #NUM_VOICES,r0
        rts
        sub     r4,r0                   /* return the voice index */

! void SVC_Update(uint16_t vc, voice_t* svc)
! Entry: r4 = vc
!        r5 = pointer to SVC voice updates

        .global _SVC_Update
_SVC_Update:
        sts.l   pr,@-r15                /* save return address */

        extu.w  r4,r3
        bsr     _SVC_Lock
        mov     #2,r4                   /* locked by MSH2 */

        shll2   r3
        shll2   r3
        shll    r3                      /* (vc * 32) */
        mov.l   ss_flush_voices,r1
        /* flush selected SVC voice */
        add     r3,r1
        mov     #0,r0
        mov.l   r0,@(0x00,r1)
        mov.l   r0,@(0x10,r1)
        mov.l   ss_voices,r2
        add     r3,r2

        /* check updates */
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      0f
        /* new wave */
        mov.l   r0,@(vc_wave,r2)
0:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      1f
        /* new index */
        mov.l   r0,@(vc_index,r2)
1:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      2f
        /* new step */
        mov.l   r0,@(vc_step,r2)
2:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      3f
        /* new loop */
        mov.l   r0,@(vc_loop,r2)
3:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      4f
        /* new length */
        mov.l   r0,@(vc_length,r2)
4:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      5f
        /* new volume */
        mov.l   r0,@(vc_left,r2)
5:
        mov.w   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      6f
        /* new flags */
        mov.w   r0,@(vc_flags,r2)
6:
        mov.l   ss_svc_purge,r1
        mov     #1,r0
        bsr     _SVC_Unlock
        mov.w   r0,@r1                  /* SSH2 needs to flush SVC voices */

        lds.l   @r15+,pr                /* restore return address */
        rts
        nop

! uint16_t SVC_Status(uint16_t vc)
! Entry: r4 = vc
! Exit:  r0 = flags for selected SVC voice

        .global _SVC_Status
_SVC_Status:
        extu.w  r4,r3
        shll2   r3
        shll2   r3
        shll    r3                      /* (vc * 32) */
        mov.l   ss_flush_voices,r1
        /* flush selected SVC voice */
        add     r3,r1
        mov     #0,r0
        mov.l   r0,@(0x00,r1)
        mov.l   r0,@(0x10,r1)
        mov.l   ss_voices,r1
        add     r3,r1
        rts
        mov.w   @(vc_flags,r1),r0

! void* SVC_SetMOD(void* Mod_p)
! Entry: r4 = Mod_p
! Exit:  r0 = previous Mod_p

        .global _SVC_SetMOD
_SVC_SetMOD:
        mov.l   ss_module,r1
        mov.l   @r1,r0
        rts
        mov.l   r4,@r1

! void SSVC_Update(uint16_t vc, voice_t* svc)
! Entry: r4 = vc
!        r5 = pointer to SVC voice updates

        .global _SSVC_Update
_SSVC_Update:
        extu.w  r4,r3
        shll2   r3
        shll2   r3
        shll    r3                      /* (vc * 32) */
        mov.l   ss_voices,r2
        add     r3,r2

        /* check updates */
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      0f
        /* new wave */
        mov.l   r0,@(vc_wave,r2)
0:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      1f
        /* new index */
        mov.l   r0,@(vc_index,r2)
1:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      2f
        /* new step */
        mov.l   r0,@(vc_step,r2)
2:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      3f
        /* new loop */
        mov.l   r0,@(vc_loop,r2)
3:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      4f
        /* new length */
        mov.l   r0,@(vc_length,r2)
4:
        mov.l   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      5f
        /* new volume */
        mov.l   r0,@(vc_left,r2)
5:
        mov.w   @r5+,r0
        cmp/eq  #0xFF,r0
        bt      6f
        /* new flags */
        mov.w   r0,@(vc_flags,r2)
6:
        rts
        nop

! void SSVC_UpdateStep(uint16_t vc, uint32_t step)
! Entry: r4 = vc
!        r5 = step

        .global _SSVC_UpdateStep
_SSVC_UpdateStep:
        extu.w  r4,r3
        shll2   r3
        shll2   r3
        shll    r3                      /* (vc * 32) */
        mov.l   ss_voices,r2
        add     r3,r2

        /* new step */
        mov.l   r5,@(vc_step,r2)

        rts
        nop

! void SSVC_UpdateVol(uint16_t vc, uint32_t vol)
! Entry: r4 = vc
!        r5 = volume

        .global _SSVC_UpdateVol
_SSVC_UpdateVol:
        extu.w  r4,r3
        shll2   r3
        shll2   r3
        shll    r3                      /* (vc * 32) */
        mov.l   ss_voices,r2
        add     r3,r2

        /* new volume */
        mov.l   r5,@(vc_left,r2)

        rts
        nop

        .align 2

ss_svc_pause:
        .long   svc_pause+0x20000000
ss_svc_state:
        .long   MARS_SYS_COMM6
ss_svc_purge:
        .long   svc_purge+0x20000000
ss_voices:
        .long   voices
ss_flush_voices:
        .long   voices+0x40000000
ss_module:
        .long   module
ss_flush:
        .long   0x40000000


!-----------------------------------------------------------------------
! Low-level SVC Functions
!-----------------------------------------------------------------------

! Uses:  r0 = scratch
!        r1 = scratch
!        r2 = voices
!        r3 = current buffer pointer
!        r4 = buffer pointer (passed in from _fill_buffer)
!        r5 = voice count
!        r6 = sample count
!        r7 = wave
!        r8 = index
!        r9 = step
!        r10 = loop
!        r11 = length
!        r12 = left volume
!        r13 = right volume
!        r14 = sample

process_svc:
        mov.l   ps_svc_pause,r1
        mov.w   @r1,r0
        cmp/pl  r0
        bt      proc_snd_exit           /* paused - just exit */

        sts.l   pr,@-r15                /* save return address */

        mov     r4,r3
        bsr     _SVC_Lock
        mov     #3,r4                   /* locked by SSH2 */
        mov     r3,r4

        mov.l   ps_svc_purge,r1
        mov.w   @r1,r0
        cmp/eq  #0,r0
        bt      do_svc_voices
        /* flush svc voices */
        mov     #NUM_VOICES,r2
        mov.l   ps_flush_voices,r3
        mov     #0,r0
1:
        mov.l   r0,@(0x00,r3)
        mov.l   r0,@(0x10,r3)
        dt      r2
        bf/s    1b
        add     #vc_sizeof,r3
        mov.w   r0,@r1

do_svc_voices:
        mov.l   ps_voices,r2
        mov     #NUM_VOICES,r5
0:
        /* check if voice playing */
        mov.w   @(vc_flags,r2),r0
        tst     #VF_PLAYING,r0
        bt      4f                      /* not playing - next voice */
        /* playing, check if belongs to module and paused */
        shlr8   r0
        tst     #VF_MODULE,r0
        bt      1f                      /* not a module voice - play */
        mov.l   ps_mod_pause,r1
        mov.w   @r1,r0
        cmp/pl  r0
        bt      4f                      /* module paused - next voice */
1:
        /* process samples */
        mov     r4,r3                   /* start of sample buffer */
        mov.l   ps_num_samples,r1
        mov.w   @r1,r6
        mov.l   @(vc_wave,r2),r7
        mov.l   @(vc_index,r2),r8
        mov.l   @(vc_step,r2),r9
        mov.l   @(vc_loop,r2),r10
        mov.l   @(vc_length,r2),r11
        mov.w   @(vc_left,r2),r0
        shlr2   r0
        mov     r0,r12
        mov.w   @(vc_right,r2),r0
        shlr2   r0
        mov     r0,r13
2:
        add     r9,r8                   /* index += step */
        cmp/hs  r11,r8
        bt      5f                      /* index >= length */
3:
        /* process one sample */
        mov     r8,r0
        shlr8   r0
        shll2   r0
        shlr8   r0
        mov.b   @(r0,r7),r14
        /* scale sample for left output */
        muls.w  r14,r12
        mov.w   @r3,r1
        sts     macl,r0
        shlr8   r0                      /* now 13-bit sample - no overflow with 8 voices */
        exts.w  r0,r0
        add     r0,r1
        mov.w   r1,@r3
        add     #2,r3
        /* scale sample for right output */
        muls.w  r14,r13
        mov.w   @r3,r1
        sts     macl,r0
        shlr8   r0
        exts.w  r0,r0
        add     r0,r1
        mov.w   r1,@r3
        add     #2,r3
        /* next sample */
        dt      r6
        bf      2b
        mov.l   r8,@(vc_index,r2)
4:
        /* next voice */
        dt      r5
        bf/s    0b
        add     #vc_sizeof,r2

        bsr     _SVC_Unlock
        nop
        lds.l   @r15+,pr                /* restore return address */
        rts
        nop
5:
        /* check if loop sample */
        mov.w   @(vc_flags,r2),r0
        shlr8   r0
        tst     #VF_LOOPS,r0
        bt/s    6f                      /* no looping */
        sub     r11,r8
        bra     3b
        add     r10,r8                  /* index = index - length + loop */
6:
        /* check if one-shot sample */
        tst     #VF_ONESHOT,r0
        bt      7f                      /* not oneshot - leave enabled */
        mov     #0,r0                   /* voice disabled */
7:
        shll8   r0
        mov.w   r0,@(vc_flags,r2)       /* voice not playing (and possibly disabled) */

        /* next voice */
        dt      r5
        bf/s    0b
        add     #vc_sizeof,r2

        bsr     _SVC_Unlock
        nop
        lds.l   @r15+,pr                /* restore return address */

proc_snd_exit:
        rts
        nop


        .align 2

ps_svc_pause:
        .long   svc_pause+0x20000000
ps_mod_pause:
        .long   mod_pause
ps_svc_purge:
        .long   svc_purge+0x20000000
ps_voices:
        .long   voices
ps_flush_voices:
        .long   voices+0x40000000
ps_num_samples:
        .long   svc_num_samples


!-----------------------------------------------------------------------
! Low-level Audio Functions
!-----------------------------------------------------------------------

! Entry: r4 = sound buffer to fill

        .global _fill_buffer
_fill_buffer:
        sts.l   pr,@-r15                /* save return address */
        mov.l   r8,@-r15
        mov.l   r9,@-r15
        mov.l   r10,@-r15
        mov.l   r11,@-r15
        mov.l   r12,@-r15
        mov.l   r13,@-r15
        mov.l   r14,@-r15
        mov     r4,r14

! set default number of samples (in case no module loaded)

        mov.l   fb_num_samples,r1
        mov.w   fb_dflt_nsmp,r0
        mov.w   r0,@r1

        /* handle module processing */
        mov.l   fb_module,r3
        mov.l   @r3,r0
        cmp/eq  #0,r0
        bt      handle_voices           /* no module set */
        mov.l   fb_uncached,r4
        or      r0,r4

        /* set number samples for module BeatsPerMinute */
        mov.w   @(10,r4),r0
        mov.l   fb_num_samples,r1
        mov.w   r0,@r1

        mov.b   @r4,r0                  /* IsPlaying */
        and     #1,r0
        xor     #1,r0
        mov.l   fb_mod_pause,r1
        mov.w   r0,@r1
        cmp/eq  #1,r0
        bt      handle_voices           /* paused or stopped */
3:
        /* call PlayMOD(Mod_p) */
        mov.l   fb_play_mod,r3
        jsr     @r3
        mov     #0,r0
        nop
        mov.l   fb_mod_pause,r3
        mov.w   r0,@r3

        /* handle sample processing */
handle_voices:
        /* clear sound buffer */
        mov     r14,r3
        mov.l   fb_num_samples,r1
        mov.w   @r1,r2
        mov     #0,r0
1:
        mov.l   r0,@r3
        dt      r2
        bf/s    1b
        add     #4,r3

        /* process sample voices */
        bsr     process_svc
        mov     r14,r4

!       convert buffer from signed 15-bit samples (4 signed 13-bit samples)
!       to unsigned PWM samples (4 to 516 @44kHz or 8 to 1032 @22kHz)
!       only assumes eqv of 4 full-range samples for better volume, this
!       may result in clipping

        mov     r4,r3
        mov.l   fb_num_samples,r1
        mov.w   @r1,r2
        mov.w   fb_sample_center,r1
        mov.w   fb_sample_max,r6
        mov.w   fb_sample_min,r5
2:
        /* process left sample */
        mov.w   @r3+,r0
        /* shift sample */
        .ifndef CD_SAMPLE_RATE
        shlr2   r0                      /* 13 bits */
        shlr2   r0                      /* 11 bits */
        shlr    r0                      /* 10 bits */
        exts.w  r0,r0
        .else
        shll2   r0                      /* 17 bits */
        shlr8   r0                      /*  9 bits */
        exts.w  r0,r0
        .endif
        /* clamp sample */
        cmp/ge  r5,r0
        bf      5f                      /* clamp to minimum sample */
        cmp/gt  r6,r0
        bt      6f                      /* clamp to maximum sample */
3:
        add     r1,r0
        mov.w   r0,@r4

        /* process right sample */
        mov.w   @r3+,r0
        /* shift sample */
        .ifndef CD_SAMPLE_RATE
        shlr2   r0                      /* 13 bits */
        shlr2   r0                      /* 11 bits */
        shlr    r0                      /* 10 bits */
        exts.w  r0,r0
        .else
        shll2   r0                      /* 17 bits */
        shlr8   r0                      /*  9 bits */
        exts.w  r0,r0
        .endif
        /* clamp sample */
        cmp/ge  r5,r0
        bf      7f                      /* clamp to minimum sample */
        cmp/gt  r6,r0
        bt      8f                      /* clamp to maximum sample */
4:
        add     r1,r0
        mov.w   r0,@(2,r4)

        dt      r2
        bf/s    2b
        add     #4,r4

        mov.l   @r15+,r14
        mov.l   @r15+,r13
        mov.l   @r15+,r12
        mov.l   @r15+,r11
        mov.l   @r15+,r10
        mov.l   @r15+,r9
        mov.l   @r15+,r8
        lds.l   @r15+,pr                /* restore return address */
        rts
        nop
5:
        bra     3b
        mov     r5,r0
6:
        bra     3b
        mov     r6,r0
7:
        bra     4b
        mov     r5,r0
8:
        bra     4b
        mov     r6,r0

        .align  2

fb_sample_center:
        .word   SAMPLE_CENTER
fb_sample_min:
        .word   SAMPLE_MIN
fb_sample_max:
        .word   SAMPLE_MAX
fb_dflt_nsmp:
        .word   (SAMPLE_RATE * 5)/250

        .align  2

fb_module:
        .long   module+0x20000000
fb_play_mod:
        .long   _PlayMOD
fb_mod_pause:
        .long   mod_pause
fb_num_samples:
        .long   svc_num_samples
fb_uncached:
        .long   0x20000000

