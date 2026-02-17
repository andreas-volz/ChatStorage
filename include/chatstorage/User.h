/*
 * User.h
 *
 *      Author: Andreas Volz
 */

#ifndef USER_H_
#define USER_H_

// system
#include <cstdint>
#include <string>

class User
{
  /**
   * friend is needed as only *and only* ChatContext is allowed to  patch a new IDs after DB insert
   */
  friend class ChatContext;

public:
  User(int64_t runtime_id, int64_t database_id, bool is_system) :
      mRuntimeId(runtime_id),
      mDatabaseId(database_id),
      mIsSystem(is_system)
  {
  }
  User(int64_t runtime_id, int64_t database_id, const std::string &name, bool is_system) :
      mRuntimeId(runtime_id),
      mDatabaseId(database_id),
      mName(name),
      mIsSystem(is_system)
  {
  }

  ~User() = default;

  int64_t getRuntimeId() const;

  int64_t getDatabaseId() const;

  void setName(const std::string name);

  const std::string& getName() const;

  bool isSystem() const;

  static constexpr int64_t RT_START_ID = 0;
  static constexpr int64_t DB_NO_ID = -1;

private:
  void setDatabaseId(int64_t database_id);

  int64_t mRuntimeId;
  int64_t mDatabaseId;
  std::string mName; // TODO: this is used until the User handling is well defined to identify a person
  bool mIsSystem;
};

#endif /* USER_H_ */
