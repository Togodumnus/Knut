#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../../LIB.h"

int chownLib(int argc, char *argv[]);
int chgrpLib(int argc, char *argv[]);
void Init(EnregisterCommande );
void chElem(char *path, int options, uid_t uid, gid_t gid);
void chDirContent(char *path, int options, uid_t uid, gid_t gid);