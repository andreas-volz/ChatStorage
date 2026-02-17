/*
 * ChatParserFactory.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATPARSERFACTORY_H_
#define CHATPARSERFACTORY_H_

// project
#include "AbstractChatParser.h"
#include "chatstorage/ChatSource.h"

// system
#include <memory>

class ChatParserFactory
{
public:
  ChatParserFactory() = default;
  ~ChatParserFactory() = default;

  static std::unique_ptr<AbstractChatParser> create(ChatSource chat_source);
};

#endif /* CHATPARSERFACTORY_H_ */
