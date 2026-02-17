/*
 * ImportMessage.h
 *
  *      Author: Andreas Volz
 */

// Project
#include "ImportUser.h"
#include "AbstractChatParser.h"

// System
#include <chrono>

#ifndef IMPORTMESSAGE_H_
#define IMPORTMESSAGE_H_

class ImportMessage
{
public:
  ImportMessage(int id, std::chrono::system_clock::time_point tp, int sender_id) :
    mId(id),
    mTimePoint(tp),
    mSenderId(sender_id) {}
  virtual ~ImportMessage() = default;

  int getId() const;

  void addMessageLine(const std::string &line);

  const std::string& getText() const;

  const std::chrono::system_clock::time_point& getTimePoint() const;

  int getSenderId() const;

  void setMediaId(int media_id);

  int64_t getMediaId();

private:
  int mId = 0;
  std::chrono::system_clock::time_point mTimePoint;
  std::string mMessage;
  int mSenderId = 0;
  int mMediaId = -1;
};

#endif /* IMPORTMESSAGE_H_ */
