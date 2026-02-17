/*
 * ChatStorage.cpp
 *
 *      Author: Andreas Volz
 */

// project API include
#include "chatstorage/ChatStorage.h"

// project internal
#include "database/SQLiteConnection.h"
#include "database/UserRepository.h"
#include "database/MessageRepository.h"
#include "database/ChatRepository.h"
#include "database/MediaRepository.h"
#include "database/PersistenceManager.h"
#include "importer/ImportManager.h"

// system
#include <filesystem>

using namespace std;

struct ChatStorage::Impl
{
  std::unique_ptr<SQLiteConnection> sql;
  std::unique_ptr<UserRepository> user_repo;
  std::unique_ptr<MessageRepository> message_repo;
  std::unique_ptr<ChatRepository> chat_repo;
  std::unique_ptr<MediaRepository> media_repo;
  std::unique_ptr<PersistenceManager> persistence;
};

ChatStorage::ChatStorage(const std::filesystem::path &db_path, const std::filesystem::path &media_perisistence_path) :
    mImpl(std::make_unique<Impl>())
{
  mImpl->sql = std::make_unique<SQLiteConnection>(db_path);

  mImpl->sql->begin();
  UserRepository::createTable(*mImpl->sql);
  MessageRepository::createTable(*mImpl->sql);
  ChatRepository::createTable(*mImpl->sql);
  MediaRepository::createTable(*mImpl->sql);
  mImpl->sql->commit();

  mImpl->user_repo = std::make_unique<UserRepository>(*mImpl->sql);
  mImpl->user_repo->createSystemUser();
  mImpl->message_repo = std::make_unique<MessageRepository>(*mImpl->sql);
  mImpl->chat_repo = std::make_unique<ChatRepository>(*mImpl->sql);
  mImpl->media_repo = std::make_unique<MediaRepository>(*mImpl->sql, media_perisistence_path);

  mImpl->persistence = std::make_unique<PersistenceManager>(*mImpl->sql, *mImpl->user_repo, *mImpl->message_repo,
      *mImpl->chat_repo, *mImpl->media_repo);

  createChatEntries();
}

ChatStorage::~ChatStorage() = default;

std::filesystem::path ChatStorage::getMediaPersistencePath()
{
  return mImpl->media_repo->getMediaPersistencePath();
}

void ChatStorage::createChatEntries()
{
  mChatEntryList.clear();
  size_t chat_index = 0;
  for (const auto &chat : listChats())
  {
    ChatEntry chat_entry = {};
    chat_entry.database_id = chat.getDatabaseId();
    chat_entry.name = chat.getName();
    mChatEntryList.emplace_back(chat_entry);
    chat_index++;
  }
}

std::vector<ChatEntry> ChatStorage::getChatEntryList()
{
  return mChatEntryList;
}

std::vector<Chat> ChatStorage::listChats()
{
  return mImpl->persistence->listChats();
}

std::unique_ptr<ChatContext> ChatStorage::loadByChatEntry(ChatEntry chat_entry)
{
  return mImpl->persistence->loadByChatId(chat_entry.database_id);
}

std::unique_ptr<ChatContext> ChatStorage::loadByChatId(int64_t chat_id)
{
  return mImpl->persistence->loadByChatId(chat_id);
}

void ChatStorage::save(ChatContext& ctx, const std::filesystem::path& import_media_path)
{
  mImpl->persistence->save(ctx, import_media_path);
}

