/*
 * Message.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/Message.h"

int64_t Message::getRuntimeId() const
{
  return mRuntimeId;
}

int64_t Message::getDatabaseId() const
{
  return mDatabaseId;
}

int64_t Message::getChatRuntimeId() const
{
  return mChatRuntimeId;
}

int64_t Message::getChatDatabaseId() const
{
  return mChatDatabaseId;
}

int64_t Message::getSenderRuntimeId() const
{
  return mSenderRuntimeId;
}

int64_t Message::getSenderDatabaseId() const
{
  return mSenderDatabaseId;
}

int64_t Message::getMediaRuntimeId() const
{
  return mMediaRuntimeId;
}

int64_t Message::getMediaDatabaseId() const
{
  return mMediaDatabaseId;
}

void Message::setDatabaseId(int64_t database_id)
{
  mDatabaseId = database_id;
}

void Message::setChatDatabaseId(int64_t chat_id)
{
  mChatDatabaseId = chat_id;
}

void Message::setSenderDatabaseId(int64_t sender_id)
{
  mSenderDatabaseId = sender_id;
}

int64_t Message::getTimestamp() const
{
  return mTimestamp;
}

std::string Message::getText() const
{
  return mText;
}


void Message::setMediaDatabaseId(int64_t media_id)
{
  mMediaDatabaseId = media_id;
}


