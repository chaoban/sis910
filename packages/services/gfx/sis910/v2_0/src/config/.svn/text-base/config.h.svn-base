#ifndef _CONFIG_H
#define _CONFIG_H

#define MAXIMUM_SANE_FILESIZE (64 * 1024)
#define _cfgMAX_JUMPBLOCK_ELEMENTS 128

#include "hash.h"

#ifdef __ECOS
    #define CFG_PATH "/romdisk/resource/config.ini"
#else
	#define CFG_PATH "/home/wyk/qj024/resource/config.ini"
#endif	//end of __ECOS




enum _cfgFinding {_cfgSECTIONEND, _cfgCOMMENTEND, _cfgKEYSTART, _cfgKEYEND,
		  _cfgCOLON, _cfgVALSTART, _cfgVALEND
}
;


struct configFile
{

    hash_table *sections;

    unsigned char *bbdg;
    /* The Big Block of Delicious Goo */
    size_t bbdgSize;
    
}
;


struct configFile *readConfigFile (char *filename);
struct configFile *readPlayList(char *filename);

//struct configFile *_cfgParseConfigFile (struct configFile *cfg);

//struct _cfgjumpBlock *_cfgNewJumpBlock (struct _cfgjumpBlock *current);


int getConfigInt (struct configFile *cfg, char *section, char *key);

char *getConfigString (struct configFile *cfg, char *section, char *key);

//int getConfigDouble (struct configFile *cfg, char *section, char *key);


char *setConfigString(struct configFile *cfg, char *section, char *key, char *value);

char *setConfigInt(struct configFile *cfg, char *section, char *key, int value);

//char *setConfigDouble(struct configFile *cfg, char *section, char *key,  double value);

void saveConfigFile(struct configFile *cfg, const char* fileName);

void printConfigFile(struct configFile *cfg);
void unloadConfigFile (struct configFile *cfg);
#endif
