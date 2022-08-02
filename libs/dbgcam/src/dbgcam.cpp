#include <iostream>
//#include <boost/log/trivial.hpp>
#include <spdlog/spdlog.h>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>

rs_bool PV_DECL pl_pvcam_init(void) {
    return PV_OK;
}
