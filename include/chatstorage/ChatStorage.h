/*
 * ChatStorage.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATSTORAGE_H_
#define CHATSTORAGE_H_

/**
 * This is the main public API header file for ChatStorage.
 */

// project public API
#include "chatstorage/ChatContext.h"

// system
#include <memory>
#include <vector>
#include <filesystem>

struct ChatEntry
{
  int64_t database_id = 0;
  std::string name;
};


class ChatStorage
{
public:
  ChatStorage(const std::filesystem::path &db_path, const std::filesystem::path &media_perisitence_path);
  ~ChatStorage();

  std::filesystem::path getMediaPersistencePath();

  std::unique_ptr<ChatContext> loadByChatEntry(ChatEntry chat_entry);

  std::unique_ptr<ChatContext> loadByChatId(int64_t chat_id);

  std::vector<ChatEntry> getChatEntryList();

  void save(ChatContext& ctx, const std::filesystem::path& import_media_path = {}); // TODO "const ChatContext& ctx", but then a lot of functions must be const...

private:
  void createChatEntries();
  std::vector<Chat> listChats();

  struct Impl;                    // forward
  std::unique_ptr<Impl> mImpl;    // PIMPL
  std::vector<ChatEntry> mChatEntryList;
};

#endif /* CHATSTORAGE_H_ */
