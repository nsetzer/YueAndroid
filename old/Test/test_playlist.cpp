#include "test_playlist.h"

using namespace yue::bell;

int test_playlist( void )
{
    int errors = 0;

    Database* db = Database::create();
    db->connect(":memory:");

    Library* lib = Library::create(db);
    PlaylistManager* pm = PlaylistManager::create(lib);

    QSharedPointer<Playlist> pl = pm->openCurrent();

    QList<Database::uid_t> lst = { 1, 2, 3, 4, 5};


    pl->insert(0,1);
    pl->insert(1,2);
    pl->insert(2,3);
    pl->insert(3,4);
    pl->insert(4,5);

    test_(pl->get(0)==1,"");
    test_(pl->get(1)==2,"");
    test_(pl->get(2)==3,"");
    test_(pl->get(3)==4,"");
    test_(pl->get(4)==5,"");

    pl->move(2,4);

    test_(pl->get(0)==1,"");
    test_(pl->get(1)==2,"");
    test_(pl->get(2)==4,"");
    test_(pl->get(3)==3,"");
    test_(pl->get(4)==5,"");

    pl->move(3,0);

    test2_(pl->get(0),3,"");
    test2_(pl->get(1),1,"");
    test2_(pl->get(2),2,"");
    test2_(pl->get(3),4,"");
    test2_(pl->get(4),5,"");


    //db->connect(":memory:");

    return 0;
}
