#ifndef PROCLISTENER_H
#define PROCLISTENER_H

#include <cstdint>
#include <cctype>
#include <atomic>

namespace pc {

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

    int createNetlinkSocket();
    bool bindNetlinkSocket(int sock, unsigned int pid);
    bool sendNetlinkMessage(int sock, void *msg, std::size_t msgsize, unsigned int pid, unsigned int groups);
    bool sendConnectorNetlinkMessageToKernel(int socket, MessageData op);
    bool sendConnectorNetlinkMessageToThread(int socket, MessageData op);
    bool receiveConnectorNetlinkMessage(int socket, void *buffer, std::size_t bufferSize);

    void processEvent(std::uint8_t *data);
    void run();

public:

    void operator()() {
        run();
    }

    bool stop();
};

}   // namespace pc

#endif // PROCLISTENER_H
