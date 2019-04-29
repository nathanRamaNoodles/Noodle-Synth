/*
   Written by Eric Paquot on 03/03/2019
  
   Song in RTTL format to play with 'MusicWithoutDelay' from nathanRamaNoodles
   We use 2 tracks, one for the soprano, the second one for the bass
   Both tracks must have the same length, and to prevent lost of synchro,
      both tracks MUST be written with the same duration (ie d=8)
   For details on the RTTL format, look at https://github.com/nathanRamaNoodles/MusicWithoutDelay-LIbrary documentation

   Header (optional):
      "name:d=..,o=..,b=..,s=..,f=..:"    (d)uration, (o)ctave, (b)pm, (s)harp, (f)lat
   Partition, for each note:
      1 2 3...      duration        (optional)
      .             period          (optional)
      a b... g p    letter          (!!!required)
      # _           sharp / flat    (optional)
      1 2...        octave          (optional)
      , +           breath / slur   (!!!required)
*/


#include <avr/pgmspace.h>



const char BEEP_01[] PROGMEM = "::8c";



//*
// ==== M Jarre - Lara's theme ====
//
//    Soprano                                      1             2   3           4   5        6   7            8   9           10  11          12  13       14  15          16  17      18        19      20  21       22          23       24   25        26      27      28  29             30             31      35          36  37          38  39       40  41         42       43  44
const char SONG_31[] PROGMEM = ":d=8,o=5,b=140,s=f:p,4.b-1,4d,a#,2.b,f,a,g,4d,c#,2.c,4.c,4d,e,2.f,e,d,c#,4d,c1,2.b,4.b-1,4d,a#,2.b,f,a,g,4d,c#,2.c,4.c,4d,e,2.f,e,d#,d,4c,a,2.g,4.e,4.g+4.g,a,g,f,4.g,4.e,2.d,4.d,4.f_+4f_,f,g,f,g,4.a,4.a#,2.c1,4.d,4d,d#,4.d,4.f,2g+g,g#,2.g,g,g#,g,f_,d#,d,d#,f_,d#,d,c,c,2g-1,4p,4.g-1,4d,a#,2.b,f,a,g,4d,c#,2.c,4.c,4d,e,2.f,e,d,c#,4.c+4.c,4.c1,2.b+2.b";
//
//    Bass (same base duration for synch)          1            2   3              4     5            6   7           8   9          10  11             12    13           14  15             16    17      18          19      20    21         22         23      24  25                 26      27             28  29             30             31            35          36  37          38  39       40  41         42      43  44
const char SONG_32[] PROGMEM = ":d=8,o=4,b=140,s=f:p,4.b-1,4d,f,2.g,d,g,d,4a-1,a-1,2.a-1,4.a-1,4b-1,d,2.d,e,d,c#,4d,a,2.g,4.b-1,4d,f,2.g,d,g,d,4b-1,b-1,2.a-1,4.a-1,4b-1,c,2.d,c,c,c,4a-1,a-1,2.g-1,4.c,4.e+4.e,f_,e,d#,4.e,4.c,2.b-1,4.b_-1,4.d+4d,f,g,f,g,4.a,4.g,2.a,d,a#-1,a-1,4g-1,d#,4.d+4.d,g-1,d#,d,4c,g#,2.g,g,g#,g,f_,d#,d,d#,f_,d#,d,c,c,b-1,d,d#,4.f_,4.b-1,4d,a#,2.b,f,a,g,4d,c#,2.c,4.c,4d,e,2.f,e,d,c#,4.c+4.c,4.a,2.g+2.g";
//*/

//*
// ==== S Joplin - The entretainer ====
//
//    Soprano                                  x    5              6             7                   8         9                10         11                     12        13             14            15                  16         17                    18                    19                  20
const char SONG_41[] PROGMEM = ":d=16,o=4,b=80:d,d#,e,8c1,e,8c1,e,4.c1,c2,d2,d#2,e2,c2,d2,8e2,b1,8d2,4.c2,d,d#,e,8c1,e,8c1,e,c1+4.c1,a1,g1,f#1,a1,c2,8e2,d2,c2,a1,4.d2,d,d#,e,8c1,e,8c1,e,4.c1,c2,d2,d#2,e2,c2,d2,8e2,b1,8d2,4.c2,c2,d2,e2,c2,d2,8e2,c2,d2,c2,e2,c2,d2,8e2,c2,d2,c2,e2,c2,d2,8e2,b1,8d2,4.c2";//,d,d#";
//
//    Bass (same base duration for synch)      x  5             6             7                8             9             10            11            12          13            14            15               16            17              18             19             20
const char SONG_42[] PROGMEM = ":d=16,o=3,b=80:8p,8c,8c1,8g,8c1,8f,8c1,8e,8c1,8g-1,8c1,8g-1,8b,8c,8c1,8c1,8b,8c,8c1,8g,8c1,8f,8c1,8e,8e_,8d,8c1,8d,8c1,8b,8g,8a,8b,8c,8c1,8g,8c1,8f,8c1,8e,8c1,8g-1,8c1,8g-1,8b,8c,8c1,8e1,8p,8c1,8e1,8b_,8e1,8a,8f1,8a_,8f1,8g,8e1,8g-1,8b,8c1,8g,8a";//,8b";
//*/


// Zelda
const char SONG_61[] PROGMEM = ":d=16,o=5,b=160,f=aeb:2b,12p,12p,12b,12b,12b,12b,8.b,16a,4b,12p,12p,12b,12b,12b,12b,8.b,16a,4b,12p,12p,12b,12b,12b,12b,8b,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,   4b,4f+8.f,b,b,c1,d1,e1,2f1,8p,8f1,12f1,12g_1,12a1,   2b1,12p,12b1,12b1,12b1,12a1,12g_1,8.a1,g_1,2f1,4f1,8e1,e1,f1,2g_1,8f1,8e1,8d_1,d_1,e1,2f1,8e1,8d_1,8c1,c1,d1,2e#1,4g1,8f1,f,f,8f,f,f,8f,f,f,8f,8f,  4b,4f+8.f,b,b,c1,d1,e1,2f1,8p,8f1,12f1,12g_1,12a1,  2b1,4p,4d_2,4c2,4a#1,4p,4f1,2g_1,4p,4b1,4a#1,4f1,4p,4f1,2g_1,4p,4b_1,4a#1,4f1,4p,4d1,2e1,4p,4g_1,4f1,4d_1,4p,4b,8c1,16c1,16d1,4e#1,4p,4g1,8f1,16f,16f,8f,16f,16f,8f,16f,16f,8f,8f,   1b,1p";
const char SONG_62[] PROGMEM = ":d=16,o=4,b=160,f=aeb:4b,12b,12b,12b,4b,12b,12b,12b,4a,12a,12a,12a,4a,12a,12a,12a,4g_,12g_,12g_,12g_,4g_,12g_,12g_,12g_,4f,4f,4f,8g,8a#,   4d,12d,12d,12c,8.d,d,d,e,f,g,8.a,b,b,c1,d1,e1,4f1,12a,12b,12c1,   8.d_1,g_,g_,a,b,c1,12d_1,12p,12d_1,12d_1,12c1,12b,8.d_,a,12a,12a,12g_,8.a,a,12a,12g_,12a,8g_,g_,f,8g_,g_,a,4b,8a,8g_,8f,f,e,8f,f,g_,4a,8g_,8f,4e#,8e#,e#,f,8g,g,a#,8b,8c1,8a#,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,a#-1,a#-1,8a#-1,8a#-1,  4d,12d,12d,12c,8.d,d,d,e,f,g,8.a,b,b,c1,d1,e1,4f1,12a,12b,12c1,  2b,4p,4e#1,4e#1,4c1,4p,4a#,12e#-2,12b-2,12d_-1,12e#-1,12b-1,12d_,4e#,4p,4f,12f-2,12f-2,12f-2,4f-2,4p,12e#-2,12b-2,12d_-1,12e#-1,12b-1,12d_,4e#,4p,4f,12f-2,12f-2,12f-2,4f-2,4p,2g_,4p,4c_1,4b,4f,4p,4d_,4e#,8e#,16e#,16f,8g,16g,16a#,8b,8c1,8a#,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,16a#-1,16a#-1,8a#-1,8a#-1,  1d-1,1p";

//Spider Dance
const char SONG_71[] PROGMEM = ":b=115,d=16,f=adeb,o=5:8c1,8a,8f,8g,  p,a,8f,  g,f_,b,d,  p,a,f,e,  g,a,g-1,a-1,c,a-1,g-1,  a,a,8p,16p,   8p,8b-1,8p,8b-1,8p,8b-1,8p,8a-1,  8a,8d,8c1,8f,  8b,8a,4g";
const char SONG_72[] PROGMEM = ":b=115,d=16,f=adeb,o=3:f,c1,b,c1,f,c1,b,c1,d,c_1,b,c_1,c,c1,g,b,  f,c1,b,c1,  g,c1,b,c1,  a,c1,b,c1,  b,d1,c1,d1,   d,a,g,a, d,b,a,b,  d,c1,b,c1,  d,d1,c1,d1, 4c, 8p,8c,4c,  c1,d#1,e#1";



#define   SONG_TABLE_MAX  4
const byte* const SONG_TABLE[][2] PROGMEM = {
  {SONG_31, SONG_32},     // ==== M Jarre - Lara's theme ====
  {SONG_41, SONG_42},     // ==== S Joplin - The entretainer ====
  {SONG_61, SONG_62},     // Zelda
  {SONG_71, SONG_72},     // Spider Dance
};














