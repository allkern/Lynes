#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int find_difference_index(std::string& a, std::string& b) {
    for (size_t i = 0; i < a.size(); i++) if (a[i] != b[i]) return i;

    return -1;
}

std::vector <std::string> lytest_results, nestest_results;

void load_files() {
    std::ifstream lytest, nestest;

    lytest.open("lynes.log", std::ios::in);
    nestest.open("nestest.log", std::ios::in);

    std::string str;

    while (lytest.peek() != EOF) { std::getline(lytest, str); lytest_results.push_back(str); }
    while (nestest.peek() != EOF) { std::getline(nestest, str); nestest_results.push_back(str); }

    lytest.close();
    nestest.close();
}

void compare_files() {
    size_t m = std::min(lytest_results.size(), nestest_results.size());

    for (int i = 0; i < m; i++) {
        int di = find_difference_index(lytest_results.at(i), nestest_results.at(i));

        if (di >= 0) {
            std::cout << "Error in execution!\n";
            std::cout << "Lynes:\t\t\t\t\t\tNES:\n";
            std::cout << "|: " << lytest_results.at(i) << "\t" << nestest_results.at(i) << std::endl;
            std::cout << "-> " << lytest_results.at(i) << "\t" << nestest_results.at(i) << std::endl;
            std::cout << std::string(di + 1, ' ') << "^\n";

            return;
        }
    }

    std::cout << "Congratulations! nestest.nes passed" << std::endl;
}

int main() {
    load_files();

    compare_files();
}
