/*
 * SQLiteConnection.cpp
 *
 *      Author: Andreas Volz
 */

// System
#include <sqlite3.h>
#include <iostream>

// project
#include "SQLiteConnection.h"
#include "common/Logger.h"

using namespace std;

static Logger logger("ChatStorage.SQLiteConnection");

SQLiteConnection::SQLiteConnection(const fs::path &database) :
    mDB(nullptr)
{
  open(database);
}

SQLiteConnection::~SQLiteConnection()
{
  close();
}

bool SQLiteConnection::open(const fs::path &database)
{
  int rc = sqlite3_open(database.string().c_str(), &mDB);
  if (rc != SQLITE_OK)
  {
    std::cerr << "Error (" << rc << ") - Cannot open DB: " << database << endl;
    return false;
  }
  LOG4CXX_INFO(logger, "Successful opened SQLite database in: " + string(fs::absolute(database)));

   // IMPORTANT: SQLite disables foreign key enforcement by default.
   // This PRAGMA must be enabled for each database connection after opening it.
  exec("PRAGMA foreign_keys = ON;");

  // Enable Write-Ahead Logging mode for better concurrency between readers and writers.
  // Allows multiple readers while a single writer can still modify the database.
  exec("PRAGMA journal_mode = WAL");

  // Set synchronous mode to NORMAL for faster commits with minimal risk of DB corruption.
  // Reduces fsync calls while keeping the database structurally consistent.
  exec("PRAGMA synchronous = NORMAL");
  return true;
}

bool SQLiteConnection::close()
{
  int rc = sqlite3_close(mDB);
  if (rc != SQLITE_OK)
  {
    std::cerr << "Error (" << rc << ") - Cannot close DB" << endl;
    ;
    return false;
  }
  return true;
}

bool SQLiteConnection::exec(const std::string &sql)
{
  char *errMsg = nullptr;
  int rc = sqlite3_exec(mDB, sql.c_str(), nullptr, nullptr, &errMsg);
  if (rc != SQLITE_OK)
  {
    std::cerr << "SQL error (" << rc << "): " << errMsg << endl;
    sqlite3_free(errMsg);
    return false;
  }
  return true;
}

bool SQLiteConnection::begin()
{
  return exec("BEGIN;");
}

bool SQLiteConnection::commit()
{
  return exec("COMMIT;");
}

bool SQLiteConnection::rollback()
{
  return exec("ROLLBACK;");
}

int64_t SQLiteConnection::lastInsertRowID()
{
  return sqlite3_last_insert_rowid(mDB);
}

std::string SQLiteConnection::makePlaceholders(size_t n)
{
  if (n == 0)
    return "";  // never return an empty string to prevent SQL problems

  std::string s = "?";
  for (size_t i = 1; i < n; ++i)
  {
    s += ", ?";
  }
  return s;
}

