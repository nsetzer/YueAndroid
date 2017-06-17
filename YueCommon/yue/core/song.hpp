
#ifndef YUE_CORE_SONG_HPP
#define YUE_CORE_SONG_HPP

#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

namespace yue {
namespace core {

/*

QDir::currentPath()

# expose a string to Qml
QUrl appPath(QString("%1").arg(app.applicationDirPath()));
engine.rootContext()->setContextProperty("appPath", appPath);

http://doc.qt.io/qt-5/sql-sqlstatements.html

a song is independant of Qt, and contains only the
data fields corresponding to the columns in the view.

a SongList is a lazy object
backed by an QSqlQuery
it is a list of N elements
when an element is accessed it is converted from aquery
into a song object.

while (query.next()) {
    QString name = query.value(0).toString();
    int salary = query.value(1).toInt();
    qDebug() << name << salary;
}

songs need to/from json methods -- maps of string:string

*/

/**
 * @brief The ConstCharStarComparator struct
 *
 * for Qt Support, use const char* instead of std::string
 */
struct ConstCharStarComparator
{
  bool operator()(const char *s1, const char *s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class Song {
  public:
    typedef std::set<const char*,ConstCharStarComparator> strset;
    typedef std::map<const char*, const char*, ConstCharStarComparator> strmap;
    //typedef std::map<cstd::string, std::string> strmap;
    typedef int32_t int_t;     // TODO int32_t or int64_t?
    typedef std::string str_t; // provided for completeness

    static const char* uid;         //
    static const char* path;        //
    static const char* source_path; //
    static const char* artist;      //
    static const char* artist_key;  //
    static const char* composer;    //
    static const char* album;       //
    static const char* title;       //
    static const char* genre;       //
    static const char* year;        //
    static const char* country;     //
    static const char* lang;        //
    static const char* comment;     //
    static const char* album_index; //
    static const char* length;      //
    static const char* last_played; //
    static const char* play_count;  //
    static const char* skip_count;  //
    static const char* rating;      //
    static const char* blocked;     //
    static const char* opm;         //
    static const char* equalizer;   //
    static const char* date_added;  //
    static const char* frequency;   //
    static const char* file_size;   // in bytes

    static const char* all_text; // in bytes

    static const int num_columns;

    static const strset StringKeys;
    static const strset IntegerKeys;

    // shortcut column names
    static const strmap abbreviations;

  public:
    Song(){}
    ~Song(){}

    template<typename T> bool has(std::string key);

    bool hasKey(std::string key);

    template<typename T> T get(std::string key);

    template<typename T> void set(std::string key, T value);

    std::string getStr(std::string key);
    int getInt(std::string key);

    // these are provided for converting between json,
    // and for debuging purposes. the map can be passed to
    // sql search Rule::check
    void fromMap(std::map<std::string, std::string> mSong);
    void toMap(std::map<std::string, std::string> &mSong);

  private:
    std::map<std::string, Song::int_t> m_mapDataInt;
    std::map<std::string, Song::str_t> m_mapDataStr;
};

} /* namespace core */
} /* namespace yue */

#endif /* YUE_CORE_SONG_HPP */
