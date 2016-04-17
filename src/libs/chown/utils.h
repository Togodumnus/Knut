#pragma once

#define _GNU_SOURCE
#include <sys/types.h>

void chElem(char *path, int options, uid_t uid, gid_t gid, int f_rec, int f_verb);
void chDirContent(char *path, int options, uid_t uid, gid_t gid, int f_rec, int f_verb);
int isNumber(char *s);
