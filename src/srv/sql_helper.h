#ifndef SQL_HELPER_H
#define SQL_HELPER_H

#include <atomic>
#include <vector>

#include "sql/connection.h"

class SqlHelper {
   public:
    static SqlHelper& instance() {
        static SqlHelper instance;
        return instance;
    }

    bool Connect();

    bool SelectSoftwareIdByPath(const std::string& filepath, int& software_id);
    bool InsertSoftware(const std::string& product, const std::string& company,
                        const std::string& version, int& software_id);
    bool InsertAsset(const std::string& path, const int software_id, int type);

   private:
    sql::Connection conn_;
    std::atomic<int> max_software_id_{0};
};

#endif