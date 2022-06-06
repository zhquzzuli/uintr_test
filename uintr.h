#include <iostream>

#include <x86gprintrin.h>

#ifndef __NR_uintr_register_handler
#define __NR_uintr_register_handler	    449
#define __NR_uintr_unregister_handler	450
#define __NR_uintr_create_fd		    451
#define __NR_uintr_register_sender	    452
#define __NR_uintr_unregister_sender	453
#define __NR_uintr_wait			        454
#endif

#define uintr_register_handler(handler, flags)	syscall(__NR_uintr_register_handler, handler, flags)
#define uintr_unregister_handler(flags)		    syscall(__NR_uintr_unregister_handler, flags)
#define uintr_create_fd(vector, flags)		    syscall(__NR_uintr_create_fd, vector, flags)
#define uintr_register_sender(fd, flags)	    syscall(__NR_uintr_register_sender, fd, flags)
#define uintr_unregister_sender(fd, flags)	    syscall(__NR_uintr_unregister_sender, fd, flags)
#define uintr_wait(flags)                       syscall(__NR_uintr_wait, flags)

class UserInterrupt{
public:
    UserInterrupt(const std::string& uds_path, int vector, bool is_server);
    ~UserInterrupt();

    void init();

    // 发送中断
    void wakeupPeer();

    // 中断回调
    static void __attribute__((interrupt))
    __attribute__((target("general-regs-only", "inline-all-stringops")))
    ui_handler(struct __uintr_frame *ui_frame, unsigned long long vector);

private:
    // 启动本端的中断处理
    void setup_local();
    // 注册对端的发送
    void register_sender(int remote_uintrfd_);

    // 启动监听
    void start_server_();
    // 连接
    void connect_();

private:
    int local_uintrfd_ ;
    int remote_uintrfd_ ;
    int remote_uipi_index ;

    int vector_;

    bool is_server_;

    std::string uds_path_;
};

