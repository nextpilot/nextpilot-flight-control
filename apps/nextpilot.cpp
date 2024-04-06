#define LOG_TAG "my"
#include "nextpilot.h"
#include "ModuleCommand.hpp"
#include "ThreadItem.hpp"

class mymodule : public ModuleCommand<mymodule>, public ModuleThread {
public:
    mymodule() {
    }
};

int mymodule_main(int argc, char *argv[]) {
    return mymodule::main(argc, argv);
}
MSH_CMD_EXPORT_ALIAS(mymodule_main, my, );
