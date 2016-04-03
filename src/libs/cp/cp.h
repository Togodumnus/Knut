#pragma once 

int kcp_file_to_file(const char *path1, const char *path2);

int kcp_file_to_dir(char *file_path, char *dir_path);

int kcp_dir_to_dir(char *dir_path_src, char *dir_path_dest);

int kcp_dirs_to_dir(int argc, char * const argv[]);

int kcp(int argc, char * const argv[]);