/*
 * Statement.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include "Statement.h"
#include "common/Logger.h"

// system
#include <sqlite3.h>
#include <iostream>

using namespace std;

/**
 * TODO: rework return vs. Exception concept of this class
 */
static Logger logger("ChatStorage.Statement");

Statement::Statement(SQLiteConnection &con) :
    mCon(con)
{
}

Statement::Statement(SQLiteConnection &con, const std::string &sql) :
    mCon(con)
{
  prepare(sql);
}

Statement::~Statement()
{
  finalize();
}

bool Statement::prepare(const std::string &sql)
{
  int rc = sqlite3_prepare_v2(mCon.mDB, sql.c_str(), -1, &mStmt, nullptr);
  if (rc != SQLITE_OK)
  {
    std::cerr << "Error (" << rc << ") - Cannot prepare Statement: " << sql << endl;
    return false;
  }
  return true;
}

bool Statement::finalize()
{
  if (!mFinalized)
  {
    int rc = sqlite3_finalize(mStmt);
    if (rc != SQLITE_OK)
    {
      LOG4CXX_ERROR(logger,"Error (" + to_string(rc) + ") - Cannot finalize Statement");
      return false;
    }
    mFinalized = true;
  }
  return true;
}

bool Statement::reset()
{
  int rc = sqlite3_reset(mStmt);
  if (rc != SQLITE_OK)
  {
    LOG4CXX_ERROR(logger,"Error (" + to_string(rc) + ") - Cannot reset Statement");
    return false;
  }
  return true;
}

bool Statement::clear_bindings()
{
  int rc = sqlite3_clear_bindings(mStmt);
  if (rc != SQLITE_OK)
  {
    LOG4CXX_ERROR(logger,"Error (" + to_string(rc) + ") - Cannot clear bindings from Statement");
    return false;
  }
  return true;
}

SQLiteConnection::Result Statement::step()
{
  // only returns allowed in those cases, so no break needed!
  switch (sqlite3_step(mStmt))
  {
    case SQLITE_BUSY:
      return SQLiteConnection::Result::Busy;
    case SQLITE_DONE:
      return SQLiteConnection::Result::Done;
    case SQLITE_ROW:
      return SQLiteConnection::Result::Row;
    case SQLITE_ERROR:
      return SQLiteConnection::Result::Error;
    case SQLITE_MISUSE:
      return SQLiteConnection::Result::Misuse;
    default:
      return SQLiteConnection::Result::Error;
  }

  // this function should never reach this point, only for the warnings
  return SQLiteConnection::Result::Error;
}

int Statement::parameterIndex(const std::string &parameter)
{
  return sqlite3_bind_parameter_index(mStmt, parameter.c_str());
}

bool Statement::bind(int parameter_index, const std::string &text)
{
  if (parameter_index != 0)
  {
    int rc = sqlite3_bind_text(mStmt, parameter_index, text.c_str(), -1 /* calculate string length */,
        SQLITE_TRANSIENT /*secure instant copy */);
    if (rc != SQLITE_OK)
    {
      LOG4CXX_ERROR(logger, "SQL Error (" + to_string(rc) +  ") - Cannot bind parameter '" + to_string(parameter_index) + "' to text '" + text + "'" );
      return false;
    }
    return true; // this is the good path
  }
  else
  {
    LOG4CXX_ERROR(logger," SQL Error: Unknown parameter Index: " + text);
  }
  return false;
}

bool Statement::bind(const std::string &parameter, const std::string &text)
{
  int parameter_index = parameterIndex(parameter);
  return bind(parameter_index, text);
}

bool Statement::bind(int parameter_index, int64_t number)
{
  if (parameter_index != 0)
  {
    int rc = sqlite3_bind_int64(mStmt, parameter_index, static_cast<sqlite3_int64>(number));
    if (rc != SQLITE_OK)
    {
      LOG4CXX_ERROR(logger,"SQL Error (" + to_string(rc) +  ") - Cannot bind parameter '" + to_string(parameter_index) + "' to number '" + to_string(number) + "'" );
      return false;
    }
    return true; // this is the good path
  }
  else
  {
    LOG4CXX_ERROR(logger,"SQL Error: Unknown parameter Index: " + to_string(number));
  }
  return false;
}

bool Statement::bind(const std::string &parameter, int64_t number)
{
  int parameter_index = parameterIndex(parameter);
  LOG4CXX_TRACE(logger,"Bind '" + parameter + "' to: " + to_string(number));
  return bind(parameter_index, number);
}

bool Statement::getColumn(int col, std::string &out_text)
{
  // TODO: wrap sqlite3_column_type
  if (sqlite3_column_type(mStmt, col) == SQLITE_NULL)
  {
    out_text.clear();
    return false;
  }

  const unsigned char *column_txt = sqlite3_column_text(mStmt, col);
  if (!column_txt)
  {
    out_text.clear();
    return false;
  }

  out_text = reinterpret_cast<const char*>(column_txt);
  return true;
}

bool Statement::getColumn(int col, int64_t &out_number)
{
  // TODO: wrap sqlite3_column_type
  if (sqlite3_column_type(mStmt, col) == SQLITE_NULL)
  {
    out_number = 0;
    return false;
  }

  sqlite3_int64 column_number = sqlite3_column_int64(mStmt, col);
  out_number = column_number;

  return true;
}

int64_t Statement::getInt64(int col)
{
  int64_t val;
  if (!getColumn(col, val))
  {
    throw std::runtime_error("Failed to get int64 column: " + to_string(col)); // TODO: custom exception
  }
  return val;
}

std::string Statement::getText(int col)
{
  std::string val;
  if (!getColumn(col, val))
  {
    throw std::runtime_error("Failed to get string column: "  + to_string(col)); // TODO: custom exception
  }
  return val;
}
