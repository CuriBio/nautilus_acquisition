
#include <toml.hpp>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
#include <QString>

#ifdef _WIN64
#include <shellapi.h>
#endif

#include "autoupdate.h"
#include "ui_autoupdate.h"

AutoUpdate::AutoUpdate(std::shared_ptr<Config> config, std::string origin, std::string channel, QWidget *parent) : QDialog(parent), ui(new Ui::AutoUpdate) {
    m_config = config;
    m_origin = origin;
    m_channel = channel;
    m_updatePath = config->userProfile / "AppData" / "Local" / "Nautilai" / "Updates";
    m_url = std::format("{}/{}.toml", m_origin, m_channel);

    spdlog::info("Set auto update url {}", m_url);
    spdlog::info("Remove old updates");
    std::filesystem::remove_all(m_updatePath);

    connect(this, &AutoUpdate::sig_notify_update, this, &AutoUpdate::show);
    connect(&m_updateProcess, &QProcess::started, this, [this] {
        spdlog::info("Update started");
    });

    connect(&m_updateProcess, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
        spdlog::info("Update finished, exitCode {}, exitStatus {}", exitCode, exitStatus);
    });

    connect(&m_updateProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError err) {
        spdlog::error("Update error: {}", err);
    });

    connect(this, &AutoUpdate::sig_start_update, this, [&] {
        std::string installer_path = (m_updatePath / m_file).string();
        spdlog::info("Start update {}", installer_path);

        QString installer = QString::fromStdString(installer_path);
        m_updateProcess.start(installer);
    });

    ui->setupUi(this);
}

void AutoUpdate::show() {
    QDialog::show();
}

bool AutoUpdate::hasUpdate() {
    spdlog::info("Checking for update");
    //QDialog::show();
    m_config->updateAvailable = downloadManifest();
    return m_config->updateAvailable;
}

void AutoUpdate::applyUpdate() {
    spdlog::info("Applying update");

#ifdef _WIN64
    std::string installer_path = (m_updatePath / m_file).string();
    std::string dest_path = (m_config->userProfile / "AppData" / "Local").string();
    spdlog::info("installer command {}", installer_path);
    ShellExecuteA(NULL, NULL, installer_path.c_str(), NULL, NULL, 1);
#endif
}

bool AutoUpdate::downloadManifest() {
    cpr::Response r = cpr::Get(cpr::Url{m_url.c_str()});

    if (r.status_code == 200) {
        spdlog::info("manifest: {}", r.text);
        std::istringstream ts(r.text);

        const auto data = toml::parse(ts, std::format("{}.toml", m_channel));
        spdlog::info("version: {}", toml::find<std::string>(data, "version"));
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

    spdlog::info("Downloading manifest {} failed {}, {}", m_url, r.status_code, r.error.message);
    return false;
}
