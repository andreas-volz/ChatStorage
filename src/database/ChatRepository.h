/*
 * ChatRepository.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATREPOSITORY_H_
#define CHATREPOSITORY_H_

// project
#include "database/SQLiteConnection.h"
#include "database/Statement.h"
#include "database/ChatRow.h"

// system
#include <vector>

class ChatRepository
{
public:
  ChatRepository(SQLiteConnection &sql_con) :
// @formatter:off
      mSQLCon(sql_con),
      mInsertStmt(mSQLCon,
          "INSERT INTO chats (account_id, name, source) "
          "VALUES (:account_id, :name, :source);"),
      mUpdateStmt(mSQLCon, "UPDATE..."),
      mSelectByIdStmt(mSQLCon,
          "SELECT account_id, name, source "
          "FROM chats "
          "WHERE chat_id=:chat_id"),
      mSelectListChatsStmt(mSQLCon,
          "SELECT chat_id, account_id, name, source "
          "FROM chats")
// @formatter:on
  {
  }

  ~ChatRepository() = default;

  int64_t insert(const ChatRow &chat_row);

  ChatRow getByChatId(int64_t chat_id);

  std::vector<ChatRow> listChats();

  static bool createTable(SQLiteConnection &sql_con);

private:
  SQLiteConnection &mSQLCon;
  Statement mInsertStmt;
  Statement mUpdateStmt;
  Statement mSelectByIdStmt;
  Statement mSelectListChatsStmt;
};

#endif /* CHATREPOSITORY_H_ */
