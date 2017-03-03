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
### Writing to the cache control bit
```sh
sudo sh -c 'echo -n 0 > /dev/cachectl' # disable
sudo sh -c 'echo -n 1 > /dev/cachectl' # enable
```
### Reading the cache control bit
```sh
sudo sh -c 'xxd /dev/cachectl'
```

- no noticable slowdown in QEMU-KVM, either that doesn't emulate cache, or this is broken
- Tested reading in QEMU, reading `/dev/cachectl` always shows 1 (even after setting it to 0)
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
