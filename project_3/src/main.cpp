#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Need to supply LLVM IR file!\n";
        return 1;
    }
    return 0;
}
