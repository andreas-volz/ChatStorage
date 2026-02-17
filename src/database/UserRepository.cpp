/*
 * UserRepository.cpp
 *
 *      Author: Andreas Volz
 */

// @formatter:off
// this file is better to understand without the Eclipse auto formatter

// project
#include "UserRepository.h"

// system
#include <string>
#include <stdexcept> // TODO: only needed until custom exception is created

int64_t UserRepository::insert(const UserRow &user_row)
{
  mInsertStmt.bind(":account_id", user_row.account_id);
  mInsertStmt.bind(":name",       user_row.name);
  mInsertStmt.bind(":is_system",  user_row.is_system);

  mInsertStmt.step();
  mInsertStmt.reset();

  return mSQLCon.lastInsertRowID();
}

UserRow UserRepository::getByUserId(int64_t user_id)
{
  mSelectByIdStmt.reset();
  mSelectByIdStmt.bind(":user_id", user_id);

  if (mSelectByIdStmt.step() == SQLiteConnection::Result::Row)
  {
    UserRow user_row {};

    user_row.user_id    = user_id;
    user_row.account_id = mSelectByIdStmt.getInt64(0);
    user_row.name       = mSelectByIdStmt.getText(1);
    user_row.is_system  = mSelectByIdStmt.getInt64(2);

    return user_row;
  }
  mSelectByIdStmt.reset();

  throw std::runtime_error("User not found"); // TODO: custom exception
}

std::vector<UserRow> UserRepository::getByUserIds(std::vector<int64_t> user_ids)
{
  if (user_ids.empty())
  {
    // return empty vector to prevent sql execution with empty list
    return {};
  }

  std::vector<UserRow> user_rows;

  std::string users_sql = "SELECT user_id, account_id, name, is_system FROM users WHERE user_id IN (" + SQLiteConnection::makePlaceholders(user_ids.size()) + ")";
  Statement users_stmt(mSQLCon, users_sql);

  users_stmt.reset();
  users_stmt.bindInt64Container(1, user_ids);

  while (users_stmt.step() == SQLiteConnection::Result::Row)
  {
    UserRow user_row {};

    user_row.user_id = users_stmt.getInt64(0);
    user_row.account_id = users_stmt.getInt64(1);
    user_row.name       = users_stmt.getText(2);
    user_row.is_system  = users_stmt.getInt64(3);

    user_rows.push_back(std::move(user_row));
  }

  return user_rows;
}

std::vector<UserRow> UserRepository::listUsers()
{
  std::vector<UserRow> user_rows;
  mSelectAllStmt.reset();

  while (mSelectAllStmt.step() == SQLiteConnection::Result::Row)
  {
    UserRow user_row {};

    user_row.user_id    = mSelectAllStmt.getInt64(0);
    user_row.account_id = mSelectAllStmt.getInt64(1);
    user_row.name       = mSelectAllStmt.getText(2);
    user_row.is_system  = mSelectAllStmt.getInt64(3);

    user_rows.push_back(std::move(user_row));
  }
  mSelectAllStmt.reset();

  return user_rows;
}

int64_t UserRepository::getSystemUserId()
{
  mSelectSystemUserStmt.reset();
  mSelectSystemUserStmt.bind(":account_id", 0); // hard coded account_id = 0 for non Cloud version

  if (mSelectSystemUserStmt.step() == SQLiteConnection::Result::Row)
  {
    int64_t system_id = mSelectSystemUserStmt.getInt64(0);

    return system_id;
  }
  mSelectSystemUserStmt.reset();

  throw std::runtime_error("User not found"); // TODO: custom exception
}

bool UserRepository::createTable(SQLiteConnection &sql_con)
{
  std::string users_table_sql =
      "CREATE TABLE IF NOT EXISTS users ("
      "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "account_id INTEGER NOT NULL, "
      "name TEXT, "
      "is_system INTEGER NOT NULL"
      ");";

  return sql_con.exec(users_table_sql);
}

void UserRepository::createSystemUser(int64_t account_id)
{
  int64_t system_user_id = 0;
  const std::string default_system_user = "__system__";
  std::string users_table_sql =
      "INSERT OR IGNORE INTO "
      "users (user_id, account_id, name, is_system) "
      "VALUES (" + std::to_string(system_user_id) +  ", " + std::to_string(account_id) + ", '" + default_system_user + "', 1)";

  mSQLCon.exec(users_table_sql);
}

// @formatter:on
/*
 void update(const User &user)
 {
 Statement stmt(sql_con, "UPDATE users SET name=:name WHERE id=:id;");
 stmt.bind(":id", user.id);
 stmt.bind(":name", user.name);
 stmt.step();
 }*/
