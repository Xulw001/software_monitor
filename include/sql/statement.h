/**
 * @file statement.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL statement interface for executing queries and updates
 * @version 0.2
 * @date 2026-02-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_STATEMENT_H
#define SQL_STATEMENT_H

#include "resultset.h"

namespace sql {
/**
 * @class StatementBase
 * @brief Abstract base class for SQL statements
 * @details Defines the interface for executing SQL queries and updates
 */
class StatementBase {
   public:
    virtual ~StatementBase() = default;

    /**
     * @brief Execute a SQL query and return a ResultSet
     * @param sql SQL query string
     * @return Reference to the result set containing query results
     */
    virtual ResultSet ExecuteQuery(const std::string& sql) = 0;

    /**
     * @brief Get the current result set
     * @return Reference to the current result set
     */
    virtual ResultSet GetResultSet() = 0;

    /**
     * @brief Execute a SQL update statement
     * @param sql SQL update string
     */
    virtual void ExecuteUpdate(const std::string& sql) = 0;

    /**
     * @brief Add a SQL statement to the batch
     * @param sql SQL statement to add to the batch
     */
    virtual void AddBatch(const std::string& sql) = 0;

    /**
     * @brief Execute all statements in the batch
     */
    virtual void ExecuteBatch() = 0;

    /**
     * @brief Clear all statements in the batch
     */
    virtual void ClearBatch() = 0;
};

/**
 * @typedef Statement
 * @brief Unique pointer to StatementBase
 */
using Statement = std::unique_ptr<StatementBase>;
}  // namespace sql

#endif