// Copyright 2006 Rutger E.W. van Beusekom.
// Copyright 2016 Erwin Holland
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <list>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <string.h>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace yaffut 
{
	std::string const version = "1.1";


namespace internal
{
template <typename T>
std::string demangle()
{
#ifdef __GNUC__
  size_t sz;
  int status;
  char* ptr = abi::__cxa_demangle(typeid(T).name(), 0, &sz, &status);
  std::string name(ptr ? ptr : "", ptr ? strlen(ptr) : 0);
  if(ptr){ free(ptr); }
  std::string::size_type pos = name.rfind("::");
  if(pos != std::string::npos)
  {
    name = name.substr(pos + 2);
  }  
#elif _MSC_VER
  std::string name(typeid(T).name());
  static const std::string struct_prefix("struct ");
  static const std::string class_prefix("class ");
  static const std::string ptr_postfix(" *");

  std::string::size_type
  at = name.find(struct_prefix);
  if(at != std::string::npos) { name.erase(at, struct_prefix.size()); }
  at = name.find(class_prefix);
  if(at != std::string::npos) { name.erase(at, class_prefix.size()); }
  at = name.find(ptr_postfix);
  if(at != std::string::npos) { name.erase(at, ptr_postfix.size()); }
#else
  std::string name(typeid(T).name());
#endif
  return name;
}

typedef char yes;
typedef char (&no)[2];

struct any_t { template <class T> any_t(const T &); };

no operator << (const any_t &, const any_t &);
no operator >> (const any_t &, const any_t &);

template <class T> yes check(T const&);
no check(no);

template <typename StreamType, typename T>
struct has_cout_support 
{
    static StreamType& stream;
    static T& x;
    static const bool value = sizeof(check(stream << x)) == sizeof(yes);
};

template <typename Expected, typename Actual, bool has_extra = false> 
struct extra_information
{ 
	static void print(std::ostringstream& os, const Expected& e, const Actual& a) {}
};
template <typename Expected, typename Actual>
struct extra_information<Expected, Actual, true>
{
	static void print(std::ostringstream& os, const Expected& e, const Actual& a)
	{
		os << "\nexpected: "
			<< "(" << demangle<Expected>() << ") " << e
			<< " != actual: " << "(" << demangle<Actual>() << ") " << a;
	}
};
} //end of namespace internal

	typedef void (*TestFunc) ();
	struct TestCaseInfo
	{
		TestCaseInfo(std::string n, std::string a, TestFunc f)
			:name(n), at(a), func(f),
			index(-1), passed(false) {} //will be filled lat
		
		std::string name;
		std::string at;
		TestFunc func;
		int index;
		bool passed;
	};
	
	struct IReporting
	{
		virtual int FailCount() = 0;
		virtual void TestRunStarting() = 0;
		virtual void TestCaseStarting(const TestCaseInfo& info)	 = 0;
		virtual void TestCaseEnded(const TestCaseInfo& info) = 0;
		virtual void TestRunEnded() = 0;
		virtual void LogFailure(const std::string s) = 0;
	};

	class BasicReporting : public IReporting
	{
		std::list<std::string> _Failures;
		size_t _fail, _pass;
		clock_t _start, _end;
	public:
		BasicReporting():_fail(0), _pass(0) {}
		int FailCount() { return (int)_fail; }
		void TestRunStarting()	{ _start = clock();}
		void TestCaseStarting(const TestCaseInfo& info)	
		{
			std::cout << std::endl <<info.at << info.name << std::flush;				
		}
		void TestCaseEnded(const TestCaseInfo& info)	
		{
			if (info.passed)
			{
				++_pass;
				std::cout << " [OK]";
			}
			else
			{
				++_fail;
				std::cout << " [FAIL]";
			}
			std::cout << ", #" << info.index << std::flush;
		}
		void TestRunEnded()
		{
			_end = clock();
			ReportSummary();
			ReportFailures();
		}
		void LogFailure(const std::string s)
		{
			_Failures.push_back(s);
		}
	private:
		void ReportSummary()
		{
			const size_t size = _pass + _fail;
			std::cout << std::endl;
			std::cout << "[TOTAL](" << size << '/' << size << ")" << std::endl;
			std::cout << "[OK](" << _pass << '/' << size << ")" << std::endl;
			if (_fail > 0)
				std::cout << "[FAIL](" << _fail << '/' << size << ")" << std::endl;

			double duration = ((double)(_end - _start))/CLOCKS_PER_SEC;
			std::cout << "[DURATION] " <<  duration << " seconds" << std::endl;
		}
		void ReportFailures()
		{
			if (_Failures.size() > 0)
			{
				std::cout << "[ALL FAILURES]" << std::endl;
				std::list<std::string>::iterator it;
				for (it = _Failures.begin(); it != _Failures.end(); ++it)
				{
					std::cout << *it << std::endl;
				}
			}
		}
	};
	
	class failure
	{
		std::string failure_;
	public:
		template <typename Expected, typename Actual>
		failure(const Expected& e, const Actual& a, const char* at = "", const char* expr = "")
		{
			std::ostringstream os;
			os << at << expr;
			internal::extra_information<Expected,Actual,
				internal::has_cout_support<std::ostream, Expected>::value>::print(os, e, a);
			
			failure_ = os.str();
		}
		failure(const char* at = "", const char* expr = "")
		{
			std::ostringstream os;
			os << at << expr;
			failure_ = os.str();
		}
		virtual ~failure() throw() {}
		virtual const char* what() const throw() { return failure_.c_str(); }
	};

	class Factory
	{		
	private:
		typedef std::list<TestCaseInfo> Tests_t;
		Tests_t m_Tests;
		int _registeredCount;
	private:
		Factory():_registeredCount(0) {}
		static bool EqualsSuiteName (std::string const &name, std::string const& s)
		{
			return (s.find(name) != std::string::npos);
		}
	public:
		~Factory(){}
		static Factory& Instance()
		{
			static Factory instance;
			return instance;
		}
		void Register(TestCaseInfo info)
		{
			info.index = _registeredCount++;
			m_Tests.push_back(info);
		}
		void List(const std::string& name)
		{
			size_t i = 0;
			for(Tests_t::const_iterator it = m_Tests.begin(); it != m_Tests.end(); ++it)
			{
				if(name.empty() || it->name == name
					|| EqualsSuiteName (name, it->name))
					std::cerr << i++ << ")\t";
				std::cout << it->name << std::endl;
			}
		}
		void RunTestCase(TestCaseInfo& info, IReporting& reporter)
		{
			try
			{
				reporter.TestCaseStarting(info);
				info.func(); //executes the test 
				info.passed = true;
			}
			catch(const yaffut::failure& f)
			{
				reporter.LogFailure(f.what());
			}
			catch(...)
			{
				reporter.LogFailure("unknown exception");
			}
			reporter.TestCaseEnded(info);
		}
		template <typename Predicate>
		void Run(Predicate& canRun, IReporting& reporter)
		{
			for(Tests_t::iterator it = m_Tests.begin(); it != m_Tests.end(); ++it)
			{
				TestCaseInfo info = *it;
				if (canRun(info))
				{
					RunTestCase(info, reporter);
				}
			}
		}
		void ExecuteTests(const char** args, int num, IReporting& reporter)
		{
			reporter.TestRunStarting();
			if (num == 0)
			{
				struct PredicateAll
				{
					bool operator()(const TestCaseInfo&) {return true;}
				};
                                PredicateAll pred;
				Run(pred, reporter);
			}
			else if(num > 0)
			{
				for(int i = 0; i < num; ++i)
				{
					int n;
					if ((std::istringstream(args[i])>>n).fail())
					{
						struct PredicateName
						{
							std::string name;
							PredicateName(std::string s): name(s) {}
							bool operator()(const TestCaseInfo& info) 
							{ return (info.name == name || EqualsSuiteName(name, info.name));}
						};
                                                PredicateName pred(args[i]);
						Run(pred, reporter);
					}
					else
					{
						struct PredicateIndex
						{
							int searchIndex;
							PredicateIndex(int i):searchIndex(i) {}
							bool operator()(const TestCaseInfo& info) 
							{ return (info.index == searchIndex);}
						};
                                                PredicateIndex pred(n);
						Run(pred, reporter);
					}
				}
			}

			reporter.TestRunEnded();
		}
		int Main(int argc, const char* argv[])
		{
			if(argc > 1
				&& (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help"))
			{
				std::cout << "Yaffut - Yet Another Framework For Unit Testing.\n\n"
					"Usage: yaffut [OPTION] [Suite:|Suite::Test]...\n\n"
					"Options:\n"
					"  -h, --help     show this help\n"
					"  -l, --list     list test cases\n"
					"  -v, --version  show version number\n"
					<< std::flush;
				return 0;
			}
			if(argc > 1
				&& (std::string(argv[1]) == "-l" || std::string(argv[1]) == "--list"))
			{
				List(argc > 2 ? argv[2] : "");
				return 0;
			}
			if(argc > 1
				&& (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version"))
			{
				std::cout << "Yaffut " << version << std::endl;
				return 0;
			}
			argc--;	argv++; //remove trailing argument with executable name
			BasicReporting reporter;
			ExecuteTests(argv, argc, reporter);

			return reporter.FailCount();
		}
	};

	template <typename Suite, typename Case = void>
	struct Test: public virtual Suite {};

	template <typename Expected, typename Actual>
	inline void equal(const Expected& e, const Actual& a, const char* at = "", const char* expr = "")
	{
		if(e != a)
		{
			throw failure(e, a, at, expr);
		}
	}
	inline void equal(const char* e, const char* a, const char* at = "", const char* expr = "")
	{
		if(std::string(e) != std::string(a))
		{
			throw failure(e, a, at, expr);
		}
	}
	inline void equal(double e, double a, const char* at = "", const char* expr = "")
	{
		double max = std::abs(std::max(e, a));
		max = max < 1.0 ? 1.0 : max;
		if(std::abs(e - a) > std::numeric_limits<double>::epsilon() * max)
		{
			throw failure(e, a, at, expr);
		}
	}
	inline void check(bool b, const char* at = "", const char* expr = "")
	{ 
		if(!b)
		{
			throw failure(at, expr);
		}
	}

	template <typename Expected, typename Actual>
	inline void unequal(const Expected& e, const Actual& a, const char* at = "", const char* expr = "")
	{
		if(e == a)
		{
			throw failure(e, a, at, expr);
		}
	}
	inline void unequal(const char* e, const char* a, const char* at = "", const char* expr = "")
	{
		if(std::string(e) == std::string(a))
		{
			throw failure(e, a, at, expr);
		}
	}
	inline void unequal(double e, double a, const char* at = "", const char* expr = "")
	{
		double max = std::abs(std::max(e, a));
		max = max < 1.0 ? 1.0 : max;
		if(std::abs(e - a) <= std::numeric_limits<double>::epsilon() * max)
		{
			throw failure(e, a, at, expr);
		}
	}
	template <typename T>
	void fail(const T& expr, const char* at = "")
	{
		std::ostringstream os;
		os << expr;
		throw failure(at, os.str().c_str());
	}

struct AutoReg
{
	AutoReg(std::string name, const char* at, TestFunc f)
    {
        Factory::Instance().Register(TestCaseInfo(name, std::string(at), f));    
    }
};

#define YAFFUT_STRINGIZE(x) YAFFUT_STRINGIZE_(x)
#define YAFFUT_STRINGIZE_(x) #x

#ifdef __GNUC__
#define YAFFUT_AT __FILE__ ":" YAFFUT_STRINGIZE(__LINE__)": "
#elif _MSC_VER
#define YAFFUT_AT __FILE__ "(" YAFFUT_STRINGIZE(__LINE__)") : "
#endif

#define YAFFUT_TEST(Case)\
	static void Case();\
	yaffut::AutoReg autoReg##Case(#Case, YAFFUT_AT, &Case);\
	static void Case() 
#define YAFFUT_SUITE_TEST(Suite, Case)\
	struct Case: public yaffut::Test<Suite, Case>{ Case(); };\
	static void Suite##Case() {Case();}\
	yaffut::AutoReg autoReg##Case(YAFFUT_STRINGIZE(Suite::Case), YAFFUT_AT, &Suite##Case);\
	Case::Case()
#define YAFFUT_EQUAL(e,a) \
	yaffut::equal (e ,a , YAFFUT_AT, "EQUAL(" #e " == " #a ") failed ")
#define YAFFUT_UNEQUAL(e,a) \
	yaffut::unequal (e, a, YAFFUT_AT, "UNEQUAL(" #e " != " #a ") failed ")
#define YAFFUT_CHECK(e) \
	yaffut::check (e, YAFFUT_AT, "CHECK(" #e ") failed ")
#define YAFFUT_FAIL(s) yaffut::fail (s, YAFFUT_AT);
#define YAFFUT_ASSERT_THROW(s, e) \
	try { \
	s; \
	throw yaffut::failure (YAFFUT_AT,  #s " failed to throw"); } \
	catch(const e&){}
}

//Published Macros
#define SUITE_TEST YAFFUT_SUITE_TEST
#define TEST YAFFUT_TEST
#define FUNC YAFFUT_TEST
#define EQUAL YAFFUT_EQUAL
#define UNEQUAL YAFFUT_UNEQUAL
#define CHECK YAFFUT_CHECK
#define FAIL YAFFUT_FAIL
#define ASSERT_THROW YAFFUT_ASSERT_THROW

#ifdef YAFFUT_MAIN
#include <iostream>
int main(int argc, const char* argv[])
{
	return yaffut::Factory::Instance().Main(argc, argv);
};
#endif /* YAFFUT_MAIN */

#ifdef _MSC_VER
// Add DLLPlugIn Support. Insert this macro to the dllmain.cpp of your Dll.
#define YAFFUT_TEST_PLUGIN() \
	extern "C" __declspec(dllexport) int runYaffutTests(int argc, const char* argv[])\
	{ return yaffut::Factory::Instance().Main(argc, argv); }

#define YAFFUT_TEST_NODE "runYaffutTests"
#endif