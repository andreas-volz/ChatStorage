/*
 * User.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/User.h"

int64_t User::getRuntimeId() const
{
  return mRuntimeId;
}

int64_t User::getDatabaseId() const
{
  return mDatabaseId;
}

void User::setDatabaseId(int64_t database_id)
{
  mDatabaseId = database_id;
}

void User::setName(const std::string name)
{
  mName = name;
}

const std::string &User::getName() const
{
  return mName;
}

bool User::isSystem() const
{
  return mIsSystem;
}
