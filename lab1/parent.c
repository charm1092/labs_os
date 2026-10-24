#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "utils.h"

int main() {
    char filename[256];
    int len = 0;
    char c;

    print_str(STDOUT_FILENO, "Enter file name: ");
    while (1) {
        int r = read(STDIN_FILENO, &c, 1);
        if (r < 0) {
            print_error("parent: read");
            exit(1);
        }
        if (r == 0 || c == '\n') {
            break;
        }
        if (len >= 255) {
            print_str(STDERR_FILENO, "parent: file name is too long\n");
            exit(1);
        }
        filename[len] = c;
        len++;
    }
    filename[len] = '\0';

    if (len == 0) {
        print_str(STDERR_FILENO, "parent: empty file name\n");
        exit(1);
    }

    int file = open(filename, O_RDONLY);
    if (file < 0) {
        print_error(filename);
        exit(1);
    }

    int pipe1[2];
    if (pipe(pipe1) < 0) {
        print_error("parent: pipe");
        close(file);
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        print_error("parent: fork");
        close(file);
        close(pipe1[0]);
        close(pipe1[1]);
        exit(1);
    }

    if (pid == 0) {
        if (dup2(file, STDIN_FILENO) < 0) {
            print_error("child: dup2 stdin");
            exit(1);
        }
        if (dup2(pipe1[1], STDOUT_FILENO) < 0) {
            print_error("child: dup2 stdout");
            exit(1);
        }
        close(file);
        close(pipe1[0]);
        close(pipe1[1]);

        execl("./child", "child", NULL);
        print_error("child: execl ./child");
        exit(1);
    }

    close(file);
    close(pipe1[1]);

    char buf[1024];
    int n;
    while ((n = read(pipe1[0], buf, sizeof(buf))) > 0) {
        if (write_all(STDOUT_FILENO, buf, n) < 0) {
            print_error("parent: write");
            break;
        }
    }
    if (n < 0) {
        print_error("parent: read from pipe");
    }
    close(pipe1[0]);

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        print_error("parent: waitpid");
        exit(1);
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code == 0) {
            return 0;
        }
        if (code == EXIT_DIV_ZERO) {
            print_str(STDERR_FILENO, "parent: child found division by zero, exiting\n");
            exit(EXIT_DIV_ZERO);
        }
        print_str(STDERR_FILENO, "parent: child exited with code ");
        print_int(STDERR_FILENO, code);
        print_str(STDERR_FILENO, "\n");
        exit(1);
    }

    if (WIFSIGNALED(status)) {
        print_str(STDERR_FILENO, "parent: child killed by signal ");
        print_int(STDERR_FILENO, WTERMSIG(status));
        print_str(STDERR_FILENO, "\n");
        exit(1);
    }

    return 0;
}
