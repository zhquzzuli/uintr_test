#include "uintr.h"

int main() {
    UserInterrupt server("/tmp/test_user_intr.sock", 0, true);
    server.init();
}