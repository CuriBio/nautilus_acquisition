#include <iostream>

#include <format>
#include <spdlog/spdlog.h>
#include <tsl/ordered_map.h>

#include <QFileDialog>

#include "stagecontrol.h"
#include "ui_stagecontrol.h"

//using namespace std::chrono_literals;

StageControl::StageControl(std::string comPort, std::shared_ptr<Config> config, std::vector<int> stepSizes, QWidget *parent) : QDialog(parent), ui(new Ui::StageControl) {
    ui->setupUi(this);
    connect(this, &StageControl::sig_stage_enable_all, this, &StageControl::enableAll);
    connect(this, &StageControl::sig_stage_disable_all, this, &StageControl::disableAll);

    m_comPort = comPort;
    m_stepSizes = stepSizes;
    m_config = config;

    m_tango = new TangoStage(m_comPort);
}

StageControl::~StageControl() {
    delete m_tango;
    delete ui;
}

bool StageControl::Connected() const {
    return m_tango->Connected();
}

bool StageControl::Calibrate() {
    if (m_tango->Connected()) {
        return (m_tango->Calibrate() && m_tango->RMeasure() && m_tango->GetCurrentPos(m_curX, m_curY));
    } else {
        return false;
    }
}

void StageControl::SetRelativePosition(double x, double y) {
    m_tango->SetRelativePos(x, y, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetAbsolutePosition(double x, double y) {
    //std::this_thread::sleep_for(2000ms);
    m_tango->SetAbsolutePos(x, y, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetRelativeX(double x) {
    m_tango->SetRelativePos(x, 0, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetAbsoluteX(double x) {
    m_tango->SetAbsolutePos(x, m_curY, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetRelativeY(double y) {
    m_tango->SetRelativePos(0, y, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetAbsoluteY(double y) {
    m_tango->SetAbsolutePos(m_curX, y, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}


const std::vector<StagePosition*>& StageControl::GetPositions() const {
    return m_positions;
}

//slots
void StageControl::on_unskipBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (0 <= row && row < m_positions.size()) {
        auto i = m_positions[row];
        spdlog::info("Unskipping stage pos_{}", i->pos_);
        i->setText(fmt::format("pos_{} - x: {}, y: {}", i->pos_, i->x, i->y).c_str());
        i->skipped = false;
    }
}

void StageControl::on_skipBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (0 <= row && row < m_positions.size()) {
        auto i = m_positions[row];
        spdlog::info("Skipping stage pos_{}", i->pos_);
        i->setText(fmt::format("pos_{} - x: {}, y: {} (skipped)", i->pos_, i->x, i->y).c_str());
        i->skipped = true;
    }
}


void StageControl::loadList(std::string fileName) {
    spdlog::info("Loading stage positions from file: {}", fileName);

    for (auto v : m_positions) { delete v; }

    m_positions.clear();
    ui->stageLocations->clear();

    auto file = toml::parse(fileName);

    auto x0_ref = toml::find<double>(file, "stage", "x0_ref");
    auto y0_ref = toml::find<double>(file, "stage", "y0_ref");
    auto numWells = toml::find<int>(file, "stage", "num_wells");
    auto wellSpacing = toml::find<int>(file, "stage", "well_spacing");

    int wellsPerFovGridSide;
    switch (numWells) {
        case 24:
            wellsPerFovGridSide = 2;
            break;
        case 96:
            wellsPerFovGridSide = 4;
            break;
        case 384:
            wellsPerFovGridSide = 8;
            break;
        case 1536:
            wellsPerFovGridSide = 16;
            break;
        default:
            spdlog::error("Invalid num_wells: {}", numWells);
            wellsPerFovGridSide = 2;
    }

    size_t n = 1;

    for (auto rFov = 1; rFov >= -1; rFov -= 2) {
        auto dyRoi = wellSpacing * (wellsPerFovGridSide / 2) * rFov;
        for (auto cFov = 1; cFov >= -1; cFov--) {
            int pos = n++;

            auto dxRoi = wellSpacing * wellsPerFovGridSide * cFov;

            auto x = x0_ref + m_config->dxCal + xAdj + m_config->scalingFactor * (dxRoi * std::cos(m_config->theta) + dyRoi * std::sin(m_config->theta));
            auto y = y0_ref + m_config->dyCal + yAdj + m_config->scalingFactor * (dyRoi * std::cos(m_config->theta) - dxRoi * std::sin(m_config->theta));

            spdlog::info("Loading stage position: ({}, {})", x, y);

            StagePosition* item = new StagePosition(pos, x, y);
            m_positions.push_back(item);
            ui->stageLocations->addItem(item);
        }
    }

    emit this->sig_stagelist_updated(m_positions.size());
}


void StageControl::on_gotoPosBtn_clicked() {
    emit sig_start_move();
    std::thread t([this] {
        int row = ui->stageLocations->currentRow();

        if (row >= 0) {
            StagePosition* item = static_cast<StagePosition*>(ui->stageLocations->item(row));
            spdlog::info("Setting stage to position x: {}, y: {}", item->x, item->y);
            m_tango->SetAbsolutePos(item->x, item->y, true);
            m_tango->GetCurrentPos(m_curX, m_curY);
        } else {
            spdlog::info("Invalid selection");
        }

        emit sig_end_move();
    });
    t.detach();
}

void StageControl::disableAll() {
    ui->skipBtn->setEnabled(false);
    ui->unskipBtn->setEnabled(false);
    ui->gotoPosBtn->setEnabled(false);

    ui->stageRightBtn1->setEnabled(false);
    ui->stageRightBtn2->setEnabled(false);
    ui->stageRightBtn3->setEnabled(false);

    ui->stageLeftBtn1->setEnabled(false);
    ui->stageLeftBtn2->setEnabled(false);
    ui->stageLeftBtn3->setEnabled(false);

    ui->stageUpBtn1->setEnabled(false);
    ui->stageUpBtn2->setEnabled(false);
    ui->stageUpBtn3->setEnabled(false);

    ui->stageDownBtn1->setEnabled(false);
    ui->stageDownBtn2->setEnabled(false);
    ui->stageDownBtn3->setEnabled(false);
}

void StageControl::enableAll() {
    ui->skipBtn->setEnabled(true);
    ui->unskipBtn->setEnabled(true);
    ui->gotoPosBtn->setEnabled(true);

    ui->stageRightBtn1->setEnabled(true);
    ui->stageRightBtn2->setEnabled(true);
    ui->stageRightBtn3->setEnabled(true);

    ui->stageLeftBtn1->setEnabled(true);
    ui->stageLeftBtn2->setEnabled(true);
    ui->stageLeftBtn3->setEnabled(true);

    ui->stageUpBtn1->setEnabled(true);
    ui->stageUpBtn2->setEnabled(true);
    ui->stageUpBtn3->setEnabled(true);

    ui->stageDownBtn1->setEnabled(true);
    ui->stageDownBtn2->setEnabled(true);
    ui->stageDownBtn3->setEnabled(true);

}
