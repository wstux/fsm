#include <array>
#include <deque>
#include <limits>
#include <map>
#include <set>

#include "fsm/fsm.h"
#include "fsm/trie.h"

#include "testdefs.h"

namespace {

using str_trans_flat = fsm::trans_traits<char, uint32_t, std::array<uint32_t, 127>, true>;
using str_trans_flex = fsm::trans_traits<char, uint32_t, std::map<char, uint32_t>, false>;

} // <anonymous> namespace

INIT_TYPE_TESTS(fsm, str_trans_flat, str_trans_flex)

TYPED_TEST(fsm, base_fsm)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_fsm fsm;
    EXPECTED(fsm.size() == 2);
    for (const std::string& str : etalon) {
        EXPECTED(fsm.insert(str)) << str << std::endl;
    }

    EXPECTED(! fsm.follow(std::string("a")));
    EXPECTED(! fsm.follow(std::string("ab")));
    EXPECTED(! fsm.follow(std::string("abc")));
    EXPECTED(! fsm.follow(std::string("xxx")));

    for (const std::string& str : etalon) {
        EXPECTED(fsm.follow(str)) << str << std::endl;
    }
}

TYPED_TEST(fsm, fsm_copy)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_fsm fsm_1;
    for (const std::string& str : etalon) {
        EXPECTED(fsm_1.insert(str)) << str << std::endl;
    }

    str_fsm fsm_2 = fsm_1;

    for (const std::string& str : etalon) {
        EXPECTED(fsm_1.follow(str)) << str << std::endl;
        EXPECTED(fsm_2.follow(str)) << str << std::endl;
    }
}

TYPED_TEST(fsm, fsm_move)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_fsm fsm_1;
    for (const std::string& str : etalon) {
        EXPECTED(fsm_1.insert(str)) << str << std::endl;
    }

    str_fsm fsm_2 = std::move(fsm_1);

    for (const std::string& str : etalon) {
        EXPECTED(fsm_2.follow(str)) << str << std::endl;
    }
}

TYPED_TEST(fsm, fsm_swap)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans>;

    const std::vector<std::string> etalon_1 = {"abcd", "abce", "apple", "banana", "banan"};
    const std::vector<std::string> etalon_2 = {"abcz", "abcx", "applz", "bananz", "banaz"};

    str_fsm fsm_1;
    for (const std::string& str : etalon_1) {
        EXPECTED(fsm_1.insert(str)) << str << std::endl;
    }
    str_fsm fsm_2;
    for (const std::string& str : etalon_2) {
        EXPECTED(fsm_2.insert(str)) << str << std::endl;
    }

    fsm_2.swap(fsm_1);

    for (const std::string& str : etalon_1) {
        EXPECTED(! fsm_1.follow(str)) << str << std::endl;
        EXPECTED(fsm_2.follow(str)) << str << std::endl;
    }
    for (const std::string& str : etalon_2) {
        EXPECTED(fsm_1.follow(str)) << str << std::endl;
        EXPECTED(! fsm_2.follow(str)) << str << std::endl;
    }
}

TYPED_TEST(fsm, base_trie)
{
    using str_trans = TType;
    using str_trie = fsm::trie<size_t, str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_trie trie;
    EXPECTED(trie.size() == 2);
    for (size_t i = 0; i < etalon.size(); ++i) {
        EXPECTED(trie.insert(etalon[i], i)) << etalon[i] << std::endl;
    }

    EXPECTED(! trie.follow(std::string("a")));
    EXPECTED(! trie.follow(std::string("ab")));
    EXPECTED(! trie.follow(std::string("abc")));
    EXPECTED(! trie.follow(std::string("xxx")));

    for (size_t i = 0; i < etalon.size(); ++i) {
        size_t val;
        EXPECTED(trie.follow(etalon[i], val)) << etalon[i] << std::endl;
        EXPECTED(val == i) << etalon[i] << ": " << val << " != " << i << std::endl;
    }
}

TYPED_TEST(fsm, trie_copy)
{
    using str_trans = TType;
    using str_trie = fsm::trie<size_t, str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_trie trie_1;
    for (size_t i = 0; i < etalon.size(); ++i) {
        EXPECTED(trie_1.insert(etalon[i], i)) << etalon[i] << std::endl;
    }

    str_trie trie_2 = trie_1;
    for (size_t i = 0; i < etalon.size(); ++i) {
        size_t val;
        EXPECTED(trie_1.follow(etalon[i], val)) << etalon[i] << std::endl;
        EXPECTED(val == i) << etalon[i] << ": " << val << " != " << i << std::endl;

        EXPECTED(trie_2.follow(etalon[i], val)) << etalon[i] << std::endl;
        EXPECTED(val == i) << etalon[i] << ": " << val << " != " << i << std::endl;
    }
}

TYPED_TEST(fsm, trie_move)
{
    using str_trans = TType;
    using str_trie = fsm::trie<size_t, str_trans>;

    const std::vector<std::string> etalon = {"abcd", "abce", "apple", "banana", "banan"};

    str_trie trie_1;
    for (size_t i = 0; i < etalon.size(); ++i) {
        EXPECTED(trie_1.insert(etalon[i], i)) << etalon[i] << std::endl;
    }

    str_trie trie_2 = std::move(trie_1);
    for (size_t i = 0; i < etalon.size(); ++i) {
        size_t val;
        EXPECTED(trie_2.follow(etalon[i], val)) << etalon[i] << std::endl;
        EXPECTED(val == i) << etalon[i] << ": " << val << " != " << i << std::endl;
    }
}

TYPED_TEST(fsm, trie_swap)
{
    using str_trans = TType;
    using str_trie = fsm::trie<size_t, str_trans>;

    const std::vector<std::string> etalon_1 = {"abcd", "abce", "apple", "banana", "banan"};
    const std::vector<std::string> etalon_2 = {"abcz", "abcx", "applz", "bananz", "banaz"};

    str_trie trie_1;
    for (size_t i = 0; i < etalon_1.size(); ++i) {
        EXPECTED(trie_1.insert(etalon_1[i], i)) << etalon_1[i] << std::endl;
    }
    str_trie trie_2;
    for (size_t i = 0; i < etalon_2.size(); ++i) {
        EXPECTED(trie_2.insert(etalon_2[i], i)) << etalon_2[i] << std::endl;
    }

    trie_2.swap(trie_1);

    for (size_t i = 0; i < etalon_1.size(); ++i) {
        size_t val;
        EXPECTED(! trie_1.follow(etalon_1[i], val)) << etalon_1[i] << std::endl;

        EXPECTED(trie_2.follow(etalon_1[i], val)) << etalon_1[i] << std::endl;
        EXPECTED(val == i) << etalon_1[i] << ": " << val << " != " << i << std::endl;
    }
    for (size_t i = 0; i < etalon_2.size(); ++i) {
        size_t val;
        EXPECTED(trie_1.follow(etalon_2[i], val)) << etalon_2[i] << std::endl;
        EXPECTED(val == i) << etalon_2[i] << ": " << val << " != " << i << std::endl;

        EXPECTED(! trie_2.follow(etalon_2[i], val)) << etalon_2[i] << std::endl;
    }
}

TYPED_TEST(fsm, DISABLED_chech_invalid)
{
    using str_trans = TType;
    using str_fsm = fsm::fsm<str_trans, std::deque>;

    const std::set<std::string> etalon = {"abcd", "abc", "app", "ban", "bana"};

    str_fsm fsm;
    EXPECTED(fsm.size() == 2);

    for (const std::string& str : etalon) {
        EXPECTED(fsm.insert(str)) << str << std::endl;
    }

    const size_t k_ch_count = 7;
    const size_t k_max_ch = 127;
    size_t counter = 0;
    for (size_t i = 0; i < std::numeric_limits<uint32_t>::max(); ++i) {
        size_t remainder = i;
        std::string str;

        bool is_failed = false;
        while (remainder > 0) {
            const char ch = static_cast<char>(remainder % k_max_ch);
            if ((ch < 1) || (ch > 126)) {
                is_failed = true;
                break;
            }
            remainder = remainder >> k_ch_count;
            str.insert(str.cbegin(), ch);
        }
        if (is_failed) {
            continue;
        }
        if (etalon.count(str) != 0) {
            continue;
        }
        ++counter;
        EXPECTED(! fsm.follow(str)) << str << std::endl;
    }

    for (const std::string& str : etalon) {
        EXPECTED(fsm.follow(str)) << str << std::endl;
    }
    //EXPECTED(counter == ) << counter << std::endl;
}

int main()
{
    return RUN_TESTS();
}

