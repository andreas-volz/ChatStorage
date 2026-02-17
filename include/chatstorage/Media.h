/*
 * Media.h
 *
 *      Author: Andreas Volz
 */

#ifndef MEDIA_H_
#define MEDIA_H_

// project public API
#include "chatstorage/Message.h"

// system
#include <cstdint>
#include <string>
#include <optional>

// @formatter:off
enum class MediaType
{
  None,
  Image,
  Video,
  Audio
};
// @formatter:on

class Media
{
  /**
   * friend is needed as only *and only* ChatContext is allowed to  to patch new IDs after DB insert
   */
  friend class ChatContext;

public:
  Media(int64_t runtime_id, int64_t database_id, MediaType type, int64_t media_size, const std::string& mime_type) :
      mRuntimeId(runtime_id),
      mDatabaseId(database_id),
      mType(type),
      mMediaSize(media_size),
      mMimeType(mime_type)
  {
  }
  ~Media() = default;

  int64_t getRuntimeId() const;

  int64_t getDatabaseId() const;

  std::string getMediaPath() const;

  std::string getMimeType() const;

  std::string getMediaExtension() const;

  int64_t getMediaSize() const;

  MediaType getType() const;

  void setImportName(const std::string &import_name);

  std::string getImportName() const;

  static constexpr int64_t RT_START_ID = 0;
  static constexpr int64_t DB_NO_ID = -1;

private:
  void setDatabaseId(int64_t database_id);

  int64_t mRuntimeId;
  int64_t mDatabaseId;
  MediaType mType;
  std::optional<std::string> mImportName;
  int64_t mMediaSize;        // size information without accessing the file (caching)
  std::string mMimeType;
};

#endif /* MEDIA_H_ */
