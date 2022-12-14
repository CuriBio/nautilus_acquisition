#include <spdlog/spdlog.h>
#include <Tango.h>

#include <TangoStage.h>

TangoStage::TangoStage(std::string comPort) {
    m_tango = new CTango();
    m_comPort = comPort;

    //should auto connect with first found tango driver instance.
    if (m_tango->ConnectSimple(1, m_comPort.data(), 57600, TRUE) != 0) {
        spdlog::info("SetAbsolutePos error: {}", GetError());
    } else {
        //get current position
        double x,y;
        GetCurrentPos(x, y);
        m_x = x; m_y = y;
    }
}

TangoStage::~TangoStage() {
    m_tango->Disconnect();
    delete m_tango;
}

bool TangoStage::GetCurrentPos(double& x, double& y) {
    double z, a;
    if (m_tango->GetPos(&x, &y, &z, &a) != 0) {
        spdlog::info("GetPos error: {}", GetError());
        return false;
    }
    return true;
}

bool TangoStage::SetRelativePos(double x, double y, bool block) {
    spdlog::info("TangoStage::SetRelativePos x: {}, y: {}", x, y);
    if (m_tango->MoveRel(x, y, 0.0, 0.0, block) != 0) {
        spdlog::info("SetRelativePos error: {}", GetError());
        return false;
    }
    return true;
}

bool TangoStage::SetAbsolutePos(double x, double y, bool block) {
    spdlog::info("TangoStage::SetAbsolutePos");
    if (m_tango->MoveAbs(x, y, 0.0, 0.0, block) != 0) {
        spdlog::info("SetAbsolutePos error: {}", GetError());
        return false;
    }
    return true;
}

int TangoStage::GetError() const {
    int error;
    m_tango->GetError(&error);
    return error;
}
