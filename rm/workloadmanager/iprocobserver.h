#ifndef IPROCOBSERVER_H
#define IPROCOBSERVER_H

#include <cstdint>

namespace wm {

/*!
 * \class interface for event notification required by the Observer design pattern
 */
class IProcObserver {
public:
    virtual void update(std::uint8_t *data, std::size_t len) = 0;
};

}   // namespace wm

#endif // IPROCOBSERVER_H
