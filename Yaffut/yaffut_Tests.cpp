#include "yaffut.h"
#include <cassert>

using namespace yaffut;

namespace yaffut_tests
{
TEST(Check_true)
{
	CHECK(true); //does not throw
}

TEST(Check_CHECK_FailThrowsFailureWithContainedCheckExpression)
{
	try
	{
		CHECK(1 == 0);
	}
	catch (const yaffut::failure& f)
	{
		std::string msg = f.what();
		
		if (msg.find("1 == 0")<= 0)
		{
			throw;
		}
	}
}

struct MyType
{
	bool operator==(const MyType& other) const	{ return true;	}
	bool operator!=(const MyType& other) const	{ return false;	}
};

std::ostream& operator<<(std::ostream& out, const MyType&)
{
   return out << "xxx";
}

struct TWithoutStream
{
	bool operator==(const TWithoutStream& other) const	{ return true;	}
	bool operator!=(const TWithoutStream& other) const	{ return false;	}
};

TEST(Check_YaffutEQUALWithNoCoutStreamingSupportStillCompilesAndExecutes)
{
	CHECK((internal::has_cout_support<std::ostream, MyType>::value));

	CHECK((internal::has_cout_support<std::ostream, TWithoutStream>::value)==false);

	TWithoutStream t1;
	TWithoutStream t2;

	EQUAL(t1,t2);  //should compile, should not fail

	MyType m1; //type supports operator <<
	MyType m2;

	EQUAL(m1, m2);//should compile, should not fail
}

TEST(Check_YaffutEQUALReportsActualValueIfSupported)
{
	try
	{
		EQUAL(1, 0);
	}
	catch (const yaffut::failure& f)
	{
		std::string msg = f.what();
		int result = msg.find("expected: (int) 1 != actual: (int) 0");
		if (result <= 0)
		{
			throw;
		}
	}
}

TEST(Check_EqualEpsilon)
{
	double d = 0;
	d += 0.1;
	d+= 0.1;
	d+= 0.1; 

	if (d == 0.3) //isn't equal because of rounding
	{
		FAIL("roundingerror");
	}
	EQUAL(d,0.3); //equal distinguishes rounding errors.

}

TEST(Check_UNEQUAL)
{
	UNEQUAL(1, 0);
}

TEST(Check_FAIL)
{
	try
	{
		FAIL("fails here");
	}
	catch(const failure& f)
	{
		if (std::string(f.what()).find("fails_here") > 0)
		{
			return; //all fine
		}
		throw; //missing error text
	}
	throw failure("FAIL does not raise exception", YAFFUT_AT);
}

void raise_exception()
{
	throw std::logic_error("throws here");
}

TEST(Check_ASSERT_THROW)
{
	ASSERT_THROW(raise_exception(), std::logic_error);
	ASSERT_THROW(raise_exception(), std::exception); 
} 

void no_throw() {}

TEST(Check_ASSERT_THROW_CatchesNoThrowCase)
{
	try
	{
		ASSERT_THROW(no_throw(), std::exception);
	}
	catch(const failure&)
	{
		return;
	}
	FAIL("ASSERT_THROW failed");
}

struct SuiteExample
{
  double d;
  SuiteExample(): d(0.123)
  {
    //setup
  }
  ~SuiteExample()
  {
    //teardown
  }
  bool Invariant() const
  {
    return true;
  }
  MyType MakeSut()
  {
	  return MyType();
  }
};
SUITE_TEST(SuiteExample, Case1)
{
	equal (0.123, d, YAFFUT_AT);
}

SUITE_TEST(SuiteExample, Case2)
{
    check(Invariant(), YAFFUT_AT);
}

SUITE_TEST(SuiteExample, Case3)
{
    unequal ("hello", "world", YAFFUT_AT);
}

SUITE_TEST(SuiteExample, Case4)
{
    MyType t = MakeSut();
	CHECK(t==t);
}

SUITE_TEST(SuiteExample, Case5) 
{
	CHECK(true);
}

class MyClass
{
public:
  MyClass(const char*){}
  void doSomethng(){}
  void doSomethingElse(){}
};

struct FixtureTestSuite
{
  float someNum;
  std::string str;
  MyClass someObject;

  FixtureTestSuite()
  : someNum(2.0f)
  , str("Hello")
  , someObject("somename") 
  {
    someObject.doSomethng();
  }
  ~FixtureTestSuite()
  {
    someObject.doSomethingElse();
  }
};
SUITE_TEST(FixtureTestSuite, Test1)
{
  EQUAL(someNum, 2.0f);
}
SUITE_TEST(FixtureTestSuite, Test3)
{
  EQUAL(str, "Hello");
}

struct SpecificSuiteCtors
{
  int i;
  std::string s;

  SpecificSuiteCtors(): i(0), s("default value")
  {
  }
  SpecificSuiteCtors(int i_): i(i_), s("default value")
  {
  }
  SpecificSuiteCtors(int i_, const std::string& s_): i(i_), s(s_)
  {
	
  }
};

SUITE_TEST(SpecificSuiteCtors, CaseA): SpecificSuiteCtors()
{
  EQUAL(0, i); 
  EQUAL("default value", s);
}

SUITE_TEST(SpecificSuiteCtors, CaseB): SpecificSuiteCtors(123)
{
	EQUAL(123,i);
}

SUITE_TEST(SpecificSuiteCtors, CaseC): SpecificSuiteCtors(123, "blah")
{
	EQUAL("blah", s);
}

} //end of namespace yaffut_tests
//@TODO: Check if 
//@TODO: YAFFUT: Write a batch for self test: all, list, version, suite, number
