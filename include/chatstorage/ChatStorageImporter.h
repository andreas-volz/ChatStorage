/*
 * ChatStorageImporter.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATSTORAGEIMPORTER_H_
#define CHATSTORAGEIMPORTER_H_

// project public API
#include "chatstorage/ChatContext.h"

// system
#include <istream>

struct ImportConfig
{
  std::string chatName = "<no name>";
  ChatSource chatSource = ChatSource::FormatA;
  std::vector<std::pair<std::string, int>> userImportMapping;
};

class ChatStorageImporter
{
  public:
    ChatStorageImporter() = default;
    ~ChatStorageImporter() = default;

    static bool importFromStream(std::istream& in_stream, const ImportConfig &import_config, ChatContext& out_ctx);

    static bool importFromFile(const std::string& filename, const ImportConfig &import_config, ChatContext& out_ctx);

  private:

};

#endif /* CHATSTORAGEIMPORTER_H_ */
