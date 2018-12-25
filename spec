#include <nusys.h>

beginseg
    name    "code"
    entry   nuBoot
    flags   BOOT OBJECT
    address NU_SPEC_BOOT_ADDR
    stack   NU_SPEC_BOOT_STACK
    include "codesegment.o"
	include "$(ROOT)/usr/lib/PR/rspboot.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspL3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.Rej.fifo.o"
    include "$(ROOT)/usr/lib/PR/aspMain.o"
	include "$(ROOT)/usr/lib/PR/n_aspMain.o"
        include "$(ROOT)/usr/lib/PR/gspF3DEX2.NoN.fifo.o"
        include "$(ROOT)/usr/lib/PR/gspF3DLX2.Rej.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspS2DEX2.fifo.o"
endseg


/*--------------------------------------------------------------
                            SFX
--------------------------------------------------------------*/

// Pointer to bank sounds
beginseg
	name "bank_sfx_pbank"
	flags RAW
	include "assets/sounds/bank_sfx.ptr"
endseg

// Bank containing sound data
beginseg
	name "bank_sfx_wbank"
	flags RAW
	include "assets/sounds/bank_sfx.wbk"
endseg

// Sound effects list
beginseg
	name "bank_sfx_sfx"
	flags RAW
	include "assets/sounds/bank_sfx.bfx"
endseg


/*--------------------------------------------------------------
                            Music
--------------------------------------------------------------*/

// Pointer to bank sounds
beginseg
	name "bank_instr_pbank"
	flags RAW
	include "assets/music/bank_instr.ptr"
endseg

// Bank containing sound data
beginseg
	name "bank_instr_wbank"
	flags RAW
	include "assets/music/bank_instr.wbk"
endseg

// Main Song 1 part 1
beginseg
	name "sng_main1_1"
	flags RAW
	include "assets/music/sng_main1_1.bin"
endseg

// Main Song 1 part 2
beginseg
	name "sng_main1_2"
	flags RAW
	include "assets/music/sng_main1_2.bin"
endseg

// Main Song 1 part 3
beginseg
	name "sng_main1_3"
	flags RAW
	include "assets/music/sng_main1_3.bin"
endseg

// Main Song 2
beginseg
	name "sng_main2"
	flags RAW
	include "assets/music/sng_main2.bin"
endseg

// Main Song 3
beginseg
	name "sng_main3"
	flags RAW
	include "assets/music/sng_main3.bin"
endseg

// Game Over Tune
beginseg
	name "sng_gameover"
	flags RAW
	include "assets/music/sng_gameover.bin"
endseg

// Menu Music
beginseg
	name "sng_menu"
	flags RAW
	include "assets/music/sng_menu.bin"
endseg


beginwave
    name    "compiledrom"
    include "code"
    
    include "bank_instr_pbank"
	include "bank_instr_wbank"
    
    include "sng_main1_1"
    include "sng_main1_2"
    include "sng_main1_3"
    include "sng_main2"
    include "sng_main3"
    include "sng_gameover"
    include "sng_menu"
    
    include "bank_sfx_pbank"
	include "bank_sfx_wbank"
	include "bank_sfx_sfx"
endwave
