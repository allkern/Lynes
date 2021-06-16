#include <iostream>
#include <fstream>
#include <string>

int find_difference_index(std::string& a, std::string& b) {
    for (size_t i = 0; i < a.size(); i++) if (a[i] != b[i]) return i;

    return -1;
}

int main() {
    std::ifstream lytest, nestest;

    lytest.open("lynes.log");
    nestest.open("nestest.log");

    bool equal = true;

    std::string a, b, pa, pb;

    while (!lytest.eof()) {
        pa = a;
        pb = b;

        std::getline(lytest, a);
        std::getline(nestest, b);

        int di = find_difference_index(a, b);

        if (di >= 0) {
            std::cout << "Error in execution!\n";
            std::cout << "Lynes:\t\t\t\t\t\tNES:\n";
            std::cout << "|: " << pa << "\t" << pb << std::endl;
            std::cout << "-> " << a << "\t" << b << std::endl;
            std::cout << std::string(di + 1, ' ') << "^\n";

            return 0;
        }
    }
}
