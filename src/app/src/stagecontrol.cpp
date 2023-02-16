#include <format>
#include <spdlog/spdlog.h>

#include <QFileDialog>

#include "stagecontrol.h"
#include "ui_stagecontrol.h"

StageControl::StageControl(std::string comPort, std::string configFile, QWidget *parent) : QDialog(parent), ui(new Ui::StageControl) {
    ui->setupUi(this);
    m_comPort = comPort;
    m_tango = new TangoStage(m_comPort);
    m_tango->GetCurrentPos(m_curX, m_curY);

    m_configFile = configFile;
    loadList(m_configFile);
}

StageControl::~StageControl() {
    saveList(m_configFile, true);
    delete m_tango;
    delete ui;
}

void StageControl::SetRelativePosition(double x, double y) {
    m_tango->SetRelativePos(x, y, true);
    m_tango->GetCurrentPos(m_curX, m_curY);
}

void StageControl::SetAbsolutePosition(double x, double y) {
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

void StageControl::AddCurrentPosition() {
    int row = m_positions.size() + 1;
    double x, y;
    m_tango->GetCurrentPos(x, y);

    spdlog::info("Add current position x: {}, y: {}", x, y);
    m_curX = x;
    m_curY = y;

    StagePosition* item = new StagePosition(row, m_curX, m_curY);
    m_positions.push_back(item);
    ui->stageLocations->addItem(item);
}

const std::vector<StagePosition*>& StageControl::GetPositions() const {
    return m_positions;
}

//slots
void StageControl::on_deleteBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (row >= 0) {
        spdlog::info("Deleting item {}", row);

        m_positions.erase(m_positions.begin()+row);
        auto i = ui->stageLocations->takeItem(row);
        delete i;

        size_t n = 1;
        for (auto& i : m_positions) {
            i->pos = n++;
            i->setText(fmt::format("pos_{} - x: {}, y: {}", i->pos, i->x, i->y).c_str());
        }
    }
}

void StageControl::on_addBtn_clicked() {
    AddCurrentPosition();
}

void StageControl::on_saveListBtn_clicked() {
    auto file = QFileDialog::getSaveFileName(this, "Save location file", "C:\\", "Text files (*.toml)");

    if (!file.isEmpty()) {
        saveList(file.toStdString(), false);
    }
}

void StageControl::saveList(std::string fileName, bool fileExists) {
    spdlog::info("Saving stage positions to file: {}", fileName);
    toml::value file;
    if (fileExists) {
        file = toml::parse<toml::preserve_comments>(fileName);
    } else {
        file = toml::value {};
    }

    toml::array vs{};
    for (auto& v : m_positions) {
        vs.push_back(toml::value{ {"x", v->x}, {"y", v->y} });
    }
    file["stage"] = toml::value{{"location", vs}};

    std::ofstream outf;
    outf.open(fileName);
    outf << file << std::endl;
    outf.close();
}

void StageControl::on_loadListBtn_clicked() {
    auto file = QFileDialog::getOpenFileName(this, "Select location file", "C:\\", "Text files (*.toml)");

    if (!file.isEmpty()) {
        loadList(file.toStdString());
    }
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
}


void StageControl::on_gotoPosBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (row >= 0) {
        StagePosition* item = static_cast<StagePosition*>(ui->stageLocations->item(row));
        spdlog::info("Setting stage to position x: {}, y: {}", item->x, item->y);
        m_tango->SetAbsolutePos(item->x, item->y, true);
        m_tango->GetCurrentPos(m_curX, m_curY);
    } else {
        spdlog::info("Invalid selection");
    }

}
