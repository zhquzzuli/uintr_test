#include <iostream>

#include <sys/socket.h>

#ifndef UINTR_TEST_FD_COMMON_H
#define UINTR_TEST_FD_COMMON_H

void InitEmptyMsghdr(msghdr& msg);
int recvfd(int connection_fd);
void sendfd(int connection_fd, int uintrfd);
void error_exit(const char* message);

#endif //UINTR_TEST_FD_COMMON_H
