
int sendall(int socket, char *buffer, int length, int flag);

void loopClient(char *addr, int port);

void loopClient(char *addr, int port, int (*readInput)(char **msg));
