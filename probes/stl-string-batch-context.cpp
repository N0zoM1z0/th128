#include "../src/StlStringBatch.hpp"

extern "C" int __stdcall StlStringBatchProbe(int selector, int arg) {
    std::string a;
    std::string b("hello");
    th128::StlStringExercise(&a, &b, (const char*)arg, (size_t)(arg & 0x3f), (size_t)(selector & 7), (size_t)((selector >> 3) & 7));
    return (int)a.size();
}
