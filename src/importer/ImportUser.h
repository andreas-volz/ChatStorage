/*
 * ImportUser.h
 *
 *      Author: Andreas Volz
 */

#ifndef IMPORTUSER_H_
#define IMPORTUSER_H_

// system
#include <string>
#include <set>

class ImportUser
{
public:
  ImportUser(int id);
  virtual ~ImportUser() = default;

  int getId() const;

  void addNameAlias(const std::string &name);
  bool hasNameAlias(const std::string &name) const;
  const std::set<std::string> &getNameAliasList() const;
  const std::string getNameAliasString() const;

  static constexpr int SYSTEM_USER_ID = 0;

private:
  int mId;
  std::set<std::string> mNameAliases;
};

#endif /* IMPORTUSER_H_ */
