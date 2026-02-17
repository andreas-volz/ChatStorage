/*
 * ImportMedia.h
 *
 *      Author: Andreas Volz
 */

#ifndef IMPORTMEDIA_H_
#define IMPORTMEDIA_H_

#include "AbstractChatParser.h"

class ImportMedia
{
public:
  ImportMedia(int id) :
      mID(id)
  {
  }
  ~ImportMedia() = default;

  int id() const;

  void setAttachmentInfo(const AbstractChatParser::AttachmentInfo &attachment_info);

  AbstractChatParser::AttachmentInfo getAttachmentInfo();

private:
  int mID = 0;
  AbstractChatParser::AttachmentInfo mAttachmentInfo = {};
};

#endif /* IMPORTMEDIA_H_ */
