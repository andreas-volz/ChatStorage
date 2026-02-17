/*
 * ImportManager.cpp
 *
 *      Author: Andreas Volz
 */

// project public API
#include "chatstorage/Message.h"
#include "chatstorage/User.h"
#include "chatstorage/Chat.h"
#include "chatstorage/Media.h"

// project private
#include "ImportManager.h"
#include "common/Logger.h"
#include "importer/ChatParserFactory.h"

// system
#include <memory>
#include <iostream>
#include <vector>

using namespace std;

static Logger logger = Logger("ChatStorage.ImportManager");

bool ImportManager::importFromStream(std::istream &in_stream, const ImportConfig &import_config, ChatContext &out_ctx)
{
  unique_ptr<AbstractChatParser> chat_parser(ChatParserFactory::create(import_config.chatSource));
  ChatImportContext ci_ctx;

  bool parse_result = chat_parser->parse(in_stream, import_config.chatName, ci_ctx);
  if (!parse_result)
  {
    LOG4CXX_ERROR(logger, "Import Parser Error!");
    return false;
  }

  for (auto user_it = ci_ctx.users.begin(); user_it != ci_ctx.users.end(); user_it++)
  {
    ImportUser &import_user = *user_it;

    string user_name = import_user.getNameAliasString();

// @formatter:off

    // Use a Lambda to find if the user_name is in the database_id mapping
    auto it = std::find_if(
        import_config.userImportMapping.begin(),
        import_config.userImportMapping.end(),
        [&](const auto& pair){ return pair.first == user_name; }
    );
    if (it != import_config.userImportMapping.end())
    {
        int mapping_id = it->second;
        out_ctx.addRuntimeToDatabaseUserMapping(import_user.getId(), mapping_id);
    }

    User user(
        import_user.getId(),
        User::DB_NO_ID,
        import_user.getNameAliasString(),
        import_user.getId() == ImportUser::SYSTEM_USER_ID ? true : false
    );
// @formatter:on
    out_ctx.addUser(user);
  }

  for (auto media_it = ci_ctx.media.begin(); media_it != ci_ctx.media.end(); media_it++)
  {
    ImportMedia &import_media_obj = *media_it;

    auto attachment_info = import_media_obj.getAttachmentInfo();
    Media media_obj(import_media_obj.id(), Media::DB_NO_ID, static_cast<MediaType>(attachment_info.type), attachment_info.size,
        attachment_info.mime_type);
    media_obj.setImportName(attachment_info.filename);
    out_ctx.addMedia(media_obj);
  }

  for (auto message_it = ci_ctx.messages.begin(); message_it != ci_ctx.messages.end(); message_it++)
  {
    ImportMessage &import_message = *message_it;
    std::chrono::system_clock::time_point tp = import_message.getTimePoint();
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

// @formatter:off
    Message message(
        import_message.getId(), Message::DB_NO_ID,
        Chat::RT_START_ID, Chat::DB_NO_ID,
        import_message.getSenderId(), User::DB_NO_ID,
        import_message.getMediaId(), Media::DB_NO_ID,
        timestamp,
        import_message.getText()
        );
// @formatter:on

    out_ctx.addMessage(message);
  }

  out_ctx.setChat(make_unique<Chat>(Chat::RT_START_ID, Chat::DB_NO_ID, ci_ctx.chat->getName(), ci_ctx.chat->getSource()));

  return true;
}
