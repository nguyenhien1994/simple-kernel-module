#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE_PATH "/dev/reverse_string"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Error opening input file " << argv[1] << ", " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::string input;
    std::getline(infile, input);

    infile.close();

    int fd = ::open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        std::cerr << "Error opening device file " << DEVICE_PATH << ", " << std::strerror(errno) << std::endl;
        return 1;
    }

    ssize_t n = ::write(fd, input.c_str(), input.length());
    if (n < 0) {
        std::cerr << "Error writing to device file " << DEVICE_PATH << ", " << std::strerror(errno) << std::endl;
        ::close(fd);
        return 1;
    }

    std::string output(input.length(), '\0');
    n = ::read(fd, &output[0], input.length());
    if (n < 0) {
        std::cerr << "Error reading from device file " << DEVICE_PATH << ", " << std::strerror(errno) << std::endl;
        ::close(fd);
        return 1;
    }

    std::cout << "Input: " << input << std::endl;
    std::cout << "Output: " << output << std::endl;

    ::close(fd);

    return 0;
}
