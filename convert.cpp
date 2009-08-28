
#include <cstdio>
#include <iostream>
#include <id3/tag.h>
#include <cstdlib>



ID3_FrameID GetFrameID(char * abr)
{
 if (strlen(abr) != 4){
   std::cerr << "Frame ID must have length 4" << std::endl; 
   return ID3FID_NOFRAME;
 }
 if (strncmp("AENC",abr,4) == 0) return ID3FID_AUDIOCRYPTO;
 if (strncmp("APIC",abr,4) == 0) return ID3FID_PICTURE;
 if (strncmp("COMM",abr,4) == 0) return ID3FID_COMMENT;
 if (strncmp("COMR",abr,4) == 0) return ID3FID_COMMERCIAL;
 if (strncmp("ENCR",abr,4) == 0) return ID3FID_CRYPTOREG;
 if (strncmp("EQUA",abr,4) == 0) return ID3FID_EQUALIZATION;
 if (strncmp("ETCO",abr,4) == 0) return ID3FID_EVENTTIMING;
 if (strncmp("GEOB",abr,4) == 0) return ID3FID_GENERALOBJECT;
 if (strncmp("GRID",abr,4) == 0) return ID3FID_GROUPINGREG;
 if (strncmp("IPLS",abr,4) == 0) return ID3FID_INVOLVEDPEOPLE;
 if (strncmp("LINK",abr,4) == 0) return ID3FID_LINKEDINFO;
 if (strncmp("MCDI",abr,4) == 0) return ID3FID_CDID;
 if (strncmp("MLLT",abr,4) == 0) return ID3FID_MPEGLOOKUP;
 if (strncmp("OWNE",abr,4) == 0) return ID3FID_OWNERSHIP;
 if (strncmp("PRIV",abr,4) == 0) return ID3FID_PRIVATE;
 if (strncmp("PCNT",abr,4) == 0) return ID3FID_PLAYCOUNTER;
 if (strncmp("POPM",abr,4) == 0) return ID3FID_POPULARIMETER;
 if (strncmp("POSS",abr,4) == 0) return ID3FID_POSITIONSYNC;
 if (strncmp("RBUF",abr,4) == 0) return ID3FID_BUFFERSIZE;
 if (strncmp("RVAD",abr,4) == 0) return ID3FID_VOLUMEADJ;
 if (strncmp("RVRB",abr,4) == 0) return ID3FID_REVERB;
 if (strncmp("SYLT",abr,4) == 0) return ID3FID_SYNCEDLYRICS;
 if (strncmp("SYTC",abr,4) == 0) return ID3FID_SYNCEDTEMPO;
 if (strncmp("TALB",abr,4) == 0) return ID3FID_ALBUM;
 if (strncmp("TBPM",abr,4) == 0) return ID3FID_BPM;
 if (strncmp("TCOM",abr,4) == 0) return ID3FID_COMPOSER;
 if (strncmp("TCON",abr,4) == 0) return ID3FID_CONTENTTYPE;
 if (strncmp("TCOP",abr,4) == 0) return ID3FID_COPYRIGHT;
 if (strncmp("TDAT",abr,4) == 0) return ID3FID_DATE;
 if (strncmp("TDLY",abr,4) == 0) return ID3FID_PLAYLISTDELAY;
 if (strncmp("TENC",abr,4) == 0) return ID3FID_ENCODEDBY;
 if (strncmp("TEXT",abr,4) == 0) return ID3FID_LYRICIST;
 if (strncmp("TFLT",abr,4) == 0) return ID3FID_FILETYPE;
 if (strncmp("TIME",abr,4) == 0) return ID3FID_TIME;
 if (strncmp("TIT1",abr,4) == 0) return ID3FID_CONTENTGROUP;
 if (strncmp("TIT2",abr,4) == 0) return ID3FID_TITLE;
 if (strncmp("TIT3",abr,4) == 0) return ID3FID_SUBTITLE;
 if (strncmp("TKEY",abr,4) == 0) return ID3FID_INITIALKEY;
 if (strncmp("TLAN",abr,4) == 0) return ID3FID_LANGUAGE;
 if (strncmp("TLEN",abr,4) == 0) return ID3FID_SONGLEN;
 if (strncmp("TMED",abr,4) == 0) return ID3FID_MEDIATYPE;
 if (strncmp("TOAL",abr,4) == 0) return ID3FID_ORIGALBUM;
 if (strncmp("TOFN",abr,4) == 0) return ID3FID_ORIGFILENAME;
 if (strncmp("TOLY",abr,4) == 0) return ID3FID_ORIGLYRICIST;
 if (strncmp("TOPE",abr,4) == 0) return ID3FID_ORIGARTIST;
 if (strncmp("TORY",abr,4) == 0) return ID3FID_ORIGYEAR;
 if (strncmp("TOWN",abr,4) == 0) return ID3FID_FILEOWNER;
 if (strncmp("TPE1",abr,4) == 0) return ID3FID_LEADARTIST;
 if (strncmp("TPE2",abr,4) == 0) return ID3FID_BAND;
 if (strncmp("TPE3",abr,4) == 0) return ID3FID_CONDUCTOR;
 if (strncmp("TPE4",abr,4) == 0) return ID3FID_MIXARTIST;
 if (strncmp("TPOS",abr,4) == 0) return ID3FID_PARTINSET;
 if (strncmp("TPUB",abr,4) == 0) return ID3FID_PUBLISHER;
 if (strncmp("TRCK",abr,4) == 0) return ID3FID_TRACKNUM;
 if (strncmp("TRDA",abr,4) == 0) return ID3FID_RECORDINGDATES;
 if (strncmp("TRSN",abr,4) == 0) return ID3FID_NETRADIOSTATION;
 if (strncmp("TRSO",abr,4) == 0) return ID3FID_NETRADIOOWNER;
 if (strncmp("TSIZ",abr,4) == 0) return ID3FID_SIZE;
 if (strncmp("TSRC",abr,4) == 0) return ID3FID_ISRC;
 if (strncmp("TSSE",abr,4) == 0) return ID3FID_ENCODERSETTINGS;
 if (strncmp("TXXX",abr,4) == 0) return ID3FID_USERTEXT;
 if (strncmp("TYER",abr,4) == 0) return ID3FID_YEAR;
 if (strncmp("UFID",abr,4) == 0) return ID3FID_UNIQUEFILEID;
 if (strncmp("USER",abr,4) == 0) return ID3FID_TERMSOFUSE;
 if (strncmp("USLT",abr,4) == 0) return ID3FID_UNSYNCEDLYRICS;
 if (strncmp("WCOM",abr,4) == 0) return ID3FID_WWWCOMMERCIALINFO;
 if (strncmp("WCOP",abr,4) == 0) return ID3FID_WWWCOPYRIGHT;
 if (strncmp("WOAF",abr,4) == 0) return ID3FID_WWWAUDIOFILE;
 if (strncmp("WOAR",abr,4) == 0) return ID3FID_WWWARTIST;
 if (strncmp("WOAS",abr,4) == 0) return ID3FID_WWWAUDIOSOURCE;
 if (strncmp("WORS",abr,4) == 0) return ID3FID_WWWRADIOPAGE;
 if (strncmp("WPAY",abr,4) == 0) return ID3FID_WWWPAYMENT;
 if (strncmp("WPUB",abr,4) == 0) return ID3FID_WWWPUBLISHER;
 if (strncmp("WXXX",abr,4) == 0) return ID3FID_WWWUSER;
 std::cerr << "Frame ID " << abr << " not detected" << std::endl; 
 return ID3FID_NOFRAME;
}

void DeleteTag(int argc, char *argv[], int optind, int whichTags)
{

    FILE * fp;
    
    for (int nIndex = optind; nIndex < argc; nIndex++)
    {
      /* cludgy to check if we have the proper perms */
      fp = fopen(argv[nIndex], "r+");
      if (fp == NULL) { /* file didn't open */
        fprintf(stderr, "fopen: %s: ", argv[nIndex]);
        perror("id3v2");
        continue;
      }
      fclose(fp);

      ID3_Tag myTag;

      std::cout << "Stripping id3 tag in \"";
      std::cout << argv[nIndex] << "\"...";

      myTag.Clear();
      myTag.Link(argv[nIndex], ID3TT_ALL);

      luint nTags;
      switch(whichTags) 
      {
        case 1: 
          nTags = myTag.Strip(ID3TT_ID3V1);
          std::cout << "id3v1 ";
          break;
        case 2:
          nTags = myTag.Strip(ID3TT_ID3V2);
          std::cout << "id3v2 ";
          break; 
        case 0:
        default:
          nTags = myTag.Strip(ID3TT_ID3);
          std::cout << "id3v1 and v2 ";
      }

      std::cout << "stripped." << std::endl;
    }

  return;
}


void ConvertTag(int argc, char *argv[], int optind)
{
    for (int nIndex = optind; nIndex < argc; nIndex++)
    {
      ID3_Tag myTag;

      std::cout << "Converting id3v1 tag to id3v2 in ";
      std::cout << argv[nIndex] << "...";

      myTag.Clear();
      myTag.Link(argv[nIndex], ID3TT_ALL);

      luint nTags;

      nTags = myTag.Update(ID3TT_ID3V2);
      std::cout << " converted ";
      std::cout << std::endl;
    }

  return;
}

void DeleteFrame(int argc, char *argv[], int optind, char * frame)
{
    for (int nIndex = optind; nIndex < argc; nIndex++)
    {
      ID3_Tag myTag;

      std::cout << "Deleting Frame " << frame << " in file ";
      std::cout << argv[nIndex] << " ..." << std::endl;

      myTag.Clear();
      myTag.Link(argv[nIndex], ID3TT_ALL);

      ID3_FrameID fid = GetFrameID(frame);
      ID3_Frame * f = myTag.Find(fid);
      
      while (f != NULL){
        ID3_Frame * todel =  myTag.RemoveFrame(f);
        delete todel;
        f = myTag.Find(fid);
      }

      luint nTags;
      
      if (myTag.HasChanged())
        nTags = myTag.Update(ID3TT_ID3V2);
    }

  return;
}


