#pragma once

//flags
const int F_FORCE; //-f
const int F_REC;   //-r, -R
const int F_VERB;  //-v

void rmDirContent(char *path, int options);
void rmElement(char *path, int options);
