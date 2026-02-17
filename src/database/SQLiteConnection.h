/*
 * SQLiteConnection.h
 *
 *      Author: Andreas Volz
 */

#ifndef SQLITECONNECTION_H_
#define SQLITECONNECTION_H_

// project
#include "common/platform.h"

// system
#include <string>

// forward declarations
typedef struct sqlite3 sqlite3;

class Statement;

class SQLiteConnection
{
public:
  enum class Result
  {
    Busy,
    Done,
    Row,
    Error,
    Misuse
  };

  SQLiteConnection(const fs::path &database);
  ~SQLiteConnection();

  bool exec(const std::string& sql);

  bool begin();

  bool commit();

  bool rollback();

  int64_t lastInsertRowID();

  static std::string makePlaceholders(size_t n);

private:
  friend Statement;
  sqlite3* mDB;

  bool open(const fs::path &database);
  bool close();
};

#endif /* SQLITECONNECTION_H_ */
