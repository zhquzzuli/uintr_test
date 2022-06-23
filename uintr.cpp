#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "uintr.h"
#include "fd_common.h"

void UserInterrupt::ui_handler(struct __uintr_frame *ui_frame, unsigned long long vector) {
    std::cout << "recv UserInterrupt ui_handler vector " << vector << std::endl;
}

UserInterrupt::UserInterrupt(const std::string& uds_path, int vector, bool is_server) :
    vector_(vector), is_server_(is_server), uds_path_(uds_path) {
}

void UserInterrupt::init() {
    setup_local();

    if (is_server_) {
        start_server_();
    } else {
        connect_();
    }
}

void UserInterrupt::register_sender(int remote_uintrfd) {
    remote_uintrfd_ = remote_uintrfd;

    remote_uipi_index = uintr_register_sender(remote_uintrfd_, 0);
    if (remote_uipi_index < 0) {
        error_exit("uintr_register_sender error");
    }
}

void UserInterrupt::setup_local() {
    if (uintr_register_handler(UserInterrupt::ui_handler, 0)) {
        error_exit("uintr_register_handler error");
    }

    local_uintrfd_ = uintr_create_fd(vector_, 0);
    if (local_uintrfd_ < 0) {
        error_exit("uintr_create_fd error");
    }

    std::cout << "create local_uintrfd_ " << local_uintrfd_ << std::endl;

    _stui();
}

void UserInterrupt::start_server_() {
    int listen_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        error_exit("socket error");
    }

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, uds_path_.c_str());
    remove(address.sun_path);

    int ret;

    ret = bind(listen_fd_, (struct sockaddr *) &address, SUN_LEN(&address));
    if (ret == -1) {
        error_exit("bind error");
    }

    ret = listen(listen_fd_, 10);
    if (ret == -1) {
        error_exit("listen error");
    }

    struct sockaddr_un client;
    socklen_t length = sizeof client;
    int connection_fd;

    connection_fd = accept(listen_fd_, (struct sockaddr *) &client, &length);
    if (connection_fd == -1) {
        error_exit("accept error");
    }
    std::cout << "server accept connection_fd " << connection_fd << std::endl;

    register_sender(recvfd(connection_fd));
    sendfd(connection_fd, local_uintrfd_);

    close(connection_fd);
    close(listen_fd_);
}

void UserInterrupt::connect_() {
    int connect_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connect_fd_ == -1) {
        error_exit("socket error");
    }

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, uds_path_.c_str());
    if (connect(connect_fd_,(struct sockaddr*)&address,SUN_LEN(&address)) == -1) {
        error_exit("connect error");
    }

    sendfd(connect_fd_, local_uintrfd_);
    register_sender(recvfd(connect_fd_));

    close(connect_fd_);
}

void UserInterrupt::wakeupPeer() {
    std::cout << "wakeupPeer remote_uipi_index " << remote_uipi_index << std::endl;
    _senduipi(remote_uipi_index);
}
