// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstring>
#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include "frametable.h"

#include <iomanip>

#include <taglib/taglib.h>
#include <taglib/id3v1genres.h>
#include <taglib/id3v1tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>

#include <taglib/urllinkframe.h>
#include <taglib/unknownframe.h>
#include <taglib/privateframe.h>
#include <taglib/commentsframe.h>
#include <taglib/popularimeterframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/unsynchronizedlyricsframe.h>
#include <taglib/generalencapsulatedobjectframe.h>

using namespace TagLib;
using namespace ID3v2;

const char *GetDescription(ByteVector frameID)
{
  for(int ii = 0; ii < frameTableCount; ii++ )
  {
    if (ByteVector(frameTable[ii].frameName) == frameID)
      return frameTable[ii].frameLongName;
  }
  return "";
}

void PrintFrameHelp(char *sName)
{
    for(int ii = 0; ii < frameTableCount - 1; ii++ )
    {
      if (frameTable[ii].frameID != ID3FID_NOFRAME) {
        cout << "    --" << frameTable[ii].frameName << "    "
             << frameTable[ii].frameLongName << endl;
      }
    }
    return;
}

void PrintGenreList()
{
  StringList genres = ID3v1::genreList();
  List<String>::Iterator iter;

  int index;
  String genre;

  for (iter=genres.begin(); iter != genres.end(); ++iter) {
    genre = *iter;
    index = ID3v1::genreIndex(genre);
    cout << setw(3) << index << ": " << genre << endl;
  }
}

const void showFrame(const CommentsFrame *frame) {
  cout << "(" << frame->description() << ")[" << frame->language() << "]: " <<
	  frame->text();
}

const void showFrame(const UnsynchronizedLyricsFrame *frame) {
  cout << "(" << frame->description() << ")[" << frame->language() << "]: " <<
	  frame->text();
}

const void showFrame(const UrlLinkFrame *frame) {
  cout << frame->url();
}

const void showFrame(const UserUrlLinkFrame *frame) {
  cout << "(" << frame->description() << "): " << frame->url();
}

const void showFrame(const AttachedPictureFrame *frame) {
  ByteVector data = frame->picture();
  // TODO: taglib doesn't seem to know the format field. figure out why.
  String format = "";

  cout << "(" << frame->description() << ")[" << format << ", " <<
	frame->type() << "]: " << frame->mimeType() << ", " << data.size() <<
	" bytes";
}

const void showFrame(const GeneralEncapsulatedObjectFrame *frame) {
  ByteVector data = frame->object();

  cout << "(" << frame->description() << ")[" << frame->fileName() << "]: " <<
	frame->mimeType() << ", " << data.size() << " bytes";
}

const void showFrame(const PopularimeterFrame *frame) {
  // TODO: test this
  String email = frame->email();
  unsigned int counter = frame->counter();
  int rating = frame->rating();

  cout << email << ", counter=" << counter << " rating=" << rating;
}

const void showFrame(const UserTextIdentificationFrame *frame) {
  String desc;
  String text;

  StringList strings = frame->fieldList();
  List<String>::Iterator iter;

  // It seems taglib doesn't allow direct access to the text itself.
  // We therefore iterate over all available strings
  //
  // Also, if only 1 string is present, it must be the text since the
  // description *can* be empty while the text cannot.
  int index = 0;

  for (iter=strings.begin(); iter != strings.end(); ++iter) {
    index++;
    if (iter == strings.begin()) {
      desc = *iter;
    } else {
      text = *iter;
      if (text == "") {
	text = desc;
        desc = "";
      }
    }
  }
  cout << "(" << desc << "): " << text;
}

// Maybe the TIPL (ex IPLS, "Involved people list") frame should also get
// special treatment (it's a TextIdentificationFrame) [TODO]
const void showFrame(const TextIdentificationFrame *frame) {
  String text = frame->toString();

  if (frame->frameID() == "TCON") {
    int genre_id;

    if (text.toInt() || (text == "0")) { // 0 is also a valid genre
      genre_id = text.toInt();
      text = ID3v1::genre(genre_id);
    } else {
      genre_id = ID3v1::genreIndex(frame->toString());
    }
    cout << text << " (" << genre_id << ")";
  } else {
    cout << text;
  }
}

const void showFrame(const PrivateFrame *frame) {
  // TODO: test this
  cout << frame->toString();
}

// Frames that used to work but are not currently supported by taglib:
// - PCNT (Play Count)
// - USER (Terms of Use)
// - ENCR (Encryption method registration)
// - GRID (Group identification registration)
const void showFrame(const UnknownFrame *frame) {
  ByteVector frameID = frame->frameID();

  if (frameID == "PCNT") {
    // this one is trivial
    cout << frame->data().toUInt();
  } else {
    cout << "(unimplemented)";
  }
}

int PrintInformation(char *sFileName, int rfc822)
{
  MPEG::File f(sFileName, false); // false -> don't complain about non-MPEGs
  ID3v2::Tag *tag = f.ID3v2Tag();

  FrameList frames = tag->frameList();

  if (frames.size() < 1)
    return 1;

  Header *header = tag->header();
  unsigned int version = header->majorVersion();
  unsigned int revision = header->revisionNumber();

  if (rfc822)
    std::cout << "\nFilename: " << sFileName << std::endl;
  else
    cout << "id3v2 tag info for " << sFileName << " (v2." << version << "." <<
        revision << "):" << endl;

  FrameList::ConstIterator iter;
  for(iter = frames.begin(); iter != frames.end(); ++iter)
  {
    Frame *frame = *iter;

    ByteVector frameID = frame->frameID();

    if (rfc822)
      cout << frameID << ": ";
    else
      cout << frameID << " (" << GetDescription(frameID) << "): ";

    if (UserTextIdentificationFrame *ut =
          	    dynamic_cast<UserTextIdentificationFrame*>(frame))
      showFrame(ut);
    else if (TextIdentificationFrame *ti =
          	    dynamic_cast<TextIdentificationFrame*>(frame))
      showFrame(ti);
    else if (CommentsFrame *cf =
          	    dynamic_cast<CommentsFrame*>(frame))
      showFrame(cf);
    else if (UnsynchronizedLyricsFrame *lf =
          	    dynamic_cast<UnsynchronizedLyricsFrame*>(frame))
      showFrame(lf);
    else if (UserUrlLinkFrame *uf =
          	    dynamic_cast<UserUrlLinkFrame*>(frame))
      showFrame(uf);
    else if (UrlLinkFrame *uf =
          	    dynamic_cast<UrlLinkFrame*>(frame))
      showFrame(uf);
    else if (AttachedPictureFrame *pf =
          	    dynamic_cast<AttachedPictureFrame*>(frame))
      showFrame(pf);
    else if (GeneralEncapsulatedObjectFrame *gf =
          	    dynamic_cast<GeneralEncapsulatedObjectFrame*>(frame))
      showFrame(gf);
    else if (PopularimeterFrame *pf =
      	            dynamic_cast<PopularimeterFrame*>(frame))
      showFrame(pf);
    else if (UnknownFrame *uf =
		    dynamic_cast<UnknownFrame*>(frame))
      showFrame(uf);
    else if (PrivateFrame *pf =
		    dynamic_cast<PrivateFrame*>(frame))
      showFrame(pf);
    else {
      cout << frame->frameID() << " (----): " << frame->toString();
    }
    cout << endl;
  }
  
  return 0;
}

int PrintID3v1Tag(char *sFileName) 
// code from id3 
{
  struct id3 {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    /* With ID3 v1.0, the comment is 30 chars long */
    /* With ID3 v1.1, if comment[28] == 0 then comment[29] == tracknum */
    char comment[30];
    unsigned char genre;
  } id3v1tag;
  FILE *fp;
  
  fp = fopen(sFileName, "r"); /* read only */

  if (fp == NULL) { /* file didn't open */
    fprintf(stderr, "fopen: %s: ", sFileName);
    perror("id3v2");
    return -1;
  }
  if (fseek(fp, -128, SEEK_END) < 0) {
    /* problem rewinding */
  } else { /* we rewound successfully */ 
    if (fread(&id3v1tag, 128, 1, fp) != 1) {
      /* read error */
      fprintf(stderr, "fread: %s: ", sFileName);
      perror("");
    }
  }
    
  fclose(fp);

    /* This simple detection code has a 1 in 16777216
     * chance of misrecognizing or deleting the last 128
     * bytes of your mp3 if it isn't tagged. ID3 ain't
     * world peace, live with it.
     */
    
  
  if (!strncmp(id3v1tag.tag, "TAG", 3))
  {
    std::cout << "id3v1 tag info for " << sFileName << ":" << std::endl;
    printf("Title  : %-30.30s  Artist: %-30.30s\n",
            id3v1tag.title, id3v1tag.artist);
    printf("Album  : %-30.30s  Year: %-4.4s, Genre: %s (%d)\n",
            id3v1tag.album, id3v1tag.year, 
            (id3v1tag.genre < ID3v1::genreList().size())
            ? ID3v1::genre(id3v1tag.genre).toCString() : 
            "Unknown", id3v1tag.genre);
    if (!id3v1tag.comment[28])
      printf("Comment: %-28.28s    Track: %d\n", 
             id3v1tag.comment, id3v1tag.comment[29]);
    else
      printf("Comment: %-30.30s\n", id3v1tag.comment);
  } 
  else 
  {
    return 1;     
  }
  return 0;
}

// This reimplements PrintID3v1Tag based on taglib. Current versions of taglib
// do however suffer from a bug that makes this code worse than the 'manual'
// way above (see comment below for details).
//
// It's here for completeness' sake though (and because the bug might be fixed
// in future versions)
/*
int PrintID3v1TagNEW(char *sFileName)
{
  MPEG::File f(sFileName);

  if (!f.isValid()) {
    perror("id3v2");
    return -1;
  }

  ID3v1::Tag *tag = f.ID3v1Tag();

  // this doesn't work atm, bug in taglib:
  // - https://bugs.kde.org/show_bug.cgi?id=168633
  // - https://bugs.kde.org/show_bug.cgi?id=214249
  if (!tag)
    return 1;

  String artist = tag->artist();
  String title = tag->title();
  String album = tag->album();
  String year = String::number(tag->year());
  String genre = tag->genre();
  String comment = tag->comment();
  int genreID = ID3v1::genreIndex(genre);
  int track = tag->track();

  if (genre == "")
    genre = "Unknown";
  if (year == "0")
    year = "";

  std::cout << "id3v1 tag info for " << sFileName << ":" << std::endl;
  printf("Title  : %-30.30s  Artist: %-30.30s\n",
          title.toCString(), artist.toCString());
  printf("Album  : %-30.30s  Year: %-4.4s, Genre: %s (%d)\n",
          album.toCString(), year.toCString(), genre.toCString(), genreID);
  printf("Comment: %-28.28s    Track: %d\n", comment.toCString(), track);

  return 0;
}*/

void ListTag(int argc, char *argv[], int optind, int rfc822)
{
  int ret = 0;

  for (int nIndex = optind; nIndex < argc; nIndex++)
  {
    bool id3v1_tag = false;
    bool id3v2_tag = false;

    if (!rfc822)
    {
      ret = PrintID3v1Tag(argv[nIndex]);
      if (ret == -1)
      {
        continue;
      }
      else if(ret == 0)
      {
        id3v1_tag = true;
      }
    }
    if(!PrintInformation(argv[nIndex], rfc822))
      id3v2_tag = true;
    if(!id3v1_tag && !id3v2_tag)
      std::cout << argv[nIndex] << ": No ID3 tag" << std::endl;
    else {
      if (!id3v1_tag && !rfc822)
        std::cout << argv[nIndex] << ": No ID3v1 tag" << std::endl;
      if (!id3v2_tag)
        std::cout << argv[nIndex] << ": No ID3v2 tag" << std::endl;
    }
    id3v1_tag = false;
    id3v2_tag = false;
  }

  return;
}

