#include "plugin/plugin.h"
#include "plugin/plugin_handler.h"

#include "time.h"

extern "C" void FuncForPlugin(rvsim::Plugin &plug, rvsim::PluginRegimes regime)
{
    switch (regime) {
        case rvsim::PluginRegimes::COSIM_RUN: /* initialization for cosimulation routine */
        {
            FILE *dump_file = fopen("cosim.trace", "a");
            if (!dump_file) {
                std::cerr << "can't open file for cosim\n";
                return;
            }

            const rvsim::Instruction *cur_instr = plug.GetCurInstr();
            if (cur_instr->id == rvsim::InstructionId::BB_END)
                return;

            rvsim::PluginHandler *handler = plug.GetHandler();

            fprintf(dump_file, "%lx", handler->GetPC());
            for (size_t i = 0; i < 32; ++i) {
                fprintf(dump_file, ",%lx", handler->GetGPR(i));
            }
            fprintf(dump_file, "\n");

            fclose(dump_file);
            break;
        }
        default:
            std::cerr << "Undefined regime of Plugin: " << std::int32_t(regime) << std::endl;
            break;
    }
}
