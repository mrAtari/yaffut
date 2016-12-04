
# yaffut
YAFFUT -- YET ANOTHER FRAMEWORK FOR UNIT TESTING

Yaffut is a framework for C++ unit testing.  Unit testing
prevents you from shooting yourself in yaffut ;-)

The only file you need is yaffut.h

FEATURES

-automatic test class registration.

-add test by adding single source file (*.cpp), no editing any other
 files, everything is contained within a single test class derivative.

-no macro support required aside from __FILE__ and __LINE__ which are
 wrapped into a macro called __AT__ which produces a string literal.

-support for simple test cases, where a test function is sufficient

-support for test fixtures which group cases, and allow to encapsulate
 the commonality between the cases.

-fixture specialisation by parametrisation.

RUNNING

Include yaffut.h
Add a call to yaffut Factory in your main: yaffut::Factory::Instance().Main(argc, argv);

EXAMPLES

TEST(yaffut_simple_checks)
{
  EQUAL ("Yaffut", "Yaf" "fut");
  UNEQUAL ("Yaffut", "Your Foot");
  CHECK (true);
  ASSERT_THROW (FAIL ("this should fail"), yaffut::failure);
}

See yaffut_Tests.cpp for complete feature self test.

PREREQUISITES

To compile, yaffut requires a standards conforming compiler and the
standard library.  Yaffut is platform independent and works on windows and unix/linux platforms.
