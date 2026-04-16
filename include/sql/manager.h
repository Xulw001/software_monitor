/**
 * @file manager.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL driver manager for connection management
 * @version 0.2
 * @date 2026-02-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_MANAGER_H
#define SQL_MANAGER_H

#include <memory>
#include <string>

#include "driver.h"
#include "exception.h"
#include "platform.h"

namespace sql {
/**
 * @class DriverManager
 * @brief SQL driver manager singleton
 * @details Manages driver loading and connection creation
 */
class DriverManager {
   public:
    /**
     * @brief Get the singleton instance of DriverManager
     * @return Reference to the singleton instance
     */
    static DriverManager& GetInstance() {
        static DriverManager instance;
        return instance;
    }

    /**
     * @brief Create a connection to the database
     * @param url Database URL
     * @param username Database username
     * @param password Database password
     * @return Database connection object
     */
    static Connection GetConnection(const char* url, const char* username,
                                    const char* password) noexcept {
        return DriverManager::GetInstance().GetConnectionInternal(url, username,
                                                                  password);
    }

    /**
     * @brief Load a database driver
     * @param drvname Driver name
     * @throws SQLException If driver loading fails
     */
    static void LoadDriver(const char* drvname) {
        DriverManager::GetInstance().LoadDriverInternal(drvname);
    }

   private:
    /**
     * @brief Private constructor for singleton pattern
     */
    DriverManager() : driver_(nullptr) { ; }

    /**
     * @brief Internal method to create a connection
     * @param url Database URL
     * @param username Database username
     * @param password Database password
     * @return Database connection object
     */
    Connection GetConnectionInternal(const char* url, const char* username,
                                     const char* password) noexcept {
        return driver_ == nullptr
                   ? nullptr
                   : driver_->GetConnection(url, username, password);
    }

    /**
     * @brief Internal method to load a driver
     * @param drvname Driver name
     * @throws SQLException If driver loading or initialization fails
     */
    void LoadDriverInternal(const std::string& drvname) {
        std::string driver_name = "./libdrv-" + drvname + SUFFIX;
        DL_HANDLE handle = DL_OPEN(driver_name.c_str());
        if (!handle) throw SQLException("load driver failed!");

        DriverBase* (*CreateDriver)() =
            (DriverBase * (*)()) DL_SYM(handle, "CreateDriver");
        if (!CreateDriver) {
            DL_CLOSE(handle);
            throw SQLException("init driver failed!");
        }

        driver_.reset(CreateDriver());
    }

   private:
    std::unique_ptr<DriverBase>
        driver_;  ///< Database driver instanceDatabase driver instance
};
}  // namespace sql

#endif