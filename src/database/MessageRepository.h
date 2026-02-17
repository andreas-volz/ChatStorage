/*
 * MessageRepository.h
 *
 *      Author: Andreas Volz
 */

#ifndef MESSAGEREPOSITORY_H_
#define MESSAGEREPOSITORY_H_

// project
#include "database/SQLiteConnection.h"
#include "database/Statement.h"
#include "database/MessageRow.h"

// system
#include <vector>

class MessageRepository
{
public:

  MessageRepository(SQLiteConnection &sql_con) :
// @formatter:off
      mSQLCon(sql_con),
      mInsertStmt(mSQLCon,
          "INSERT INTO messages (account_id, chat_id, sender_id, media_id, timestamp, text) "
          "VALUES (:account_id, :chat_id, :sender_id, :media_id, :timestamp, :text);"),
      mUpdateStmt(mSQLCon, "UPDATE..."),
      mSelectByIdStmt(mSQLCon,
          "SELECT account_id, chat_id, sender_id, media_id, timestamp, text "
          "FROM messages "
          "WHERE message_id = :message_id"),
      mSelectByChatIdStmt(mSQLCon,
          "SELECT message_id, account_id, sender_id, media_id, timestamp, text FROM messages "
          "WHERE chat_id = :chat_id"),
      mSelectByDistinctSenderIdStmt(mSQLCon,
          "SELECT DISTINCT sender_id "
          "FROM messages "
          "WHERE chat_id = :chat_id;"),
      mSelectByDistinctMediaIdStmt(mSQLCon,
          "SELECT DISTINCT media_id "
          "FROM messages "
          "WHERE chat_id = :chat_id;")
// @formatter:on
  {
  }

  int64_t insert(const MessageRow &message_row);

  MessageRow getByMessageId(int64_t message_id);

  std::vector<int64_t> getDistinctSenderIdsByChatId(int64_t chat_id);

  std::vector<int64_t> getDistinctMediaIdsByChatId(int64_t chat_id);

  std::vector<MessageRow> getByChatId(int64_t chat_id);

// TODO: update()
// TODO: delete()

  static bool createTable(SQLiteConnection &sql_con);

  ~MessageRepository() = default;

private:
  SQLiteConnection &mSQLCon;
  Statement mInsertStmt;
  Statement mUpdateStmt;
  Statement mSelectByIdStmt;
  Statement mSelectByChatIdStmt;
  Statement mSelectByDistinctSenderIdStmt;
  Statement mSelectByDistinctMediaIdStmt;
};

#endif /* MESSAGEREPOSITORY_H_ */
