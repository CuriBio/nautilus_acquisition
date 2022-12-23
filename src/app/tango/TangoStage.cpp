#include <spdlog/spdlog.h>
#include <Tango.h>

#include <TangoStage.h>

TangoStage::TangoStage() {
    m_tango = new CTango();

    //should auto connect with first found tango driver instance.
    m_tango->ConnectSimple(-1, NULL, 57600, TRUE);

    //get current position
    GetCurrentPos(m_x, m_y);
}

TangoStage::~TangoStage() {
    m_tango->Disconnect();
    delete m_tango;
}

bool TangoStage::GetCurrentPos(double& x, double& y) {
    spdlog::info("TangoStage::GetCurrentPos");

    double z, a;
    return m_tango->GetPos(&m_x, &m_y, &z, &a) == 0;
}

bool TangoStage::SetRelativePos(double x, double y, bool block) {
    spdlog::info("TangoStage::SetRelativePos x: {}, y: {}", x, y);
    return m_tango->MoveRel(x, y, 0.0, 0.0, block) == 0;
}

bool TangoStage::SetAbsolutePos(double x, double y, bool block) {
    spdlog::info("TangoStage::SetAbsolutePos");
    return m_tango->MoveAbs(x, y, 0.0, 0.0, block) == 0;
}

int TangoStage::GetError() const {
    spdlog::info("TangoStage::GetError");
    int error;
    m_tango->GetError(&error);
    return error;
}
