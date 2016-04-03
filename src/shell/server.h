#pragma once

#include <stdbool.h>

typedef int SOCKET;

const int PIPE_READ;
const int PIPE_WRITE;

const int BUFFER_SIZE;

const char EOT;

bool isSocket(int fd);
ssize_t getLineSocket(char **line, size_t *size, int fd);

void loopServer(void (*callbackInit)(int fd), void (*callbackSockInit)(int fd),
        int (*readFd)(int fd));
