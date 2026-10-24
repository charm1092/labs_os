#ifndef UTILS_H
#define UTILS_H

#define EXIT_DIV_ZERO 2
#define EXIT_BAD_INPUT 3

int write_all(int fd, const char *buf, int n);
int str_len(const char *s);
void print_str(int fd, const char *s);
void print_int(int fd, int x);
void print_error(const char *msg);

#endif
