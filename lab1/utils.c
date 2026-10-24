#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

int write_all(int fd, const char *buf, int n) {
    int done = 0;
    while (done < n) {
        int w = write(fd, buf + done, n - done);
        if (w < 0) {
            return -1;
        }
        done += w;
    }
    return 0;
}

int str_len(const char *s) {
    int n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

void print_str(int fd, const char *s) {
    write_all(fd, s, str_len(s));
}

void print_int(int fd, int x) {
    char buf[12];
    char out[12];
    int i = 0;
    int neg = 0;
    long long v = x;

    if (v == 0) {
        print_str(fd, "0");
        return;
    }
    if (v < 0) {
        neg = 1;
        v = -v;
    }
    while (v > 0) {
        buf[i] = v % 10 + '0';
        v = v / 10;
        i++;
    }
    if (neg) {
        buf[i] = '-';
        i++;
    }
    for (int j = 0; j < i; j++) {
        out[j] = buf[i - 1 - j];
    }
    write_all(fd, out, i);
}

void print_error(const char *msg) {
    int err = errno;
    print_str(STDERR_FILENO, msg);
    print_str(STDERR_FILENO, ": ");
    print_str(STDERR_FILENO, strerror(err));
    print_str(STDERR_FILENO, "\n");
}
