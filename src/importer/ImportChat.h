/*
 * ImportChat.h
 *
 *      Author: Andreas Volz
 */

#ifndef IMPORTCHAT_H_
#define IMPORTCHAT_H_

// project
#include "chatstorage/ChatSource.h"

// system
#include <string>

class ImportChat
{
public:
  ImportChat(const std::string& name, ChatSource source);
  ~ImportChat() = default;

  const std::string& getName() const;

  ChatSource getSource() const;

private:
  std::string mName;
  ChatSource mSource;
};

#endif /* IMPORTCHAT_H_ */
