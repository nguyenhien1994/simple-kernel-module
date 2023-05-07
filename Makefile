KERNEL_SRC := module
TEST_SRC := test

CC := g++
CFLAGS := -Wall -Werror

all: build_kernel build_test_app

build_kernel:
	$(MAKE) -C $(KERNEL_SRC)

build_test_app:
	$(CC) $(TEST_SRC)/test.cpp $(CFLAGS) -o $(TEST_SRC)/test

clean:
	$(MAKE) -C $(KERNEL_SRC) clean
	rm -f $(TEST_SRC)/test
