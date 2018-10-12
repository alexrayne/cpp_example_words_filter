/*
 * charsmap.cpp
 *
 *  Created on: 11 окт. 2018 г.
 *      Author: alexrayne
 */

#include "charsmap.hpp"
#include <cctype>

void ENBindedkeysMap::ENSet::assign(const char* chars){
    for (; *chars != '\0'; chars++){
        char x = *chars;
        if (is_valid_char(x))
            set(idx(x));
    }
}

std::ostream& ENBindedkeysMap::ENSet::dump(std::ostream& dst) const{
    for (unsigned i = 0; i < size(); i++){
        if (test(i))
            dst<<chr(i);
    }
    return dst;
}

bool ENBindedkeysMap::is_valid_char(char x) const {
    if (!set_t::is_valid_char(x))
        return false;
    const set_t& node = _map[idx(x)];
    return !node.none();
};

bool ENBindedkeysMap::is_allowed(char key, char next) const{
    if (is_valid_char(key) ) {
        const set_t& node = _map[idx(key)];
        if (node.is_valid_char(next))
            return node.test(node.idx(next));
    }
    return false;
}

void ENBindedkeysMap::clear(){
    for( unsigned i = 0 ; i < map_limit; i++){
        _map[i].reset();
    }
}

// строит карту из строк вида <key>{<allowed key>}+\n
bool ENBindedkeysMap::init_by_plainlist(std::istream* src){
    char line[256];
    clear();
    while (src->good()){
        src->getline(line, sizeof(line) );
        if (!src->good())
            return true;
        char key = line[0];
        if (set_t::is_valid_char(key)){
            set_t& nodelow = _map[idx(tolower(key))];
            set_t& nodeup  = _map[idx(toupper(key))];
            for (const char* s = line+1; *s != '\0'; s++){
                char x = *s;
                if ( set_t::is_valid_char(x) ){
                    nodelow.set( idx(tolower(x)) );
                    nodelow.set( idx(toupper(x)) );
                    nodeup.set( idx(tolower(x)) );
                    nodeup.set( idx(toupper(x)) );
                }
            }
        }
    }
    return true;
}



std::ostream& ENBindedkeysMap::dump_map(std::ostream& dst) const{
    //dst<<'\n';
    for (unsigned i=0;i<map_limit;i++){
        const set_t& node = _map[i];
        if (node.count() > 0){
            dst<< set_t::chr(i);
            node.dump(dst);
            dst<<'\n';
        }
    }
    return dst;
}

