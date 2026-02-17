/*
 * ImportChat.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "ImportChat.h"

ImportChat::ImportChat(const std::string& name, ChatSource source) :
  mName(name),
  mSource(source)
{

}

const std::string& ImportChat::getName() const
{
  return mName;
}

ChatSource ImportChat::getSource() const
{
  return mSource;
}
