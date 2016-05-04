KERNELDIR=/home/jkl/sram/dcb/linux-2.4.21

CROSSCC = arm-linux-gcc
CFLAGS  = -O -Wall
KCFLAGS = -D__KERNEL__ -DMODULE -I$(KERNELDIR)/include -O -Wall

kobjects = sram.o

all : $(kobjects)

sram.o: sram.c sram.h
	$(CROSSCC) $(KCFLAGS) -c -o $@ $<

clean:
	rm -f $(kobjects)

.PHONY: ipkg
ipkg:
	fakeroot ipkg-buildpackage
