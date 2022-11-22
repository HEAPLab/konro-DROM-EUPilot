#ifndef KONROHTTP_H
#define KONROHTTP_H

#include <thread>
#include <log4cpp/Category.hh>
#include "../../lib/httplib/httplib.h"

class KonroHttp {
    log4cpp::Category &cat_;
    std::thread httpThread_;
    httplib::Server srv_;

    /*! The thread function */
    void run();
    void handleRequest(const httplib::Request &, httplib::Response &res);

public:
    KonroHttp();

    /*! Runs in the same thread */
    void operator()() {
        run();
    }

    /*!
     * \brief Starts in a separate thread
     */
    void start();

    void stop() {
        srv_.stop();
    }

};

#endif // KONROHTTP_H
