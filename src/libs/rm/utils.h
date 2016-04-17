#pragma once

//flags
const int RM_F_FORCE; //-f
const int RM_F_REC;   //-r, -R
const int RM_F_VERB;  //-v

void rmDirContent(char *path, int options);
void rmElement(char *path, int options);
