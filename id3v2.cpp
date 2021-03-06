#if defined HAVE_CONFIG_H
#include <config.h>
#endif
                                                                                
#include <cstdio>
#include <iostream>
#include <cstring>
#include <id3/tag.h>
#include <getopt.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v1genres.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>

#include <taglib/commentsframe.h>
#include <taglib/textidentificationframe.h>
#ifdef WIN32
#include <io.h>
#define snprintf _snprintf
#endif

#include <sys/types.h>
#include <sys/stat.h>

#define MAXNOFRAMES 1000

#define TMPSIZE 255    

using namespace TagLib;
using namespace ID3v2;

/* Write both tags by default */
flags_t UpdFlags = MPEG::File::AllTags;


void PrintUsage(char *sName)
{
  std::cout << "Usage: " << sName << " [OPTION]... [FILE]..." << std::endl;
  std::cout << "Adds/Modifies/Removes/Views id3v2 tags, modifies/converts/lists id3v1 tags" << std::endl;
  std::cout << std::endl;
  std::cout << "  -h,  --help               Display this help and exit" << std::endl;
  std::cout << "  -f,  --list-frames        Display all possible frames for id3v2" << std::endl;
  std::cout << "  -L,  --list-genres        Lists all id3v1 genres" << std::endl;
  std::cout << "  -v,  --version            Display version information and exit" << std::endl;
  std::cout << "  -l,  --list               Lists the tag(s) on the file(s)" << std::endl;
  std::cout << "  -R,  --list-rfc822        Lists using an rfc822-style format for output" << std::endl;
  std::cout << "  -d,  --delete-v2          Deletes id3v2 tags" << std::endl;
  std::cout << "  -s,  --delete-v1          Deletes id3v1 tags" << std::endl;
  std::cout << "  -D,  --delete-all         Deletes both id3v1 and id3v2 tags" << std::endl;
  std::cout << "  -C,  --convert            Converts id3v1 tag to id3v2" << std::endl;
  std::cout << "  -1,  --id3v1-only         Writes only id3v1 tag" << std::endl;
  std::cout << "  -2,  --id3v2-only         Writes only id3v2 tag" << std::endl;
  std::cout << "  -a,  --artist  \"ARTIST\"   Set the artist information" << std::endl;
  std::cout << "  -A,  --album   \"ALBUM\"    Set the album title information" << std::endl;
  std::cout << "  -t,  --song    \"SONG\"     Set the song title information" << std::endl;
  std::cout << "  -c,  --comment \"DESCRIPTION\":\"COMMENT\":\"LANGUAGE\"  "<< std::endl
       << "                            Set the comment information (both" << std::endl
       << "                             description and language optional)" << std::endl;    
  std::cout << "  -g,  --genre   num        Set the genre number" << std::endl;
  std::cout << "  -y,  --year    num        Set the year" << std::endl;
  std::cout << "  -T,  --track   num/num    Set the track number/(optional) total tracks" << std::endl;
  std::cout << std::endl;
  std::cout << "You can set the value for any id3v2 frame by using '--' and then frame id" << std::endl;
  std::cout << "For example: " << std::endl;
  std::cout << "        id3v2 --TIT3 \"Monkey!\" file.mp3" << std::endl;
  std::cout << "would set the \"Subtitle/Description\" frame to \"Monkey!\". " << std::endl;
  std::cout << std::endl;
}


StringList split(String subject, unsigned int num) {
  StringList list = StringList();
  String current = subject;

  while (num-- > 0) {
    int pos = current.find(":");
    if (num == 0) {
      list.append(current);
    } else if (pos > -1) {
      list.append(current.substr(0, pos));
      current = current.substr(pos + 1);
    } else {
      list.append(current);
      current = String::null;
    }
  }

  return list;
}

void PrintVersion(char *sName)
{
  size_t nIndex;
  std::cout << sName << " " << VERSION << std::endl;
  std::cout << "Uses " << ID3LIB_FULL_NAME << " and taglib-" <<
      TAGLIB_MAJOR_VERSION << "." << TAGLIB_MINOR_VERSION << "." <<
      TAGLIB_PATCH_VERSION << std::endl << std::endl;

  std::cout << "This program adds/modifies/removes/views id3v2 tags, " << std::endl 
       << "and can convert from id3v1 tags" << std::endl;
}


extern void ListTag(int argc, char *argv[], int optind, int rfc822);
extern void PrintFrameHelp(char *sName);
extern void PrintGenreList();

extern void DeleteTag(int argc, char *argv[], int optind, int whichTags); 
extern void ConvertTag(int argc, char *argv[], int optind);

//#ifdef SORT_RUNTIME
//extern void InitGenres();
//#endif  // SORT_RUNTIME

int main( int argc, char *argv[])
{
  int iOpt;
  int argCounter = 0;
  //int ii;
  char tmp[TMPSIZE];
  //FILE * fp;
  
  // TODO: REMOVE THIS
  //struct frameInfo {
  //  enum ID3_FrameID id;
  //  char *data;
  //} frameList[MAXNOFRAMES];
  
  //int frameCounter = 0;
  // TODO: END
  
  Map<String, String> *vals = new Map<String, String>();
  ID3v1::Tag* v1tag = new ID3v1::Tag();
  ID3v2::Tag* v2tag = new ID3v2::Tag();

  while (true)
  {
    int option_index = 0;
    int iLongOpt = 0;
    int optFrameID = ID3FID_NOFRAME;
    static struct option long_options[] = 
    { 
    // help and info
      { "help",    no_argument,       &iLongOpt, 'h' },
      { "list-frames",
                   no_argument,       &iLongOpt, 'f' },
      { "list-genres",   
                  no_argument,        &iLongOpt, 'L' },
      { "version", no_argument,       &iLongOpt, 'v' },

    // list / remove / convert
      { "list",   no_argument,        &iLongOpt, 'l' },
      { "list-rfc822",   
                   no_argument,       &iLongOpt, 'R' },
      { "delete-v2",  no_argument,    &iLongOpt, 'd' },
      { "delete-v1",  
                   no_argument,       &iLongOpt, 's' },
      { "delete-all",  
                   no_argument,       &iLongOpt, 'D' },
      { "convert", no_argument,       &iLongOpt, 'C' },
      { "id3v1-only", no_argument,       &iLongOpt, '1' },
      { "id3v2-only", no_argument,       &iLongOpt, '2' },

    // infomation to tag
      { "artist",  required_argument, &iLongOpt, 'a' },
      { "album",   required_argument, &iLongOpt, 'A' },
      { "song",    required_argument, &iLongOpt, 't' },
      { "comment", required_argument, &iLongOpt, 'c' },
      { "genre",   required_argument, &iLongOpt, 'g' },
      { "year",    required_argument, &iLongOpt, 'y' },
      { "track",   required_argument, &iLongOpt, 'T' },
      { "AENC",    required_argument, &optFrameID, ID3FID_AUDIOCRYPTO },
      { "APIC",    required_argument, &optFrameID, ID3FID_PICTURE },
      { "COMM",    required_argument, &optFrameID, ID3FID_COMMENT },
    /* COMR too complex */
      { "ENCR",    required_argument, &optFrameID, ID3FID_CRYPTOREG },
      { "EQUA",    required_argument, &optFrameID, ID3FID_EQUALIZATION },
      { "ETCO",    required_argument, &optFrameID, ID3FID_EVENTTIMING },
      { "GEOB",    required_argument, &optFrameID, ID3FID_GENERALOBJECT },
      { "GRID",    required_argument, &optFrameID, ID3FID_GROUPINGREG },
      { "IPLS",    required_argument, &optFrameID, ID3FID_INVOLVEDPEOPLE },
      { "LINK",    required_argument, &optFrameID, ID3FID_LINKEDINFO },
      { "MCDI",    required_argument, &optFrameID, ID3FID_CDID },
      { "MLLT",    required_argument, &optFrameID, ID3FID_MPEGLOOKUP },
      { "OWNE",    required_argument, &optFrameID, ID3FID_OWNERSHIP },
      { "PRIV",    required_argument, &optFrameID, ID3FID_PRIVATE },
      { "PCNT",    required_argument, &optFrameID, ID3FID_PLAYCOUNTER },
      { "POPM",    required_argument, &optFrameID, ID3FID_POPULARIMETER },
      { "POSS",    required_argument, &optFrameID, ID3FID_POSITIONSYNC },
      { "RBUF",    required_argument, &optFrameID, ID3FID_BUFFERSIZE },
      { "RVAD",    required_argument, &optFrameID, ID3FID_VOLUMEADJ },
      { "RVRB",    required_argument, &optFrameID, ID3FID_REVERB },
      { "SYLT",    required_argument, &optFrameID, ID3FID_SYNCEDLYRICS },
      { "SYTC",    required_argument, &optFrameID, ID3FID_SYNCEDTEMPO },
      { "TALB",    required_argument, &optFrameID, ID3FID_ALBUM },
      { "TBPM",    required_argument, &optFrameID, ID3FID_BPM },
      { "TCOM",    required_argument, &optFrameID, ID3FID_COMPOSER },
      { "TCON",    required_argument, &optFrameID, ID3FID_CONTENTTYPE },
      { "TCOP",    required_argument, &optFrameID, ID3FID_COPYRIGHT },
      { "TDAT",    required_argument, &optFrameID, ID3FID_DATE },
      { "TDLY",    required_argument, &optFrameID, ID3FID_PLAYLISTDELAY },
      { "TENC",    required_argument, &optFrameID, ID3FID_ENCODEDBY },
      { "TEXT",    required_argument, &optFrameID, ID3FID_LYRICIST },
      { "TFLT",    required_argument, &optFrameID, ID3FID_FILETYPE },
      { "TIME",    required_argument, &optFrameID, ID3FID_TIME },
      { "TIT1",    required_argument, &optFrameID, ID3FID_CONTENTGROUP },
      { "TIT2",    required_argument, &optFrameID, ID3FID_TITLE },
      { "TIT3",    required_argument, &optFrameID, ID3FID_SUBTITLE },
      { "TKEY",    required_argument, &optFrameID, ID3FID_INITIALKEY },
      { "TLAN",    required_argument, &optFrameID, ID3FID_LANGUAGE },
      { "TLEN",    required_argument, &optFrameID, ID3FID_SONGLEN },
      { "TMED",    required_argument, &optFrameID, ID3FID_MEDIATYPE },
      { "TOAL",    required_argument, &optFrameID, ID3FID_ORIGALBUM },
      { "TOFN",    required_argument, &optFrameID, ID3FID_ORIGFILENAME },
      { "TOLY",    required_argument, &optFrameID, ID3FID_ORIGLYRICIST },
      { "TOPE",    required_argument, &optFrameID, ID3FID_ORIGARTIST },
      { "TORY",    required_argument, &optFrameID, ID3FID_ORIGYEAR },
      { "TOWN",    required_argument, &optFrameID, ID3FID_FILEOWNER },
      { "TPE1",    required_argument, &optFrameID, ID3FID_LEADARTIST },
      { "TPE2",    required_argument, &optFrameID, ID3FID_BAND },
      { "TPE3",    required_argument, &optFrameID, ID3FID_CONDUCTOR },
      { "TPE4",    required_argument, &optFrameID, ID3FID_MIXARTIST },
      { "TPOS",    required_argument, &optFrameID, ID3FID_PARTINSET },
      { "TPUB",    required_argument, &optFrameID, ID3FID_PUBLISHER },
      { "TRCK",    required_argument, &optFrameID, ID3FID_TRACKNUM },
      { "TRDA",    required_argument, &optFrameID, ID3FID_RECORDINGDATES },
      { "TRSN",    required_argument, &optFrameID, ID3FID_NETRADIOSTATION },
      { "TRSO",    required_argument, &optFrameID, ID3FID_NETRADIOOWNER },
      { "TSIZ",    required_argument, &optFrameID, ID3FID_SIZE },
      { "TSRC",    required_argument, &optFrameID, ID3FID_ISRC },
      { "TSSE",    required_argument, &optFrameID, ID3FID_ENCODERSETTINGS },
      { "TXXX",    required_argument, &optFrameID, ID3FID_USERTEXT },
      { "TYER",    required_argument, &optFrameID, ID3FID_YEAR },
      { "UFID",    required_argument, &optFrameID, ID3FID_UNIQUEFILEID },
      { "USER",    required_argument, &optFrameID, ID3FID_TERMSOFUSE },
      { "USLT",    required_argument, &optFrameID, ID3FID_UNSYNCEDLYRICS },
      { "WCOM",    required_argument, &optFrameID, ID3FID_WWWCOMMERCIALINFO },
      { "WCOP",    required_argument, &optFrameID, ID3FID_WWWCOPYRIGHT },
      { "WOAF",    required_argument, &optFrameID, ID3FID_WWWAUDIOFILE },
      { "WOAR",    required_argument, &optFrameID, ID3FID_WWWARTIST },
      { "WOAS",    required_argument, &optFrameID, ID3FID_WWWAUDIOSOURCE },
      { "WORS",    required_argument, &optFrameID, ID3FID_WWWRADIOPAGE },
      { "WPAY",    required_argument, &optFrameID, ID3FID_WWWPAYMENT },
      { "WPUB",    required_argument, &optFrameID, ID3FID_WWWPUBLISHER },
      { "WXXX",    required_argument, &optFrameID, ID3FID_WWWUSER },
      { 0, 0, 0, 0 }
    };
    iOpt = getopt_long (argc, argv, "12hfLvlRdsDCa:A:t:c:g:y:T:",
                        long_options, &option_index);

    if (iOpt == -1  && argCounter == 0)
    {
      PrintUsage(argv[0]);
      exit(0);
    } 
    else if (iOpt == -1)
      break;
    argCounter++;

    if (iOpt == 0) iOpt = iLongOpt;
//    if (iOpt == 0) iOpt = optFrameID;

#ifdef SORT_RUNTIME
    InitGenres();
#endif  // SORT_RUNTIME

    switch (iOpt)
    {
      case 0:
                // TODO
                //frameList[frameCounter].id   = (enum ID3_FrameID)optFrameID;
                //frameList[frameCounter].data = optarg;
                //frameCounter++;
                break;
      case '?': 
      case 'h': PrintUsage(argv[0]);    exit (0);
      case 'f': PrintFrameHelp(argv[0]);exit (0);
      case 'L': PrintGenreList();       exit (0);
      case 'v': PrintVersion(argv[0]);  exit (0);

    // listing / remove / convert -- see list.cpp and convert.cpp
      case 'l': ListTag(argc, argv, optind, 0);    
                                        exit (0);
      case 'R': ListTag(argc, argv, optind, 1);    
                                        exit (0);
      case 'd': DeleteTag(argc, argv, optind, 2);    
                                        exit (0);
      case 's': DeleteTag(argc, argv, optind, 1);    
                                        exit (0);
      case 'D': DeleteTag(argc, argv, optind, 0);    
                                        exit (0);
      case 'C': ConvertTag(argc, argv, optind);    
                                        exit (0);
      case '1':
		UpdFlags = MPEG::File::ID3v1;
		break;
      case '2':
		UpdFlags = MPEG::File::ID3v2;
		break;
    // Tagging stuff 
      case 'a': 
                vals->insert("TPE1", optarg);
                break;
      case 'A': 
	        vals->insert("TALB", optarg);
                break;
      case 't': 
                vals->insert("TIT2", optarg);
                break;
      case 'c': 
                vals->insert("COMM", optarg);
                break;
      case 'g': 
          {
                int genre_id = 255;
                char *genre_str;
                sscanf(optarg, "%d", &genre_id);
                if (genre_id == 255)
                    genre_id = ID3v1::genreIndex(optarg);
                if (genre_id == 255)
                    genre_str = optarg;
                else {
                    sprintf(tmp, "(%d)", genre_id);
                    genre_str = tmp;
                }
                vals->insert("TCON", genre_str);
          }
                break;
      case 'y': 
                vals->insert("TDRC", optarg);
                break;
      case 'T': 
                vals->insert("TRCK", optarg);
                break;
    // other tags
    
      default:
		std::cerr << "This isn't supposed to happen" << std::endl;
                exit(1);
    }
/*    FrameList frames = t->frameList();

    FrameList::ConstIterator iter;
    for(iter = frames.begin(); iter != frames.end(); ++iter)
    {
      Frame *frame = *iter;
      cout << frame->frameID() << "=" << frame->toString() << endl;
    }*/
  }

  // loop thru the files
  if (optind == argc)
  {
	  std::cerr << "No file to work on." << std::endl;
    exit(1);
  }
  
  // set v2
  for (size_t nIndex = optind; (unsigned int) nIndex < argc; nIndex++)
  {
    MPEG::File f(argv[nIndex]);

    ID3v1::Tag *tag1 = f.ID3v1Tag(true); // create if none
    ID3v2::Tag *tag2 = f.ID3v2Tag(true); // create if none

    // id3v1
    Map<String, String>::Iterator iter1;
    String key;

    for(iter1 = vals->begin(); iter1 != vals->end(); ++iter1)
    {
      key = (*iter1).first;
      if (key == "TPE1") {
        tag1->setArtist((*iter1).second);
        tag2->setArtist((*iter1).second);
      } else if (key == "TALB") {
        tag1->setAlbum((*iter1).second);
        tag2->setAlbum((*iter1).second);
      } else if (key == "TIT2") {
        tag1->setTitle((*iter1).second);
        tag2->setTitle((*iter1).second);
      } else if (key == "COMM") { //TODO

        StringList s = split((*iter1).second, 3);
        CommentsFrame *f;

        FrameList oldframes = tag2->frameList("COMM");
	if (oldframes.isEmpty())
	  f = new CommentsFrame();
	else
	  f = dynamic_cast<CommentsFrame*>(oldframes.front());

        f->setDescription(s[0]);
        f->setText(s[1]);
        f->setLanguage(ByteVector(s[2].toCString()));

        tag1->setComment(s[1]);
      } else if (key == "TCON") {
        tag1->setGenre((*iter1).second);
        tag2->setGenre((*iter1).second);
      } else if (key == "TDRC") {
        int year = (*iter1).second.toInt();
        tag1->setYear(year);
        tag2->setYear(year);
      } else if (key == "TRCK") {
        int track = (*iter1).second.toInt();
        tag1->setTrack(track);
        tag2->setTrack(track);
      }
    }

    // TODO: add 'generic' frames. Some code:
    // Frame *f = new TextIdentificationFrame("TRCK");
    // f->setText(optarg);
    // v2tag->addFrame(f);

/*    const FrameList frames = v2tag->frameList();
    FrameList::ConstIterator iter, iter2;

    for(iter = frames.begin(); iter != frames.end(); ++iter)
    {
      Frame *frame = *iter;
      FrameList oldframes = tag2->frameList(frame->frameID());

      if (oldframes.isEmpty())
        tag2->addFrame(frame);
      else {
	Frame *upd = oldframes.front();
	upd->setData(frame->render());
      }
    }*/

    f.save(UpdFlags, false); // don't strip tags that are not included
  }
  return 0;

#ifdef THIS_NEVER_HAPPENS
  // loop thru the files
  if (optind == argc) 
  {
	  std::cerr << "No file to work on." << std::endl;
    exit(1);
  }
  
  for (size_t nIndex = optind; (unsigned int) nIndex < argc; nIndex++)
  {
    ID3_Tag myTag;
    struct stat filestat;
    
    // std::cout << "Tagging " << argv[nIndex] << ": ";

    // fix me - not checking to see if we can link to it
    
    
    if (stat(argv[nIndex], &filestat)) 
    {
      std::cerr << "Couldn't stat file '" << argv[nIndex] << "'\n";
      break;
    }
    
    /* cludgy to check if we have the proper perms */
    fp = fopen(argv[nIndex], "r+");
    if (fp == NULL) { /* file didn't open */
      fprintf(stderr, "fopen: %s: ", argv[nIndex]);
      perror("id3v2");
      continue;
    }
    fclose(fp);

    size_t ret;
    ret = myTag.Link(argv[nIndex] /*, ID3TT_ID3V2 */);

    // loop thru the frames we need to add/modify
    for (ii = 0; ii < frameCounter; ii++) 
    {
      ID3_Frame *myFrame;
      myFrame = new ID3_Frame;
      if (NULL == myFrame)
      {
         std::cout << "\nOut of memory\n" << std::endl;
         exit(1);
      }
      
      myFrame->SetID(frameList[ii].id);
      
      ID3_Frame *pFrame;
      pFrame = myTag.Find(frameList[ii].id);
                          
      switch (frameList[ii].id)
      {
      //  strings
        case ID3FID_ALBUM:
        case ID3FID_BPM:
        case ID3FID_COMPOSER:
        case ID3FID_CONTENTTYPE:
        case ID3FID_COPYRIGHT:
        case ID3FID_DATE:
        case ID3FID_PLAYLISTDELAY:
        case ID3FID_ENCODEDBY:
        case ID3FID_LYRICIST:
        case ID3FID_FILETYPE:
        case ID3FID_TIME:
        case ID3FID_CONTENTGROUP:
        case ID3FID_TITLE:
        case ID3FID_SUBTITLE:
        case ID3FID_INITIALKEY:
        case ID3FID_LANGUAGE:
        case ID3FID_SONGLEN:
        case ID3FID_MEDIATYPE:
        case ID3FID_ORIGALBUM:
        case ID3FID_ORIGFILENAME:
        case ID3FID_ORIGLYRICIST:
        case ID3FID_ORIGARTIST:
        case ID3FID_ORIGYEAR:
        case ID3FID_FILEOWNER:
        case ID3FID_LEADARTIST:
        case ID3FID_BAND:
        case ID3FID_CONDUCTOR:
        case ID3FID_MIXARTIST:
        case ID3FID_PARTINSET:
        case ID3FID_PUBLISHER:
        case ID3FID_RECORDINGDATES:
        case ID3FID_NETRADIOSTATION:
        case ID3FID_NETRADIOOWNER:
        case ID3FID_SIZE:
        case ID3FID_ISRC:
        case ID3FID_ENCODERSETTINGS:
        case ID3FID_YEAR:
        {
          if (pFrame != NULL) 
          {
            myTag.RemoveFrame(pFrame);
          }
          if (strlen(frameList[ii].data) > 0) {
            myFrame->Field(ID3FN_TEXT) = frameList[ii].data;
            myTag.AttachFrame(myFrame);
          }
          break;
        }
        case ID3FID_TRACKNUM:
        {
          // this ought to check if there is a total track number and
          // combine it with the given track number, but it doesn't.
          char *currentTrackNum = NULL;
          char *newTrackNum = NULL;

          if (pFrame != NULL) 
          {
            currentTrackNum = ID3_GetString(pFrame, ID3FN_TEXT);
            if (*currentTrackNum == '/') 
            {
              newTrackNum = (char *)malloc(strlen(currentTrackNum) 
                                   + strlen(frameList[ii].data)); 
              strcpy(newTrackNum, frameList[ii].data);
              strcat(newTrackNum, currentTrackNum);
            }
            else
            {
              myTag.RemoveFrame(pFrame);
            }
          }
          
          myFrame->Field(ID3FN_TEXT) = frameList[ii].data;
          myTag.AttachFrame(myFrame);

          free(newTrackNum);
          break;
        }
        case ID3FID_USERTEXT:
        {
          if (pFrame != NULL) 
          {
            myTag.RemoveFrame(pFrame);
          }

          // split the string at the ':' remember if no : then leave
          // descrip empty
          char *text;
          text = strchr(frameList[ii].data, ':');
          if (text == NULL) 
          {
            myFrame->Field(ID3FN_TEXT) = frameList[ii].data;
          } else {
            *text = '\0';
            text++;
            myFrame->Field(ID3FN_DESCRIPTION) = frameList[ii].data;
            myFrame->Field(ID3FN_TEXT) = text;
          }
          if (strlen(ID3_GetString(myFrame, ID3FN_TEXT)) > 0) {
            myTag.AttachFrame(myFrame);
          }
          
          break;
        }
        case ID3FID_COMMENT:
        case ID3FID_UNSYNCEDLYRICS:
        {
          // split the string at the ':' (ignoring "\:" and splitting into no more than 3 pieces
          // remember if no ':' then leave descrip/lang empty
          char *pieces[3] = { frameList[ii].data, NULL, NULL };
          if (strchr(frameList[ii].data, ':')) {
            size_t read_at = 0, inst_at = 0, piece = 0;
            unsigned int olen = strlen(frameList[ii].data);
            for (read_at = 0; read_at < olen; read_at++, inst_at++) {
              if (frameList[ii].data[read_at] == '\\' && frameList[ii].data[read_at+1] == ':') {
                read_at++;
              }
              else if (frameList[ii].data[read_at] == ':' && piece < 2) {
                frameList[ii].data[read_at] = '\0';
                pieces[++piece] = frameList[ii].data + inst_at + 1;
              }
              frameList[ii].data[inst_at] = frameList[ii].data[read_at];
            }
            frameList[ii].data[inst_at] = '\0';
          }
          // fprintf(stdout, "1: %s, 2: %s, 3: %s\n", pieces[0], pieces[1], pieces[2]);
          
          if (pieces[1] == NULL) {
            myFrame->Field(ID3FN_TEXT) = pieces[0];
          } else if (pieces[2] == NULL) {
            myFrame->Field(ID3FN_DESCRIPTION) = pieces[0];
            myFrame->Field(ID3FN_TEXT) = pieces[1];
          } else {
            myFrame->Field(ID3FN_DESCRIPTION) = pieces[0];
            myFrame->Field(ID3FN_TEXT) = pieces[1];
            myFrame->Field(ID3FN_LANGUAGE) = pieces[2];
          }
          /* debug
          std::cout << ID3_GetString(myFrame, ID3FN_DESCRIPTION) << std::endl
               << ID3_GetString(myFrame, ID3FN_TEXT) << std::endl
               << ID3_GetString(myFrame, ID3FN_LANGUAGE) << std::endl;
          */

          // now try and find a comment/lyrics with the same descript 
          // and lang as what we have
          ID3_Frame *pFirstFrame = NULL;
          do {
            // if pFrame is NULL, either there were no comments/lyrics 
            // to begin with, or we removed them all in the process          
            if (pFrame == NULL) break;
            
            if (pFirstFrame == NULL) 
            {
              pFirstFrame = pFrame;
            }

            char *tmp_desc = ID3_GetString(pFrame, ID3FN_DESCRIPTION);
            char *tmp_my_desc = ID3_GetString(myFrame, ID3FN_DESCRIPTION);
            char *tmp_lang = ID3_GetString(pFrame, ID3FN_LANGUAGE);
            char *tmp_my_lang = ID3_GetString(myFrame, ID3FN_LANGUAGE);
            if ((strcmp(tmp_desc, tmp_my_desc) == 0) &&
                (strcmp(tmp_lang, tmp_my_lang) == 0)) 
            {
              myTag.RemoveFrame(pFrame);
              if (pFrame == pFirstFrame) 
              {
                pFirstFrame = NULL;
              }
            }
            delete [] tmp_desc;
            delete [] tmp_my_desc;
            delete [] tmp_lang;
            delete [] tmp_my_lang;
            
            // get the next frame until it wraps around
          } while ((pFrame = myTag.Find(frameList[ii].id)) != pFirstFrame);
          
          if (strlen(ID3_GetString(myFrame, ID3FN_TEXT)) > 0) {
            myTag.AttachFrame(myFrame);
          }
          
          break;
        }
        case ID3FID_WWWAUDIOFILE:
        case ID3FID_WWWARTIST:
        case ID3FID_WWWAUDIOSOURCE:
        case ID3FID_WWWCOMMERCIALINFO:
        case ID3FID_WWWCOPYRIGHT:
        case ID3FID_WWWPUBLISHER:
        case ID3FID_WWWPAYMENT:
        case ID3FID_WWWRADIOPAGE:
        {
          if (pFrame != NULL) 
          {
            char *sURL = ID3_GetString(pFrame, ID3FN_URL);
            if (strcmp(frameList[ii].data, sURL) == 0)
              myTag.RemoveFrame(pFrame);  
          }

          if (strlen(frameList[ii].data) > 0) {
            myFrame->Field(ID3FN_URL) = frameList[ii].data;
            myTag.AttachFrame(myFrame);
          }
                  
          break;
                                
        }
        case ID3FID_WWWUSER:
        {
          char 
            *sURL = ID3_GetString(myFrame, ID3FN_URL),
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION);
          std::cout << "(" << sDesc << "): " << sURL << std::endl;
          delete [] sURL;
          delete [] sDesc;
          break;
        }
        case ID3FID_INVOLVEDPEOPLE:
        {
          // This isn't the right way to do it---will only get first person
          size_t nItems = myFrame->Field(ID3FN_TEXT).GetNumTextItems();
          for (size_t nIndex = 1; nIndex <= nItems; nIndex++)
          {
            char *sPeople = ID3_GetString(myFrame, ID3FN_TEXT, nIndex);
            std::cout << sPeople;
            delete [] sPeople;
            if (nIndex < nItems)
            { 
              std::cout << ", ";
            }
          }
          std::cout << std::endl;
          break;
        }
        case ID3FID_PICTURE:
        {
          char
            *sMimeType = ID3_GetString(myFrame, ID3FN_MIMETYPE),
            *sDesc     = ID3_GetString(myFrame, ID3FN_DESCRIPTION),
            *sFormat   = ID3_GetString(myFrame, ID3FN_IMAGEFORMAT);
          size_t
            nPicType   = myFrame->Field(ID3FN_PICTURETYPE).Get(),
            nDataSize  = myFrame->Field(ID3FN_DATA).Size();
          std::cout << "(" << sDesc << ")[" << sFormat << ", "
               << nPicType << "]: " << sMimeType << ", " << nDataSize
               << " bytes" << std::endl;
          delete [] sMimeType;
          delete [] sDesc;
          delete [] sFormat;
          break;
        }
        case ID3FID_GENERALOBJECT:
        {
          char 
            *sMimeType = ID3_GetString(myFrame, ID3FN_TEXT), 
            *sDesc = ID3_GetString(myFrame, ID3FN_DESCRIPTION), 
            *sFileName = ID3_GetString(myFrame, ID3FN_FILENAME);
          size_t 
          nDataSize = myFrame->Field(ID3FN_DATA).Size();
          std::cout << "(" << sDesc << ")[" 
              << sFileName << "]: " << sMimeType << ", " << nDataSize
              << " bytes" << std::endl;
          delete [] sMimeType;
          delete [] sDesc;
          delete [] sFileName;
          break;
        }
        case ID3FID_UNIQUEFILEID:
        {
          if (pFrame != NULL) 
          {       
            char *sOwner = ID3_GetString(pFrame, ID3FN_TEXT);
            size_t nDataSize = pFrame->Field(ID3FN_DATA).Size();
            std::cout << sOwner << ", " << nDataSize
                 << " bytes" << std::endl;
            delete [] sOwner;
          }
          break;
        }
        case ID3FID_PLAYCOUNTER:
        {
          if (pFrame != NULL) 
          {
            size_t nCounter = pFrame->Field(ID3FN_COUNTER).Get();
            std::cout << nCounter << std::endl;
          }
          break;
        }
        case ID3FID_POPULARIMETER:
        {
          if (pFrame != NULL)
          {
            char *sEmail = ID3_GetString(pFrame, ID3FN_EMAIL);
            size_t
              nCounter = pFrame->Field(ID3FN_COUNTER).Get(),
              nRating = pFrame->Field(ID3FN_RATING).Get();
            std::cout << sEmail << ", counter=" 
                 << nCounter << " rating=" << nRating;
            delete [] sEmail;
          }
          break;
        }
        case ID3FID_CRYPTOREG:
        case ID3FID_GROUPINGREG:
        {
          char *sOwner = ID3_GetString(myFrame, ID3FN_OWNER);
          size_t 
            nSymbol = myFrame->Field(ID3FN_ID).Get(),
            nDataSize = myFrame->Field(ID3FN_DATA).Size();
          std::cout << "(" << nSymbol << "): " << sOwner
               << ", " << nDataSize << " bytes";
          break;
        }
        case ID3FID_AUDIOCRYPTO:
        case ID3FID_EQUALIZATION:
        case ID3FID_EVENTTIMING:
        case ID3FID_CDID:
        case ID3FID_MPEGLOOKUP:
        case ID3FID_OWNERSHIP:
        case ID3FID_PRIVATE:
        case ID3FID_POSITIONSYNC:
        case ID3FID_BUFFERSIZE:
        case ID3FID_VOLUMEADJ:
        case ID3FID_REVERB:
        case ID3FID_SYNCEDLYRICS:
        case ID3FID_SYNCEDTEMPO:
        case ID3FID_METACRYPTO:
        {
          std::cout << " (unimplemented)" << std::endl;
          break;
        }
        default:
        {
          std::cout << " frame" << std::endl;
          break;
        }
      }
    }  // steping thru frames
   
    luint nTags = myTag.Update(UpdFlags);

    /* update file with old mode */
    if (chmod(argv[nIndex], filestat.st_mode)) 
    {
	    std::cerr << "Couldn't reset permissions on '" << argv[nIndex] << "'\n";
    }    
  }

  return 0;
#endif
}
