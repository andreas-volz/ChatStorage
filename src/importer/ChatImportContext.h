/*
 * ChatImportContext.h
 *
 *      Author: Andreas Volz
 */

// project
#include "importer/ImportUser.h"
#include "importer/ImportMessage.h"
#include "importer/ImportChat.h"
#include "importer/ImportMedia.h"

// system
#include <deque>
#include <memory>

#ifndef CHATIMPORTCONTEXT_H_
#define CHATIMPORTCONTEXT_H_

struct ChatImportContext
{
  std::unique_ptr<ImportChat> chat;
  std::deque<ImportUser> users;
  std::deque<ImportMessage> messages;
  std::deque<ImportMedia> media;
};

#endif /* CHATIMPORTCONTEXT_H_ */
