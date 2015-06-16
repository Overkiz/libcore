if TESTS

testdir = ${prefix}/tests/@PACKAGE_NAME@

test_PROGRAMS = test_lib

noinst_LTLIBRARIES = libtest.la

libtest_la_SOURCES = test_main.cpp

libtest_la_CXXFLAGS = -I$(top_srcdir)/include

libtest_la_LIBADD = $(CPPUNIT_LIBS)

test_lib_SOURCES = test_Time.cpp

test_lib_CXXFLAGS = -I$(top_srcdir)/include

test_lib_LDADD = libtest.la $(top_builddir)/src/libCore.la

endif