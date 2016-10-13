// =============================================================================
//
// Copyright (c) 2015-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "Poco/RWLock.h"
#include "SQLiteCpp.h"


namespace SQLite {


/// \brief An SQLite connection that can cache Statements.
///
/// This connection should not be accessed simultaneously by more than one
/// thread. Connections are ideally accessed by a thread safe
/// SQLiteConnectionPool.
class SQLiteConnection
{
public:
    enum class Mode
    {
        /// \brief Read only mode.
        READ_ONLY,
        /// \brief Write mode, will not create a new file.
        READ_WRITE,
        /// \brief Write mode will create a new file if it doesn't exist.
        READ_WRITE_CREATE
    };

    /// \brief Construct an SQLiteConnection with the given database and mode.
    /// \param filename The filename of the sqlite database.
    /// \param mode The access mode used during this connection.
    /// \param databaseTimeoutMilliseconds the SQLite setBusy value.
    /// \param mutex The mutex that can be shared within a connection pool.
    SQLiteConnection(const std::string& filename,
                     Mode mode = Mode::READ_ONLY,
                     uint64_t databaseTimeoutMilliseconds = 0,
                     std::size_t index = 0);

    /// \brief Destroy the SQLiteConnection.
    virtual ~SQLiteConnection();

    /// \returns a reference to the database.
    Database& database();

    /// \returns a const reference to the database.
    const Database& database() const;

    /// \brief Query whether the prepared statement exists.
    /// \param query The query key to search for.
    /// \returns true if the given statement exists.
    bool hasStatement(const std::string& query) const;

    /// \brief Get a cached statement by string.
    ///
    /// Statements are stored in a map with the query as the unique key.
    ///
    /// If the statement was used previously, it will be reset and the bindings
    /// will be cleared before returning it.
    ///
    /// \query The query to use that defines the prepared Statement.
    /// \returns a reference to the prepared statement.
    Statement& getStatement(const std::string& query) const;

    std::size_t useCount() const
    {
        return _useCount;
    }

    std::size_t index() const
    {
        return _index;
    }

    void increment()
    {
        _useCount++;
    }
protected:
    /// \brief Convert the SQLiteConnection::Mode to the native sqlite flags.
    /// \param mode The mode to convert.
    /// \returns the sqlite access flags.
    static int _toAccessFlag(Mode mode);

    /// \brief The connection access mode.
    Mode _mode = Mode::READ_ONLY;

    /// \brief The database to access.
    mutable Database _database;

    /// \brief A collection of prepared statements for this connection.
    ///
    /// These will be destroyed when the connection is destroyed.
    mutable std::map<std::string, std::unique_ptr<Statement>> _statements;

    std::size_t _useCount = 0;
    std::size_t _index = 0;
};


} // nampace SQLite
