#include <cstring>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <vector>
#include <zlib.h>

#define CHUNK_SIZE 24
#define FPS_ASCII 15
#define FPS 30
#define FRAME_DURATION_NS 1000000000LL / FPS
#define FB_VID_WIDTH 480
#define FB_VID_HEIGHT 240
#define FB_VID_BUFSIZE FB_VID_WIDTH * FB_VID_HEIGHT

const std::chrono::milliseconds FRAME_DURATION(1000 / FPS_ASCII);
const std::string screen_clear = "\x1B[H\x1B[J";

void hcf(std::string reason) { // I dont fucking care anymore
    std::cout << "Halting and Catching Fire (reason: " << reason << ")" << std::endl;
    while(1) {}
}

int play_video_fb() {
	int fbfd = open("/dev/fb0", O_RDWR);
	if(fbfd == -1) {
		perror("Couldn't open /dev/fb0. Does your kernel have support?");
		return 1;
	}
	struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1 ||
        ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading screen info");
        close(fbfd);
        return 1;
    }
    long screensize = finfo.smem_len;
    uint32_t *fbp = (uint32_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((long)fbp == -1) {
        perror("Error mapping framebuffer device to memory");
        close(fbfd);
        return 1;
    }
    gzFile video = gzopen("fb_video.gz", "rb");
    if(!video) {
    	perror("Failed to open fb_video.gz");
     	return 1;
    }
    std::vector<uint8_t> fbvid_buf(FB_VID_BUFSIZE);
    struct timespec start_ts;
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    int64_t start_ns = (int64_t)start_ts.tv_sec * 1000000000LL + start_ts.tv_nsec;
    int64_t frame_count = 0;
    while(gzread(video, fbvid_buf.data(), fbvid_buf.size())) {
    	for(uint32_t Y = 0; Y < vinfo.yres; Y++) {
            uint32_t *row_dest = (uint32_t *)((char*)fbp + Y * finfo.line_length);
            int y = Y * FB_VID_HEIGHT / vinfo.yres;
            for(uint32_t X = 0; X < vinfo.xres; X++) {
                int x = X * FB_VID_WIDTH / vinfo.xres;
                uint8_t gray = fbvid_buf[y * FB_VID_WIDTH + x];
                row_dest[X] = (gray << 16) | (gray << 8) | gray;
            }
        }
        frame_count++;
        int64_t target_ns = start_ns + (frame_count * FRAME_DURATION_NS);
        struct timespec target_ts;
        target_ts.tv_sec = target_ns / 1000000000LL;
        target_ts.tv_nsec = target_ns % 1000000000LL;
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &target_ts, NULL);
    }
    return 0;
}

int play_video() {
    std::ifstream apple("badapple.txt");
    if(!apple.is_open()) {
        std::cout << "apple: FATAL! Couldn't open the input file for reading. :(" << std::endl;
        return 1;
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
    return 0;
}

int play_audio() {
    int dsp_fd = open("/dev/dsp", O_WRONLY);
    if(dsp_fd < 0) {
        perror("WARNING: Error while opening /dev/dsp! You won't be able to hear anything");
        return 1;
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
    hcf("Audio thread is done");
    return 0;
}

int main() {
    char stdin_buf;
    bool egg = false;
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if(stdin_flags == -1) {
        perror("Failed to get stdin flags");
        return 1;
    }
    if(fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK)) {
        perror("Failed to make stdin non-blocking");
        return 1;
    }
    for(int i = 1; i > 0; i--) {
        std::cout << screen_clear << "BadKernel by fwfy\n\nThe show will begin in " << i << " seconds. Adjust your volume!" << std::endl;
        if(egg) {
            std::cout << ":3" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(read(STDIN_FILENO, &stdin_buf, 1) > 0) {
            if(stdin_buf == 'v') {
                egg = true;
            }
        }
    }
    pid_t pid = fork();
   	std::cout << "\x1b[?1049h\x1b[?25l" << std::flush;; // no blinkencursor pls
    if(pid == 0) {
    	if(play_video_fb()) {
     		std::cout << "play_video_fb() exited with a non-zero return! Falling back to ASCII mode." << std::endl;
	        if(play_video()) {
	            std::cout << "play_video() exited with a non-zero return! :(" << std::endl;
	        }        
     	}
    } else {
        if(play_audio()) {
            std::cout << "play_audio() exited with a non-zero return! :(" << std::endl;
        }
    }
    if(reboot(RB_POWER_OFF) != 0) {
        perror("Failed to power off the system");
    }
}
