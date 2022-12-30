#include "konromanager.h"
#include "threadname.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <memory>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/SimpleLayout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

static void trapCtrlC();

// KonroManager is declared at file scope in order
// to be usable from ctrlCHandler
static KonroManager *konroManager;

/*!
 * Configures log4cpp
 */
static void setupLogging()
{
    // Log4CPP configuration

    log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cout);
    log4cpp::PatternLayout *layout1 = new log4cpp::PatternLayout();
    layout1->setConversionPattern("%d [%p] %m%n");
    appender1->setLayout(layout1);

    log4cpp::Appender *appender2 = new log4cpp::FileAppender("logfile", "konro.log");
    log4cpp::PatternLayout *layout2 = new log4cpp::PatternLayout();
    layout2->setConversionPattern("%d [%p] %m%n");
    appender2->setLayout(layout2);

    log4cpp::Category &cat = log4cpp::Category::getRoot();
    cat.setPriority(log4cpp::Priority::DEBUG);
    cat.addAppender(appender1);
    cat.addAppender(appender2);

    cat.info("KONRO starting");
}

int main(int argc, char *argv[])
{
    std::string configFile;
    int c, optionIndex;
    static struct option longOptions[] = {
        {"config", required_argument, 0, 'c' },
        {0,        0,                 0, 0 }
    };

    while (true) {
        c = getopt_long(argc, argv, "c:", longOptions, &optionIndex);
        if (c == -1)
            break;      // all options parsed)
        switch (c) {
        case 'c':
            configFile = optarg;
            break;
        default:
            std::cout << "Unknown option\n"
                      << "Usage: konro [--config,-c  CONFIG_FILE]\n";
            exit(EXIT_FAILURE);
        }
    }
    // ignore remaining (non option) arguments

    rmcommon::setThreadName("MAIN");
    setupLogging();
    trapCtrlC();
    log4cpp::Category::getRoot().info("MAIN creating KonroManager");
    konroManager = new KonroManager(configFile);
    konroManager->run();

    // KonroManager must be deleted before exiting from main
    // or a segmentation fault occurs in log4cpp
    delete konroManager;

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
#if 1
    signal(SIGINT, ctrlCHandler);
#else
    struct sigaction sig_action;

    memset(&sig_action,0, sizeof(sig_action));
    sig_action.sa_handler = ctrlCHandler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = 0;
    sig_action.sa_restorer = nullptr;

    sigaction(SIGINT, &sig_action, nullptr);
#endif
}
