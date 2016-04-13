#pragma once


int copy_and_delete(char * path_to_move, char * dir_path);

int kmv_one_to_dir(char * path_to_move, char * dir_path);

int kmv_some_to_dir(int argc, char * argv[]);

int kmv(int argc, char * argv[]);