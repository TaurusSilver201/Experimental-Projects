#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <sys/statvfs.h>

void print_cpu_info() {
    long num_processors = sysconf(_SC_NPROCESSORS_CONF);
    printf("Number of Processors: %ld\n", num_processors);
}

void print_memory_info() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    long total_memory = pages * page_size / (1024 * 1024); // Convert to MB
    printf("Total Memory: %ld MB\n", total_memory);
}

void print_disk_info() {
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        long block_size = stat.f_frsize;
        long total_blocks = stat.f_blocks;
        long total_space = total_blocks * block_size / (1024 * 1024 * 1024); // Convert to GB
        printf("Total Disk Space: %ld GB\n", total_space);
    }
}

void print_load_average() {
    double loadavg[3];
    if (getloadavg(loadavg, 3) != -1) {
        printf("Load Average (1min, 5min, 15min): %.2f, %.2f, %.2f\n",
               loadavg[0], loadavg[1], loadavg[2]);
    }
}

void print_user_limits() {
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
        printf("Maximum number of open files: %ld\n", rlim.rlim_cur);
    }
}

int main() {
    struct utsname unameData;

    // Get system information
    if (uname(&unameData) != 0) {
        perror("uname");
        return EXIT_FAILURE;
    }

    // Print basic system information
    printf("System Information:\n");
    printf("  OS: %s %s\n", unameData.sysname, unameData.release);
    printf("  Node Hostname: %s\n", unameData.nodename);
    printf("  OS Version: %s\n", unameData.version);
    printf("  Machine Type: %s\n", unameData.machine);

    // Print advanced system information
    printf("\nAdvanced System Information:\n");
    print_cpu_info();
    print_memory_info();
    print_disk_info();
    print_load_average();
    print_user_limits();

    return EXIT_SUCCESS;
}
