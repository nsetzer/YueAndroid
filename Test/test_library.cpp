#include "test_playlist.h"

using namespace yue::bell;





int test_library( void )
{
    int errors = 0;
    Database* db = Database::create();

    db->connect(":memory:");

    Library* lib = Library::create(db);

    test_(lib->size()==0,"wut");

    QMap<QString,QVariant> data;
    data["path"] = "path";
    data["artist"] = "artist";
    data["album"] = "album";
    data["title"] = "title";
    data["playcount"] = 12;
    Database::uid_t uid = lib->insert( data );

    test_(lib->size()==1,"");

    QSqlQuery q(db->db());
    q.prepare("SELECT artist,album,title,playcount FROM library WHERE album=?");
    q.addBindValue("album");
    test_(Database::checked_select(q),"query failed");
    test_(check_value<QString>(q,0,"artist"),"");
    test_(check_value<QString>(q,1,"album"),"");
    test_(check_value<QString>(q,2,"title"),"");
    test_(check_value<int>(q,3,12),"");

    QMap<QString,QVariant> data2;
    data2["title"] = "title2";
    data2["playcount"] = 15;
    lib->update( uid,  data2 );

    // run the query again
    test_(Database::checked_select(q),"query failed");

    test_(lib->size()==1,"");
    test_(check_value<QString>(q,0,"artist"),"");
    test_(check_value<QString>(q,1,"album"),"");
    test_(check_value<QString>(q,2,"title2"),"");
    test_(check_value<int>(q,3,15),"");

    lib->incrementPlaycount(uid);
    // run the query again
    test_(Database::checked_select(q),"query failed");
    test_(check_value<int>(q,3,16),"");

    QMap<QString,QVariant> data3;
    data3["path"] = "path";
    data3["artist"] = "artist";
    data3["album"] = "album";
    data3["title"] = "title";
    lib->insert( data3 );
    test_(lib->size()==2,"");

    QList<Database::uid_t> lst= lib->createPlaylist("",lib->size());
    test_(lst.size()==lib->size(),"");

    delete lib;
    delete db; // close connection

    return errors;
}
