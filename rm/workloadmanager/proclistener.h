#ifndef PROCLISTENER_H
#define PROCLISTENER_H

#include "iprocobserver.h"
#include <log4cpp/Category.hh>
#include <cstdint>
#include <cctype>
#include <atomic>

namespace wm {

/*!
 * \brief Interface to the Linux kernel Proc Connector
 */
class ProcListener final {
    enum MessageData {
        LISTEN = 1,
        IGNORE = 2,
        STOP = 6
    };

    int errno_;
    /*! Netlink socket */
    int nl_socket_;
    /*! unique id used for Netlink communication */
    unsigned int nl_pid_;
    /*! stop flag for the thread */
    std::atomic_bool stop_;
    IProcObserver *observer_;
    log4cpp::Category &cat_;

    /*!
     * \brief Creates a socket for the Netlink protocol
     * \return the socket or -1 in case of error
     */
    int createNetlinkSocket();

    /*!
     * Bind the netlink socket to the specified address (pid) in the
     * CN_IDX_PROC group (multicast)
     *
     * \param sock the Netlink socket
     * \param pid the Netlink address (unique id)
     * \return the outcome of the operation
     */
    bool bindNetlinkSocket(int sock, unsigned int pid);
    bool sendNetlinkMessage(int sock, void *msg, std::size_t msgsize, unsigned int pid, unsigned int groups);

    /*!
     * \brief Sends a message over the specified socket
     * \param socket the socket to use to send the message
     * \param op the message to send
     * \return the outcome of the operation
     */
    bool sendConnectorNetlinkMessageToKernel(int socket, MessageData op);
    bool sendConnectorNetlinkMessageToThread(int socket, MessageData op);

    /*!
     * Receives a Netlink message from the kernel (Proc Connector)
     *
     * \return the outcome of the operation
     */
    bool receiveConnectorNetlinkMessage(int socket, void *buffer, std::size_t bufferSize);

    /*!
     * \brief Notifies the WorkloadManager of a new event
     */
    void processEvent(std::uint8_t *data, size_t len);
    void run();

    void notify(std::uint8_t *data, size_t len);

public:
    ProcListener() : observer_(nullptr), cat_(log4cpp::Category::getRoot()) {
    }

    void operator()() {
        run();
    }

    /*!
     * \brief Sends a STOP message to the thread using Netlink
     * \return the outcome of the operation
     */
    bool stop();

    void attach(IProcObserver *o) {
        observer_ = o;
    }

    void detach() {
        observer_ = nullptr;
    }
};

}   // namespace wm

#endif // PROCLISTENER_H
