/*
 * ChatContext.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/ChatContext.h"

// project private API
#include "common/Logger.h"
#include "database/ChatRepository.h"
#include "database/UserRepository.h"
#include "database/MessageRepository.h"
#include "database/MediaRepository.h"

// system
#include <iostream>

static Logger logger = Logger("ChatStorage.ChatContext");

using namespace std;

void ChatContext::setChat(std::unique_ptr<Chat> chat)
{
  this->mChat = std::move(chat);
}

Chat* ChatContext::getChat()
{
  return mChat.get();
}

const Chat* ChatContext::getChat() const
{
  return mChat.get();
}

void ChatContext::addUser(User user)
{
  int64_t user_runtime_id = user.getRuntimeId();
  int64_t user_database_id = user.getDatabaseId();
  size_t user_index = mUserList.size();
  mUserList.emplace_back(std::move(user)); // not access 'user' below this point
  mUserIndexByRuntimeId.emplace(user_runtime_id, user_index);
  mUserIndexByDatabaseId.emplace(user_database_id, user_index);
}

void ChatContext::setUserList(std::vector<User> users)
{
  mUserList = std::move(users); // not access 'users' below this point -> use mUsers
  mUserIndexByRuntimeId.clear();
  mUserIndexByDatabaseId.clear();
  size_t user_index = 0;
  for (auto user_it = mUserList.begin(); user_it != mUserList.end(); user_it++)
  {
    const User &user = *user_it;
    int64_t user_runtime_id = user.getRuntimeId();
    int64_t user_database_id = user.getDatabaseId();
    mUserIndexByRuntimeId.emplace(user_runtime_id, user_index);
    mUserIndexByDatabaseId.emplace(user_database_id, user_index);
    user_index++;
  }
}

const std::vector<User>& ChatContext::getUserList() const
{
  return mUserList;
}

std::vector<User>& ChatContext::getUserList()
{
  return mUserList;
}

void ChatContext::addMedia(Media media_obj)
{
  int64_t media_runtime_id = media_obj.getRuntimeId();
  int64_t media_database_id = media_obj.getDatabaseId();
  size_t media_index = mMediaList.size();
  mMediaList.emplace_back(std::move(media_obj)); // not access 'media_obj' below this point
  mMediaIndexByRuntimeId.emplace(media_runtime_id, media_index);
  mMediaIndexByDatabaseId.emplace(media_database_id, media_index);
}

const User& ChatContext::getUserBySenderRuntimeId(int64_t sender_runtime_id) const
{
  return mUserList.at(mUserIndexByRuntimeId.at(sender_runtime_id));
}

const User& ChatContext::getUserBySenderDatabaseId(int64_t sender_database_id) const
{
  return mUserList.at(mUserIndexByDatabaseId.at(sender_database_id));
}

const Media& ChatContext::getMediaByMediaRuntimeId(int64_t media_runtime_id) const
{
  return mMediaList.at(mMediaIndexByRuntimeId.at(media_runtime_id));
}

const Media& ChatContext::getMediaByMediaDatabaseId(int64_t media_database_id) const
{
  return mMediaList.at(mMediaIndexByDatabaseId.at(media_database_id));
}

void ChatContext::persistChat(ChatRepository &chat_repo)
{
  if (mChat->getDatabaseId() == Chat::DB_NO_ID)
  {
    ChatRow chat_row {};

    // chat_row.account_id: ignore for now
    chat_row.name = mChat->getName();
    chat_row.source = static_cast<int64_t>(mChat->getSource());

    int64_t new_id = chat_repo.insert(chat_row);
    mChat->setDatabaseId(new_id);
  }
  else
  {
    // TODO: implement UPDATE
  }
}

void ChatContext::persistUsers(UserRepository &user_repo)
{
  for (auto &user : mUserList)
  {
    // never persist any runtime system users as they're yet in the database
    if (!user.isSystem())
    {
      // first check if yet in DB existing User should be used
      auto it = std::find_if(
          mRuntimeToDatabaseUserMapping.begin(),
          mRuntimeToDatabaseUserMapping.end(),
          // Use a Lambda to find if the user_id is in the database_id mapping
          [&](const auto& pair){ return pair.first == user.getRuntimeId(); }
      );
      if (it != mRuntimeToDatabaseUserMapping.end())
      {
          int64_t maping_user_id = it->second;
          //cout << "found for user: " << user.getName() << endl;

          UserRow loaded_user_row = user_repo.getByUserId(maping_user_id);
          // TODO: copy here all other settings from the database back into the runtime User
          user.setName(loaded_user_row.name);
          user.setDatabaseId(loaded_user_row.user_id);
      }

      // if not import or update a new user
      if (user.getDatabaseId() == User::DB_NO_ID)
      {
        UserRow user_row {};

        // user_row.account_id: ignore for now
        user_row.name = user.getName();

        int64_t new_id = user_repo.insert(user_row);
        user.setDatabaseId(new_id);
      }
      else
      {
        // TODO: implement UPDATE
      }
    }

    else // User::isSystem()
    {
      // for system users patch the real database user ID back
      user.setDatabaseId(user_repo.getSystemUserId());
    }
  }
}

void ChatContext::persistMedia(MediaRepository &media_repo)
{
  for (auto &media_obj : mMediaList)
  {
    if (media_obj.getDatabaseId() == Media::DB_NO_ID)
    {
      // write the Media object to DB
      MediaRow media_row;

      // user_row.account_id: ignore for now
      media_row.media_size = media_obj.getMediaSize();
      media_row.mime_type = media_obj.getMimeType();
      media_row.type = static_cast<int64_t>(media_obj.getType());
      int64_t new_id = media_repo.insert(media_row);
      // after inserting update the Message object with the new database id
      media_obj.setDatabaseId(new_id);

      MediaRepository::MediaAction media_action {};
      media_action.type = MediaRepository::MediaAction::Type::Copy;
      // use the old filename as long as it's available
      media_action.src = media_obj.getImportName();
      media_action.dst = to_string(media_obj.getDatabaseId()) + "." + media_obj.getMediaExtension();

      media_repo.enqueueAction(media_action);
    }
    else
    {
      // TODO: implement UPDATE
    }
  }
}

void ChatContext::persistMessages(MessageRepository &message_repo)
{
  for (auto &message : mMessageList)
  {
    if (message.getDatabaseId() == Message::DB_NO_ID)
    {
      // pre-step: update the Message object with database values from Chat and User DB IDs
      message.setChatDatabaseId(mChat->getDatabaseId());
      // get the user which has send the message from the runtime_id map
      const User &user = getUserBySenderRuntimeId(message.getSenderRuntimeId());
      message.setSenderDatabaseId(user.getDatabaseId());

      if (message.getMediaRuntimeId() > Media::DB_NO_ID)
      {
        const Media &media = getMediaByMediaRuntimeId(message.getMediaRuntimeId());
        message.setMediaDatabaseId(media.getDatabaseId());
      }

      // now write the Message object to DB
      MessageRow message_row {};

      // message_row.account_id: ignore for now
      message_row.chat_id = mChat->getDatabaseId();
      message_row.sender_id = message.getSenderDatabaseId();
      message_row.timestamp = message.getTimestamp();
      message_row.text = message.getText();
      message_row.media_id = message.getMediaDatabaseId();
      int64_t new_id = message_repo.insert(message_row);

      // after inserting update the Message object with the new database id
      message.setDatabaseId(new_id);
    }
    else
    {
      // TODO: implement UPDATE
    }
  }
}

void ChatContext::addMessage(Message message)
{
  mMessageList.emplace_back(std::move(message));
}

void ChatContext::setMessageList(std::vector<Message> messages)
{
  mMessageList = std::move(messages);
}

const std::vector<Message>& ChatContext::getMessageList() const
{
  return mMessageList;
}

std::vector<Message>& ChatContext::getMessageList()
{
  return mMessageList;
}

/**
 *  Sets the media list. If a temporary vector is passed, it will be moved into mMediaList.
 *  After a move, the input vector is valid but its content is unspecified.
 */
void ChatContext::setMediaList(std::vector<Media> media_list)
{
  mMediaList = std::move(media_list); // not access 'media_list' below this point -> use mMediaList
  mMediaIndexByRuntimeId.clear();
  mMediaIndexByDatabaseId.clear();
  size_t media_index = 0;
  for (auto media_it = mMediaList.begin(); media_it != mMediaList.end(); media_it++)
  {
    const Media &media_obj = *media_it;
    int64_t media_runtime_id = media_obj.getRuntimeId();
    int64_t media_database_id = media_obj.getDatabaseId();
    mMediaIndexByRuntimeId.emplace(media_runtime_id, media_index);
    mMediaIndexByDatabaseId.emplace(media_database_id, media_index);
    media_index++;
  }
}

std::vector<Media> ChatContext::getMediaList() const
{
  return mMediaList;
}

void ChatContext::addRuntimeToDatabaseUserMapping(int64_t runtime_id, int64_t database_id)
{
  mRuntimeToDatabaseUserMapping.emplace_back(runtime_id, database_id);
}

