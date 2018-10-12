/*
 * keyfilters.hpp
 *
 *  Created on: 11 окт. 2018 г.
 *      Author: alexrayne
 */

#ifndef KEYFILTERS_HPP_
#define KEYFILTERS_HPP_

#include "charsmap.hpp"

/* проверка последовательности символов по словарю map - каждый символ сверяется в контексте предыдущего
 * \arg size - длина буфера символов
 * \return >=0 - длина валидной последовательности.
 *         < 0 - длина последовательности невалидных символов.
 * */
template<typename Tmap>
int check_seq_LR1(Tmap& map, const char* src, unsigned size){
    if (size <= 0)
        return 0;
    const char* ps = src;
    char key = *ps++;
    if (!map.is_valid_char(key)){
        for (const char* plim = src+size; ps < plim; ps++){
            if (map.is_valid_char(*ps))
                break;
        }
        return (src-ps);
    }
    for (unsigned i = 1; i < size; key=*ps++, i++){
        if (!map.is_allowed(key, *ps)){
            return i;
        }
    }
    return size;
};

class FilterWordsBuffered {
public:
    typedef int (*TestSeq)(const char* src, unsigned size);

    FilterWordsBuffered(TestSeq testfunc, unsigned bufsize = 4096);
    ~FilterWordsBuffered();

    void filter(FILE* src, FILE* dst);

protected:
    TestSeq test;
    char*   buf;
    const char* buf_limit;
    unsigned buf_size;
    unsigned buf_len;
    int     buf_load(FILE* src, char* sp);
    char*   skip_space(char* sp);
    char*   skip_word(char* sp);
};



#endif /* KEYFILTERS_HPP_ */
