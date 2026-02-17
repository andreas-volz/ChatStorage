/*
 * ImportManager.h
 *
 *      Author: Andreas Volz
 */

#ifndef IMPORTMANAGER_H_
#define IMPORTMANAGER_H_

// project public API
#include "chatstorage/ChatContext.h"
#include "chatstorage/ChatStorageImporter.h"

// project private
#include "importer/ChatImportContext.h"

// system
#include <unordered_map>

// forward declarations
class Chat;



class ImportManager
{
public:
  ImportManager() = default;
  ~ImportManager() = default;

  static bool importFromStream(std::istream &in_stream, const ImportConfig &import_config, ChatContext &out_ctx);
};

#endif /* IMPORTMANAGER_H_ */
