/*
 * MediaRepository.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "MediaRepository.h"
#include "common/StringUtil.h"

int64_t MediaRepository::insert(const MediaRow &media_row)
{
  mInsertStmt.bind(":account_id", media_row.account_id);
  mInsertStmt.bind(":type",       media_row.type);
  mInsertStmt.bind(":media_size", media_row.media_size);
  mInsertStmt.bind(":mime_type",  media_row.mime_type);

  mInsertStmt.step();
  mInsertStmt.reset();

  return mSQLCon.lastInsertRowID();
}

fs::path MediaRepository::getMediaPersistencePath()
{
  return mMediaPersistencePath;
}

MediaRow MediaRepository::getByMediaId(int64_t media_id)
{
  mSelectByIdStmt.reset();
  mSelectByIdStmt.bind(":media_id", media_id);

  if (mSelectByIdStmt.step() == SQLiteConnection::Result::Row)
  {
    MediaRow media_row {};

    media_row.media_id  = media_id;
    media_row.account_id  = mSelectByIdStmt.getInt64(0);
    media_row.type        = mSelectByIdStmt.getInt64(1);
    media_row.media_id  = mSelectByIdStmt.getInt64(2);
    media_row.media_size  = mSelectByIdStmt.getInt64(3);
    media_row.mime_type   = mSelectByIdStmt.getText(4);

    return media_row;
  }

  throw std::runtime_error("Message not found"); // TODO: custom exception
}

std::vector<MediaRow> MediaRepository::getByMediaIds(std::vector<int64_t> media_ids)
{
  if (media_ids.empty())
  {
    // return empty vector to prevent sql execution with empty list
    return {};
  }

  std::vector<MediaRow> media_rows;

// @formatter:off
  std::string media_sql =
      "SELECT media_id, account_id, type, media_size, mime_type "
      "FROM media "
      "WHERE media_id IN (" + SQLiteConnection::makePlaceholders(media_ids.size()) + ")";
// @formatter:on
  Statement media_stmt(mSQLCon, media_sql);

  media_stmt.reset();
  media_stmt.bindInt64Container(1, media_ids);

  while (media_stmt.step() == SQLiteConnection::Result::Row)
  {
    MediaRow media_row {};

    media_row.media_id   = media_stmt.getInt64(0);
    media_row.account_id = media_stmt.getInt64(1);
    media_row.type       = media_stmt.getInt64(2);
    media_row.media_size = media_stmt.getInt64(3);
    media_row.mime_type  = media_stmt.getText(4);

    media_rows.push_back(std::move(media_row));
  }

  return media_rows;
}

void MediaRepository::enqueueAction(const MediaRepository::MediaAction& action)
{
  mActions.push_back(action);
}

void MediaRepository::executeActions(fs::path mMediaImportPath)
{
  for (const auto &a : mActions)
  {
    fs::path abs_src(mMediaImportPath / a.src);
    fs::path abs_dst(mMediaPersistencePath / a.dst);

    try
    {
      switch (a.type)
      {
        case MediaAction::Type::Copy:
          // TODO: create media folder recursive
          std::filesystem::copy_file(abs_src, abs_dst, std::filesystem::copy_options::overwrite_existing);
          break;
        case MediaAction::Type::Move:
          std::filesystem::rename(abs_src, abs_dst);
          break;
        case MediaAction::Type::Delete:
          // TODO: document that only the src is removed
          std::filesystem::remove(abs_src);
          break;
      }
    }
    catch(std::filesystem::__cxx11::filesystem_error &fs_ex)
    {
      std::cerr << "throw a std::filesystem::__cxx11::filesystem_error" << std::endl;
    }
  }
  mActions.clear();
}

void MediaRepository::clearActions()
{
  mActions.clear();
}

bool MediaRepository::createTable(SQLiteConnection &sql_con)
{
  std::string messages_table_sql =
      "CREATE TABLE IF NOT EXISTS media ("
      "media_id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "account_id INTEGER NOT NULL, "
      "type INTEGER, "
      "media_size INTEGER NOT NULL,"
      "mime_type TEXT"
      ");";

  return sql_con.exec(messages_table_sql);
}
