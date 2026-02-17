/*
 * ChatRepository.cpp
 *
 *      Author: Andreas Volz
 */

// @formatter:off
// this file is better to understand without the Eclipse auto formatter

#include "ChatRepository.h"

// system
#include <string>
#include <stdexcept> // TODO: only needed until custom exception is created

int64_t ChatRepository::insert(const ChatRow &chat_row)
{
  mInsertStmt.bind(":account_id", chat_row.account_id);
  mInsertStmt.bind(":name",       chat_row.name);
  mInsertStmt.bind(":source",     chat_row.source);

  mInsertStmt.step();
  mInsertStmt.reset();

  return mSQLCon.lastInsertRowID();
}

ChatRow ChatRepository::getByChatId(int64_t chat_id)
{
  mSelectByIdStmt.reset();
  mSelectByIdStmt.bind(":chat_id", chat_id);

  if (mSelectByIdStmt.step() == SQLiteConnection::Result::Row)
  {
    ChatRow chat_row {};
    chat_row.chat_id    = chat_id;
    chat_row.account_id = mSelectByIdStmt.getInt64(0);
    chat_row.name       = mSelectByIdStmt.getText(1);
    chat_row.source     = mSelectByIdStmt.getInt64(2);

    return chat_row;
  }

  throw std::runtime_error("Chat not found"); // TODO: custom exception
}

std::vector<ChatRow> ChatRepository::listChats()
{
  mSelectListChatsStmt.reset();

  std::vector<ChatRow> chat_rows;

  while (mSelectListChatsStmt.step() == SQLiteConnection::Result::Row)
  {
    ChatRow chat_row {};
    chat_row.chat_id    = mSelectListChatsStmt.getInt64(0);
    chat_row.account_id = mSelectListChatsStmt.getInt64(1);
    chat_row.name       = mSelectListChatsStmt.getText(2);
    chat_row.source     = mSelectListChatsStmt.getInt64(3);
    chat_rows.emplace_back(std::move(chat_row));
  }

  return chat_rows;
}

bool ChatRepository::createTable(SQLiteConnection &sql_con)
{
  std::string chats_table_sql =
      "CREATE TABLE IF NOT EXISTS chats ("
      "chat_id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "account_id INTEGER NOT NULL, "
      "name TEXT, "
      "source INTEGER"
      ");";

  return sql_con.exec(chats_table_sql);
}
// @formatter:on
