/**
 * @file savepoint.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief SQL savepoint interface for transaction management
 * @version 0.2
 * @date 2026-03-14
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_SAVEPOINT_H
#define SQL_SAVEPOINT_H

#include <string>

namespace sql {
/**
 * @class SavepointBase
 * @brief Abstract base class for SQL savepoints
 * @details Defines the interface for transaction savepoints
 */
class SavepointBase {
   public:
    virtual ~SavepointBase() = default;

    /**
     * @brief Get the name of the savepoint
     * @return Reference to the savepoint name
     */
    virtual const std::string& name() const = 0;
};

/**
 * @typedef Savepoint
 * @brief Unique pointer to SavepointBase
 */
using Savepoint = std::unique_ptr<SavepointBase>;
}  // namespace sql
#endif