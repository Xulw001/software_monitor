/**
 * @file resultset.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL result set interface for query results
 * @version 0.2
 * @date 2026-03-08
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_RESULTSET_H
#define SQL_RESULTSET_H

#include <memory>
#include <string>

#include "value.h"

namespace sql {
/**
 * @class ResultSetBase
 * @brief Abstract base class for SQL result sets
 * @details Defines the interface for accessing query results
 * @note Column index numbering is 1-based.
 * @warning Invoking value retrieval methods before calling NextRow() will
 *          result in undefined behavior.
 */
class ResultSetBase {
   public:
    virtual ~ResultSetBase() = default;

    /**
     * @brief Move to the next row in the result set
     * @return True if there is a next row, false otherwise
     */
    virtual bool NextRow() = 0;

    /**
     * @brief Get string value by column index
     * @param index Column index
     * @return String value at the specified column
     */
    virtual std::string GetString(int index) = 0;

    /**
     * @brief Get string value by column name
     * @param column_name Column name
     * @return String value at the specified column
     */
    virtual std::string GetString(const std::string& column_name) = 0;

    /**
     * @brief Get boolean value by column index
     * @param index Column index
     * @return Boolean value at the specified column
     */
    virtual bool GetBoolean(int index) = 0;

    /**
     * @brief Get boolean value by column name
     * @param column_name Column name
     * @return Boolean value at the specified column
     */
    virtual bool GetBoolean(const std::string& column_name) = 0;

    /**
     * @brief Get short value by column index
     * @param index Column index
     * @return Short value at the specified column
     */
    virtual short GetShort(int index) = 0;

    /**
     * @brief Get short value by column name
     * @param column_name Column name
     * @return Short value at the specified column
     */
    virtual short GetShort(const std::string& column_name) = 0;

    /**
     * @brief Get int value by column index
     * @param index Column index
     * @return Int value at the specified column
     */
    virtual int GetInt(int index) = 0;

    /**
     * @brief Get int value by column name
     * @param column_name Column name
     * @return Int value at the specified column
     */
    virtual int GetInt(const std::string& column_name) = 0;

    /**
     * @brief Get long long value by column index
     * @param index Column index
     * @return Long long value at the specified column
     */
    virtual long long GetLong(int index) = 0;

    /**
     * @brief Get long long value by column name
     * @param column_name Column name
     * @return Long long value at the specified column
     */
    virtual long long GetLong(const std::string& column_name) = 0;

    /**
     * @brief Get float value by column index
     * @param index Column index
     * @return Float value at the specified column
     */
    virtual float GetFloat(int index) = 0;

    /**
     * @brief Get float value by column name
     * @param column_name Column name
     * @return Float value at the specified column
     */
    virtual float GetFloat(const std::string& column_name) = 0;

    /**
     * @brief Get double value by column index
     * @param index Column index
     * @return Double value at the specified column
     */
    virtual double GetDouble(int index) = 0;

    /**
     * @brief Get double value by column name
     * @param column_name Column name
     * @return Double value at the specified column
     */
    virtual double GetDouble(const std::string& column_name) = 0;

    /**
     * @brief Get blob value by column index
     * @param index Column index
     * @return Blob value at the specified column
     */
    virtual Blob GetBlob(int index) = 0;

    /**
     * @brief Get blob value by column name
     * @param column_name Column name
     * @return Blob value at the specified column
     */
    virtual Blob GetBlob(const std::string& column_name) = 0;

    /**
     * @brief Check if the current value is NULL
     * @return True if the current value is NULL, false otherwise
     */
    virtual bool IsNull() = 0;
};

/**
 * @typedef ResultSet
 * @brief Reference to unique pointer of ResultSetBase
 */
using ResultSet = std::unique_ptr<ResultSetBase>&;
}  // namespace sql
#endif