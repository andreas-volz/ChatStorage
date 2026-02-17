/*
 * ChatParserFactory.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include <importer/ChatFormatAStreamParser.h>
#include <importer/ChatParserFactory.h>

std::unique_ptr<AbstractChatParser> ChatParserFactory::create(ChatSource chat_source)
{
  // no switch-break needed as only return calls used
  // hint: a return of unique_ptr calls implicit a std::move, so this is allowed
  switch (chat_source)
  {
    case ChatSource::FormatA:
      return std::make_unique<ChatFormatAStreamParser>();
      // add here new chat source file formats
    default:
      return nullptr;
  }
  return nullptr;
}
