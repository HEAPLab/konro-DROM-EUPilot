#ifndef KONROHTTP_H
#define KONROHTTP_H

#include "simpleeventbus.h"
#include <thread>
#include <memory>
#include <log4cpp/Category.hh>

namespace http {

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
    explicit KonroHttp(rmcommon::EventBus &eventBus);
    ~KonroHttp();

    /*!
     * \brief Starts the HTTP server in a separate thread
     */
    void start();

    /*!
     * \brief Stops the HTTP server
     */
    void stop();
};

}   // namespace http

#endif // KONROHTTP_H
