/**
 * @file exception.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL exception class for error handling
 * @version 0.2
 * @date 2026-03-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_EXCEPTION_H
#define SQL_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace sql {
/**
 * @class SQLException
 * @brief SQL exception class
 * @exception std::exception
 */
class SQLException : public std::exception {
   public:
    /**
     * @brief Construct a new SQLException object with C-style string message
     * @param message Error message
     */
    explicit SQLException(const char* message) noexcept : message_(message) {}

    /**
     * @brief Construct a new SQLException object with std::string message
     * @param message Error message
     */
    explicit SQLException(std::string message) noexcept
        : message_(std::move(message)) {}

    /**
     * @brief Get the error message
     * @return Error message as C-style string
     */
    const char* what() const noexcept override { return message_.c_str(); }

   private:
    std::string message_;  ///< Stored error message string
};

}  // namespace sql

#endif