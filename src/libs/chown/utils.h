#pragma once

#define _GNU_SOURCE
#include <sys/types.h>

void chElem(char *path, int options, uid_t uid, gid_t gid);
void chDirContent(char *path, int options, uid_t uid, gid_t gid);
int isNumber(char *s);
