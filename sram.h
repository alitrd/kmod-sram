#ifndef SCOM3
#define SCOM3

#ifdef __cplusplus
extern "C" {
#endif

#define SRAMNAME "dcs_sram3"
#define SRAMWDTNAME1 "dcs_sram3_wdt1"
#define SRAMWDTNAME2 "dcs_sram3_wdt2"
#define WDTSTRING "alive"
#define SRAMBASE ((void *) 0x80000080)
#define SRAMSIZE 0x0000003F
#define SRAMMAJO 0
#define SRAMWIDTH 8

#define MMUBASE ((void *) 0x80000080)
#define MMUSIZE 0x0000003F

#define SRAM_OUT_CH1 1
#define SRAM_OUT_CH2 2
#define SRAM_OUT_CH3 3
#define SRAM_OUT_CH4 4
#define SRAM_OUT_CH5 5
#define SRAM_OUT_CH6 6
#define SRAM_OUT_CH7 7
#define SRAM_OUT_CH8 8
#define SRAM_OUT_CH9 9

typedef struct {
  unsigned int in;
  unsigned int option;
  unsigned int adr;
  unsigned int out;
} sram_data_s;

#define TK_BITSET(reg,bitnum)      reg=((reg)|(1<<bitnum))
#define TK_BITCLEAR(reg,bitnum)    reg=((reg)&(~(1<<bitnum)))

#ifdef __cplusplus
}
#endif

#endif
