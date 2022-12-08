#include "konromanager.h"
#include "threadname.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <memory>
#include <log4cpp/Category.hh>

static void trapCtrlC();

// KonroManager is declared at file scope in order
// to be usable from ctrlCHandler
static KonroManager *konroManager;

int main(int argc, char *argv[])
{
    rmcommon::setThreadName("MAIN");

    if (argc >= 2) {
        trapCtrlC();
        long pidToMonitor = strtol(argv[1], nullptr, 10);
        konroManager = new KonroManager();
        konroManager->run(pidToMonitor);
        // KonroManager must be deleted before exiting from main
        // or a segmentation fault occurs in log4cpp
        delete konroManager;
    }
    log4cpp::Category::getRoot().info("MAIN exiting");
    return EXIT_SUCCESS;
}

static void ctrlCHandler(int s)
{
    puts("Ctrl-C");
    log4cpp::Category::getRoot().info("MAIN Ctrl-C: stopping konro");
    konroManager->stop();
}

static void trapCtrlC()
{
#if 0
    struct sigaction sig_action;

    memset(&sig_action,0, sizeof(sig_action));
    sig_action.sa_handler = ctrlCHandler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sig_action.sa_restorer = nullptr;

    sigaction(SIGINT, &sig_action, nullptr);
#else
    signal(SIGINT, ctrlCHandler);
#endif
}
