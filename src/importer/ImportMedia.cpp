/*
 * ImportMedia.cpp
 *
 *      Author: Andreas Volz
 */

#include "ImportMedia.h"

int ImportMedia::id() const
{
  return mID;
}

void ImportMedia::setAttachmentInfo(const AbstractChatParser::AttachmentInfo &attachment_info)
{
  mAttachmentInfo = attachment_info;
}

AbstractChatParser::AttachmentInfo ImportMedia::getAttachmentInfo()
{
  return mAttachmentInfo;
}
