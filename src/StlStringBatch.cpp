#include "StlStringBatch.hpp"

namespace th128 {

void StlStringExercise(std::string* a, const std::string* b, const char* s, size_t n, size_t pos, size_t cnt) {
    a->assign(s);
    a->assign(s, n);
    a->assign(*b, pos, cnt);
    a->append(s);
    a->append(s, n);
    a->append(*b, pos, cnt);
    a->erase(pos, cnt);
    a->reserve(n);
    a->resize(n);
    a->push_back('x');
    std::string c(*b);
    *a = c;
}

} // namespace th128
