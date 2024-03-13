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

    int i = 0;
    for (auto [x, y] : config->stageLocations) {
        StagePosition* item = new StagePosition(++i, x, y);
        m_positions.push_back(item);
        ui->stageLocations->addItem(item);
    }
}

StageControl::~StageControl() {
    saveList(m_config->configFile, true);
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
        spdlog::info("Unskipping stage position {}", row);
        auto i = m_positions[row];
        i->skipped = false;
        i->setText(fmt::format("pos_{} - x: {}, y: {}", i->pos, i->x, i->y).c_str());
    }
}

void StageControl::on_skipBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (0 <= row && row < m_positions.size()) {
        spdlog::info("Skipping stage position {}", row);
        auto i = m_positions[row];
        i->skipped = true;
        i->setText(fmt::format("pos_{} - x: {}, y: {} (skipped)", 1, i->x, i->y).c_str());
    }
}


void StageControl::saveList(std::string fileName, bool fileExists) {
    spdlog::info("Saving stage positions to file: {}", std::filesystem::path(fileName).string());
    toml::value file;
    if (fileExists) {
        file = toml::parse<toml::preserve_comments, tsl::ordered_map>(fileName);
    } else {
        file = toml::value {};
    }

    toml::array vs{};
    for (auto& v : m_positions) {
        vs.push_back(toml::value{ {"x", v->x}, {"y", v->y} });
    }
    file["stage"] = toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"location", vs}};

    std::ofstream outf(fileName);

    if (file.contains("debug")) {
        outf << std::setw(20) << toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"debug", file["debug"].as_table()}};
    }

    outf << std::setw(200) << toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"nautilai", file["nautilai"].as_table()}};
    outf << std::setw(20) << toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"device", file["device"].as_table()}};
    outf << std::setw(40) << toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"acquisition", file["acquisition"].as_table()}};
    outf << std::setw(20) << toml::basic_value<toml::preserve_comments, tsl::ordered_map>{{"stage", file["stage"].as_table()}};

    outf.close();
}


void StageControl::loadList(std::string fileName) {
    spdlog::info("Loading stage positions from file: {}", fileName);

    for (auto v : m_positions) { delete v; }

    m_positions.clear();
    ui->stageLocations->clear();

    size_t n = 1;

    auto file = toml::parse(fileName);
    for (auto& v : toml::find_or<std::vector<toml::table>>(file, "stage", "location", std::vector<toml::table>{})) {
        int pos = n++;
        auto x = static_cast<double>(v.at("x").as_floating());
        auto y = static_cast<double>(v.at("y").as_floating());

        spdlog::info("Loading stage position: ({}, {})", x, y);

        StagePosition* item = new StagePosition(pos, x, y);
        m_positions.push_back(item);
        ui->stageLocations->addItem(item);
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
