#include <deque>
#include <map>
#include <set>

#include "fsm/fsm.h"

#include "testdefs.h"

namespace {

using str_trans = fsm::trans_traits<char, uint32_t, std::map<char, uint32_t>, false>;
using str_fsm = fsm::fsm<str_trans, std::deque>;

} // <anonymous> namespace

TEST(fsm, base)
{
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

TEST(fsm, chech_invalid)
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
}

int main()
{
    return RUN_TESTS();
}

