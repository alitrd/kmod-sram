#ifndef SRAM
#define SRAM

#ifdef __cplusplus
extern "C" {
#endif

#define SRAMNAME "sram"
#define SRAMBASE ((void *) 0xA0000000)
#define SRAMSIZE 0x00004000
#define SRAMMAJO 0

#define MMUBASE ((void *) 0x7FFFC000)
#define MMUSIZE 0x00004000

typedef struct {
  unsigned int in;
  unsigned int option;
  unsigned int adr;
  unsigned int out;
} sram_data_s;

#ifdef __cplusplus
}
#endif

#endif
