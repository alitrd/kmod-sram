# kmod-sram
`kmod-sram` provides a kernel module (targeted at Linux 2.4.21 on ARM) to access the SRAM-0 on the ALICE (TRD) DCS boards which survives a reboot. The memory (16 kiB) is exposed as the character device `/dev/sram`.

## build
The kernel module can be built using make:
```
make
```
To build an ipkg use:
```
make ipkg
```

# Disclaimer
This kernel module is still in the early testing phase.
