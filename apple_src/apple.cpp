#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

int main() {
    std::ifstream apple("badapple.txt"); // FIXME: FATAL: TODO: IMPORTANT: README: DANGER:
    if(!apple.is_open()) {
        std::cout << "apple: FATAL! Couldn't open the input file for reading. :(" << std::endl;
        return 1;
    }
    std::string line;
    std::string screen_clear = "\x1B[H\x1B[J";
    std::cout << screen_clear;
    std::string screen_buffer;
    while(std::getline(apple, line)) {
        if(line == "--") {
            std::cout << screen_clear << screen_buffer;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            screen_buffer = "";
        } else {
            screen_buffer += line + "\n";
        }
    }
    std::cout << "All done!" << std::endl;
}
