
# yaffut
YAFFUT -- YET ANOTHER FRAMEWORK FOR UNIT TESTING
Unit testing prevents you from shooting yourself in yaffut ;-)

Yaffut is a lightweight testrunner for C++ unit testing (<500 lines of code).    

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
-different reporters may be injected by client user(e.g. for different development environments or CI systems)
-macros may be adapted to different testing frameworks (e.g. google test)
-works perfectly together with hippomocks mocking framework

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
