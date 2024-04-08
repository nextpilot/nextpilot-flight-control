#define LOG_TAG "my"
#include "nextpilot.h"

using namespace nextpilot::global_params;
class mymodule : public ModuleCommand<mymodule>, public ModuleParams, public nextpilot::WorkItem {
public:
    mymodule() :
        ModuleParams(nullptr),
        WorkItem("aba", nextpilot::wq_configurations::hp_default) {
    }

    void Run() override {
    }

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MC_PITCHRATE_P>)_param_mc_pitchrate_p,
        (ParamFloat<params_id::MC_PITCHRATE_I>)_param_mc_pitchrate_i,
        (ParamFloat<params_id::MC_PR_INT_LIM>)_param_mc_pr_int_lim,
        (ParamFloat<params_id::MC_PITCHRATE_D>)_param_mc_pitchrate_d,
        (ParamFloat<params_id::MC_PITCHRATE_FF>)_param_mc_pitchrate_ff,
        (ParamFloat<params_id::MC_PITCHRATE_K>)_param_mc_pitchrate_k,
        (ParamFloat<params_id::MC_ROLLRATE_P>)_param_mc_rollrate_p)
};

int mymodule_main(int argc, char *argv[]) {
    return mymodule::main(argc, argv);
}
// MSH_CMD_EXPORT_ALIAS(mymodule_main, my, );
// void abc() {
//     nextpilot::WorkItem a("ab", nextpilot::wq_configurations::rate_ctrl);
// }
