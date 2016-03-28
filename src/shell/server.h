#pragma once

#include <stdbool.h>

bool isSocket(int fd);
ssize_t getLineSocket(char **line, size_t *size, int fd);

void loop(void (*callbackInit)(int fd), int (*readFd)(int fd));
