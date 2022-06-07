#include "fd_common.h"

void InitEmptyMsghdr(msghdr& msg) {
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    // for normal data
    msg.msg_iov = NULL;
    msg.msg_iovlen = 0;
    // for control msg
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
}

int recvfd(int connection_fd) {
    struct msghdr msg = {0};
    InitEmptyMsghdr(msg);
    char tmp_buf[CMSG_SPACE(1*sizeof(int))];
    msg.msg_control = tmp_buf;
    msg.msg_controllen = sizeof(tmp_buf);

    struct iovec iov2[1];
    char data[1] = "";
    iov2[0].iov_base = data;
    iov2[0].iov_len = 1;
    msg.msg_iov = iov2;
    msg.msg_iovlen = 1;

    int ret = recvmsg(connection_fd, &msg, 0);
    if (ret <= 0) {
        throw("recvmsg error");
    }

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);

    // fds
    int fds[1];
    memcpy(fds, (int*)CMSG_DATA(cmsg), 1 * sizeof(int));
    int recvfd = fds[0];
    std::cout << "recv remote uintrfd " << recvfd << std::endl;

    return recvfd;
}

void sendfd(int connection_fd, int uintrfd) {
    struct msghdr msg = {0};
    InitEmptyMsghdr(msg);

    char tmp_buf[CMSG_SPACE(1 * sizeof(int))];
    msg.msg_control = tmp_buf;
    msg.msg_controllen = sizeof(tmp_buf);

    struct iovec iov[1];
    char data[1] = "";
    iov[0].iov_base = data;
    iov[0].iov_len = 1;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(1 * sizeof(int));
    memcpy((int*)CMSG_DATA(cmsg), &(uintrfd), sizeof(int));

    int ret = sendmsg(connection_fd, &msg, 0);
    if (ret <= 0) {
        throw("sendmsg error");
    }
}

void error_exit(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}