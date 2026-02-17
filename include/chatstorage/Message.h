/*
 * Message.h
 *
 *      Author: Andreas Volz
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

// system
#include <cstdint>
#include <string>
#include <optional>

class Message
{
  /**
   * friend is needed as only *and only* ChatContext is allowed to  to patch a new IDs after DB insert
   */
  friend class ChatContext;

public:
  Message(int64_t runtime_id, int64_t database_id, int64_t chat_runtime_id, int64_t chat_database_id, int64_t sender_runtime_id,
      int64_t sender_database_id, int64_t media_runtime_id, int64_t media_database_id, int64_t timestamp,
      const std::string &text) :
      mRuntimeId(runtime_id),
      mDatabaseId(database_id),
      mChatRuntimeId(chat_runtime_id),
      mChatDatabaseId(chat_database_id),
      mSenderRuntimeId(sender_runtime_id),
      mSenderDatabaseId(sender_database_id),
      mMediaRuntimeId(media_runtime_id),
      mMediaDatabaseId(media_database_id),
      mTimestamp(timestamp),
      mText(text)
  {
  }

  ~Message() = default;

  int64_t getRuntimeId() const;

  int64_t getDatabaseId() const;

  int64_t getChatRuntimeId() const;

  int64_t getChatDatabaseId() const;

  int64_t getSenderRuntimeId() const;

  int64_t getSenderDatabaseId() const;

  int64_t getMediaRuntimeId() const;

  int64_t getMediaDatabaseId() const;

  int64_t getTimestamp() const;

  std::string getText() const;

  static constexpr int64_t RT_START_ID = 0;
  static constexpr int64_t DB_NO_ID = -1;
  static constexpr int64_t MEDIA_NO_ID = -1;

private:
  void setDatabaseId(int64_t database_id);
  void setChatDatabaseId(int64_t chat_id);
  void setSenderDatabaseId(int64_t sender_id);
  void setMediaDatabaseId(int64_t media_id);

  int64_t mRuntimeId;
  int64_t mDatabaseId;
  int64_t mChatRuntimeId;
  int64_t mChatDatabaseId;
  int64_t mSenderRuntimeId;
  int64_t mSenderDatabaseId;
  int64_t mMediaRuntimeId;
  int64_t mMediaDatabaseId;
  int64_t mTimestamp;
  std::string mText;         // text or system messages
};

#endif /* MESSAGE_H_ */
