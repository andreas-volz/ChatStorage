/*
 * Media.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/Media.h"

int64_t Media::getRuntimeId() const
{
  return mRuntimeId;
}

int64_t Media::getDatabaseId() const
{
  return mDatabaseId;
}

void Media::setDatabaseId(int64_t database_id)
{
  mDatabaseId = database_id;
}

std::string Media::getMediaExtension() const
{
  std::size_t pos = mMimeType.find('/');
  if (pos != std::string::npos || pos + 1 >= mMimeType.size())
  {
    return mMimeType.substr(pos + 1);
  }
  return "";
}

std::string Media::getMimeType() const
{
  return mMimeType;
}

int64_t Media::getMediaSize() const
{
  return mMediaSize;
}

MediaType Media::getType() const
{
  return mType;
}
void Media::setImportName(const std::string &import_name)
{
  mImportName = import_name;
}

std::string Media::getImportName() const
{
  if (mImportName)
  {
    return mImportName.value();
  }
  return "";
}
