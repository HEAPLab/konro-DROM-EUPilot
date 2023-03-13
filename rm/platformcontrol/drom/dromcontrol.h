#ifndef DROMCONTROL_H
#define DROMCONTROL_H

#include "iplatformcontrol.h"
#include <log4cpp/Category.hh>

namespace pc {

class DromControl : public IPlatformControl {
public:
  explicit DromControl()
      : cat_(log4cpp::Category::getRoot()) {      }
  virtual ~DromControl(){};

  DromControl(const DromControl &rgt) = delete;
  DromControl(DromControl &&rgt) = delete;
  DromControl &operator=(const DromControl &) = delete;
  DromControl &operator=(DromControl &&) = delete;

  // Logger
  log4cpp::Category &cat_;

  /*!
   * \brief Adds an application under the management of Konro.
   * \param app the application to manage
   */

  bool addApplication(std::shared_ptr<rmcommon::App> app) override;

  /*!
   * \brief Removes an application from the management of Konro.
   * \param app the application to remove from Konro's management
   */
  bool removeApplication(std::shared_ptr<rmcommon::App> app) override;
};
} // namespace pc

#endif