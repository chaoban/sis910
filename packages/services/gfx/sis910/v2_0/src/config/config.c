/**
 *\file config.c
 *\brief To provide config.ini parser-related API
 *\author nf
 *\date 2008-04-03
 *
 *$Id: config.c 627 2008-04-03 09:04:06Z  $
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "config.h"

#define myisblank(c)   ((c) == ' ' || (c) == '\t')

static struct configFile *_cfgParseConfigFile(struct configFile *cfg);
static inline int isKeyValSep(char c);
static char *trimTailSpace(char *str);
static inline int isCommentStart(char c);
static void _cfgdbgPrintConfigDatum(char *key, void *data);
static void _cfgdbgPrintConfigSection(char *key, void *section);
static void _cfgdbgSaveConfigDatum(char *key, void *data, FILE * fp);
static void _cfgdbgSaveConfigSection(char *key, void *section, FILE * fp);

#define _GLOBAL_CFG 1
#ifdef _GLOBAL_CFG
static struct configFile *gcfg = NULL;
static struct configFile *gplist = NULL;
#endif
/**
 *\brief  Attempt to read and parse a config file. Return a handle to the config file
 * structure, or NULL if errors were encountered.
 *\author nf
 *\param filename: config file's name
 *\date 2008-04-03
 *\return NULL if failed,or return a point which point a struct which contain all the information about this config file
 */
struct configFile *readConfigFile(char *filename)
{
    /* TODO: Attempt to read from homedir, /etc, etc if file not found in cwd. */
    struct stat filestats;
    struct configFile *cfg;
    int fileHandle, bytesRead;
    FILE *fin;

    if (stat(filename, &filestats) != 0) {
        /*perror ("ReadConfigFile: stat"); */
        return NULL;
    }
    if (filestats.st_size > MAXIMUM_SANE_FILESIZE)
        return NULL;
#ifdef _GLOBAL_CFG
    if(gcfg == NULL)
        gcfg = malloc(sizeof(struct configFile));
    else
        return gcfg;
                
    cfg = gcfg;
#else    
    cfg = malloc(sizeof(struct configFile));
#endif
    
    cfg->bbdg = malloc(filestats.st_size);
    cfg->sections = hashConstructTable(31);

    if ((fin = fopen(filename, "r")) == NULL) {
        printf("can not open %s\n", filename);
        return NULL;
    }

    if ((bytesRead = fread(cfg->bbdg, 1, filestats.st_size, fin))
        != filestats.st_size) {
        printf("read %s error\n", filename);
        return NULL;
    }
    fclose(fin);
    cfg->bbdgSize = filestats.st_size;
    return _cfgParseConfigFile(cfg);
}

struct configFile *readPlayList(char *filename)
{
    /* TODO: Attempt to read from homedir, /etc, etc if file not found in cwd. */
    struct stat filestats;
    struct configFile *cfg;
    int fileHandle, bytesRead;
    FILE *fin;

    if (stat(filename, &filestats) != 0) {
        /*perror ("ReadConfigFile: stat"); */
        return NULL;
    }
    if (filestats.st_size > MAXIMUM_SANE_FILESIZE)
        return NULL;
#ifdef _GLOBAL_CFG
    if(gplist == NULL)
        gplist = malloc(sizeof(struct configFile));
    else
        return gplist;
                
    cfg = gplist;
#else    
    cfg = malloc(sizeof(struct configFile));
#endif
    
    cfg->bbdg = malloc(filestats.st_size);
    cfg->sections = hashConstructTable(31);

    if ((fin = fopen(filename, "r")) == NULL) {
        printf("can not open %s\n", filename);
        return NULL;
    }

    if ((bytesRead = fread(cfg->bbdg, 1, filestats.st_size, fin))
        != filestats.st_size) {
        printf("read %s error\n", filename);
        return NULL;
    }
    fclose(fin);
    cfg->bbdgSize = filestats.st_size;
    return _cfgParseConfigFile(cfg);
}
/**
 *\brief To decide if c is the seperator
 *\author nf
 *\param c 
 *\date 2008-04-03
 *\return 1 if c is the seperator,or return 0
 */
static inline int isKeyValSep(char c)
{
    return (c == ':' || c == '=');
}

/**
 *\brief trim the tail space of str
 *\author nf
 *\param str
 *\date 2008-04-03
 *\return trimmed str
 */
static char *trimTailSpace(char *str)
{
    char *tail = str + strlen(str) - 1;
    while (*tail == ' ' || *tail == '\t' || *tail == '\r') {
        --tail;
    }
    *(tail + 1) = '\0';
    return str;
}

/**
 *\brief To decide if c is comment symbol '#'
 *\author nf
 *\param c
 *\date 2008-04-03
 *\return 1 if c is '#',or return 0
 */
static inline int isCommentStart(char c)
{
    return (c == '#');
}


/**
 *\brief To parser all the content and build the configFile structure 
 *
 * Small FSM. States indicate what the machine is looking for *next*,
 * so eg _cfgKEYSTART means "looking for the token that indicates the start
 * of a key"
 *
 *\author nf
 *\param cfg
 *\date 2008-04-03
 *\return  a pointor which point a struct which contain all the information
 */
static struct configFile *_cfgParseConfigFile(struct configFile *cfg)
{
    char *currentSectionString = "DEFAULT";
    char *currentStringStart = NULL;
    char *currentKey = NULL;
    unsigned int currentElement = 0, filePos = 0, state = _cfgKEYSTART;
    hash_table *tempHash;
    /* Create the default section. */
    tempHash = hashConstructTable(31);
    hashInsert(currentSectionString, tempHash, cfg->sections);
    while (filePos < cfg->bbdgSize) {
        switch (state) {
            case _cfgKEYSTART:
                if (cfg->bbdg[filePos] == '[') {
                    filePos++;
                    currentStringStart = &(cfg->bbdg[filePos]);
                    state = _cfgSECTIONEND;
                    break;
                }
                if (isCommentStart(cfg->bbdg[filePos])) {
                    filePos++;
                    state = _cfgCOMMENTEND;
                    break;
                }
                if (!isspace(cfg->bbdg[filePos])) {
                    currentStringStart = &(cfg->bbdg[filePos]);
                    state = _cfgKEYEND;
                } else {
                    filePos++;
                }
                break;
            case _cfgCOMMENTEND:
                if (cfg->bbdg[filePos] == '\n') {
                    state = _cfgKEYSTART;
                }
                filePos++;
                break;
            case _cfgSECTIONEND:
                if (cfg->bbdg[filePos] == ']') {
                    cfg->bbdg[filePos] = '\0';
                    currentSectionString = currentStringStart;
                    state = _cfgKEYSTART;
                }
                filePos++;
                break;
            case _cfgKEYEND:
                if (isspace(cfg->bbdg[filePos]) || isKeyValSep(cfg->bbdg[filePos])) {
                    if (isKeyValSep(cfg->bbdg[filePos])) {
                        cfg->bbdg[filePos] = '\0';
                    } else {
                        cfg->bbdg[filePos] = '\0';
                        filePos++;
                    }
                    currentKey = currentStringStart;
                    state = _cfgCOLON;
                } else {
                    //Do this in search routine instead (with strcasecmp)
                    //cfg->bbdg[filePos] = tolower(cfg->bbdg[filePos]);
                    filePos++;
                }
                break;
            case _cfgCOLON:
                if (isKeyValSep(cfg->bbdg[filePos]) || cfg->bbdg[filePos] == '\0') {
                    state = _cfgVALSTART;
                }
                filePos++;
                break;
            case _cfgVALSTART:
                if (!myisblank(cfg->bbdg[filePos])) {
                    currentStringStart = &(cfg->bbdg[filePos]);
                    state = _cfgVALEND;
                } else {
                    filePos++;
                }
                break;
            case _cfgVALEND:
                if (cfg->bbdg[filePos] == '\n' || isCommentStart(cfg->bbdg[filePos])) {
                    /* First see if the current section exists. */
                    tempHash = hashLookup(currentSectionString, cfg->sections);
                    if (tempHash == NULL) {
                        tempHash = hashConstructTable(31);
                        hashInsert(currentSectionString, tempHash, cfg->sections);
                    }

                    /* Now stick it in the table. */
                    if (isCommentStart(cfg->bbdg[filePos])) {
                        cfg->bbdg[filePos] = '\0';
                        hashInsert(currentKey, strdup(currentStringStart), tempHash);
                        state = _cfgCOMMENTEND;
                    } else {
                        cfg->bbdg[filePos] = '\0';
                        hashInsert(currentKey, strdup(currentStringStart), tempHash);
                        state = _cfgKEYSTART;
                    }
                }

                filePos++;
                break;
        }

    }
    return cfg;
}


/**
 *\brief set the specified section's specified key's value to the specified value
 *\author nf
 *\param cfg
 *\param section
 *\param key
 *\param value
 *\date 2008-04-03
 *\return NULL if don't exist the key in specified secion,or return the specified value
 */
char *setConfigString(struct configFile *cfg, char *section, char *key, char *value)
{
    struct hash_table *hashSection;
    hashSection = hashLookup(section, cfg->sections);
    if (!hashSection){
        printf("do not exit section %s\n",section);
        return NULL;
    }


    unsigned val = hashHash(key) % hashSection->size;
    bucket *ptr;

    if (NULL == (hashSection->table)[val])
        return NULL;
    for (ptr = (hashSection->table)[val]; NULL != ptr; ptr = ptr->next) {
        if (0 == strcasecmp(key, ptr->key)) {
            if (strlen(value) > strlen(ptr->data)) {
                ptr->data = realloc(ptr->data, strlen(value) + 1);
                if (ptr->data == NULL) {
                    printf("out of memory\n");
                    exit(-1);
                }
            }
            strcpy(ptr->data, value);
            return ptr->data;
        }
    }
}


/**
 *\brief set the specified section's specified key's value to the specified number value
 *\author nf
 *\param cfg
 *\param section
 *\param key
 *\param value
 *\date 2008-04-03
 *\return NULL if don't exist the key in specified secion,or return the specified value(in char * format)
 */
char *setConfigInt(struct configFile *cfg, char *section, char *key, int value)
{
    char buffer[32];
    sprintf(buffer, "%d", value);
    return setConfigString(cfg, section, key, buffer);
}


/**
 *\brief get the specified section's specified key's value
 *\author nf
 *\param cfg
 *\param section
 *\param key
 *\date 2008-04-03
 *\return NULL if don't exist the key in specified secion,or return the specified value
 */
char *getConfigString(struct configFile *cfg, char *section, char *key)
{
    struct hash_table *hashSection;
    char *value;
    hashSection = hashLookup(section, cfg->sections);
    if (!hashSection)
        return NULL;

    value = hashLookup(key, hashSection);
    if (value) {
        trimTailSpace(value);
        return value;
    } else {
        return NULL;
    }
}


/**
 *\brief get the specified section's specified key's value and covert int to number
 *\author nf
 *\param cfg
 *\param section
 *\param key
 *\date 2008-04-03
 *\return NULL if don't exist the key in specified secion,or return the specified value
 */
int getConfigInt(struct configFile *cfg, char *section, char *key)
{
    char *configString;
    if ((configString = getConfigString(cfg, section, key)) == NULL) {
        return 0;
    }
    return atoi(configString);
}


/**
 *\brief a callback fun which is used to output the key-value pair
 *\author nf
 *\param key
 *\param data
 *\date 2008-04-03
 *\return
 */
static void _cfgdbgPrintConfigDatum(char *key, void *data)
{
    printf("\t\t%s=%s\n", key, (void *) data);
}


/**
 *\brief a callback fun which is used to output a specified section
 *\author nf
 *\param key
 *\param section
 *\date 2008-04-03
 *\return
 */
static void _cfgdbgPrintConfigSection(char *key, void *section)
{
//    printf("\tConfig section %s:\n", key);
    hashEnumerate((hash_table *) section, _cfgdbgPrintConfigDatum);
}


/**
 *\brief print all content of configFile
 *\author nf
 *\param cfg
 *\date 2008-04-03
 *\return
 */
void printConfigFile(struct configFile *cfg)
{
    printf("Config file:\n");
    hashEnumerate(cfg->sections, _cfgdbgPrintConfigSection);
}


/**
 *\brief a calback fun which is used to write a key-value pair to a file
 *\author nf
 *\param key
 *\param data
 *\param fp
 *\date 2008-04-03
 *\return
 */
static void _cfgdbgSaveConfigDatum(char *key, void *data, FILE * fp)
{
    fprintf(fp, "%s=%s\n", key, (void *) data);
}


/**
 *\brief a callback fun which is used to write a specified section to a file
 *\author nf
 *\param key
 *\param section
 *\param fp
 *\date 2008-04-03
 *\return
 */
static void _cfgdbgSaveConfigSection(char *key, void *section, FILE * fp)
{
    fprintf(fp, "[%s]\n", key);
    hashEnumerate2((hash_table *) section, _cfgdbgSaveConfigDatum, fp);
    fprintf(fp, "\n");
}


/**
 *\brief save the structure cfg to a file named fileName
 *\author nf
 *\param cfg
 *\param fileName
 *\date 2008-04-03
 *\return
 */
void saveConfigFile(struct configFile *cfg, const char *fileName)
{
    FILE *fp;
    
#ifdef _GLOBAL_CFG
    return;
#endif        
    
    if ((fp = fopen(fileName, "w+")) == NULL) {
        printf("can not open %s to write\n", fileName);
        exit(-1);
        return;
    }
    hashEnumerate2(cfg->sections, _cfgdbgSaveConfigSection, fp);
    fclose(fp);
}

/*\brief This function called by hashFreeTable, itself called from unloadConfigFile.
 * Memory-based data structure of config file is a hash table (config sections)
 * of hash tables (config options). This function frees the config options
 * hashtables as each config section is freed.
 *
 * \author nf
 * \param data
 * \data 2008-04-03
 * \return 
 */
static void unloadConfigSection(void *data)
{
    struct hash_table *sectionTable = data;
    hashFreeTable(sectionTable, free);
}


/**
 *\brief unload the structure cfg and free all the memory space
 *\author nf
 *\param cfg
 *\date 2008-04-03
 *\return
 */
void unloadConfigFile(struct configFile *cfg)
{
#ifdef _GLOBAL_CFG
    return;
#endif
        
    if (!cfg)
        return;
    hashFreeTable(cfg->sections, unloadConfigSection);
    free(cfg->bbdg);
    free(cfg);
}
