#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <vector>

#define CHUNK_SIZE 24
#define FPS 15

const std::chrono::milliseconds FRAME_DURATION(1000 / FPS);
const std::string screen_clear = "\x1B[H\x1B[J";

void hcf() { // I dont fucking care anymore
    std::cout << "Halting and Catching Fire" << std::endl;
    while(1) {}
}

void play_video() {
    std::ifstream apple("badapple.txt");
    if(!apple.is_open()) {
        std::cout << "apple: FATAL! Couldn't open the input file for reading. :(" << std::endl;
        _exit(1);
    }
    std::string line;
    std::cout << screen_clear;
    std::string screen_buffer;
    bool frameskip = false;
    auto startTime = std::chrono::steady_clock::now();
    while(std::getline(apple, line)) {
        if(line == "--") {
            if(frameskip) {
                frameskip = false;
            } else {
                std::cout << screen_clear << screen_buffer;
                auto endTime = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                if(elapsed < FRAME_DURATION) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DURATION - elapsed));
                } else if(elapsed > FRAME_DURATION) {
                    frameskip = true;
                }
                startTime = std::chrono::steady_clock::now();
            }
            screen_buffer = "";
        } else {
            screen_buffer += line + "\n";
        }
    }
    std::cout << "All done!" << std::endl;
    hcf();
}

void play_audio() {
    int dsp_fd = open("/dev/dsp", O_WRONLY);
    if(dsp_fd < 0) {
        std::cout << "WARNING: Error while opening /dev/dsp! Errno: " << errno << std::endl;
        std::cout << "You will not be able to hear anything!" << std::endl;
        hcf();
    }
    int format = AFMT_U8;
    int channels = 1;
    int rate = 16000;

    ioctl(dsp_fd, SNDCTL_DSP_SETFMT, &format);
    ioctl(dsp_fd, SNDCTL_DSP_CHANNELS, &channels);
    ioctl(dsp_fd, SNDCTL_DSP_SPEED, &rate);
    
    int audio_fd = open("badapple.pcm", O_RDONLY);
    std::vector<uint8_t> audio_buffer(CHUNK_SIZE);

    if(audio_fd < 0) {
        std::cout << "FATAL: Error while opening badapple.pcm! Errno: " << errno << std::endl;
        _exit(1);
    }

    while(read(audio_fd, &audio_buffer, CHUNK_SIZE)) {
        write(dsp_fd, &audio_buffer, CHUNK_SIZE);
    }

    hcf();
}

int main() {
    std::cout << screen_clear << "BadKernel by fwfy" << std::endl << "The show will begin in 5 seconds. Adjust your volume!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << screen_clear;
    pid_t pid = fork();
    if(pid == 0) {
        play_video();
        _exit(0);
    }
    play_audio();
}


