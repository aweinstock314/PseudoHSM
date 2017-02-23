# PseudoHSM
## Installation
To install:
```sh
make
sudo insmod PseudoHSM.ko
```
To remove:
```sh
sudo rmmod PseudoHSM
```
## Cache enable/disable
```sh
sudo sh -c 'echo 0 > /dev/set_cpu_cache' # disable
sudo sh -c 'echo 1 > /dev/set_cpu_cache' # enable
```
- no noticable slowdown in QEMU-KVM, either that doesn't emulate cache, or this is broken
- TODO: implement `file_operations::read`
- TODO: test in Bochs?

## Encryption
2-byte invocation of crypto primitives with a key set in advance (Not yet fully implemented)

To set key:
```sh
sudo sh -c 'echo "KEY_GOES_HERE" > /dev/pseudohsm'
```
To encrypt/decrypt (fetch a CTR block?) (registers for data not yet chosen):
```asm
int $0x22
```
