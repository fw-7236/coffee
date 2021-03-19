;;
;;  aTouch_InitROM.asm
;;
        PUBLIC CO_TKEN0
        PUBLIC CO_TKEN1
        PUBLIC CO_TKCON
        PUBLIC CO_TKSW
        PUBLIC CO_TKPSSR
        PUBLIC CO_TKWKTRICNT
        PUBLIC CO_MODE
        PUBLIC CO_WKTKEN0
        PUBLIC CO_WKTKEN1
        PUBLIC CO_SYSCTRL
        PUBLIC CO_DEBOUNCE
        PUBLIC CO_RELEASE
        PUBLIC CO_SENSITIVITY
        PUBLIC _Touch_InitROM
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


    CSEG    AT  1FC0H
_Touch_InitROM:
    CSEG    AT  1FC0H
    DB    0x02

    CSEG    AT  1FC1H
    DB    0x20

    CSEG    AT  1FC2H
    DB    0x00

    CSEG    AT  1FC4H
CO_TKEN0:
    DB    0x0C

    CSEG    AT  1FC5H
CO_TKEN1:
    DB    0x00

    CSEG    AT  1FC7H
CO_TKCON:
    DB    0x01

    CSEG    AT  1FC8H
CO_TKSW:
    DB    0x14

    CSEG    AT  1FC9H
CO_TKPSSR:
    DB    0x15

    CSEG    AT  1FCAH
CO_TKWKTRICNT:
    DB    0x02

    CSEG    AT  1FCBH
CO_MODE:
    DB    0x00

    CSEG    AT  1FCCH
CO_WKTKEN0:
    DB    0x00

    CSEG    AT  1FCDH
CO_WKTKEN1:
    DB    0x00

;bit 0: 1: EA don't care, 0: EA=0
;bit 1: entering PD mode by user or touchlyzer (1: user, 0: touchlyzer)
    CSEG    AT  1FCFH
CO_SYSCTRL:
    DB    0x01

    CSEG    AT  1FD0H
CO_DEBOUNCE:
    DB    0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A
    DB    0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A

    CSEG    AT  1FE0H
CO_RELEASE:
    DB    0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x00
    DB    0x03,0x00,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B

    CSEG    AT  1FF0H
CO_SENSITIVITY:
    DB    0xCD,0xCD,0xF0,0xF0,0xCD,0xCD,0xCD,0xCD
    DB    0xCD,0xCD,0xC3,0xC3,0xC3,0xCD,0xCD,0xCD


    END