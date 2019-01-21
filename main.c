/*========================================================
				Initialize the game
========================================================*/
/*=================================
            Libraries
=================================*/

#include <nusys.h>
#include <nualstl_n.h>
#include "config.h"
#include "assets/stages/definitions.h"


/*=================================
             Prototypes
=================================*/

static void callback_stage_disclaimer(int pendingTaskCount);
static void callback_stage_menu(int pendingTaskCount);
static void callback_stage_game1(int pendingTaskCount);
static void callback_stage_game2(int pendingTaskCount);
static void callback_prenmi();
static void initAudio();


/*=================================
         Global Variables
=================================*/

// Game related variables
u32 GameScore1;
u32 GameScore2;
u32 GameScore;
u8 GameSpeed;

// Game saves
char global_eeprom_loaded;
u8 global_save_magic[8]; // Check "EEPROM Magic.txt" for how data is stored
u8 global_save_unlocks[8]; // Check "EEPROM Unlocks.txt" for how data is stored
u8 global_save_settings[8]; // Check "EEPROM Settings.txt" for how data is stored
u8 global_save_highscores[8]; // Check "EEPROM Highscores.txt" for how data is stored

// Controller
NUContData contdata[2];
u8 contPattern;

// Stage stuff
volatile short int global_stage;
u8 returnmode = RETURN_LOGO;

// Buffers
int ptr_buf[NU_AU_SAMPLE_SIZE];
int sfx_buf[NU_AU_SE_SIZE];
int ptr2_buf[NU_AU_SAMPLE_SIZE];
int tune1_buf[NU_AU_SONG_SIZE];
int tune2_buf[NU_AU_SONG_SIZE];
int tune3_buf[NU_AU_SONG_SIZE];

// Heap
char mem_heap[1024*512]; 	 // Half a megabyte of heap memory

// Music and sound handles
musHandle seqHandle1 = 0;
musHandle seqHandle2 = 0;
musHandle seqHandle3 = 0;
musHandle sndHandle = 0;
musHandle sndHandle_pyoro1 = 0;
musHandle sndHandle_pyoro2 = 0;


/*=================================
              Main
=================================*/

void mainproc(void * dummy)
{
    // Start by selecting the proper television
    if (TV_TYPE == PAL)
    {
        osViSetMode(&osViModeTable[OS_VI_FPAL_LAN1]);
        osViSetYScale(0.833); // Dirty but works
        nuPreNMIFuncSet((NUScPreNMIFunc)callback_prenmi);
    }
    else if (TV_TYPE == MPAL)
        osViSetMode(&osViModeTable[OS_VI_MPAL_LAN1]);

    // Initialize and activate the graphics thread and Graphics Task Manager.
    nuGfxInit();
    
    // Initialize the heap for dynamic memory allocation
	if (InitHeap(mem_heap, sizeof(mem_heap)) == -1)
        return; // or stop the program if something horrible happens.
        
    // Initialize audio thread
    initAudio();
    
    // Start in the disclaimer screen
    global_stage = STAGE_DISCLAIMER;
    
    // Initialize controller manager.
    contPattern = nuContInit();
    
    // Initialize the EEPROM
    nuEepromMgrInit();
    
    // Set the default save
    global_eeprom_loaded = 0;
    SetDefaultSave();
    
    // Overwrite the default save if we have an EEPROM
    if (nuEepromCheck() == EEPROM_TYPE_4K || nuEepromCheck() == EEPROM_TYPE_16K)
    {
        // Tell the game that we managed to read the EEPROM
        global_eeprom_loaded = 1;
        
        // If our magic value isn't correct, then our EEPROM needs to be reset
        nuEepromRead(SAVESLOT_MAGIC, global_save_magic, 1);
        if (global_save_magic[0] != 342/2)
            SetDefaultSave();
        
        // Read the saved data
        nuEepromRead(SAVESLOT_MAGIC, global_save_magic, 1);
        nuEepromRead(SAVESLOT_UNLOCKS, global_save_unlocks, 1);
        nuEepromRead(SAVESLOT_SETTINGS, global_save_settings, 1);
        nuEepromRead(SAVESLOT_HIGHSCORES, global_save_highscores, 1);
        
        // Check if the second controller is plugged in
        if (!(CheckBit(contPattern,1)))
            global_save_settings[SAVE_SETTINGS_PLAYER] = 0;
        
        // Set the volume
        nuAuStlSndPlayerSetMasterVol(0x7FFF*((float)(0x0F - (global_save_settings[SAVE_SETTINGS_VOLUME] & 0x0F))/15));
        MusSetMasterVolume(MUSFLAG_SONGS, 0x7FFF*((float)(0x0F - (global_save_settings[SAVE_SETTINGS_VOLUME] >> 4))/15));
    }
   
    // Loop forever so the main will keep running
    while(1)
    {
        switch(global_stage)
        {
            case STAGE_DISCLAIMER:
                global_stage = -1; // Change stage to -1 to cause the loop to wait
                stage_disclaimer_init(); 
                nuGfxFuncSet((NUGfxFunc)callback_stage_disclaimer);
                nuGfxDisplayOn();
                break;
            case STAGE_MENU:
                global_stage = -1; // Change stage to -1 to cause the loop to wait
                stage_menu_init();
                nuGfxFuncSet((NUGfxFunc)callback_stage_menu);
                nuGfxDisplayOn();
                break;
            case STAGE_GAME1:
                global_stage = -1; // Change stage to -1 to cause the loop to wait
                stage_game1_init();
                nuGfxFuncSet((NUGfxFunc)callback_stage_game1);
                nuGfxDisplayOn();
                break; 
            case STAGE_GAME2:
                global_stage = -1; // Change stage to -1 to cause the loop to wait
                stage_game2_init();
                nuGfxFuncSet((NUGfxFunc)callback_stage_game2);
                nuGfxDisplayOn();
                break;
            default:
                break;
        }
      
        // Wait for the stage to change
        while(global_stage == -1)
            ;
            
        // Clear the display
        nuGfxDisplayOff();
    }
}


/*=================================
             initAudio
    Initialize the audio thread
=================================*/

void initAudio(void)
{
    musConfig c;

    c.control_flag	    = 0;                                // Set to MUSCONTROL_RAM if wbk file also placed in RAM
    c.channels		    = NU_AU_CHANNELS;                   // Maximum total number of channels
    c.sched		        = NULL;                             // The address of the Scheduler structure. NuSystem uses NULL
    c.thread_priority	= NU_AU_MGR_THREAD_PRI;             // Thread priority (highest)
    c.heap		        = (unsigned char*)NU_AU_HEAP_ADDR;  // Heap address
    c.heap_length	    = NU_AU_HEAP_SIZE;                  // Heap size
    c.ptr		        = NULL;                             // Allows you to set a default ptr file
    c.wbk		        = NULL;                             // Allows you to set a default wbk file
    c.default_fxbank	= NULL;                             // Allows you to set a default bfx file
    c.fifo_length	    = NU_AU_FIFO_LENGTH;                // The size of the library's FIFO buffer
    c.syn_updates	    = NU_AU_SYN_UPDATE_MAX;             // Specifies the number of updates usable by the synthesizer.
    c.syn_output_rate	= 44100;                            // Audio output rate. The higher, the better quality
    c.syn_rsp_cmds	    = NU_AU_CLIST_LEN;                  // The maximum length of the audio command list.
    c.syn_retraceCount	= 1;                                // The number of frames per retrace message
    c.syn_num_dma_bufs	= NU_AU_DMA_BUFFER_NUM;             // Specifies number of buffers Audio Manager can use for DMA from ROM.
    c.syn_dma_buf_size	= NU_AU_DMA_BUFFER_SIZE;            // The length of each DMA buffer

    // Initialize the Audio Manager.
    nuAuStlMgrInit(&c);

    // Register the PRENMI function.
    nuAuPreNMIFuncSet(nuAuPreNMIProc);
    
    // Read and register the sound effects bank and its pointers.
    Rom2Ram((void *)PBANK_SFX_START, (void *)ptr_buf, PBANK_SFX_END-PBANK_SFX_START);
    MusPtrBankInitialize(ptr_buf, WBANK_SFX_START);

    // Read and register the sound effects.
    Rom2Ram((void *)BANK_SFX_START, (void *)sfx_buf, BANK_SFX_END-BANK_SFX_START);
	MusFxBankInitialize(sfx_buf);
    
    // Read and register the instrument bank and its pointers.
    Rom2Ram((void *)PBANK_INSTR_START, (void *)ptr2_buf, PBANK_INSTR_END-PBANK_INSTR_START);
    MusPtrBankInitialize(ptr2_buf, WBANK_INSTR_START);
}


/*=================================
          vsyncCallback
This is a callback functions which
  execute instructions, and once
it has run out of instructions, it
  activates the display process.
=================================*/

void callback_stage_disclaimer(int pendingTaskCount)
{
    // Update the stage
    stage_disclaimer_update();

    // If we've run out of tasks, draw the stage
    if (global_stage == -1 && pendingTaskCount < 1)
        stage_disclaimer_draw();
}


void callback_stage_menu(int pendingTaskCount)
{
    // Update the stage
    stage_menu_update();

    // If we've run out of tasks, draw the stage
    if (global_stage == -1 && pendingTaskCount < 1)
        stage_menu_draw();
}


void callback_stage_game1(int pendingTaskCount)
{
    // Update the stage
    stage_game1_update();

    // If we've run out of tasks, draw the stage
    if (global_stage == -1 && pendingTaskCount < 1)
        stage_game1_draw();
}

void callback_stage_game2(int pendingTaskCount)
{
    // Update the stage
    stage_game2_update();

    // If we've run out of tasks, draw the stage
    if (global_stage == -1 && pendingTaskCount < 1)
        stage_game2_draw();
}

// Prevent crashing on resets
void callback_prenmi()
{
    nuGfxDisplayOff();
    osViSetYScale(1);
}