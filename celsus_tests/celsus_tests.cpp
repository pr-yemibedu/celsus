// celsus_tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <functional>
#include <vector>
#include <celsus/celsus.hpp>
#include <celsus/path_utils.hpp>
#include <celsus/Logger.hpp>
#include <celsus/CelsusExtra.hpp>
#include <celsus/string_utils.hpp>

struct TestBase
{
	TestBase(const std::string& name);
	virtual ~TestBase() {}
	virtual void run() = 0;
	std::string _name;
};

struct TestManager
{
public:
	static TestManager& instance();

	typedef std::function<void()> TestFn;
	void add_test(TestBase *test);
	void run_tests();
	void report_fail(const char *test_name, const char *expr, const char *file, const int line);
private:
	TestManager();

	typedef std::map<std::string, std::vector<TestBase*> > Tests;
	std::set<std::string> _failed_tests;
	Tests _tests;

	static TestManager *_instance;
};

TestBase::TestBase(const std::string& name)
	: _name(name)
{
	TestManager::instance().add_test(this);
}

void TestManager::add_test(TestBase *test)
{
	_tests[test->_name].push_back(test);
}

void TestManager::run_tests()
{
	OutputDebugStringA(string2::fmt("Running %d tests\n", _tests.size()));

	for (auto i = _tests.begin(), e = _tests.end(); i != e; ++i) {
		const std::string& name = i->first;
		for (auto j = i->second.begin(); j != i->second.end(); ++j) {
			(*j)->run();
			if (_failed_tests.find(name) != _failed_tests.end()) {
				// do we want to break if any test in the bunch fails? probably not..
			}
		}
	}
}

TestManager *TestManager::_instance = NULL;

TestManager& TestManager::instance()
{
	if (!_instance)
		_instance = new TestManager();
	return *_instance;
}

TestManager::TestManager()
{
}


void TestManager::report_fail(const char *test_name, const char *expr, const char *file, const int line)
{
	_failed_tests.insert(test_name);
	OutputDebugStringA(to_string("%s(%d): FAILED TEST: %s\n", file, line, expr).c_str());
}

#define CHECK_TRUE(x) if (!(x)) TestManager::instance().report_fail(_name.c_str(), #x, __FILE__, __LINE__); else {}

#define TEST(x) \
struct Test ## x : public TestBase { Test ## x() : TestBase("Test" # x) {} virtual void run(); }; \
Test ## x GEN_NAME(ANON, __LINE__); \
void Test ## x::run()


TEST(replace_ext)
{
	// tests for extension replacement
	CHECK_TRUE("tjong.log" == Path::replace_extension("tjong.txt", "log"));
	CHECK_TRUE("tjong.log" == Path::replace_extension("tjong", "log"));
	CHECK_TRUE("tjong.log" == Path::replace_extension("tjong.", "log"));
	CHECK_TRUE("" == Path::replace_extension("", ""));
	CHECK_TRUE("" == Path::replace_extension("", "log"));
}

TEST(path_utils)
{
	string2 filename = "filename";
	string2 ext = "ext";

	string2 path_name1("c:/test1/test2/filename.ext");
	string2 path_name2("c:/test1/test2/filename.");
	string2 path_name3("c:/test1/test2/");

	Path p1(path_name1);
	Path p2(path_name2);
	Path p3(path_name3);

	CHECK_TRUE(ext == p1.get_ext());
	CHECK_TRUE(filename == p1.get_filename_without_ext());
	CHECK_TRUE(filename + "." + ext == p1.get_filename());

	CHECK_TRUE(string2::empty_string == p2.get_ext());
	CHECK_TRUE(filename + "." == p3.get_filename());
	CHECK_TRUE(filename == p3.get_filename_without_ext());

	CHECK_TRUE(string2::empty_string == p3.get_filename());
	CHECK_TRUE(string2::empty_string == p3.get_ext());
	CHECK_TRUE(string2::empty_string == p3.get_filename_without_ext());

}

TEST(string)
{
	string2 s0;
	CHECK_TRUE(s0.empty());
	CHECK_TRUE(s0.size() == 0);

	string2 s1("magnus");
	CHECK_TRUE(!s1.empty());
	CHECK_TRUE(s1.size() == 6);

	string2 s2;
	s2 = "magnus";
	CHECK_TRUE(s1 == s2);

	CHECK_TRUE(string2("apa") != string2("bapa"));
	CHECK_TRUE(string2("apa") == string2("apa"));
	CHECK_TRUE(string2("apa") != "bapa");
	CHECK_TRUE(string2("apa") == "apa");

	std::set<string2> s;
	s.insert(s1);
	CHECK_TRUE(s.find("magnus") != s.end());
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestManager::instance().run_tests();
	return 0;
}

