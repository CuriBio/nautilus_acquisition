#include<iostream>
//#include <boost/log/trivial.hpp>
#include <spdlog/spdlog.h>

#include <pvcam/master.h>
#include <pvcam/pvcam.h>

int main(int argc, char* argv[]) {
    //BOOST_LOG_TRIVIAL(info) << "Running";
    spdlog::info("Running");
    rs_bool result = pl_pvcam_init();
    spdlog::info("pl_pvcam_init: {}", result);
}
