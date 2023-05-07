## Simple kernel module for Linux & Windows that can accept a string from the user-space application and then return the string in reverse.

### Install kernel headers and build tools:

```
sudo apt-get install build-essential linux-headers-$(uname -r)
```

### How to build and run the example:

- Clone the source code
```
git clone git@github.com:nguyenhien1994/simple-kernel-module.git
cd simple-kernel-module
```
- Build kernel module and test program with `make`
- Insert kernel module into the Linux Kernel `sudo insmod module/reverse_string.ko`
- Update your test string to `./test/test.txt`
- Run the test program with `sudo ./test/test ./test/test.txt`
- Remove module after using `sudo rmmod module/reverse_string.ko`

- **All above steps are done via `run.sh` script. Just run this script with `bash run.sh`**

### Tests
- With small string size:
```
hiennm@hiennm-sb-virt:~/projects/github/simple-kernel-module$ sudo ./test/test ./test/test.txt
Input: Hien Nguyen Minh
Output: hniM neyugN neiH
```

- With large string size (2KB):
```
hiennm@hiennm-sb-virt:~/projects/github/simple-kernel-module$ sudo ./test/test ./test/test.txt
Input: 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
Output: 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
```

### Improvement?
- Handle file with multiple lines?
- Load/unload in the userspace program with `system()`/`libkmod`
- Use mmap or netlink?
- Since the module is only test on `4.19.0-17-amd64`, so maybe a bug on other kernel versions.
