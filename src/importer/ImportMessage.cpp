/*
 * ImportMessage.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "ImportMessage.h"

int ImportMessage::getId() const
{
  return mId;
}

void ImportMessage::addMessageLine(const std::string &line)
{
  if (!mMessage.empty())
    mMessage += '\n';

  mMessage += line;
}

const std::string& ImportMessage::getText() const
{
  return mMessage;
}

const std::chrono::system_clock::time_point& ImportMessage::getTimePoint() const
{
  return mTimePoint;
}

int ImportMessage::getSenderId() const
{
  return mSenderId;
}

void ImportMessage::setMediaId(int media_id)
{
  mMediaId = media_id;
}

int64_t ImportMessage::getMediaId()
{
  return mMediaId;
}

