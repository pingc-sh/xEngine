///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "xLog.hpp"
#include "xTimeNow.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::xLog;
using xEngine::xUtils::TimeNow;

///////////////////////////////////////////////////////////////////////////////

int main()
{
    TimeNow t = TimeNow();
    std::cout << t.s_str() << std::endl;
    std::cout << t.f_str() << std::endl;

    xLog.start("xEngine", "./");
    int x =100;
    const char* s = "value_x";

    xLog.info("stared");
    xLog.info("x: %d", x);
    xLog.warn("warning");
    xLog.debug("value: %s", s);
    xLog.error("error");

    xLog.stop();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
