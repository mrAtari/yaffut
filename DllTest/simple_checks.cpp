#include "stdafx.h"
#include <math.h>
#include "..\Yaffut\yaffut.h"

namespace simpleTests
{
FUNC (checks)
{
  EQUAL ("Yaffut", "Yaf" "fut");
  UNEQUAL ("Yaffut", "Your Foot");
  CHECK (true);
  ASSERT_THROW (FAIL ("fails here"), yaffut::failure);
}
}