/*
 * TestHelpers.h
 *
 *      Author: Andreas Volz
 */

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include <string>
#include <cstring>

#define ASSERT_MSG(expr, msg) \
  do { \
      if (!(expr)) { \
          std::ostringstream oss; \
          oss << msg; \
          CPPUNIT_FAIL(oss.str()); \
      } \
  } while (0)

#define ASSERT_EQUAL_MSG(expected, actual, msg) \
  do { \
      if ((expected) != (actual)) { \
          std::ostringstream oss; \
          oss << msg << " | Expected: " << (expected) << ", Actual: " << (actual); \
          CPPUNIT_FAIL(oss.str()); \
      } \
  } while (0)

#endif /* TESTHELPERS_H */
