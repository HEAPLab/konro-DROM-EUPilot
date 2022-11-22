#ifndef KONROHTTP_H
#define KONROHTTP_H

#include <thread>
#include <memory>
#include <log4cpp/Category.hh>

class KonroHttp {
    struct KonroHttpImpl;
    std::unique_ptr<KonroHttpImpl> pimpl_;
    log4cpp::Category &cat_;
    std::thread httpThread_;

    /*! The thread function */
    void run();

public:
    KonroHttp();
    ~KonroHttp();

    /*! Runs in the same thread */
    void operator()() {
        run();
    }

    /*!
     * \brief Starts the HTTP server in a separate thread
     */
    void start();

    /*!
     * \brief Stops the HTTP server
     */
    void stop();

};

#endif // KONROHTTP_H
