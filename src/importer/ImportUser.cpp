/*
 * ImportUser.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "ImportUser.h"

// system
#include <string>

using namespace std;

ImportUser::ImportUser(int id) :
    mId(id)
{
}

int ImportUser::getId() const
{
  return mId;
}

void ImportUser::addNameAlias(const std::string &name)
{
  mNameAliases.insert(name);
}

bool ImportUser::hasNameAlias(const std::string &name) const
{
  auto found = mNameAliases.find(name);
  if(found != mNameAliases.end())
    return true;

  return false;
}

const std::set<std::string> &ImportUser::getNameAliasList() const
{
  return mNameAliases;
}

const std::string ImportUser::getNameAliasString() const
{
  string alias_string;
  for(auto alias_it = mNameAliases.begin(); alias_it != mNameAliases.end(); alias_it++)
  {
    alias_string += *alias_it;

    if(next(alias_it) != mNameAliases.end())
    {
      alias_string += ":";
    }
  }
  return alias_string;
}
