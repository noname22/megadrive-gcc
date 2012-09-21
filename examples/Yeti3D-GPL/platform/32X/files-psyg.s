        .text

! MODs for level music

        .align  2
modName1:
        .asciz  "SOTBeast2_2.MOD"
modName2:
        .asciz  "SOTBeast2_4.MOD"
modName3:
        .asciz  "SOTBeast2_5.MOD"

        .align  4
mod1:
        .incbin "music/SOTBeast2_2.MOD"
modEnd1:
        .align  4
mod2:
        .incbin "music/SOTBeast2_4.MOD"
modEnd2:
        .align  4
mod3:
        .incbin "music/SOTBeast2_5.MOD"
modEnd3:

        .align  4

        .global _modName
_modName:
        .long   modName1
        .long   modName2
        .long   modName3

        .global _modSize
_modSize:
        .long   modEnd1 - mod1
        .long   modEnd2 - mod2
        .long   modEnd3 - mod3

        .global _modPtr
_modPtr:
        .long   mod1
        .long   mod2
        .long   mod3


! SFXs for game sounds

        .align  2
sfxName1:
        .asciz  "pl-oof.raw"
sfxName2:
        .asciz  "pl-sht.raw"
sfxName3:
        .asciz  "pl-pain.raw"
sfxName4:
        .asciz  "pl-dth.raw"
sfxName5:
        .asciz  "en-near.raw"
sfxName6:
        .asciz  "en-atk.raw"
sfxName7:
        .asciz  "en-pain.raw"
sfxName8:
        .asciz  "en-dth.raw"

        .align  4
sfx1:
        .incbin "sounds/pl-oof.raw"
sfxEnd1:
        .align  4
sfx2:
        .incbin "sounds/pl-sht.raw"
sfxEnd2:
        .align  4
sfx3:
        .incbin "sounds/pl-pain.raw"
sfxEnd3:
        .align  4
sfx4:
        .incbin "sounds/pl-dth.raw"
sfxEnd4:
        .align  4
sfx5:
        .incbin "sounds/en-near.raw"
sfxEnd5:
        .align  4
sfx6:
        .incbin "sounds/en-atk.raw"
sfxEnd6:
        .align  4
sfx7:
        .incbin "sounds/en-pain.raw"
sfxEnd7:
        .align  4
sfx8:
        .incbin "sounds/en-dth.raw"
sfxEnd8:

        .align  4

        .global _sfxName
_sfxName:
        .long   sfxName1
        .long   sfxName2
        .long   sfxName3
        .long   sfxName4
        .long   sfxName5
        .long   sfxName6
        .long   sfxName7
        .long   sfxName8

        .global _sfxSize
_sfxSize:
        .long   sfxEnd1 - sfx1
        .long   sfxEnd2 - sfx2
        .long   sfxEnd3 - sfx3
        .long   sfxEnd4 - sfx4
        .long   sfxEnd5 - sfx5
        .long   sfxEnd6 - sfx6
        .long   sfxEnd7 - sfx7
        .long   sfxEnd8 - sfx8

        .global _sfxPtr
_sfxPtr:
        .long   sfx1
        .long   sfx2
        .long   sfx3
        .long   sfx4
        .long   sfx5
        .long   sfx6
        .long   sfx7
        .long   sfx8

        .align  4

