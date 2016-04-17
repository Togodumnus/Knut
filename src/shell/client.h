#pragma once

int sendall(int socket, char *buffer, int length, int flag);

void loopClient(char *addr, int port);
