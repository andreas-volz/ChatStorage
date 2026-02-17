/*
 * PersistenceManager.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "PersistenceManager.h"
#include "common/StringUtil.h"
#include "database/MediaRepository.h"

// system
#include <memory>

using namespace std;

void PersistenceManager::save(ChatContext &ctx, const fs::path& import_media_path)
{
  mSQLCon.begin();

  // the order is important!
  ctx.persistChat(mChatRepo);
  ctx.persistUsers(mUserRepo);
  ctx.persistMedia(mMediaRepo);
  ctx.persistMessages(mMessageRepo);

  bool commit_success = mSQLCon.commit();

  if (commit_success)
  {
    mMediaRepo.executeActions(import_media_path);
  }
  else
  {
    mSQLCon.rollback();
    cerr << "SAVE - Rollback!" << endl;
  }
}

std::unique_ptr<ChatContext> PersistenceManager::loadByChatId(int64_t chat_id)
{
  auto ctx = std::make_unique<ChatContext>();

  ChatRow chat_row = mChatRepo.getByChatId(chat_id);

// @formatter:off
  Chat chat(
      Chat::RT_START_ID,
      chat_row.chat_id,
      chat_row.name,
      static_cast<ChatSource>(chat_row.source)
  );
// @formatter:on

  // -> get Chat from DB

  ctx->setChat(make_unique<Chat>(chat));

  // -> get Users from DB

  // get a list of all user IDs which are available in this chat
  auto distinct_sender_ids = mMessageRepo.getDistinctSenderIdsByChatId(chat_id);

  vector<UserRow> user_rows = mUserRepo.getByUserIds(distinct_sender_ids);
  vector<User> users;
  int64_t user_index = 0;
  for (auto ur_it = user_rows.begin(); ur_it != user_rows.end(); ur_it++)
  {
    UserRow &user_row = *ur_it;

    User user(user_index, user_row.user_id, user_row.name, user_row.is_system);
    users.emplace_back(std::move(user));
    user_index++;
  }
  ctx->setUserList(users);

  // -> get Media from DB

  // get a list of all media IDs which are available in this chat
  auto distinct_media_ids = mMessageRepo.getDistinctMediaIdsByChatId(chat_id);
  vector<MediaRow> media_rows = mMediaRepo.getByMediaIds(distinct_media_ids);
  vector<Media> media_list;
  int64_t media_index = 0;
  for (auto mr_it = media_rows.begin(); mr_it != media_rows.end(); mr_it++)
  {
    MediaRow &media_row = *mr_it;

    Media media_obj(media_index, media_row.media_id, static_cast<MediaType>(media_row.type), media_row.media_size, media_row.mime_type);
    media_list.emplace_back(std::move(media_obj));
    media_index++;
  }
  ctx->setMediaList(media_list);

  // -> get Messages from DB

  vector<MessageRow> message_rows = mMessageRepo.getByChatId(chat_id);
  vector<Message> messages;
  int64_t message_index = 0;
  for (auto mr_it = message_rows.begin(); mr_it != message_rows.end(); mr_it++)
  {
    MessageRow &message_row = *mr_it;

    const User &user = ctx->getUserBySenderDatabaseId(message_row.sender_id);

    int64_t media_runtime_id = message_row.media_id;
    if (media_runtime_id != Media::DB_NO_ID)
    {
      const Media &media = ctx->getMediaByMediaDatabaseId(message_row.media_id);
      media_runtime_id = media.getRuntimeId();
    }

// @formatter:off
    Message message(
        message_index, message_row.message_id,
        ctx->getChat()->getRuntimeId(), chat_id,
        user.getRuntimeId(), message_row.sender_id,
        media_runtime_id, message_row.media_id,
        message_row.timestamp,
        message_row.text
    );
// @formatter:on

    messages.emplace_back(std::move(message));

    message_index++;
  }

  ctx->setMessageList(messages);

  return ctx;
}

std::unique_ptr<ChatContext> PersistenceManager::loadByMessageId(int64_t message_id)
{
  auto ctx = std::make_unique<ChatContext>();

  // TODO: implement
  //ctx.setUsers(mUserRepo.getAll());
  //ctx.emplaceMessage(mMessageRepo.getById(message_id));

  return ctx;
}

std::unique_ptr<ChatContext> PersistenceManager::loadByUserId(int64_t user_id)
{
  auto ctx = std::make_unique<ChatContext>();

  // TODO: implement
  //ctx.emplaceUser(mUserRepo.getById(user_id));

  return ctx;
}

std::vector<Chat> PersistenceManager::listChats()
{
  vector<ChatRow> chat_rows = mChatRepo.listChats();
  vector<Chat> chats;

  int64_t runtime_chat_id = Chat::RT_START_ID;
  for (auto cr_it = chat_rows.begin(); cr_it != chat_rows.end(); cr_it++)
  {
    const ChatRow &chat_row = *cr_it;

// @formatter:off
      Chat chat(
          runtime_chat_id,
          chat_row.chat_id,
          chat_row.name,
          static_cast<ChatSource>(chat_row.source)
      );
// @formatter:on
      chats.emplace_back(std::move(chat));
      runtime_chat_id++;
  }
  return chats;
}

