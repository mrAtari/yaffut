#include "yaffut.h"

#include <stdexcept>
#include <cassert>

struct Fail
{
  void assert_failure ()
  {
    assert (0);
  }
  void segfault ()
  {
    int *i = 0;
    *i = 0;
  }
};

SUITE_TEST(Fail, _check)
{
  CHECK(0);
}

SUITE_TEST(Fail, _equal)
{
  EQUAL("Yaffut", "bloated");
}

SUITE_TEST(Fail, _unequal)
{
  UNEQUAL("Yaffut", "Yaffut");
}

SUITE_TEST(Fail, _fail)
{
  FAIL("My failure message");
}

SUITE_TEST(Fail, _assert_throw)
{
  int a;
  ASSERT_THROW(a = 2, std::logic_error);
}
#if 0 //activate this, if you want to see a segfault -> can't recover from that
SUITE_TEST(Fail, _unexpected_segfault)
{
  segfault ();
}
#endif

#if 0
SUITE_TEST(Fail, _unexpected_assertion)
{
  assert_failure ();
}
#endif

SUITE_TEST(Fail, _unexpected_exception)
{
  throw std::logic_error("fails here");
}

TEST(Test2)
{
  float someNum = 0.0f;
  EQUAL(someNum, 2.0f);
}


