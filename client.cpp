#include <unistd.h>

#include "uintr.h"

int main() {
    UserInterrupt client("/tmp/test_user_intr.sock", 1, false);
    client.init();

    client.wakeupPeer();

    sleep(5);
}