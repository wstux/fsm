#include <array>
#include <deque>
#include <map>
#include <set>

#include "fsm/fsm.h"

#include "testdefs.h"

namespace {

using str_trans_flat = fsm::trans_traits<char, uint32_t, std::array<uint32_t, 127>, true>;
using str_trans_flex = fsm::trans_traits<char, uint32_t, std::map<char, uint32_t>, false>;

} // <anonymous> namespace

INIT_TYPE_TESTS(fsm, str_trans_flat, str_trans_flex)

TYPED_TEST(fsm, base)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans, std::deque>;

    str_fsm fsm;
    EXPECTED(fsm.size() == 2);

    EXPECTED(fsm.insert(std::string("abcd")));
    EXPECTED(fsm.insert(std::string("abce")));
    EXPECTED(fsm.insert(std::string("apple")));
    EXPECTED(fsm.insert(std::string("banana")));
    EXPECTED(fsm.insert(std::string("banan")));

    EXPECTED(! fsm.follow(std::string("a")));
    EXPECTED(! fsm.follow(std::string("ab")));
    EXPECTED(! fsm.follow(std::string("abc")));
    EXPECTED(fsm.follow(std::string("abcd")));
    EXPECTED(fsm.follow(std::string("abce")));
    EXPECTED(fsm.follow(std::string("apple")));
    EXPECTED(fsm.follow(std::string("banana")));
    EXPECTED(fsm.follow(std::string("banan")));
}

/*TEST(fsm, chech_invalid)
{
    str_fsm fsm;
    EXPECTED(fsm.size() == 2);

    EXPECTED(fsm.insert(std::string("abcd")));
    EXPECTED(fsm.insert(std::string("abce")));
    EXPECTED(fsm.insert(std::string("apple")));
    EXPECTED(fsm.insert(std::string("banana")));
    EXPECTED(fsm.insert(std::string("banan")));

    std::string prefix(1, 1);
    for (size_t i = 0; i < 6; ++i) {
        
        for (char ch = 1; ch < 127; ++ch) {
        }
    }

    EXPECTED(! fsm.follow(std::string("a")));
    EXPECTED(! fsm.follow(std::string("ab")));
    EXPECTED(! fsm.follow(std::string("abc")));
    EXPECTED(fsm.follow(std::string("abcd")));
    EXPECTED(fsm.follow(std::string("abce")));
    EXPECTED(fsm.follow(std::string("apple")));
    EXPECTED(fsm.follow(std::string("banana")));
    EXPECTED(fsm.follow(std::string("banan")));
}*/

int main()
{
    return RUN_TESTS();
}

