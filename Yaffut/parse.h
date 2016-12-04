#include <boost/spirit/core.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <string>

class Output
{
public:
  typedef boost::function<void(const char*, const char*)> parse_t;
  int pid_;
  parse_t set_current_test;
  parse_t show_test;
  parse_t set_msg;
  parse_t show_msg;
  parse_t show_result;
  Output(): pid_(0){}
};

class Cout_output : public Output
{
  std::string current_test;
  std::string current_message;

  void set_current_test_(const char* begin, const char* end)
  {
    current_test.assign(begin,end);
  }
  void show_test_(const char*, const char*)
  {
    if(pid_)
    {
      std::cout << "pid =\n\"" << pid_ << '"' << std::endl;
      pid_ = 0;
      current_message.clear();
    }
    if(!current_message.empty())
    {
      std::cout << "message =\n\"" << current_message << '"' << std::endl;
      current_message.clear();
    }
    std::cout << "test =\n\"" << current_test << '"' << std::endl;
  }
  void set_msg_(const char* begin, const char* end)
  {
    current_message.append(begin, end);
  }
  void show_msg_(const char*, const char*)
  {
    if(!current_message.empty())
    {
      std::cout << "message =\n\"" << current_message << '"' << std::endl;
      current_message.clear();
    }
  }
  void show_result_(const char* begin, const char* end)
  {
    std::cout << "result =\n\"" << std::string(begin, end) << '"' << std::endl;
  }

public:
  Cout_output()
  {
    set_current_test = boost::bind(&Cout_output::set_current_test_, this, _1, _2);
    show_test = boost::bind(&Cout_output::show_test_, this, _1, _2);
    set_msg = boost::bind(&Cout_output::set_msg_, this, _1, _2);
    show_msg = boost::bind(&Cout_output::show_msg_, this, _1, _2);
    show_result = boost::bind(&Cout_output::show_result_, this, _1, _2);
  }
};

struct test_parser : public boost::spirit::grammar<test_parser>
{
  Output &output_;

  test_parser (Output &output)
    : output_ (output)
  {
  }

  template <typename ScannerT>
  struct definition
  {
    definition(const test_parser& self)
      : pid("pid")
      , ok("[OK]")
      , fail("[FAIL]")
      , total("[TOTAL]")
      , scope_operator("::")
      , uscore('_')
      , slash('/')
      , lpar('(')
      , rpar(')')
    {
      Output& output = self.output_;
      using namespace boost::spirit;

      test_output
        = (!(pid_number >> +eol_p) >> test_index >> 
	   test_name[output.set_current_test][output.show_test] >>
	   *blank_p >> outcome)[output.show_msg] >> !summary
        ;

      pid_number
        =  pid >> lpar >> int_p[assign(output.pid_)] >> rpar
        ;

      test_index
        =  +digit_p >> ch_p(')') >> ch_p(' ')
	;
	
      test_name
        =  !identifier >> scope_operator >> identifier
        ;

      identifier
        =  (uscore | alpha_p) >> +(uscore | alnum_p)
        ;

      outcome
        =  pass[output.show_result] >> !(test_output)
        |  failure[output.show_result] >>
	+ (test_output | (message - pid_number - summary - test_index)[output.set_msg])
        ;

      pass
        =  ok >> *blank_p >> +eol_p
        ;

      failure
        =  fail >> *blank_p >> eol_p
        ;

      message
        =  *(anychar_p - eol_p) >> eol_p
        ;

      summary
        =  total >> count >> ok >> count >> !(fail >> count)
	;

      count
        =  lpar >> int_p >> slash >> int_p >> rpar >> eol_p
        ;
    }

    boost::spirit::rule<ScannerT> test_output, pid_number, test_index, test_name, outcome, identifier, pass, failure, message, summary, count;

    boost::spirit::strlit<> pid, ok, fail, total, scope_operator;

    boost::spirit::chlit<> uscore, slash, lpar, rpar;
    boost::spirit::rule<ScannerT> const& start() const { return test_output; }
  };
};
