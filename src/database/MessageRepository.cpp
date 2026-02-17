/*
 * MessageRepository.cpp
 *
 *      Author: Andreas Volz
 */

// @formatter:off
// this file is better to understand without the Eclipse auto formatter

// project
#include "MessageRepository.h"

// system
#include <string>
#include <stdexcept> // TODO: only needed until custom exception is created

int64_t MessageRepository::insert(const MessageRow &message_row)
{
  mInsertStmt.bind(":account_id", message_row.account_id);
  mInsertStmt.bind(":chat_id",    message_row.chat_id);
  mInsertStmt.bind(":sender_id",  message_row.sender_id);
  mInsertStmt.bind(":media_id",   message_row.media_id);
  mInsertStmt.bind(":timestamp",  message_row.timestamp);
  mInsertStmt.bind(":text",       message_row.text);

  mInsertStmt.step();
  mInsertStmt.reset();

  return mSQLCon.lastInsertRowID();
}

MessageRow MessageRepository::getByMessageId(int64_t message_id)
{
  mSelectByIdStmt.reset();
  mSelectByIdStmt.bind(":message_id", message_id);

  if (mSelectByIdStmt.step() == SQLiteConnection::Result::Row)
  {
    MessageRow message_row {};

    message_row.message_id  = message_id;
    message_row.account_id  = mSelectByIdStmt.getInt64(0);
    message_row.chat_id     = mSelectByIdStmt.getInt64(1);
    message_row.sender_id   = mSelectByIdStmt.getInt64(2);
    message_row.media_id    = mSelectByIdStmt.getInt64(3);
    message_row.timestamp   = mSelectByIdStmt.getInt64(4);
    message_row.text        = mSelectByIdStmt.getText(5);

    return message_row;
  }

  throw std::runtime_error("Message not found"); // TODO: custom exception
}

std::vector<int64_t> MessageRepository::getDistinctSenderIdsByChatId(int64_t chat_id)
{
  std::vector<int64_t> distinct_sender_ids;
  mSelectByDistinctSenderIdStmt.reset();

  mSelectByDistinctSenderIdStmt.bind(":chat_id", chat_id);

  while (mSelectByDistinctSenderIdStmt.step() == SQLiteConnection::Result::Row)
  {
    distinct_sender_ids.push_back(mSelectByDistinctSenderIdStmt.getInt64(0));
  }

  return distinct_sender_ids;
}

std::vector<int64_t> MessageRepository::getDistinctMediaIdsByChatId(int64_t chat_id)
{
  std::vector<int64_t> distinct_media_ids;
  mSelectByDistinctMediaIdStmt.reset();

  mSelectByDistinctMediaIdStmt.bind(":chat_id", chat_id);

  while (mSelectByDistinctMediaIdStmt.step() == SQLiteConnection::Result::Row)
  {
    distinct_media_ids.push_back(mSelectByDistinctMediaIdStmt.getInt64(0));
  }

  return distinct_media_ids;
}

std::vector<MessageRow> MessageRepository::getByChatId(int64_t chat_id)
{
  std::vector<MessageRow> messages;
  mSelectByChatIdStmt.reset();

  mSelectByChatIdStmt.bind(":chat_id", chat_id);

  while (mSelectByChatIdStmt.step() == SQLiteConnection::Result::Row)
  {
    MessageRow message_row {};

    message_row.chat_id = chat_id;
    message_row.message_id = mSelectByChatIdStmt.getInt64(0);
    message_row.account_id = mSelectByChatIdStmt.getInt64(1);
    message_row.sender_id = mSelectByChatIdStmt.getInt64(2);
    message_row.media_id = mSelectByChatIdStmt.getInt64(3);
    message_row.timestamp = mSelectByChatIdStmt.getInt64(4);
    message_row.text = mSelectByChatIdStmt.getText(5);

    messages.push_back(std::move(message_row));
  }

  return messages;
}

bool MessageRepository::createTable(SQLiteConnection &sql_con)
{
  std::string messages_table_sql =
      "CREATE TABLE IF NOT EXISTS messages ("
      "message_id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "account_id INTEGER NOT NULL, "
      "chat_id INTEGER NOT NULL, "
      "sender_id INTEGER, "
      "media_id INTEGER, "
      "timestamp INTEGER, "
      "text TEXT"
      ");";

  return sql_con.exec(messages_table_sql);
}
// @formatter:on
