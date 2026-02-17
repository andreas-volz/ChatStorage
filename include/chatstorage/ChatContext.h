/*
 * ChatContext.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATCONTEXT_H_
#define CHATCONTEXT_H_

// project public API
#include "chatstorage/Chat.h"
#include "chatstorage/Message.h"
#include "chatstorage/User.h"
#include "chatstorage/Media.h"

// system
#include <memory>
#include <vector>
#include <unordered_map>

// forward declarations
class ChatRepository;
class UserRepository;
class MessageRepository;
class MediaRepository;

/**
 * ChatContext ensures a consistent runtime set of Chat, Users and Messages.
 * Therefore those objects have a valid set of runtime id variables that point at each other.
 */
class ChatContext
{
public:
  ChatContext() = default;

  // Copy constructor is disabled: ChatContext cannot be copied
  ChatContext(const ChatContext&) = delete;

  // Copy assignment is disabled: ChatContext cannot be assigned from another instance
  ChatContext& operator=(const ChatContext&) = delete;

  // Move constructor is disabled: ChatContext cannot be moved
  ChatContext(ChatContext&&) = delete;

  // Move assignment is disabled: ChatContext cannot be move-assigned
  ChatContext& operator=(ChatContext&&) = delete;

  void setChat(std::unique_ptr<Chat> chat);

  const Chat *getChat() const;

  Chat *getChat();

  void addUser(User user);

  /**
   * Replaces the current user list.
   * The input vector is taken by value and moved into the context.
   * Callers may pass a temporary or std::move(...) to avoid copying.
   */
  void setUserList(std::vector<User> users);

  const std::vector<User>& getUserList() const;

  std::vector<User>& getUserList();

  // TODO: hide some of the "private" functions

  const User& getUserBySenderRuntimeId(int64_t sender_runtime_id) const;

  const User& getUserBySenderDatabaseId(int64_t sender_database_id) const;

  const Media& getMediaByMediaRuntimeId(int64_t media_runtime_id) const;

  const Media& getMediaByMediaDatabaseId(int64_t media_database_id) const;

  void persistChat(ChatRepository& chat_repo);
  void persistUsers(UserRepository& user_repo);
  void persistMessages(MessageRepository& message_repo);
  void persistMedia(MediaRepository &media_repo);

  void addMessage(Message message);

  void addMedia(Media media_obj);

  /**
   * Replaces the current message list.
   * The input vector is taken by value and moved into the context.
   * Callers may pass a temporary or std::move(...) to avoid copying.
   */
  void setMessageList(std::vector<Message> messages);

  const std::vector<Message>& getMessageList() const;

  std::vector<Message>& getMessageList();

  /**
   * Replaces the current media list.
   * The input vector is taken by value and moved into the context.
   * Callers may pass a temporary or std::move(...) to avoid copying.
   */
  void setMediaList(std::vector<Media> media_list);

  std::vector<Media> getMediaList() const;

  void addRuntimeToDatabaseUserMapping(int64_t runtime_id, int64_t database_id);

private:
  std::unique_ptr<Chat> mChat;
  std::vector<User> mUserList;
  std::vector<Message> mMessageList;
  std::vector<Media> mMediaList;
  std::unordered_map<int64_t, size_t> mUserIndexByRuntimeId;
  std::unordered_map<int64_t, size_t> mUserIndexByDatabaseId;
  std::unordered_map<int64_t, size_t> mMediaIndexByRuntimeId;
  std::unordered_map<int64_t, size_t> mMediaIndexByDatabaseId;

  // this is a specific mapping to allow User Imports with specific existing database IDs
  std::vector<std::pair<int64_t /* User runtime_id */, int64_t /* User database_id */>> mRuntimeToDatabaseUserMapping;
};

#endif /* CHATCONTEXT_H_ */
