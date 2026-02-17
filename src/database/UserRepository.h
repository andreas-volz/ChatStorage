/*
 * UserRepository.h
 *
 *      Author: Andreas Volz
 */

#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

// project
#include "database/SQLiteConnection.h"
#include "database/Statement.h"
#include "database/UserRow.h"

// system
#include <vector>

class UserRepository
{
public:
  UserRepository(SQLiteConnection &sql_con) :
// @formatter:off
      mSQLCon(sql_con),
      mInsertStmt(mSQLCon,
          "INSERT INTO users (account_id, name, is_system) "
          "VALUES (:account_id, :name, :is_system);"),
      mUpdateStmt(mSQLCon, "UPDATE..."),
      mSelectByIdStmt(mSQLCon,
          "SELECT account_id, name, is_system "
          "FROM users "
          "WHERE user_id=:user_id"),
      mSelectAllStmt(mSQLCon,
          "SELECT user_id, account_id, name, is_system "
          "FROM users"),
      mSelectSystemUserStmt(mSQLCon,
          "SELECT user_id "
          "FROM users "
          "WHERE account_id=:account_id AND is_system=1")
// @formatter:on
  {
  }

  ~UserRepository() = default;

  void createSystemUser(int64_t account_id = 0);

  int64_t insert(const UserRow &user_row);

  UserRow getByUserId(int64_t user_id);

  int64_t getSystemUserId();

  std::vector<UserRow> listUsers();

  std::vector<UserRow> getByUserIds(std::vector<int64_t> user_ids);

  static bool createTable(SQLiteConnection &sql_con);

  // TODO: update()
  // TODO: delete()

private:
  SQLiteConnection &mSQLCon;
  Statement mInsertStmt;
  Statement mUpdateStmt;
  Statement mSelectByIdStmt;
  Statement mSelectAllStmt;
  Statement mSelectSystemUserStmt;
  // TODO: more prepared statement if needed
};

#endif /* USERREPOSITORY_H_ */
