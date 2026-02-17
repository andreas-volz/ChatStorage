/*
 * Chat.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/Chat.h"

// system
#include <stdexcept>

int64_t Chat::getRuntimeId() const
{
  return mRuntimeId;
}

int64_t Chat::getDatabaseId() const
{
  return mDatabaseId;
}

const std::string& Chat::getName() const
{
  return mName;
}
ChatSource Chat::getSource() const
{
  return mSource;
}

void Chat::setDatabaseId(int64_t database_id)
{
  mDatabaseId = database_id;
}

/*size_t Chat::getMessageCount() const
{
  return mMessages.size();
}

const Message& Chat::getMessage(size_t index) const
{
  if (index >= mMessages.size())
  {
    throw std::out_of_range("Index out of range");
  }
  return mMessages[index];
}*/
