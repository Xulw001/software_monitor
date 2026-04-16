/**
 * @file value.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL value types and blob handling utilities
 * @version 0.2
 * @date 2026-03-01
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_VALUE_H
#define SQL_VALUE_H

#include <string.h>

#include <memory>

namespace sql {
/**
 * @enum ValueType
 * @brief SQL value types
 * @details Represents the different types of values supported in SQL
 */
enum class ValueType {
    TYPE_NULL = 0,  ///< NULL value
    TYPE_INTEGER,   ///< Integer value
    TYPE_REAL,      ///< Real/floating-point value
    TYPE_TEXT,      ///< Text string value
    TYPE_BLOB       ///< Binary blob value
};

/**
 * @struct Blob
 * @brief Binary blob value
 * @details Manages binary blob data with proper memory allocation and access
 */
struct Blob {
   public:
    /**
     * @brief Construct a new Blob object
     * @param v Pointer to the blob data
     * @param n Size of the blob data in bytes
     */
    explicit Blob(const void* v, int n) : size_(n + 1) {
        blob_.reset(new uint8_t[size_]);
        memcpy(blob_.get(), v, size_ - 1);
    }

    /**
     * @brief Get the size of the blob
     * @return Size of the blob in bytes
     */
    int size() const { return size_ - 1; }

    /**
     * @brief Get a pointer to the blob data
     * @return Pointer to the blob's data buffer
     */
    void* ptr() const { return blob_.get(); }

   private:
    int size_;  ///< Size of the blob value in bytes
    std::shared_ptr<uint8_t[]>
        blob_;  ///< Shared pointer to the blob's data buffer
};
}  // namespace sql

#endif