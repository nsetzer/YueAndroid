
#include <exception>
#include <iostream>
#include "yue/core/song.hpp"

#include "yue/core/search/rules.hpp"

namespace yue {
namespace core {

//-----------------------------------------------
// Static Definitions

// names of columns in the library view of the database
const char* Song::uid = "uid";
const char* Song::path = "path";
const char* Song::source_path = "source_path";
const char* Song::artist = "artist";
const char* Song::artist_key = "artist_key";
const char* Song::composer = "composer";
const char* Song::album = "album";
const char* Song::title = "title";
const char* Song::genre = "genre";
const char* Song::year = "year";

const char* Song::country = "country";
const char* Song::lang = "lang";
const char* Song::comment = "comment";
const char* Song::album_index = "album_index";
const char* Song::length = "length";
const char* Song::last_played = "last_played";
const char* Song::play_count = "playcount";
const char* Song::skip_count = "skip_count";
const char* Song::rating = "rating";
const char* Song::blocked = "blocked";

const char* Song::opm = "opm";
const char* Song::equalizer = "equalizer";
const char* Song::date_added = "date_added";
const char* Song::frequency = "frequency";
const char* Song::file_size = "file_size";

const char* Song::all_text = "all_text";

const int Song::num_columns = 25;

// The static vector contains all string keys for a song.

static const char* _strkeys[] = {
    Song::path,     Song::source_path, Song::artist,  Song::artist_key,
    Song::composer, Song::album,       Song::title,   Song::genre,
    Song::country,  Song::lang,        Song::comment, Song::uid,
};
const Song::strset Song::StringKeys(_strkeys,_strkeys+sizeof(_strkeys)/sizeof(char*));

static const char* _intkeys[] = {
    Song::year,        Song::album_index, Song::length,
    Song::last_played, Song::play_count, Song::skip_count,  Song::rating,
    Song::blocked,     Song::opm,        Song::equalizer,   Song::date_added,
    Song::frequency,   Song::file_size,
};
const Song::strset Song::IntegerKeys(_intkeys,_intkeys+sizeof(_intkeys)/sizeof(char*));

const Song::strmap Song::abbreviations = {
    {"id", Song::uid},
    {"uid", Song::uid},
    {"path", Song::path},
    {"src", Song::source_path},
    {"source_path", Song::source_path},
    {"art", Song::artist},
    {"artist", Song::artist},
    {"composer", Song::composer},
    {"abm", Song::album},
    {"alb", Song::album},
    {"album", Song::album},
    {"ttl", Song::title},
    {"tit", Song::title},
    {"title", Song::title},
    {"gen", Song::genre},
    {"genre", Song::genre},
    {"year", Song::year},
    {"country", Song::country},
    {"lang", Song::lang},
    {"com", Song::comment},
    {"comm", Song::comment},
    {"comment", Song::comment},
    {"index", Song::album_index},
    {"album_index", Song::album_index},
    {"len", Song::length},
    {"length", Song::length},
    {"date", Song::last_played},
    {"last_played", Song::last_played},
    {"pcnt", Song::play_count},
    {"count", Song::play_count},
    {"play_count", Song::play_count},
    {"playcount", Song::play_count},
    {"skip", Song::skip_count},
    {"scnt", Song::skip_count},
    {"skip_count", Song::skip_count},
    {"skipcount", Song::skip_count},
    {"rate", Song::rating},
    {"rating", Song::rating},
    {"text", Song::all_text},
    {"all_text", Song::all_text},
    {"ban", Song::blocked},
    {"banned", Song::blocked},
    {"blocked", Song::blocked},
    {"deleted", Song::blocked},
    {"opm", Song::opm},
    {"eq", Song::equalizer},
    {"equalizer", Song::equalizer},
    {"added", Song::date_added},
    {"freq", Song::frequency},
    {"frequency", Song::frequency},
    {"size", Song::file_size},
    {"file_size", Song::file_size},
};

//-----------------------------------------------
// member access

template<>
bool
Song::has<Song::int_t>(std::string key) {
    return m_mapDataInt.count(key) != 0;
}

template<>
bool
Song::has<Song::str_t>(std::string key) {
    return m_mapDataStr.count(key) != 0;
}

template<>
Song::int_t
Song::get<Song::int_t>(std::string key) {
    if (!has<Song::int_t>(key)) {
        throw std::invalid_argument(std::string("Invalid Key: ") + key);
    }
    return m_mapDataInt[key];
}

template<>
Song::str_t
Song::get<Song::str_t>(std::string key) {
    if (!has<Song::str_t>(key)) {
        throw std::invalid_argument(std::string("Invalid Key: ") + key);
    }
    return m_mapDataStr[key];
}

template<>
void
Song::set<Song::int_t>(std::string key, Song::int_t value) {
    m_mapDataInt[key] = value;
}

template<>
void
Song::set<Song::str_t>(std::string key, Song::str_t value) {
    m_mapDataStr[key] = value;
}

bool
Song::hasKey(std::string key) {
    return has<Song::str_t>(key) || has<Song::int_t>(key);
}

std::string
Song::getStr(std::string key) {
    return get<str_t>(key);
}

int
Song::getInt(std::string key) {
    return get<int_t>(key);
}

//-----------------------------------------------
// format conversion

void
Song::fromMap(std::map<std::string, std::string> mSong) {
    for (auto &key : Song::StringKeys) {
        set<Song::str_t>(key, mSong[key]);
    }

    for (auto &key : Song::IntegerKeys) {
        set<Song::int_t>(key, util::fromString<int>(mSong[key]));
    }
}

void
Song::toMap(std::map<std::string, std::string> &mSong) {
    for (auto &key : Song::StringKeys) {
        if (has<Song::str_t>(key))
            mSong[key] = get<Song::str_t>(key);
        else
            mSong[key] = "";
    }

    for (auto &key : Song::IntegerKeys) {
        if (has<Song::int_t>(key))
            mSong[key] = util::toString<int>(get<Song::int_t>(key));
        else
            mSong[key] = "0";
    }
}

} /* namespace core */
} /* namespace yue */

/*
int main(void)
{
    using namespace yue::core;

    yue::core::Song s;

    for (auto& key : Song::StringKeys)
    {
        s.set<Song::str_t>(key,key);
    }
    for (auto& key : Song::IntegerKeys)
    {
        s.set<Song::int_t>(key,1);
    }

    std::map<std::string,std::string> mData;

    s.toMap(mData);

    for (auto& it : mData)
    {
        std::cout << it.first << ":" << it.second << std::endl;
    }

    yue::core::Song s2;
    s2.fromMap(mData);

    std::cout << s2.get<Song::str_t>(Song::artist) << std::endl;
    std::cout << s2.getInt(Song::play_count) << std::endl;

    std::cout << Song::StringKeys.size() + Song::IntegerKeys.size() <<
std::endl; std::cout << Song::StringKeys.size()  << std::endl; std::cout <<
Song::IntegerKeys.size()  << std::endl; return 0;
}


*/
