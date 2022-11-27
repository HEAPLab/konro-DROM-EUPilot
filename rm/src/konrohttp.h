#ifndef KONROHTTP_H
#define KONROHTTP_H

#include <thread>
#include <memory>
#include <log4cpp/Category.hh>

/*!
 * \brief a simple server running in a dedicated thread.
 * Can be used by external processes to send requests or information
 * to Konro.
 */
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
