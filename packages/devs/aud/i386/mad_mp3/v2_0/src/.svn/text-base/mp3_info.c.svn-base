#include "mp3_info.h"

#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif

void RemoveEndingSpaces(char* buff)
{
    int i = 0;
	int size = strlen(buff);
	for( i = size - 1; i >= 0; i--) 
	{
		if(buff[i] == ' ')
		{
			buff[i] = 0;
		}
		else
		{
			return;
		}

	}

}
const char *sGenres [] =
	{
		"Blues",
		"Classic Rock",
		"Country",
		"Dance",
		"Disco",
		"Funk",
		"Grunge",
		"Hip-Hop",
		"Jazz",
		"Metal",
		"New Age",
		"Oldies",
		"Other",
		"Pop",
		"R&B",
		"Rap",
		"Reggae",
		"Rock",
		"Techno",
		"Industrial",
		"Alternative",
		"Ska",
		"Death Metal",
		"Pranks",
		"Soundtrack",
		"Euro-Techno",
		"Ambient",
		"Trip Hop",
		"Vocal",
		"Jazz+Funk",
		"Fusion",
		"Trance",
		"Classical",
		"Instrumental",
		"Acid",
		"House",
		"Game",
		"Sound Clip",
		"Gospel",
		"Noise",
		"Alternative Rock",
		"Bass",
		"Soul",
		"Punk",
		"Space",
		"Meditative",
		"Instrumental Pop",
		"Instrumental Rock",
		"Ethnic",
		"Gothic",
		"Darkwave",
		"Techno-Industrial",
		"Electronic",
		"Pop-Folk",
		"Eurodance",
		"Dream",
		"Southern Rock",
		"Comedy",
		"Cult",
		"Gangsta Rap",
		"Top 40",
		"Christian Rap",
		"Pop/Punk",
		"Jungle",
		"Native American",
		"Cabaret",
		"New Wave",
		"Phychedelic",
		"Rave",
		"Showtunes",
		"Trailer",
		"Lo-Fi",
		"Tribal",
		"Acid Punk",
		"Acid Jazz",
		"Polka",
		"Retro",
		"Musical",
		"Rock & Roll",
		"Hard Rock",
		"Folk",
		"Folk/Rock",
		"National Folk",
		"Swing",
		"Fast-Fusion",
		"Bebob",
		"Latin",
		"Revival",
		"Celtic",
		"Blue Grass",
		"Avantegarde",
		"Gothic Rock",
		"Progressive Rock",
		"Psychedelic Rock",
		"Symphonic Rock",
		"Slow Rock",
		"Big Band",
		"Chorus",
		"Easy Listening",
		"Acoustic",
		"Humour",
		"Speech",
		"Chanson",
		"Opera",
		"Chamber Music",
		"Sonata",
		"Symphony",
		"Booty Bass",
		"Primus",
		"Porn Groove",
		"Satire",
		"Slow Jam",
		"Club",
		"Tango",
		"Samba",
		"Folklore",
		"Ballad",
		"power Ballad",
		"Rhythmic Soul",
		"Freestyle",
		"Duet",
		"Punk Rock",
		"Drum Solo",
		"A Capella",
		"Euro-House",
		"Dance Hall",
		"Goa",
		"Drum & Bass",
		"Club-House",
		"Hardcore",
		"Terror",
		"indie",
		"Brit Pop",
		"Negerpunk",
		"Polsk Punk",
		"Beat",
		"Christian Gangsta Rap",
		"Heavy Metal",
		"Black Metal",
		"Crossover",
		"Comteporary Christian",
		"Christian Rock",
		"Merengue",
		"Salsa",
		"Trash Metal",
		"Anime",
		"JPop",
		"Synth Pop"
	};

bool LoadID3(AUD_tCtrlBlk *pAcb,int id3Version)
{
// load ID3 data into id3 structure
    AUD_tAttr *pAttr = &(pAcb->Attr);
	
	switch(id3Version) 
	{

    	case ID3_VERSION1:
        {
            if(pAttr->Album) 
            {
            	free(pAttr->Album);
                pAttr->Album = 0;
            }

        	pAttr->Album = (char*) malloc(31);
			*pAttr->Album = 0;

             if(pAttr->Artist) 
             {
             	free(pAttr->Artist);
                pAttr->Artist = 0;
             }

			pAttr->Artist = (char*) malloc(31);
			*pAttr->Artist = 0;

            if(pAttr->Comment)
            {
            	free(pAttr->Comment);
                pAttr->Comment = 0;
            }

			pAttr->Comment = (char*) malloc(31);
			*pAttr->Comment = 0;

             if(pAttr->TrackTitle)
             {
             	free(pAttr->TrackTitle);
                pAttr->TrackTitle = 0;
             }

			pAttr->TrackTitle = (char*) malloc(31);
			*pAttr->TrackTitle = 0;

            if(pAttr->Year) 
            {
            	free(pAttr->Year);
                pAttr->Year = 0;
            }

			pAttr->Year = (char*) malloc(5);
			*pAttr->Year = 0;

            if(pAttr->TrackNum)
            {
            	free(pAttr->TrackNum);
                pAttr->TrackNum = 0;
            }

            pAttr->TrackNum = (char*) malloc(4);
			*pAttr->TrackNum = 0;

			if(pAttr->Genre) 
			{
            	free(pAttr->Genre);
                pAttr->Genre = 0;
            }

            pAttr->Genre = (char*) malloc(1);
			*pAttr->Genre = 0;

	
            if(!pAttr->c_ID3v1) 
            {  	
                EPRINTF(("WMp3::LoadID3->There is no ID3v1 tag")); 
				return 0;
            }
 
			
			memcpy(pAttr->TrackTitle, pAcb->Abv.end - 124 ,30);
			pAttr->TrackTitle[30] = 0;
			RemoveEndingSpaces(pAttr->TrackTitle);

			memcpy(pAttr->Artist,pAcb->Abv.end - 94, 30);
			pAttr->Artist[30] = 0;
			RemoveEndingSpaces(pAttr->Artist);

			memcpy(pAttr->Album,pAcb->Abv.end - 64, 30);
			pAttr->Album[30] = 0;
			RemoveEndingSpaces(pAttr->Album);

			memcpy(pAttr->Year,pAcb->Abv.end - 34, 4);
			pAttr->Year[4] = 0;
			RemoveEndingSpaces(pAttr->Year);

			memcpy(pAttr->Comment,pAcb->Abv.end - 30, 30);
			pAttr->Comment[30] = 0;

			memcpy(pAttr->Genre, pAcb->Abv.end,1);

            if( (unsigned char) pAttr->Genre[0] > 146) 
            {
            	*pAttr->Genre = 0;
			}
            else 
            {
				unsigned char tmp = pAttr->Genre[0]; 
				free(pAttr->Genre);
				if(tmp > 147) tmp = 0;
				pAttr->Genre = (char*) malloc(strlen(sGenres[(char) tmp]) + 1);
				strcpy(pAttr->Genre,sGenres[(char) tmp]); 
			}

            char tracknum = 0;
            if( pAttr->Comment[28] == 0 )
            {
                tracknum = pAttr->Comment[29];
            }
          	sprintf(pAttr->TrackNum, "%u", tracknum);
	
	
			
        }
        return 1;


//        case ID3_VERSION2:
//        {
//
//			if(!mp3->c_ID3v2) 
//			{
//				mp3->error("WMp3::LoadID3->There is no ID3v2 tag");
//				return FALSE;
//			}
//
//        	char	tmpHeader[10];
//    		tmpHeader[0] = 0;
//			DWORD dwRead;
//
//		
//			memcpy( mp3->w_myID3.Id3v2.Header,mp3->c_stream_start, 10);
//
//
//			// check if correct header
//			if(memcmp( mp3->w_myID3.Id3v2.Header, "ID3", 3) != 0) {
//            	mp3->error("WMp3::LoadID3->There is no ID3v2 tag");
//			
//				return FALSE;
//            }
//
//
//            // read flag field
//            mp3->w_myID3.Id3v2._flags			= ( mp3->w_myID3.Id3v2.Header[5] & 255);
//            mp3->w_myID3.Id3v2.Flags.Unsync = ExtractBits ( mp3->w_myID3.Id3v2._flags , 7, 1);
//            mp3->w_myID3.Id3v2.Flags.Extended = ExtractBits ( mp3->w_myID3.Id3v2._flags , 6, 1);
//            mp3->w_myID3.Id3v2.Flags.Experimental = ExtractBits ( mp3->w_myID3.Id3v2._flags , 5, 1);
//            mp3->w_myID3.Id3v2.Flags.Footer = ExtractBits ( mp3->w_myID3.Id3v2._flags , 4, 1);
//
//            // read size of tag
//			mp3->w_myID3.Id3v2.Size			= GetFourByteSyncSafe( mp3->w_myID3.Id3v2.Header[6], mp3->w_myID3.Id3v2.Header[7], mp3->w_myID3.Id3v2.Header[8], mp3->w_myID3.Id3v2.Header[9]);
//
//
//            if(mp3->w_myID3.album) free(mp3->w_myID3.album);
//			mp3->w_myID3.album = 0;
//            mp3->w_myID3.album		= _RetrField(mp3, "TALB", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.album, dwRead);
//
//
//            if(mp3->w_myID3.artist) free( mp3->w_myID3.artist);
//			mp3->w_myID3.artist = 0;
//			mp3->w_myID3.artist		= _RetrField(mp3, "TPE1", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.artist, dwRead);
//
//
//            if(mp3->w_myID3.comment) free(mp3->w_myID3.comment);
//			mp3->w_myID3.comment = 0;
//		   	mp3->w_myID3.comment	= _RetrField(mp3, "COMM", &dwRead);
//            DWORD newsize = _RemoveLeadingNULL( mp3->w_myID3.comment, dwRead);
//
//			if(newsize < 2) {
//				free(mp3->w_myID3.comment);
//				mp3->w_myID3.comment = 0;
//				mp3->w_myID3.comment = (char*) malloc(2);
//				strcpy(mp3->w_myID3.comment,"");
//			}
//			else {
//				char* tmp = ( char* ) memchr(mp3->w_myID3.comment , '\0', newsize - 1);
//				if(tmp) {
//            		strcpy(mp3->w_myID3.comment, tmp +1);
//				}
//			}
//
//
//            if(mp3->w_myID3.genre) free(mp3->w_myID3.genre);
//			mp3->w_myID3.genre = 0;
//		   	mp3->w_myID3.genre		= _RetrField(mp3, "TCON", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.genre, dwRead);
//
//
//            if(mp3->w_myID3.title) free(mp3->w_myID3.title);
//			mp3->w_myID3.title = 0;
//		   	mp3->w_myID3.title		= _RetrField(mp3, "TIT2", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.title, dwRead);
//
//            if(mp3->w_myID3.tracknum) free( mp3->w_myID3.tracknum);
//			mp3->w_myID3.tracknum = 0;
//		   	mp3->w_myID3.tracknum	= _RetrField(mp3, "TRCK", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.tracknum, dwRead);
//
//            if(mp3->w_myID3.year) free(mp3->w_myID3.year);
//			mp3->w_myID3.year = 0;
//		   	mp3->w_myID3.year		= _RetrField(mp3, "TYER", &dwRead);
//            _RemoveLeadingNULL( mp3->w_myID3.year, dwRead);
//
//			
//        }
//        return TRUE;


    }

    return 0;
	
}

char* GetArtist(AUD_tCtrlBlk *pAcb)
{
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->Artist;
}

char* GetTrackTitle(AUD_tCtrlBlk *pAcb)
{
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->TrackTitle;
}

char* GetTrackNum(AUD_tCtrlBlk *pAcb)
{ 
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->TrackNum;
}

char* GetAlbum(AUD_tCtrlBlk *pAcb)
{ 
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->Album;
}
char* GetYear(AUD_tCtrlBlk *pAcb)
{
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->Year;
}
char* GetComment(AUD_tCtrlBlk *pAcb)
{ 
    AUD_tAttr *pAttr = &(pAcb->Attr);
    return pAttr->Comment;
}

char* GetGenre(AUD_tCtrlBlk *pAcb)
{ 
	AUD_tAttr *pAttr = &(pAcb->Attr);
	if(pAttr->Genre)
	{
		return pAttr->Genre;
	}
	else
	{
		return (char*) "\0";
	}
}

bool DisplayID3(AUD_tCtrlBlk *pAcb,int id3Version)
{
    AUD_tAttr *pAttr = &(pAcb->Attr);
    switch(id3Version) 
	{

    	case ID3_VERSION1:
        	diag_printf("ID3 VERSION 1\r\n");
            diag_printf("Title: %s\r\n", pAttr->TrackTitle);
            diag_printf("Artist: %s\r\n", pAttr->Artist);
            diag_printf("Album: %s\r\n", pAttr->Album);
            diag_printf("Year: %s\r\n", pAttr->Year);
            diag_printf("Comment: %s\r\n", pAttr->Comment);
            diag_printf("Genre: %s\r\n", pAttr->Genre);
			diag_printf("Track: %s\r\n\r\n", pAttr->TrackNum);
    } 
}
