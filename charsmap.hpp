/*
 * charsmap.hpp
 *
 *  Created on: 10 окт. 2018 г.
 *      Author: alexrayne
 */

#ifndef CHARSMAP_HPP_
#define CHARSMAP_HPP_

//#include <unordered_map>
#include <bitset>
#include <iostream>

// интерфейс карты разрешенных символов
class BindedkeysMap {
    public:
        bool is_allowed(char key, char next) const;

        // строит карту из строк вида <key>{<allowed keys>}+\n
        bool init_by_plainlist(std::istream* src);
};

// карта аглицких символов сделаю тупым массивом битовых масок 
class ENBindedkeysMap 
    : public BindedkeysMap
{

public:
    bool is_valid_char(char x) const;
    bool is_allowed(char key, char next) const;

    // строит карту из строк вида <key>{<allowed key>}+\n
    bool init_by_plainlist(std::istream* src);
    std::ostream& dump_map(std::ostream& dst) const;
    void clear();

protected:
        class ENSet : public std::bitset<64> {
            public:
                // en символы начинаются с '@'= 'A'-1 ==> idx0
                // код 0 - не смоответсвует символу, и всегда невалиден 
                static
                unsigned idx(char x) {return x-'A'+1;};
                static
                char chr(unsigned id) {return id+'A'-1;};

                static
                bool is_valid_char(char x){ return isalpha(x); };

                void assign(const char* chars);
                std::ostream& dump(std::ostream& dst) const;
        };

        typedef ENSet       set_t;
        static const unsigned map_limit = 64;
        set_t   _map[map_limit];

        static
        unsigned idx(char x) {return set_t::idx(x);};
};



#endif /* CHARSMAP_HPP_ */
