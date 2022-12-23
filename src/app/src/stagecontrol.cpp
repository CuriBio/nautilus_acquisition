#include <format>
#include <spdlog/spdlog.h>

#include <QFileDialog>

#include "stagecontrol.h"
#include "ui_stagecontrol.h"

StageControl::StageControl(QWidget *parent) : QDialog(parent), ui(new Ui::StageControl) {
    ui->setupUi(this);
    m_tango = new TangoStage();
}

StageControl::~StageControl() {
    delete m_tango;
    delete ui;
}

void StageControl::SetRelativePosition(double x, double y) {
    m_curX += x;
    m_curY += y;
    m_tango->SetRelativePos(x, y, true);
}

void StageControl::SetAbsolutePosition(double x, double y) {
    m_curX = x; m_curY = y;
    m_tango->SetAbsolutePos(x, y, true);
}

void StageControl::SetRelativeX(double x) {
    m_curX += x;
    m_tango->SetRelativePos(x, 0, true);
}

void StageControl::SetAbsoluteX(double x) {
    m_curX = x;
    m_tango->SetAbsolutePos(m_curX, m_curY, true);
}

void StageControl::SetRelativeY(double y) {
    m_curY += y;
    m_tango->SetRelativePos(0, m_curY, true);
}

void StageControl::SetAbsoluteY(double y) {
    m_curY = y;
    m_tango->SetAbsolutePos(m_curX, m_curY, true);
}

const std::vector<LocationData*>& StageControl::GetLocations() const {
    return m_locations;
}

//slots
void StageControl::on_deleteBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (row >= 0) {
        spdlog::info("Deleting item {}", row);

        m_locations.erase(m_locations.begin()+row);
        auto i = ui->stageLocations->takeItem(row);
        delete i;

        size_t n = 1;
        for (auto& i : m_locations) {
            i->pos = n++;
            i->setText(fmt::format("pos_{} - x: {}, y: {}", i->pos, i->x, i->y).c_str());
        }
    }
}

void StageControl::on_addBtn_clicked() {
    int row = m_locations.size() + 1;

    LocationData* item = new LocationData(row, row+m_curX, row+m_curY);
    m_locations.push_back(item);
    ui->stageLocations->addItem(item);
}

void StageControl::on_saveListBtn_clicked() {
    auto file = QFileDialog::getSaveFileName(this, "Save location file", "C:\\", "Text files (*.toml)");

    if (!file.isEmpty()) {
        toml::array vs{};
        for (auto& v : m_locations) {
            vs.push_back(toml::value{ {"x", v->x}, {"y", v->y} });
        }
        const toml::value stage{ {"stage", toml::value{{"location", vs}}} };

        std::ofstream outf;
        outf.open(file.toStdString());
        outf << stage << std::endl;
        outf.close();
    }
}

void StageControl::on_loadListBtn_clicked() {
    auto file = QFileDialog::getOpenFileName(this, "Select location file", "C:\\", "Text files (*.toml)");

    if (!file.isEmpty()) {
        spdlog::info("Selected file: {}", file.toStdString());

        for (auto v : m_locations) { delete v; }
        m_locations.clear();
        ui->stageLocations->clear();

        size_t n = 1;

        auto list =  toml::parse(file.toStdString());
        for (auto& v : toml::find_or<std::vector<toml::table>>(list, "stage", "location", std::vector<toml::table>{})) {
            int pos = n++;
            auto x = static_cast<double>(v.at("x").as_floating());
            auto y = static_cast<double>(v.at("y").as_floating());

            LocationData* item = new LocationData(pos, x, y);
            m_locations.push_back(item);
            ui->stageLocations->addItem(item);
        }
    }
}


void StageControl::on_gotoPosBtn_clicked() {
    int row = ui->stageLocations->currentRow();

    if (row >= 0) {
        LocationData* item = static_cast<LocationData*>(ui->stageLocations->item(row));
        spdlog::info("Setting stage to position x: {}, y: {}", item->x, item->y);
        m_curX = item->x; m_curY = item->y;
    } else {
        spdlog::info("Invalid selection");
    }

}
