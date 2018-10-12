/*
 * keyfilters.cpp
 *
 *  Created on: 11 окт. 2018 г.
 *      Author: alexrayne
 */
#include <string.h>
#include "keyfilters.hpp"

// ********************************* FilterWordsBuffered *********************************
FilterWordsBuffered::FilterWordsBuffered(FilterWordsBuffered::TestSeq testfunc, unsigned bufsize){
    test = testfunc;
    buf_size = bufsize;
    buf = new char[bufsize];
    buf_len = 0;
    buf_limit = buf+buf_size;
}

FilterWordsBuffered::~FilterWordsBuffered(){
    delete[] buf;
    buf = nullptr;
}

void FilterWordsBuffered::filter(FILE* src, FILE* dst){
    char* sp = buf;
    char* wordp = nullptr;
    int wordlen;
    while(!feof(src)){

        if (sp >= buf_limit)
            sp = buf;
        int readen = buf_load(src, sp);
        if (readen <= 0)
            break;

        if (wordp == sp){
            //эта ситуация используется для пропуска текущего слова (непрошедшего тест)
            sp = skip_word(sp);
            if (sp < buf_limit)
                wordp = nullptr;
            else
                continue;
        }
        if (isspace(*sp)){
            sp = skip_space(sp);
        }

        while (sp < buf_limit){
            if (wordp == nullptr)
                wordp = sp;

            wordlen = test(sp, (buf_limit-sp));
            if (wordlen >= 0){
                sp += wordlen;
                if (sp < buf_limit){
                    if (isspace(*sp)){
                        //прoтестировано целое слово
                        fwrite(wordp, 1, (sp-wordp)+1, dst);
                        //fputs("\n", dst);
                    }
                    else {
                        //пропущу остаток слова
                        sp = skip_word(sp);
                        if (sp >= buf_limit){
                            sp      = buf;
                            wordp   =buf;
                            //подгружу буфер
                            break;
                        }
                    }
                    wordp = nullptr;
                }
                else {
                    //уперлись в границу буфера, сброшу буфер до слова
                    if (wordp > buf){
                        memcpy(buf, wordp, wordlen);
                        wordp = buf;
                        sp = buf + wordlen;
                        //подгружу буфер
                        break;
                    }
                    else {
                        //буфер уже забит словом, отброшу его ибо слишком длинное
                        sp = buf;
                        //подгружу буфер
                        break;
                    }
                }
            }//if (wordlen
            else {
                wordp = nullptr;
                // пропустить несимволы с текущей позиции
                sp -= wordlen;
            }

            if (sp >= buf_limit){
                // надо подгрузить в буфер следущие данные
                break;
            }

            sp = skip_space(sp);
        }//while (sp

    }//while(!feof
    if ((wordp != nullptr) && (wordp != sp)){
        //в буфере осталось слово ожидающее подгрузки
        fwrite(wordp, 1, (sp-wordp), dst);
    }
}

int FilterWordsBuffered::buf_load(FILE* src, char* sp){
    buf_len = (sp-buf);
    unsigned freebuf = buf_size - (sp-buf);
    int readen = fread(sp, 1, freebuf, src);
    buf_len += readen;
    buf_limit = buf+buf_len;
    return readen;
}

char*   FilterWordsBuffered::skip_space(char* sp){
    for (;sp < buf_limit; sp++)
        if ((*sp != '\0') && !isspace(*sp))
            break;
    return sp;
}

char*   FilterWordsBuffered::skip_word(char* sp){
    for (;sp < buf_limit; sp++)
        if ((*sp != '\0') && isspace(*sp))
            break;
    return sp;
}

