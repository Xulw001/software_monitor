/**
 * @file prepared_statment.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL prepared statement interface for parameterized queries
 * @version 0.2
 * @date 2026-02-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_PREPARED_STATMENT_H
#define SQL_PREPARED_STATMENT_H

#include "exception.h"
#include "statement.h"

namespace sql {
/**
 * @class PreparedStatementBase
 * @brief Abstract base class for SQL prepared statements
 * @details Defines the interface for parameterized SQL queries
 * @extends StatementBase
 */
class PreparedStatementBase : public StatementBase {
   public:
    virtual ~PreparedStatementBase() = default;

    /**
     * @brief Execute the prepared query and return a ResultSet
     * @return Reference to the result set containing query results
     */
    virtual ResultSet ExecuteQuery() = 0;

    /**
     * @brief Execute the prepared update statement
     */
    virtual void ExecuteUpdate() = 0;

    /**
     * @brief Set string parameter
     * @param index Parameter index (1-based)
     * @param value String value to set
     */
    virtual void SetString(int index, std::string value) = 0;

    /**
     * @brief Set float parameter
     * @param index Parameter index (1-based)
     * @param value Float value to set
     */
    virtual void SetFloat(int index, float value) = 0;

    /**
     * @brief Set double parameter
     * @param index Parameter index (1-based)
     * @param value Double value to set
     */
    virtual void SetDouble(int index, double value) = 0;

    /**
     * @brief Set short parameter
     * @param index Parameter index (1-based)
     * @param value Short value to set
     */
    virtual void SetShort(int index, short value) = 0;

    /**
     * @brief Set int parameter
     * @param index Parameter index (1-based)
     * @param value Int value to set
     */
    virtual void SetInt(int index, int value) = 0;

    /**
     * @brief Set long long parameter
     * @param index Parameter index (1-based)
     * @param value Long long value to set
     */
    virtual void SetLong(int index, long long value) = 0;

    /**
     * @brief Set boolean parameter
     * @param index Parameter index (1-based)
     * @param value Boolean value to set
     */
    virtual void SetBoolean(int index, bool value) = 0;

    /**
     * @brief Set NULL parameter
     * @param index Parameter index (1-based)
     */
    virtual void SetNull(int index) = 0;

    /**
     * @brief Set blob parameter
     * @param index Parameter index (1-based)
     * @param value Blob value to set
     */
    virtual void SetBlob(int index, Blob value) = 0;

    /**
     * @brief Add current parameter set to batch
     * @details Accumulates the current parameter values for later execution
     */
    virtual void AddBatch() = 0;

    /**
     * @brief Clear all set parameters
     */
    virtual void ClearParameters() = 0;

    /**
     * @brief Execute all statements in the batch
     */
    virtual void ExecuteBatch() = 0;

    /**
     * @brief Execute query with SQL string
     * @param sql SQL query string
     * @throws SQLException Always throws "unsupported call!"
     */
    virtual ResultSet ExecuteQuery(const std::string& sql) override {
        throw SQLException("unsupported call!");
    }

    /**
     * @brief Execute update with SQL string
     * @param sql SQL update string
     * @throws SQLException Always throws "unsupported call!"
     */
    virtual void ExecuteUpdate(const std::string& sql) override {
        throw SQLException("unsupported call!");
    }

    /**
     * @brief Add batch with SQL string
     * @param sql SQL statement string
     * @throws SQLException Always throws "unsupported call!"
     */
    virtual void AddBatch(const std::string& sql) override {
        throw SQLException("unsupported call!");
    }
};

/**
 * @typedef PreparedStatement
 * @brief Unique pointer to PreparedStatementBase
 */
using PreparedStatement = std::unique_ptr<PreparedStatementBase>;
}  // namespace sql

#endif