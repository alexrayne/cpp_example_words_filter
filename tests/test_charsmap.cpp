#include "test_charsmap.h"
#include "cute.h"

#include <unistd.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <cstring>

#include <charsmap.hpp>
#include <keyfilters.hpp>

class charsmapTests{
public:
    charsmapTests(){
        auto keymap_io = std::unique_ptr<std::istringstream>(new std::istringstream(inimap));
        allowed_keys.init_by_plainlist(keymap_io.get());
    }

    void TestLoad() const;
    void TestValidate() const;
protected:
    static const char inimap[];
    ENBindedkeysMap allowed_keys;
    int check_seq(const char* src, unsigned size) const{
        return check_seq_LR1(allowed_keys, src, size);
    }
};
const char charsmapTests::inimap[] = "a 1b\n"
            "Bc\n"
            "cD\n"
            "daA\n";

void charsmapTests::TestLoad() const{
    const char dump[] = "ABb\n"
            "BCc\n"
            "CDd\n"
            "DAa\n"
            "aBb\n"
            "bCc\n"
            "cDd\n"
            "dAa\n";
    auto keymap_dump = std::unique_ptr<std::ostringstream>( new std::ostringstream());
    allowed_keys.dump_map(*keymap_dump);
    const std::ostringstream::__string_type& s = keymap_dump->str();
    bool same = s.compare(dump) == 0;
	ASSERTM("ENBindedkeysMap:init_by_plainlist", same);
}

void charsmapTests::TestValidate() const{
    ASSERTM("ENBindedkeysMap:seq none", check_seq("",0) == 0);
    ASSERTM("ENBindedkeysMap:seq none1", check_seq("",1) == -1);
    ASSERTM("ENBindedkeysMap:seq1", check_seq("A",2) == 1);
    ASSERTM("ENBindedkeysMap:seq1 no", check_seq("t",2) == -2);
    ASSERTM("ENBindedkeysMap:seq2", check_seq("ab",3) == 2);
    ASSERTM("ENBindedkeysMap:seq2+", check_seq("ab 3",5) == 2);
    ASSERTM("ENBindedkeysMap:seq2+/", check_seq("ab,",4) == 2);
    ASSERTM("ENBindedkeysMap:seq5", check_seq("aBcDa",6) == 5);
    ASSERTM("ENBindedkeysMap:seq5+", check_seq("aBcDa \n",9) == 5);
    ASSERTM("ENBindedkeysMap:seq5+/", check_seq("aBcDa,\n",8) == 5);
    ASSERTM("ENBindedkeysMap:seq5+*", check_seq("aBcDatre,\n",10) == 5);
}

cute::suite make_suite_charsmap() {
	cute::suite s { };
    static charsmapTests   tests;
	s.push_back(CUTE_CONTEXT_MEMFUN(tests, charsmapTests, TestLoad));
    s.push_back(CUTE_CONTEXT_MEMFUN(tests, charsmapTests, TestValidate));
	return s;
}
