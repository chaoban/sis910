/********************************************************
*
 *********************************************************/
#include "common.h"
#define INITIAL_SIZE        4096
#define DIRENT_D_RECLEN 128
#define B_FILE_NAME_LENGTH  500


static char gName[B_FILE_NAME_LENGTH];

static int
myscandir(const char *name, struct dirent ***list, int (*selector)(struct dirent *), int (*sorter)(const void *, const void *))
{
  struct dirent        **names; 
  struct dirent        *entp; 
  DIR          *dirp; 
  int           i; 
  int           size; 
  int		dirent_size = sizeof(struct dirent);

  /* Get initial list space and open directory. */ 
  size = INITIAL_SIZE; 
  names = (struct dirent **)malloc(size * sizeof names[0]); 
  if (names == NULL) 
    return -1; 
  dirp = opendir(name);
  strcpy(gName,name); 
  if (dirp == NULL) 
    return -1; 
  i = 0;
  /* Read entries in the directory. */ 
  while ((entp = readdir(dirp)) != NULL ) 
    if (selector == NULL || (*selector)(entp))
      { 
	/* User wants them all, or he wants this one. */ 
	if (++i >= size)
	  { 
	    size <<= 1; 
	    names = (struct dirent **) 
	      realloc((char *)names, size * sizeof names[0]); 
	    if (names == NULL)
	      { 
		closedir(dirp); 
		return -1; 
	      } 
	  } 

	/* Copy the entry. */ 
	names[i - 1] = (struct dirent *)malloc(sizeof(struct dirent) 
					       + DIRENT_D_RECLEN + 1); //hacked by nifei
	if (names[i - 1] == NULL) { 
	  closedir(dirp); 
	  return -1; 
	}
	memcpy(names[i-1],entp,dirent_size);
	memcpy(names[i - 1]->d_name, entp->d_name,DIRENT_D_RECLEN); 
	names[i-1]->d_name[DIRENT_D_RECLEN] = '\0';
      } 

  /* Close things off. */ 
  names[i] = NULL; 
  *list = names; 
  closedir(dirp); 

  /* Sort? */ 
  if (i && sorter) 
    qsort((char *)names, i, sizeof names[0], sorter); 

  return i;
}
//
int ap_scandir (const char *name, struct dirent ***list, 
		int (*selector)(struct dirent *), 
		int (*sorter)(const void *, const void *))
{   
#ifndef __ECOS
	return scandir(name,list,selector,sorter);
#else
        printf("myscandir is called\n");
	return myscandir(name,list,selector,sorter);
#endif	
}

//only get the file name 
void getFileName(char * name,const char *path)
{
        int length;
        assert(name);
        length=strlen(path);
        //char *p=strstr(path,".txt");
        char *p= (char *)path+length-4;
        if (p==NULL) return;
        while(*p!='/')  p--;
        p++;
        while(*p!='.')   *name++=*p++;
        *name='\0';
}

