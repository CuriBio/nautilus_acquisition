
#include <toml.hpp>
#include <cpr/cpr.h>
#include <QString>

#ifdef _WIN64
#include <shellapi.h>
#include <synchapi.h>
#endif

#include "autoupdate.h"

AutoUpdate::AutoUpdate(std::shared_ptr<Config> config, std::string origin, std::string channel, QWidget *parent) : QDialog(parent), ui(new Ui::AutoUpdate) {
    m_config = config;
    m_origin = origin;
    m_channel = channel;
    m_updatePath = config->userProfile / "AppData" / "Local" / "Nautilai" / "Updates";
    m_url = std::format("{}/{}.toml", m_origin, m_channel);

    spdlog::info("Set auto update url {}", m_url);
    spdlog::info("Remove old updates");
    std::filesystem::remove_all(m_updatePath);

    ui->setupUi(this);
    ui->current->setText(QString::fromStdString(std::format("You are currently using version {}.", m_config->version)));
}

void AutoUpdate::show() {
    QDialog::show();
}

bool AutoUpdate::hasUpdate() {
    spdlog::info("Checking for update");
    m_config->updateAvailable = downloadManifest();
    return m_config->updateAvailable;
}

void AutoUpdate::applyUpdate() {
    spdlog::info("Applying update");

#ifdef _WIN64
    std::string installer_path = (m_updatePath / m_file).string();
    std::string dest_path = (m_config->userProfile / "AppData" / "Local").string();
    spdlog::info("installer command {}", installer_path);
    HINSTANCE handle = ShellExecuteA(NULL, NULL, installer_path.c_str(), "/S", NULL, 1);
    WaitForSingleObject(handle, 0);
#endif
}

bool AutoUpdate::downloadManifest() {
    cpr::Response r = cpr::Get(cpr::Url{m_url.c_str()});

    if (r.status_code == 200) {
        std::istringstream ts(r.text);

        const auto data = toml::parse(ts, std::format("{}.toml", m_channel));
        std::string version = toml::find<std::string>(data, "version");
        spdlog::info("version: {}", version);
        ui->version->setText(QString::fromStdString(std::format("Version {} of Nautilai is now available.", version)));


        if (m_config && m_config->version != version) {
            m_file = toml::find<std::string>(data, "path");
            spdlog::info("path: {}", m_file);

            auto url = std::format("{}/{}", m_origin, m_file);
            spdlog::info("downloading update {}", url);

            std::filesystem::create_directory(m_updatePath);
            std::filesystem::path dl = m_updatePath / m_file;

            std::ofstream of(dl, std::ios::binary);
            cpr::Response dlr = cpr::Download(of, cpr::Url{url});

            if (dlr.status_code != 200) {
                spdlog::info("download failed: {}, {}", dlr.status_code, dlr.error.message);
                return false;
            }

            return true;
        }

        return false;
    }

    spdlog::info("Downloading manifest {} failed {}, {}", m_url, r.status_code, r.error.message);
    return false;
}
