### Simple kernel module for Linux & Windows that can accept a string from the user-space application and then return the string in reverse.

1. Install kernel headers and build tools:

```
sudo apt-get install build-essential linux-headers-$(uname -r)
```

2. How to build and run the example:

- Clone the source code
```
git clone ...
```

- Build kernel module and test program with `make`
- Insert kernel module into the Linux Kernel `sudo insmod module/reverse_string.ko`
- Update your test string to `./test/test.txt`
- Run the test program with `sudo ./test/test ./test/test.txt`
- Remove module after using `sudo rmmod module/reverse_string.ko`

- **All above steps are done via `run.sh` script. Just run this script with `bash run.sh`**

3. Improvement?
- Handle file with multiple lines?
- Load/unload in the userspace program with `system()`/`libkmod`
- Use mmap or netlink?
