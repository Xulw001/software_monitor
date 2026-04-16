#include "sql_helper.h"

#include "log_util.h"
#include "sql/manager.h"

bool SqlHelper::Connect() {
    try {
        sql::DriverManager::LoadDriver("sqlite");
        conn_ = std::move(sql::DriverManager::GetConnection(
            "jdbc:sqlite:install.db?journal_mode=WAL", nullptr, nullptr));

        auto stmt = conn_->PrepareStatement(
            "create table if not exists app (id integer primary key, product "
            "text, company text, version text)");
        stmt->ExecuteUpdate();

        stmt = conn_->PrepareStatement(
            "create table if not exists asset (file text primary key, "
            "software_id integer, type integer)");
        stmt->ExecuteUpdate();

        auto query = conn_->CreateStatement();
        auto& rs = query->ExecuteQuery("select max(id) from app;");
        if (rs->NextRow()) {
            max_software_id_ = rs->GetInt(1);
        }

        LOG_DBG << "Connect success!";
    } catch (const sql::SQLException& e) {
        LOG_ERR << "Connect failed, error: " << e.what();
        return false;
    }
    return true;
}

bool SqlHelper::SelectSoftwareIdByPath(const std::string& filepath,
                                       int& software_id) {
    try {
        auto stmt = conn_->PrepareStatement(
            "select software_id from asset where file = ?");
        if (!stmt) {
            LOG_ERR << "prepare statement failed!";
            return false;
        }

        stmt->SetString(1, filepath);
        auto& rs = stmt->ExecuteQuery();
        if (rs->NextRow()) {
            software_id = rs->GetInt(1);
        }
        return true;
    } catch (const std::exception& e) {
        LOG_ERR << "query software id failed, error: " << e.what();
    }
    return false;
}

bool SqlHelper::InsertSoftware(const std::string& product,
                               const std::string& company,
                               const std::string& version, int& software_id) {
    try {
        auto stmt = conn_->PrepareStatement(
            "insert into app (id, product, company, version) values (?, ?, ?, "
            "?)");
        if (!stmt) {
            LOG_ERR << "prepare statement failed!";
            return false;
        }
        // generate software id
        software_id = ++max_software_id_;

        stmt->SetInt(1, software_id);
        stmt->SetString(2, product);
        stmt->SetString(3, company);
        stmt->SetString(4, version);
        stmt->ExecuteUpdate();
        return true;
    } catch (const std::exception& e) {
        LOG_ERR << "insert software failed, error: " << e.what();
    }
    return false;
}

bool SqlHelper::InsertAsset(const std::string& path, const int software_id,
                            int type) {
    try {
        auto stmt = conn_->PrepareStatement(
            "insert into asset (file, software_id, type) values (?, ?, ?)");
        if (!stmt) {
            LOG_ERR << "prepare statement failed!";
            return false;
        }

        stmt->SetString(1, path);
        stmt->SetInt(2, software_id);
        stmt->SetInt(3, type);
        stmt->ExecuteUpdate();
        return true;
    } catch (const std::exception& e) {
        LOG_ERR << "insert asset failed, error: " << e.what();
    }
    return false;
}
