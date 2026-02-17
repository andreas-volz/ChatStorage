/*
 * ChatStorageImporter.cpp
 *
 *      Author: Andreas Volz
 */

// project API include
#include "chatstorage/ChatStorageImporter.h"

// project internal
#include "importer/ImportManager.h"

// system
#include <iostream>
#include <fstream>

using namespace std;

bool ChatStorageImporter::importFromStream(std::istream& in_stream, const ImportConfig &import_config, ChatContext& out_ctx)
{
  return ImportManager::importFromStream(in_stream, import_config, out_ctx);
}

bool ChatStorageImporter::importFromFile(const std::string& filename, const ImportConfig &import_config, ChatContext& out_ctx)
{
  std::ifstream import_stream(filename);

  if (!import_stream)
  {
    std::cerr << "Error to open the file: " << filename << endl;
    return false;
  }

  return importFromStream(import_stream, import_config, out_ctx);
}
