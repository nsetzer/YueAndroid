
#include "taglib/taglib.h"
#include "taglib/fileref.h"
#include "taglib/id3v2tag.h"
#include "taglib/mpegfile.h"
#include "taglib/tlist.h"
#include "taglib/flacfile.h"
#include "taglib/attachedpictureframe.h"

#include <iostream>

int main(void)
{
    const char* path="/Users/nsetzer/Music/Library/Beast/Beast/01_Devil.mp3";

    TagLib::FileRef f(path);

    //virtual String title() const = 0;
    //virtual String artist() const = 0;
    //virtual String album() const = 0;
    //virtual String comment() const = 0;
    //virtual String genre() const = 0;
    //virtual unsigned int year() const = 0;
    //virtual unsigned int track() const = 0;


    std::cout << f.tag()->artist() << std::endl;
    std::cout << f.tag()->album() << std::endl;
    std::cout << f.tag()->title() << std::endl;
    std::cout << f.tag()->genre() << std::endl;

    TagLib::MPEG::File file(path);
    TagLib::ID3v2::Tag *m_tag = file.ID3v2Tag(true);
    TagLib::ID3v2::FrameList frameList = m_tag->frameList("APIC");
   if(frameList.isEmpty()) {
       return 1;
   }

   TagLib::ID3v2::AttachedPictureFrame *coverImg = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());

   std::cout <<"has img:" << (coverImg != NULL) << std::endl;
   std::cout <<"has img:" << coverImg->type() << std::endl;
   //QImage coverQImg;
   //coverQImg.loadFromData((const uchar *) coverImg->picture().data(), coverImg->picture().size());

   const char* path2 = "/Users/nsetzer/Music/Library/LISAGO/SUNBEAMS/06_Baby_Honey.flac";


   TagLib::FileRef f2(path2);
   std::cout << f2.tag()->artist() << std::endl;
   std::cout << f2.tag()->album() << std::endl;
   std::cout << f2.tag()->title() << std::endl;
   std::cout << f2.tag()->genre() << std::endl;

   TagLib::FLAC::File flacfile(path2);
   std::cout << flacfile.tag()->artist() << std::endl;
   std::cout << flacfile.tag()->album() << std::endl;
   std::cout << flacfile.tag()->title() << std::endl;
   std::cout << flacfile.tag()->genre() << std::endl;

   TagLib::List< TagLib::FLAC::Picture * > lst = flacfile.pictureList ();
   std::cout << lst.size() << std::endl;


    return 0;
}
