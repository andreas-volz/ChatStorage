/*
 * PersistenceManager.h
 *
 *      Author: Andreas Volz
 */

#ifndef PERSISTENCEMANAGER_H_
#define PERSISTENCEMANAGER_H_

// project public API
#include "chatstorage/ChatContext.h"

// project private
#include "database/SQLiteConnection.h"
#include "database/UserRepository.h"
#include "database/MessageRepository.h"
#include "database/MediaRepository.h"
#include "database/ChatRepository.h"
#include "common/platform.h"

class PersistenceManager
{
public:
  PersistenceManager(SQLiteConnection &sql_con, UserRepository &user_repo, MessageRepository &message_repo,
      ChatRepository &chat_repo, MediaRepository &media_repo) :
      mSQLCon(sql_con),
      mUserRepo(user_repo),
      mMessageRepo(message_repo),
      mChatRepo(chat_repo),
      mMediaRepo(media_repo)
  {
  }

  ~PersistenceManager() = default;

  /**
   * @param import_media_path The absolute path where the referenced source media is located. This is the copy source path..
   */
  void save(ChatContext& ctx, const fs::path& import_media_path = {});

  std::unique_ptr<ChatContext> loadByChatId(int64_t chat_id);

  std::unique_ptr<ChatContext> loadByMessageId(int64_t message_id);

  std::unique_ptr<ChatContext> loadByUserId(int64_t user_id);

  std::vector<Chat> listChats();

private:
  SQLiteConnection &mSQLCon;
  UserRepository &mUserRepo;
  MessageRepository &mMessageRepo;
  ChatRepository &mChatRepo;
  MediaRepository &mMediaRepo;
};

#endif /* PERSISTENCEMANAGER_H_ */
