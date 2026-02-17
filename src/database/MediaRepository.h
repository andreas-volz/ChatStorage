/*
 * MediaRepository.h
 *
 *      Author: Andreas Volz
 */

#ifndef MEDIAREPOSITORY_H_
#define MEDIAREPOSITORY_H_

// project
#include "database/SQLiteConnection.h"
#include "database/Statement.h"
#include "database/MediaRow.h"
#include "common/platform.h"

class MediaRepository
{
public:
  MediaRepository(SQLiteConnection &sql_con, const fs::path &media_persistence_path) :
// @formatter:off
      mSQLCon(sql_con),
      mInsertStmt(mSQLCon,
          "INSERT INTO media (account_id, type, media_size, mime_type) "
          "VALUES (:account_id, :type, :media_size, :mime_type);"),
      mUpdateStmt(mSQLCon, "UPDATE..."),
      mSelectByIdStmt(mSQLCon,
          "SELECT account_id, type, media_size, mime_type "
          "FROM media "
          "WHERE media_id = :media_id"),
      mMediaPersistencePath(media_persistence_path)
// @formatter:on
  {
  }

  ~MediaRepository() = default;

  struct MediaAction
  {
    enum class Type
    {
      Copy, Delete, Move
    };
    Type type = Type::Copy;
    std::filesystem::path src;
    std::filesystem::path dst;
  };

  int64_t insert(const MediaRow &media_row);

  fs::path getMediaPersistencePath();

  MediaRow getByMediaId(int64_t media_id);

  std::vector<MediaRow> getByMediaIds(std::vector<int64_t> media_ids);

  void enqueueAction(const MediaRepository::MediaAction &action);

  void executeActions(fs::path mMediaImportPath);

  void clearActions();

  static bool createTable(SQLiteConnection &sql_con);

private:
  SQLiteConnection &mSQLCon;
  Statement mInsertStmt;
  Statement mUpdateStmt;
  Statement mSelectByIdStmt;
  fs::path mMediaPersistencePath;
  std::vector<MediaAction> mActions;
};

#endif /* MEDIAREPOSITORY_H_ */
