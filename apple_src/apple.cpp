#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

int main() {
    std::ifstream apple("/badapple.txt");
    if(!apple.is_open()) {
        std::cout << "apple: FATAL! Couldn't open the input file for reading. :(" << std::endl;
        return 1;
    }
    std::string line;
    std::string screen_clear = "\x1B[H\x1B[J";
    std::cout << screen_clear;
    while(std::getline(apple, line)) {
        if(line == "--") {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << screen_clear;
        } else {
            std::cout << line << std::endl;
        }
    }
    std::cout << "All done!" << std::endl;
}
