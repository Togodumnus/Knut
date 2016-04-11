#pragma once 

void mkdirFull(char *path);

int kcp_file_to_file(char *path1, char *path2);

int kcp_file_to_dir(char *file_path, char *dir_path);

int kcp_files_to_dir(int argc, char * argv[]);

int kcp_dir_to_dir(char *dir_path_src, char *dir_path_dest);

int kcp_dirs_to_dir(int argc, char * argv[]);

int kcp(int argc, char * argv[]);