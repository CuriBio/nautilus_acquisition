#include "PMemCopy.h"
#include <chrono>
#include <thread>


int main(int argc, char* argv[]) {
    PMemCopy p(6);

    auto a = new uint8_t[100*1024*1024];
    auto b = new uint8_t[100*1024*1024];

    printf("here\n");
    p.Copy(a, b, 100*1024*1024);
    p.Copy(b, a, 100*1024*1024);
    return 0;
}
