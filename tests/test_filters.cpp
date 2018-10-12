#include "test_filters.h"
#include "cute.h"

#include <unistd.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>

#include <charsmap.hpp>
#include <keyfilters.hpp>


using namespace std;

class FilterWordsBufferedTests{
public:
    FilterWordsBufferedTests();
    FilterWordsBufferedTests(const FilterWordsBufferedTests& x);
    ~FilterWordsBufferedTests();

    void TestPlain();
    void TestLongSpace();
    void TestLongWord();
protected:
    static const char inimap[];
    static
    ENBindedkeysMap allowed_keys;
    static
    int check_seq(const char* src, unsigned size) {
        return check_seq_LR1(allowed_keys, src, size);
    };

    FilterWordsBuffered words_filter;
    FILE* src;
    FILE* dst;
    void filter(){
        words_filter.filter(src, dst);
    };
    void assign_src(const char* s);
    bool issame_dst(const char* s);
};
const char FilterWordsBufferedTests::inimap[] = "a 1b\n"
            "Bc\n"
            "cD\n"
            "daA\n";
ENBindedkeysMap FilterWordsBufferedTests::allowed_keys;

FilterWordsBufferedTests::FilterWordsBufferedTests()
:words_filter(check_seq,8)
{
    auto keymap_io = std::unique_ptr<std::istringstream>(new std::istringstream(inimap));
    allowed_keys.init_by_plainlist(keymap_io.get());
    //src = fopen("./FilterWordsBufferedTests.src", "w+");
    src = tmpfile();
    ASSERT(src != nullptr);
    //dst = fopen("./FilterWordsBufferedTests.dst", "w+");
    dst = tmpfile();
    ASSERT(dst != nullptr);
}

FilterWordsBufferedTests::FilterWordsBufferedTests(const FilterWordsBufferedTests& x)
:words_filter(check_seq,8)
{
    //src = fopen("./FilterWordsBufferedTests.src", "w+");
    src = tmpfile();
    ASSERT(src != nullptr);
    //dst = fopen("./FilterWordsBufferedTests.dst", "w+");
    dst = tmpfile();
    ASSERT(dst != nullptr);
}

FilterWordsBufferedTests::~FilterWordsBufferedTests(){
    if (src != nullptr)
        fclose(src);
    if (dst != nullptr)
        fclose(dst);
    //remove("./FilterWordsBufferedTests.src");
    //remove("./FilterWordsBufferedTests.dst");
}

void FilterWordsBufferedTests::assign_src(const char* s){
    rewind(src);
    int slen = strlen(s);
    fwrite(s, 1, slen, src);
    ftruncate(fileno_unlocked(src), slen);
    rewind(src);
}

bool FilterWordsBufferedTests::issame_dst(const char* s){

    int slen = strlen(s);
    string dstr;
    dstr.resize(slen);

    rewind(dst);
    int dlen = fread((void*)dstr.data(), 1, slen, dst);
    bool iseof = (fgetc(dst) == EOF) ;
    rewind(dst);

    if ( (dlen != slen) || (!iseof) )
        return false;
    return dstr.compare(s) == 0;
}



void FilterWordsBufferedTests::TestPlain(){
    assign_src("\n a\nt ...\nabc \ncde ,bcd");
    filter();
    bool ok = issame_dst("a\nabc bcd");
    ASSERTM("plain filter", ok);
}

void FilterWordsBufferedTests::TestLongSpace(){
    assign_src("\n a \t     \n                   \nabc\nbcd");
    filter();
    bool ok = issame_dst("a abc\nbcd");
    ASSERTM("long space", ok);
}

void FilterWordsBufferedTests::TestLongWord(){
    assign_src("\n a ,abababababababababaabababab\nabc\tbcd\n");
    filter();
    bool ok = issame_dst("a abc\tbcd\n");
    ASSERTM("long word", ok);
}



cute::suite make_suite_test_filters() {
	cute::suite s { };
	static FilterWordsBufferedTests   tests;
	s.push_back(CUTE_CONTEXT_MEMFUN(tests, FilterWordsBufferedTests, TestPlain));
	s.push_back(CUTE_CONTEXT_MEMFUN(tests, FilterWordsBufferedTests, TestLongSpace));
	s.push_back(CUTE_CONTEXT_MEMFUN(tests, FilterWordsBufferedTests, TestLongWord));
	return s;
}
