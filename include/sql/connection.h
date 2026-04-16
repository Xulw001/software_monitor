/**
 * @file connection.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL connection interface for database sessions
 * @version 0.2
 * @date 2026-02-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_CONNECTION_H
#define SQL_CONNECTION_H

#include <memory>

#include "prepared_statement.h"
#include "savepoint.h"
#include "statement.h"

namespace sql {
/**
 * @class ConnectionBase
 * @brief Abstract base class for SQL connections
 * @details Defines the interface for database sessions and transactions
 */
class ConnectionBase {
   public:
    virtual ~ConnectionBase() = default;

    /**
     * @brief Get the auto-commit mode
     * @return True if auto-commit is enabled, false otherwise
     */
    virtual bool auto_commit() = 0;

    /**
     * @brief Set the auto-commit mode
     * @param auto_commit True to enable auto-commit, false to disable
     */
    virtual void set_auto_commit(bool auto_commit) = 0;

    /**
     * @brief Create a new statement
     * @return Pointer to the statement object
     */
    virtual Statement CreateStatement() = 0;

    /**
     * @brief Create a new prepared statement
     * @param sql SQL statement string
     * @return Pointer to the prepared statement object
     */
    virtual PreparedStatement PrepareStatement(const std::string& sql) = 0;

    /**
     * @brief Commit the current transaction
     */
    virtual void Commit() = 0;

    /**
     * @brief Rollback to a savepoint
     * @param savepoint Savepoint to rollback to
     */
    virtual void Rollback(Savepoint& savepoint) = 0;

    /**
     * @brief Rollback the current transaction
     */
    virtual void Rollback() = 0;

    /**
     * @brief Release a savepoint
     * @param savepoint Savepoint to release
     */
    virtual void ReleaseSavepoint(Savepoint& savepoint) = 0;

    /**
     * @brief Set a savepoint with a name
     * @param name Savepoint name
     * @return Pointer to the savepoint object
     */
    virtual Savepoint SetSavepoint(const std::string& name) = 0;
};

/**
 * @typedef Connection
 * @brief Unique pointer to ConnectionBase
 */
using Connection = std::unique_ptr<ConnectionBase>;
}  // namespace sql
#endif