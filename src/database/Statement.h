/*
 * Statement.h
 *
 *      Author: Andreas Volz
 */

#ifndef STATEMENT_H_
#define STATEMENT_H_

// project
#include "SQLiteConnection.h"

// system
#include <string>

// forward declarations
typedef struct sqlite3_stmt sqlite3_stmt;

/**
 * call order:
 * prepare
 * → bind
 * → step (loop)
 * → reset
 * → bind
 * → step
 * → finalize
 * (It doesn't hurt to make a reset always before bind...)
 */
class Statement
{
public:
  Statement(SQLiteConnection &con);

  /**
   * This constructor calls prepare() with the attached sql string
   */
  Statement(SQLiteConnection &con, const std::string &sql);

  ~Statement();

  bool prepare(const std::string &sql);

  bool finalize();

  bool reset();

  bool clear_bindings();

  SQLiteConnection::Result step();

  bool bind(int parameter_index, const std::string &text);
  bool bind(const std::string &parameter, const std::string &text);

  bool bind(int parameter_index, int64_t number);
  bool bind(const std::string &parameter, int64_t number);
  // TODO: all other bind()

  /**
   * This template is able to bind a generic C++ std:: iterator based <int64_t> container to SQL statements
   *
   * @param start_index the first '?' parameter is '1' in SQL, but if other parameters are bind before in the Statement use an higher index
   */
  template<typename It>
  bool bindInt64Range(int start_index, It begin, It end)
  {
    using Value = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_same_v<Value, int64_t>,
        "bindInt64Range requires int64_t values");

    int idx = start_index;
    for (auto it = begin; it != end; ++it, ++idx)
    {
      if (!bind(idx, *it))
        return false;
    }
    return true;
  }

  /**
   * This template is a convenience function to use the bindInt64Range template function. It calls begin() and end() if the iterator
   *
   * @param start_index the first '?' parameter is '1' in SQL, but if other parameters are bind before in the Statement use an higher index
   */
  template<typename Container>
  void bindInt64Container(int start_index, const Container &c)
  {
    bindInt64Range(start_index, std::begin(c), std::end(c));
  }

  bool getColumn(int col, std::string &out_text);
  bool getColumn(int col, int64_t &out_number);
  // TODO: all other sqlite3_column_*()

  int64_t getInt64(int col);
  std::string getText(int col);

private:
  SQLiteConnection &mCon;
  sqlite3_stmt *mStmt = nullptr;
  bool mFinalized = false;

  int parameterIndex(const std::string &parameter);
};

#endif /* STATEMENT_H_ */
