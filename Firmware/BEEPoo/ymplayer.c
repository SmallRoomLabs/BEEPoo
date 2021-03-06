//
// BEEPoo - Standalone AY-3-8912 player with ATMEGA32U4 with Arduino bootloader
//
// Copyright (c) 2014 SmallRoomLabs / Mats Engstrom - mats.engstrom@gmail.com
// 
// Hardware and Firmware released under the MIT License (MIT) than can be
// found in full in the file named LICENCE in the git repository.
//
// https://github.com/SmallRoomLabs/BEEPoo
//


//http://pastebin.com/myhLgp0v


//
//
// 00000000  59 4d 35 21 4c 65 4f 6e  41 72 44 21 00 00 36 9b  |YM5!LeOnArD!..6.|
// 00000010  00 00 00 01 00 00 00 1e  84 80 00 32 00 00 00 00  |...........2....|
// 00000020  00 00 4c 61 73 74 20 4e  69 6e 6a 61 00 4d 61 64  |..Last Ninja.Mad|
// 00000030  20 4d 61 78 00 47 65 6e  65 72 61 74 65 64 20 77  | Max.Generated w|
// 00000040  69 74 68 20 53 61 69 6e  54 00 a6 9e 96 96 9e a6  |ith SainT.......|
// 00000050  ae b6 ae a6 9e 96 96 9e  a6 ae b6 ae a6 9e 96 96  |................|
// 00000060  9e a6 ae b6 ae a6 9e 96  96 9e a6 ae b6 ae a6 9e  |................|
// 00000070  96 96 9e a6 ae b6 ae a6  9e 96 96 9e a6 ae b6 ae  |................|
//
//
//
// fileid 		59 4d 35 21 				YM5!
// check$		4c 65 4f 6e 41 72 44 21 	LeOnArD!
// frame# 		00 00 36 9b
// attributes 	00 00 00 01 
// drums# 		00 00 
// clock 		00 1e 84 80 
// framerate 	00 32 
// loopframe	00 00 00 00
// skip bytes 	00 00 
// song name 	4c 61 73 74 20 4e 69 6e 6a 61 00 
// author name 4d 61 64 20 4d 61 78 00 
// comment 	47 65 6e  65 72 61 74 65 64 20 77 69 74 68 20 53 61 69 6e 54 00 
// data 		a6 9e 96 96 9e a6 ae b6 ae a6 9e 96 96 9e a6 ae 
// 			b6 ae a6 9e 96 96 9e a6 ae b6 ae a6 9e 96 96 9e 
// 			a6 ae b6 ae a6 9e 96 96 9e a6 ae b6 ae a6 9e 96 
//
//
//
// 0 	LWORD		4 	File ID "YM6!"
// 4 	STRING[8]	8 	Check string "LeOnArD!"
// 12 	LWORD		4 	Nb of frame in the file
// 16	LWORD		4 	Song attributes
// 20 	WORD 		2 	Nb of digidrum samples in file (can be 0)
// 22 	LWORD		4 	YM master clock in Hz(2000000=ATARI-ST,  1773400=ZX-SPECTRUM)
// 26 	WORD		2 	Original player frame in Hz (traditionnaly 50)
// 28 	LWORD 		4 	Loop frame (traditionnaly 0 to loop at the beginning)
// 32 	WORD 		2 	Bytes of extra data to skip
//
// Then, for each digidrum: (nothing if no digidrum)
// 34 	LWORD 		4 	Sample size
// 38 	BYTES 		n 	Sample data (8 bits sample)
//
// Then some additionnal informations
// ? 	NT-String 	? 	Song name
// ?	NT-String 	?	Author name
// ?	NT-String	?	Song comment
// ?	BYTES 		?	YM register data bytes. (r0,r1,r2....,r15 for each frame). Order depend on the "interleaved" bit. It takes 16*nbFrame bytes.
//
// ?	LWORD		4 	End ID marker. Must be "End!"






 // YM

 // Emulator: StSound, SainT, PaCifiST, Arnold, CPCe
 // Author: Arnaud Carre (Leonard/OXYGENE)

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // Leonard/OXYGENE designed this format for his AY-emulator StSound.
 // At the beginning this emulator was an Atari ST sound chip emulator, but nowadays
 // this is a Multi-Computer Sound Emulator able to play AY/YM sounds and some
 // Amiga/Paula chip modules.
 // Now this project consists of many programs (players, converters) and archives of
 // music. But Spectrum-archives are in very bad condition. Don't forget to set chip
 // frequency to 2.000.000 Hz for Atari and to 1.000.000 Hz for Amstrad while listening
 // YM2 and YM3 files with Atari and Amstrad music (this subtypes do not care about
 // frequency settings).

 // A AY/YM-file consist of an LHA-archive (type 5) created by the LhA.exe by Haruyasu
 // Yoshizaki with the -h0 switch enabled. This will create an "H0" Header (Type 5).

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM2!

 // First four bytes is the ASCII identifier "YM2!".
 // AY Emulator doesn't know the difference between YM2 and YM3 types.

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM3!

 // First four bytes is again the ASCII identifier "YM3!".

 // ------------------------------------------------------
 // Offset	Size    Name    Value   Contents
 // ------------------------------------------------------
 // 0      4       ID      "YM3!"  File type Identificator


 // The next bytes are the data block of AY chip registers values.

 // Registers are updates one time per VBL interrupt. If music length is N interrupts,
 // then block consist first N bytes for register 0, further N bytes for register 1
 // and so on. In total: N*14 bytes. The number of used VBL for music can be computed
 // as follow: nvbl = (ymfile_size-4)/14;

 //  VBL1:
 //    store reg0,reg1,reg2,...,reg12,reg13  (14 regs)
 //  VBL2:
 //    store reg0,reg1,reg2,...,reg12,reg13  (14 regs)
 //        ..........
 //  VBLn:
 //    store reg0,reg1,reg2,...,reg12,reg13  (14 regs)


 // If the current interrupt features no output to register 13 then the byte of the
 // data block for this interrupt and for this register has the value 255 ($FF).

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM3b!

 // This format is nearly identical with YM3. It adds only the ability to use loops.

 // First four bytes is the ASCII identifier "YM3b".
 // The following bytes are the data block (see YM3 description).
 // Last four bytes is a DWORD (32bits integers) data and contains the frame number
 // at wich the loop restart. That's all.

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM4!

 // Legend: BYTE            8 bits integer
 //         WORD           16 bits integer
 //         DWORD          32 bits integer
 //         String[n]      ASCCI string of n bytes length
 //         NT-String      NULL-terminated string


 // WARNING: All DWORD or WORD are stored in MOTOROLA order in the file (INTEL reverse)

 // ----------------------------------------------------------------
 // Offset	Size	Type	    	Comment
 // ----------------------------------------------------------------
 // 0	4	DWORD		ID of YM4 format. ('YM4!')
 // 4	8	String[8]	Check String ('LeOnArD!')
 // 12	4	DWORD		Nb of valid VBL of the file
 // 16	4	DWORD		Song attributes (see below)
 // 20	2	DWORD		Nb of digi-drum sample (can be 0)
 // 24	4	DWORD		Frame loop start (generally 0)


 // For each digidrum sample:

 // {
 // .. ?? DWORD sample size
 // .. ?? BYTES sample data (8bits per sample)
 // }

 // .. ? NT-String Name of the song
 // .. ? NT-String Name of the author
 // .. ? NT-String Comments (Name of the YM converter !)
 // .. ?  All YM2149 registers
 // .. 4 DWORD End-File check ('End!')

 // So you've seen in the YM3 format that all 14 registers of the YM2149 are saved in file
 // each VBL. Now, I had to add 2 "virtual" registers to store extended information.
 // So there is 16 bytes in the file for each VBL. Now the VBL number n will be noticed "Vn"
 // and register m "Rm". First VBL is V0 and first register is R0. (R0 to R15)

 //        The file data block will be composed of:

 //        V0R0,V0R1,V0R2,....,V0R14,V0R15
 //        V1R0,V1R1,V1R2,....,V1R14,V1R15
 //        .....
 //        This is the non-interleaved format block.



 //        The data block can be on a different form:

 //        V0R0,V1R0,V2R0,....,VnR0
 //        V0R1,V1R1,V2R1,....,VnR1
 //        ....
 //        V0R15,V1R15,V2R15,....,VnR15

 //        This is the interleaved format block.


 // Those who are reading till now have noticed that the YM3 format is an interleaved format.
 // In fact, the YM4 format can be interleaved or not (Interleaved format offers a very powerfull
 // compression ratio when compressed with LHA).


 //        Let's see the DWORD "song attribute":
 //        (bn represent the bit n of the DWORD)

 //        b0:     Set if Interleaved data block.
 //        b1:     Set if the digi-drum samples are signed data.
 //        b2:     Set if the digidrum is already in ST 4 bits format.

 //        b3-b31: Not used yet, MUST BE 0.

 //        Here come valid bits for standard 14 first registers:
 //        (Same as YM3 Format.)
 //        (- means Unused, X means used)

 //        NOTE: Now, digi-drum will be noticed DD and Timer-Synth will be
 //              noticed TS.


 //        -------------------------------------------------------
 //              b7 b6 b5 b4 b3 b2 b1 b0
 //         r0:  X  X  X  X  X  X  X  X   Period voice A
 //         r1:  -  -  -  -  X  X  X  X   Period voice A
 //         r2:  X  X  X  X  X  X  X  X   Period voice B
 //         r3:  -  -  -  -  X  X  X  X   Period voice B
 //         r4:  X  X  X  X  X  X  X  X   Period voice C
 //         r5:  -  -  -  -  X  X  X  X   Period voice C
 //         r6:  -  -  -  X  X  X  X  X   Noise period
 //         r7:  X  X  X  X  X  X  X  X   Mixer control
 //         r8:  -  -  -  X  X  X  X  X   Volume voice A
 //         r9:  -  -  -  X  X  X  X  X   Volume voice B
 //        r10:  -  -  -  X  X  X  X  X   Volume voice C
 //        r11:  X  X  X  X  X  X  X  X   Waveform period
 //        r12:  X  X  X  X  X  X  X  X   Waveform period
 //        r13:  -  -  -  -  X  X  X  X   Waveform shape
 //        -------------------------------------------------------
 //        New "virtual" registers to store extra data:
 //        -------------------------------------------------------
 //        r14:  -  -  -  -  -  -  -  -   Frequency for DD1 or TS1
 //        r15:  -  -  -  -  -  -  -  -   Frequency for DD2 or TS2


 // As we've seen, r13 has a particular status. If the value stored in the file is 0xff,
 // YM emulator will not reset the waveform position.

 // The YM4 format provides to start or run 1 TS and 1 DD during the same VBL.

 // You notice there is a least 28 free bits in the 14 standard registers. YM4 format use
 // them to store "extra" information. Let's see the "extra" bits:


 //        r1 free bits are used to code TS:
 //        r1 bits b5-b4 is a 2bits code wich means:

 //        00:     No TS.
 //        01:     TS running on voice A
 //        10:     TS running on voice B
 //        11:     TS running on voice C


 // r1 bit b6 is only used if there is a TS running. If b6 is set, YM emulator must restart
 // the TIMER to first position (you must be VERY sound-chip specialist to hear the difference).


 //        r3 free bits are used to code a DD start.
 //        r3 b5-b4 is a 2bits code wich means:

 //        00:     No DD
 //        01:     DD starts on voice A
 //        10:     DD starts on voice B
 //        11:     DD starts on voice C


 // WARNING:
 // If a DD starts on voice V, the volume register corresponding to V (Ex r8 for voice A,
 // r9 for B and r10 for C) contains the sample number in 5 low bits (That mean you have
 // 32 digiDrum max in a song).

 // Concerning DD and TS, both effects need to be played at a given frequency. On the ATARI-ST,
 // play routine use the MFP-TIMER chip. That is, any frequency can be coded on 11bits:

 // 8bits for timer count, and 3bits for timer predivisor. The MFP runs at 2457600 Hz and has
 // a preset of predivisor.


 //        000:    Timer stop.
 //        001:    Prediv by 4
 //        010:    Prediv by 10
 //        011:    Prediv by 16
 //        100:    Prediv by 50
 //        101:    Prediv by 64
 //        110:    Prediv by 100
 //        111:    Prediv by 200

 //        The 8 bits timer count value is named TC and 3bits prediv value
 //        is named TP.

 //        Some ex to endserstand how it works:

 //        TP      TC      Frequency (Hz)
 //        1       28      (2457600/4)/28   = 21942 Hz
 //        4       150     (2457600/50)/150 = 327 Hz


 // Encode:
 //        TP for TS is stored in the 3 free bits of r6 (b7-b5)
 //        TP for DD is stored in the 3 free bits of r8 (b7-b5)
 //        TC for TS is stored in the 8 bits of r14
 //        TC for DD is stored in the 8 bits of r15

 //        4bits volume value (vmax) for TS is stored in the 4 free bits of r5 (b7-b4)

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM5!

 // This is the actual and most common used format and consist consists of additional information:
 // chip frequency, player frequency, title, author name, comment and specific Atari ST data
 // (Digi-Drum and SID effects). In AY_Emulator this additional Atari effects are not supported.


 // The AY/YM5 file format:

 // Offset Size Type Comment


 // 22 4 DWORD
 // (ex:2000000 for ATARI-ST version,
 // 1000000 for AMSTRAD CPC)
 // 26 2 WORD Player frequency in Hz
 // (Ex: 50Hz for almost player)
 // 28 4 DWORD VBL number to loop the song. (0 is default)
 // 32 2 WORD Size (in bytes) of future additinal data.
 // (must be 0 for the moment)

 // ----------------------------------------------------------------
 // Offset	Size	Type	    	Comment
 // ----------------------------------------------------------------
 // 0	4	DWORD		ID of YM4 format. ('YM4!')
 // 4	8	String[8]	Check String ('LeOnArD!')
 // 12	4	DWORD		Nb of valid VBL of the file
 // 16	4	DWORD		Song attributes (see below)
 // 20	4	DWORD		Nb of digi-drum sample (can be 0)
 // 22	4	DWORD		YM2149 External frequency in Hz (2000000 = ATARI-ST, 1000000 = AMSTRAD CPC)
 // 26	2	WORD		Player frequency in Hz (Ex: 50Hz for almost every player)
 // 28	4	DWORD		VBL number to loop the song (0 is default)
 // 32	2	WORD		Size (in bytes) of future additinal data (must be 0 for now)


 // For each digidrum sample:

 // {
 // .. ?? DWORD sample size
 // .. ?? BYTES sample data (8bits per sample)
 // }
 // .. ? NT-String Name of the song
 // .. ? NT-String Name of the author
 // .. ? NT-String Comments (Name of the YM converter !)
 // .. ?  All YM2149 registers
 // .. 4 DWORD End-File check ('End!')


 // Data block contents now values for 16 registers (14 AY registers plus 2 virtual registers
 // for Atari special effects). If bit 0 of field Song Attributes is set, data block are stored
 // in YM3-style order (interleaved). If this bit is reset, then data block consists first
 // 16 bytes of first VBL, then next 16 bytes for second VBL and so on. In second case YM5 file
 // is compressed more badly.

 // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 // YM6!

 // This format is equivalent to YM5! but can use yet another special Atari effect.
 // More info about YM files can be found in the official ST-Sound package or on the ST-Sound
 // Project Homepage: http://leonard.oxg.free.fr

#include <Arduino.h>
#include "pff.h"
#include "timer1.h"

#define YMFRAMESIZE	16
#define BLOCKSIZE	512

#define MAXINFOLEN  13

static uint8_t buf[BLOCKSIZE];         	// File read buffer
static uint8_t frame[YMFRAMESIZE];      // Sound frame to send to AY
static uint8_t lastFrame[YMFRAMESIZE];  // Last sound frame to sent to AY


typedef struct {
  char      fileId[4];
  char      check[8];
  uint32_t  frames;
  uint32_t  attributes;
  uint16_t  drums;
  uint32_t  clock;
  uint16_t  rate;
  uint32_t  loopframe;
  uint16_t  skip;
} YM_HDR1_t; 


inline static  uint16_t E16(uint16_t x) {
    return (x>>8) | (x<<8);
}

inline static  uint32_t E32(uint32_t x) {
    return (E16(x&0xffff)<<16) | (E16(x>>16));
}

//
//
//
void YmPlay(char *filename) {
  FRESULT res;
  unsigned int br;
  uint16_t frameNo;
  uint16_t lastTick;
  uint16_t  rp;
  uint8_t   wp;
  uint16_t cnt;
  YM_HDR1_t *ym_hdr1;
  uint32_t ymFrames;
  uint32_t ymAttributes; 
  uint16_t ymDrums;
  uint32_t ymClock;
  uint16_t ymRate; 
  uint32_t ymLoop;
  uint16_t ymSkip;



  LcdClear();
  LcdXY(0,0);LcdString("Open=");
  res=pf_open(filename);
  LcdCharacter(48+res);

  // Get meta data from part of first block
  res=pf_read(buf, sizeof(buf), &br);    
  ym_hdr1=&buf;

  ymFrames=E32(ym_hdr1->frames);
  ymAttributes=E32(ym_hdr1->attributes); 
  ymDrums=E16(ym_hdr1->drums);
  ymClock=E32(ym_hdr1->clock);
  ymRate=E16(ym_hdr1->rate); 
  ymLoop=E32(ym_hdr1->loopframe);
  ymSkip=E16(ym_hdr1->skip);
  rp=sizeof(YM_HDR1_t);

  LcdPrintUint16(ymDrums, 1);


  // Song name
  LcdXY(0,1);
  cnt=0;
  do {
    if (cnt++<MAXINFOLEN) LcdCharacter(buf[rp]);
    rp++;
  } while (buf[rp]);
  
  // Author name
  LcdXY(0,2);
  rp++;
  cnt=0;
  do {
    if (cnt++<MAXINFOLEN) LcdCharacter(buf[rp]);
    rp++;
  } while (buf[rp]);
  rp++;
  cnt=0;
 
  // Comments
  LcdXY(0,3);
  do {
    if (cnt++<MAXINFOLEN) LcdCharacter(buf[rp]);
    rp++;
  } while (buf[rp]);
  rp++;
  cnt=0;

  // Now start sending data frames to AY-chip
  wp=0;
  lastTick=timer1Tick;
  for (frameNo=0; frameNo<ymFrames; frameNo++) {
    for (wp=0; wp<YMFRAMESIZE; wp++) {
      if (rp==BLOCKSIZE) {
        res=pf_read(buf, sizeof(buf), &br);
        rp=0;
      }
      frame[wp]=buf[rp++];
    }

    // Wait for a new 50 Hz tick to occur
    while (lastTick==timer1Tick);
    lastTick=timer1Tick;
    // Send only the updated frame parts to the AY chip
    for (wp=0; wp<YMFRAMESIZE-2; wp++) {  // Skip R14/R15
      if (lastFrame[wp]!=frame[wp]) {
        lastFrame[wp]=frame[wp];
        // Ignore R13 updates that is FF as according to 
        // http://www.atari-forum.com/viewtopic.php?f=68&t=4340
        if (wp!=13 || frame[wp]!=0xFF) AyRegister(wp, frame[wp]);
      }
    }
  
    LcdXY(0,6);
    LcdPrintUint16((uint16_t)ymFrames, 1);
    LcdTinyDigit(':');
    LcdPrintUint16(frameNo, 1);
    LcdTinyDigit(':');
    frameNo++;
  }
}
