#include<stdio.h>
#include <unistd.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include <memory>
#include <cctype>
#include <getopt.h>

#include "charsmap.hpp"
#include "keyfilters.hpp"

const struct option main_options[] = {
      { "keysmap", required_argument, NULL, 'k' }
    , { "out", required_argument, NULL, 'o' }
    , { "help", no_argument, NULL, 'h' }
    , { NULL, 0, NULL, 0}
};

ENBindedkeysMap allowed_keys;
int allowed_keys_seq(const char* src, unsigned size){
    return check_seq_LR1(allowed_keys, src, size);
}

bool load_keysmap(const char* filename);
bool process_file(std::istream* src, FILE*dst);

int main(int argc, char **argv) {
    int opt;
    int opidx = 0;
    FILE*   src     = stdin;
    FILE*   dst     = stdout;
    const char* srcname = nullptr;
    const char* dstname = nullptr;
    bool havemap = false;

    while (optind < argc) {

    while ((opt = getopt_long(argc, argv, "k:", main_options, &opidx)) != -1) {
        switch (opt) {
        case 'k': {
            if (optarg != nullptr){
                if (!load_keysmap(optarg))
                    exit(EXIT_FAILURE);
                havemap = true;
            }
            else {
                fprintf(stderr, "Usage: --keysmap  option have no argument <filename>\n");
                exit(EXIT_FAILURE);
            }
            break;
        }

        case 'o':{
            if (optarg != nullptr)
                dstname = optarg;
            else {
                fprintf(stderr, "Usage: --out option have no argument <filename>\n");
                exit(EXIT_FAILURE);
            }
            break;
        }

        case 'h':{
            printf("Usage: %s {<options> {file...}* }*\n"
                   "    -k --keysmap <file> - provide an chars map for LR1 lookup filtering"
                   "    -o --out <file>     - denote destination file. by default prints to stdout"
                    , argv[0]);
            break;
        }

        default:
            fprintf(stderr, "Usage: %s --help for help screen\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } //while ((opt

        if (optind < argc){
            // src file name provided
            srcname = argv[optind++];
            src = fopen(srcname, "r");
            if ( src == nullptr ) {
                fprintf(stderr, "can`t open source file %s\n", srcname);
                exit(EXIT_FAILURE);
            }
        }
        if (!havemap){
            load_keysmap(nullptr);
            havemap = true;
        }

        if (src != NULL){

            if (dstname != nullptr){
                dst = fopen(srcname, "a");
            }

            if (dst == nullptr){
                fprintf(stderr, "can`t open for append file %s\n", dstname);
            }
            else {
                FilterWordsBuffered keys_filter(allowed_keys_seq);
                keys_filter.filter(src, dst);
                /*if (process_file(src, dst))
                    fprintf(stderr, "source %s done\n", srcname);
                else
                    fprintf(stderr, "failed process source %s\n", srcname);
               */
            }

            if (dstname != nullptr) {
                fclose(dst);
                dst = nullptr;
            }
            if (srcname != nullptr){
                fclose(src);
                src = nullptr;
            }
        } //if (src != NULL)
    } //while (optind < argc)

    return 0;
}


const char keysmap_default[] =
    "q12wa\n"
    "wq23esa\n"
    "ew34rds\n"
    "re45tfd\n"
    "tr56ygf\n"
    "yt67uhg\n"
    "uy78ijh\n"
    "iu89okj\n"
    "oi90plk\n"
    "po0-[;l\n"
    "aqwsz\n"
    "sawedxz\n"
    "dserfcx\n"
    "fdrtgvc\n"
    "gftyhbv\n"
    "hgyujnb\n"
    "jhuikmn\n"
    "kjiol,m\n"
    "lkop;.,\n"
    "zasx\n"
    "xzsdc\n"
    "cxdfv\n"
    "vcfgb\n"
    "bvghn\n"
    "nbhjm\n"
    "mnjk,\n"
    ;

bool load_keysmap(const char* filename){
    // prepare keysmap source
    std::unique_ptr<std::istream> keymap_io;
    if ( filename != NULL){
        keymap_io = std::make_unique<std::ifstream>(filename, std::ifstream::in );
        if (keymap_io->good()){
            fprintf(stderr, "use keysmap file %s\n", filename);
        }
        else {
            fprintf(stderr, "absent keysmap file %s\n", filename);
            return false;
        }
    }
    else {
        keymap_io = std::make_unique<std::istringstream>(keysmap_default);
        fputs("use default keysmap", stderr);
    }

    if ( !allowed_keys.init_by_plainlist( keymap_io.get() ) ) {
        fprintf(stderr, "keysmap failed load at %d\n", (int)keymap_io->tellg());
        return false;
    }
    return true;
}

char skip_space(std::istream* src);
void skip_word(std::istream* src);

// процессинг считывает слова, разделенные пробелами. чтение ведется буферами
//  удачные слова пишутся в вывод.
// реализую тупой буфер чтобы не связываться с констрейнами потоков
bool process_file(std::istream* src, FILE*dst){
    const unsigned buf_limit = 256;
    static char buf[buf_limit];
    while (src->good() && ! src->eof() ){
        char key = ' ';
        char next= ' ';
        char* word = buf;
        key = skip_space(src);
        if (key=='\0')
            break;
        *word++ = key;
        for (; src->good() && ! src->eof(); ) {
            key=next;
            next = src->get();
            if (allowed_keys.is_allowed(key, next)){
                *word++ = next;
                if ((word - buf) < buf_limit)
                    continue;
                fwrite(buf, 1, (word - buf), dst);
                word = buf;
                continue;
            }
            else if ( isspace(next) ){
                // слово успшно проверено целиком, значит можно его выдать
                *word++ = '\n';
                fwrite(buf, 1, (word - buf), dst);
                word = buf;
            }
            else {
                // слово не проходит тест, пропущу его
                skip_word(src);
            }
            next = skip_space(src);
            if (next=='\0')
                break;
            word = buf;
        }//for (next
    }//while (src->good()
    return true;
}

char skip_space(std::istream* src){
    while (src->good() && ! src->eof()){
        char x = src->get();
        if (!isspace(x))
            return x;
    }
    return '\0';
}

void skip_word(std::istream* src){
    while (src->good() && ! src->eof()){
        char x = src->get();
        if (isspace(x))
            return ;
    }
}
