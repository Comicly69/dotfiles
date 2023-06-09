#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

// Function to generate a random number between min and max (inclusive)
int generateRandomNumber(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function to retrieve CPU usage on Linux-based systems
float getCPUUsage() {
    FILE* file = fopen("/proc/stat", "r");
    if (file == NULL) {
        printf("Failed to open /proc/stat file.\n");
        return -1;
    }

    char buffer[256];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    unsigned long long prev_idle = 0, prev_total = 0;
    float cpuUsage = 0.0;

    if (fgets(buffer, sizeof(buffer), file)) {
        sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
        unsigned long long idle_time = idle + iowait;
        unsigned long long total_time = user + nice + system + idle + iowait + irq + softirq + steal;

        unsigned long long totald = total_time - prev_total;
        unsigned long long idled = idle_time - prev_idle;

        cpuUsage = (float)(totald - idled) / totald * 100.0;

        prev_idle = idle_time;
        prev_total = total_time;
    }

    fclose(file);
    return cpuUsage;
}

// Function to retrieve memory usage on Linux-based systems
int getMemoryUsage(int* usedMemoryMB, int* totalMemoryMB) {
    FILE* file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        printf("Failed to open /proc/meminfo file.\n");
        return 0;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strncmp(buffer, "MemTotal:", 9) == 0) {
            sscanf(buffer + 9, "%d", totalMemoryMB);
            *totalMemoryMB /= 1024;  // Convert from kilobytes to megabytes
        } else if (strncmp(buffer, "MemAvailable:", 13) == 0) {
            sscanf(buffer + 13, "%d", usedMemoryMB);
            *usedMemoryMB /= 1024;  // Convert from kilobytes to megabytes
        }
    }

    fclose(file);
    return 1;
}

// Function to retrieve disk usage on Linux-based systems
int getDiskUsage(int* usedDisk) {
    struct statvfs diskInfo;
    if (statvfs("/", &diskInfo) == -1) {
        printf("Failed to retrieve disk usage.\n");
        return 0;
    }

    unsigned long long totalDisk = diskInfo.f_blocks * diskInfo.f_frsize;
    unsigned long long freeDisk = diskInfo.f_bfree * diskInfo.f_frsize;
    unsigned long long usedDiskSize = totalDisk - freeDisk;
    *usedDisk = (int)((usedDiskSize * 100) / totalDisk);

    return 1;
}

// Function to retrieve IP address on Linux-based systems
int getIPAddress(char* ipAddress) {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char* addr;

    if (getifaddrs(&ifap) == -1) {
        printf("Failed to retrieve IP address.\n");
        return 0;
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *)ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                // Split IP address into four parts
                int part1, part2, part3, part4;
                sscanf(addr, "%d.%d.%d.%d", &part1, &part2, &part3, &part4);

                // Generate random numbers to scramble IP address
                srand(time(NULL));
                part1 = generateRandomNumber(0, 255);
                part2 = generateRandomNumber(0, 255);
                part3 = generateRandomNumber(0, 255);
                part4 = generateRandomNumber(0, 255);

                // Store scrambled IP address
                snprintf(ipAddress, INET_ADDRSTRLEN, "%d.%d.%d.%d", part1, part2, part3, part4);
                break;
            }
        }
    }

    freeifaddrs(ifap);
    return 1;
}

int getLinuxDistribution(){
    return 1;
}

int main() {
    char operatingSystem[256];
    if (!getLinuxDistribution(operatingSystem)) {
        printf("Failed to retrieve Linux distribution.\n");
        return 1;
    }

    float cpuPercentage = getCPUUsage();
    int usedMemory, totalMemory;
    if (!getMemoryUsage(&usedMemory, &totalMemory)) {
        printf("Failed to retrieve memory usage.\n");
        return 1;
    }
    int usedDisk;
    if (!getDiskUsage(&usedDisk)) {
        printf("Failed to retrieve disk usage.\n");
        return 1;
    }

    char ipAddress[INET_ADDRSTRLEN];
    if (!getIPAddress(ipAddress)) {
        printf("Failed to retrieve IP address.\n");
        return 1;
    }

    printf("           \033[38;5;208mmax\033[1;37m@\033[0m\033[38;5;208mrosepine\033[0m\n");
    printf("\033[1;37m           \033[0m󰻠      \033[38;5;208m%.2f%%\n", cpuPercentage);
    printf("\033[1;37m   {\\_/}\033[0m   󰍛    \033[38;5;208m  %d/%d MB\n", usedMemory, totalMemory);
    printf("\033[1;37m   (●ᴗ●)\033[0m   󰋊     \033[38;5;208m (%d%%)\n", usedDisk);
    printf("\033[1;37m   ( >🥕)\033[0m  󱂇      \033[38;5;208m%s \033[38;5;202m\033[38;5;208m\n\033[0m", ipAddress);
    printf("\033[1;37m           \033[0m\033[38;5;208m %s\n", "     Linux x86_64");
    printf("\033[0m\033[1;37m           \033[0m    \033[0m  \033[38;5;208mbash\n");
    printf("\033[0m"); // Reset color to default

    return 0;
}

