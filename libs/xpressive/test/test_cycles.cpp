///////////////////////////////////////////////////////////////////////////////
// test_cycles.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// defining this causes regex_impl objects to be counted, allowing us to detect
// leaks portably.
#define BOOST_XPRESSIVE_DEBUG_CYCLE_TEST

#ifdef _MSC_VER
# include <crtdbg.h>
#endif

#include <iostream>
#include <boost/xpressive/xpressive.hpp>
#include "./test_minimal.hpp"

using namespace boost::xpressive;

///////////////////////////////////////////////////////////////////////////////
// test_main
// regexes refered to by other regexes are kept alive via reference counting.
// but cycles are handled naturally. the following works as expected and doesn't leak.
int test_main( int, char*[] )
{
    {
        sregex v;
        {
            sregex a,b,c;
            a = 'a' >> !by_ref(b);
            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            // just for giggles
            c = a;
            c = epsilon >> 'a';

            b = epsilon >> by_ref(c);
            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            c = epsilon >> by_ref(a);
            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            v = a;
        }
        std::string const s("aaa");
        smatch m;
        if(!regex_match(s, m, v))
        {
            BOOST_ERROR("cycle test 1 failed");
        }
    }

    if(0 != detail::regex_impl<std::string::const_iterator>::instances)
    {
        BOOST_ERROR("leaks detected (cycle test 1)");
    }

    {
        sregex v;
        {
            sregex a,b,c;
            b = epsilon >> by_ref(c);
            a = 'a' >> !by_ref(b);
            c = epsilon >> by_ref(a);

            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            v = a;
        }
        std::string const s("aaa");
        smatch m;
        if(!regex_match(s, m, v))
        {
            BOOST_ERROR("cycle test 2 failed");
        }
    }

    if(0 != detail::regex_impl<std::string::const_iterator>::instances)
    {
        BOOST_ERROR("leaks detected (cycle test 2)");
    }

    {
        sregex v;
        {
            sregex a,b,c;

            b = epsilon >> by_ref(c);
            c = epsilon >> by_ref(a);
            a = 'a' >> !by_ref(b);

            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            v = a;
        }
        std::string const s("aaa");
        smatch m;
        if(!regex_match(s, m, v))
        {
            BOOST_ERROR("cycle test 3 failed");
        }
    }

    if(0 != detail::regex_impl<std::string::const_iterator>::instances)
    {
        BOOST_ERROR("leaks detected (cycle test 3)");
    }

    {
        sregex v;
        {
            sregex a,b,c;
            c = epsilon >> by_ref(a);
            b = epsilon >> by_ref(c);
            a = 'a' >> !by_ref(b);

            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;

            v = a;
        }
        std::string const s("aaa");
        smatch m;
        if(!regex_match(s, m, v))
        {
            BOOST_ERROR("cycle test 4 failed");
        }
    }

    if(0 != detail::regex_impl<std::string::const_iterator>::instances)
    {
        BOOST_ERROR("leaks detected (cycle test 4)");
    }

    {
        sregex v;
        {
            sregex a,b,c;
            a = 'a' >> !by_ref(b);
            b = epsilon >> by_ref(c);
            c = epsilon >> by_ref(a);

            sregex d,e;
            d = epsilon >> by_ref(e);
            e = epsilon >> "aa";

            c = d;

            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;
            //std::cout << e << std::endl;

            e = 'a' >> by_ref(c);

            //std::cout << "-new loop!\n";
            //std::cout << a << std::endl;
            //std::cout << b << std::endl;
            //std::cout << c << std::endl;
            //std::cout << e << std::endl;

            v = a;

            //std::cout << v << std::endl;

        }
        std::string const s("aaa");
        smatch m;
        if(regex_match(s, m, v)) // OK, this shouldn't match
        {
            BOOST_ERROR("cycle test 5 failed");
        }
    }

    if(0 != detail::regex_impl<std::string::const_iterator>::instances)
    {
        BOOST_ERROR("leaks detected (cycle test 5)");
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Debug stuff
//
namespace
{
    const struct debug_init
    {
        debug_init()
        {
            #ifdef _MSC_VER
            // Send warnings, errors and asserts to STDERR
            _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_WARN,   _CRTDBG_FILE_STDERR);
            _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ERROR,  _CRTDBG_FILE_STDERR);
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

            // Check for leaks at program termination
            _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));

            //_CrtSetBreakAlloc(221);
            #endif
        }
    } dbg;
}
