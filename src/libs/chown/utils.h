#pragma once

#include "../../LIB.h"

int chownLib(int argc, char *argv[]);
int chgrpLib(int argc, char *argv[]);
void chElem(char *path, int options, uid_t uid, gid_t gid);
void chDirContent(char *path, int options, uid_t uid, gid_t gid);
int Init(EnregisterCommande);