
# yaffut
YAFFUT -- YET ANOTHER FRAMEWORK FOR UNIT TESTING
Unit testing prevents you from shooting yourself in yaffut ;-)

Yaffut is a lightweight testrunner for C++ unit testing (<500 lines of code).    

The only file you need is yaffut.h

FEATURES

- Automatic test class registration.
- Add test by adding single source file (*.cpp), no editing any other
 files, everything is contained within a single test class derivative.
- No macro support required aside from __FILE__ and __LINE__ which are
 wrapped into a macro called __AT__ which produces a string literal.
- Support for simple test cases, where a test function is sufficient
- Support for parameterizeable test fixtures which group cases, and allow to encapsulate
 the commonality between the cases.
- Different reporters may be injected by client user(e.g. for different development environments or CI systems)
- Macros may be adapted to different testing frameworks (e.g. google test). See published macros section
- Works perfectly together with hippomocks mocking framework
- On Windows systems there's a dll testrunner provided (see YaDllRunner)
- Yaffut testrunner has simple parametrization options provided: you may run tests by name, number and common name

RUNNING

Include yaffut.h
Add a call to yaffut Factory in your main: yaffut::Factory::Instance().Main(argc, argv);

EXAMPLES

```
TEST(yaffut_simple_checks)
{
  EQUAL ("Yaffut", "Yaf" "fut");
  UNEQUAL ("Yaffut", "Your Foot");
  CHECK (true);
  ASSERT_THROW (FAIL ("this should fail"), yaffut::failure);
}
```
See yaffut_Tests.cpp for complete feature self test.

PREREQUISITES

To compile, yaffut requires a standards conforming compiler (C++98 is enough), exception support and the
standard library.  Yaffut is platform independent and works on windows and unix/linux platforms.
