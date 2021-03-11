#include "App.h"

extern "C" {
#ifdef ESP_PLATFORM
void
app_main()
{
    App app{};
    app.start();
}
#else
int
main(int /*argc*/, char** /*argv*/)
{
    App app{};
    app.start();
    return 0;
}
#endif
}