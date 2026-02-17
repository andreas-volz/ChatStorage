/*
 * AbstractChatParser.h
 *
  *      Author: Andreas Volz
 */
#ifndef ABSTRACTCHATPARSER_H_
#define ABSTRACTCHATPARSER_H_

// project
#include "chatstorage/ChatStorage.h"

// system
#include <istream>

// forward declarations
class ChatImportContext;


class AbstractChatParser
{
public:
  struct AttachmentInfo
  {
    std::string filename;
    std::string mime_type;
    int size = 0;
    MediaType type = MediaType::None;
  };

  AbstractChatParser() = default;
  virtual ~AbstractChatParser() = default;

  virtual bool parse(std::istream& in_stream, const std::string &chat_name, ChatImportContext& out_ctx) = 0;
};

#endif /* ABSTRACTCHATPARSER_H_ */
