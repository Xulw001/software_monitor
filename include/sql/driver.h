/**
 * @file driver.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL driver interface for database connectivity
 * @version 0.2
 * @date 2026-02-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_DRV_H
#define SQL_DRV_H

#include "connection.h"

namespace sql {
/**
 * @class DriverBase
 * @brief Abstract base class for SQL drivers
 * @details Defines the interface for creating database connections
 */
class DriverBase {
   public:
    /**
     * @brief Create a database connection
     * @param url Database URL
     * @param username Database username
     * @param password Database password
     * @return Database connection object
     */
    virtual Connection GetConnection(const char* url, const char* username,
                                     const char* password) = 0;
};
}  // namespace sql

#endif