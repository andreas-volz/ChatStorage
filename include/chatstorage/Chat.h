/*
 * Chat.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHAT_H_
#define CHAT_H_

// project public API
#include "chatstorage/Message.h"
#include "chatstorage/User.h"
#include "chatstorage/ChatSource.h"

// system
#include <cstdint>
#include <string>
#include <vector>

class Chat
{
  /**
   * friend is needed as only *and only* ChatContext is allowed to  to patch a new IDs after DB insert
   */
  friend class ChatContext;

public:
  Chat(int64_t runtime_id, int64_t database_id, std::string name, ChatSource source) :
      mRuntimeId(runtime_id),
      mDatabaseId(database_id),
      mName(std::move(name)),
      mSource(source)
  {
  }

  ~Chat() = default;

  int64_t getRuntimeId() const;

  int64_t getDatabaseId() const;

  const std::string& getName() const;

  ChatSource getSource() const;

  static constexpr int64_t RT_START_ID = 0;
  static constexpr int64_t DB_NO_ID = -1;

private:
  void setDatabaseId(int64_t database_id);

  int64_t mRuntimeId;
  int64_t mDatabaseId;
  std::string mName;
  ChatSource mSource;
};

#endif /* CHAT_H_ */
