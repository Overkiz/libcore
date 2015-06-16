#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <kizbox/framework/core/Time.h>

class TimeTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TimeTest);
  CPPUNIT_TEST(compare);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

protected:
  void compare()
  {
    Overkiz::Time::Real t1;
    Overkiz::Time::Real t2;
    sleep(1);
    t2 = Overkiz::Time::Real::now();
    CPPUNIT_ASSERT(t1 < t2);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TimeTest);