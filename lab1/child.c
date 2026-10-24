#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "utils.h"

int line = 1;
int count = 0;
int result = 0;
int in_num = 0;
int neg = 0;
int digits = 0;
long long cur = 0;

void line_error(const char *msg) {
    print_str(STDERR_FILENO, "child: ");
    print_str(STDERR_FILENO, msg);
    print_str(STDERR_FILENO, " in line ");
    print_int(STDERR_FILENO, line);
    print_str(STDERR_FILENO, "\n");
}

void end_number() {
    if (!in_num) {
        return;
    }
    if (digits == 0) {
        line_error("bad number");
        exit(EXIT_BAD_INPUT);
    }

    long long value = cur;
    if (neg) {
        value = -cur;
    }
    if (value > INT_MAX || value < INT_MIN) {
        line_error("number does not fit in int");
        exit(EXIT_BAD_INPUT);
    }

    if (count == 0) {
        result = (int)value;
    } else {
        if (value == 0) {
            line_error("division by zero");
            exit(EXIT_DIV_ZERO);
        }
        if (result == INT_MIN && value == -1) {
            line_error("overflow");
            exit(EXIT_BAD_INPUT);
        }
        result = result / (int)value;
    }
    count++;

    in_num = 0;
    neg = 0;
    digits = 0;
    cur = 0;
}

void end_line() {
    end_number();
    if (count > 0) {
        print_int(STDOUT_FILENO, result);
        print_str(STDOUT_FILENO, "\n");
    }
    count = 0;
    line++;
}

void process_char(char c) {
    if (c >= '0' && c <= '9') {
        in_num = 1;
        cur = cur * 10 + (c - '0');
        digits++;
        if (cur > 2147483648LL) {
            line_error("number does not fit in int");
            exit(EXIT_BAD_INPUT);
        }
    } else if (c == '-') {
        if (in_num) {
            line_error("bad number");
            exit(EXIT_BAD_INPUT);
        }
        in_num = 1;
        neg = 1;
    } else if (c == ' ' || c == '\t' || c == '\r') {
        end_number();
    } else if (c == '\n') {
        end_line();
    } else {
        line_error("unexpected symbol");
        exit(EXIT_BAD_INPUT);
    }
}

int main() {
    char buf[1024];
    int n;

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            process_char(buf[i]);
        }
    }
    if (n < 0) {
        print_error("child: read");
        exit(1);
    }

    if (in_num || count > 0) {
        end_line();
    }

    return 0;
}
