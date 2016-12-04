// For standalone compilation of this file, uncomment the line below
// #define YAFFUT_MAIN

#include "yaffut.h"

FUNC (yaffut_simple_checks)
{
  EQUAL ("Yaffut", "Yaf" "fut");
  UNEQUAL ("Yaffut", "Your Foot");
  CHECK (true);
  ASSERT_THROW (FAIL ("this should fail"), yaffut::failure);
}
