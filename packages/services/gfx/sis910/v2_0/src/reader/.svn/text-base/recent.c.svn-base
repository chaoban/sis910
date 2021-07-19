/**
*@recently use array
*@return: index
*/
#include "recent.h"
#include <assert.h>

/**
*@return:index else -1 if error
*/
int initRecent(char *recent[],char * filepath)
{
        int i,num,index;
        char *tmp=(char *)malloc(64*sizeof(char));
        assert(tmp);
        if (tmp==NULL) return;
        char *p;
        FILE *fp=NULL;
        //init
         for (i=0;i<RECENT_NUM;i++)
        {
                recent[i]=NULL;
        }
        printf("recentfile:%s\n",filepath);
        fp=fopen(filepath,"r");
        if (fp==NULL) return -1;
        fscanf(fp,"%d\n", &num);
        fscanf(fp,"%d\n", &index);
        i=0;
        while(num){
                fscanf(fp,"%s\n",tmp);
                if (strstr(tmp,"####")!=0) break;
                recent[i]=(char *)malloc((strlen(tmp)+1)*sizeof(char));
                strcpy(recent[i],tmp);
                num--;
                i++;
            }
        ///
        free(tmp);
        return index;
}

//add to listbox
void addRecentToListBox(int index, const char *recent[],TN_WIDGET *widget)
{   
        int i;
        //int len;
        char *tmp,*p;
        char nm[5];
        if (widget==NULL) return;
        for (i=0;i<RECENT_NUM;i++)
            {
                if (index<0) index+=RECENT_NUM;
                if (recent[index]!=NULL){
                          //EPRINTF(("List:%s\n",recent[index]));
                            int length;
                            length=strlen(recent[index]);
                            p= (char *)recent[index]+length-4;
                            while(*p!='/')  p--;
                            p++;                          
                          tmp=(char *)malloc((strlen(p)+6)*sizeof(char));
                          assert(tmp);
                          if (tmp==NULL) return;
                          sprintf(tmp,"%d %s",i+1,p);
                          tnAddItemToListBox(widget,tmp,0);
                          free(tmp);
                    }
                index--;
            }        
}
////////////////////////////////////////////////////////////
//         >=0,success,the index if aleardy exist
int addToRecent(int index,char *recent[],char * path)
{       
        int i,j;
        for (i=0;i<RECENT_NUM;i++){
                if (recent[i]==NULL) continue;
                if (strcmp(recent[i],path)==0){
		        printf("AddToRecent:already exist\n");
			return index;   //aleardy exist
		    }	
            }
	//the path not exist
        index++;
        if (index==RECENT_NUM)
                index=0;
        if (recent[index]==NULL)
            {
                recent[index]=(char *)malloc((strlen(path)+1)*sizeof(char));
                assert(recent[index]);
                strcpy(recent[index],path);
            }
        else
            {
                recent[index]=(char *)realloc(recent[index],(strlen(path)+1)*sizeof(char));
                assert(recent[index]);
                strcpy(recent[index],path);                    
            }
        return index;
}
/////////////////////////////////////////////////////////
void SaveRecentFile(char * recent[],int index,char * file)
{
        FILE *fp=NULL;
        int i;
        //index <0 then the list is empty, don't save
        if (index<0) return;        
        fp=fopen(file,"w");
        if (fp==NULL) return;
        fprintf(fp,"%d\n",RECENT_NUM);
        //fputc('\n');
        fprintf(fp,"%d\n",index);
        for (i=0;i<RECENT_NUM;i++)
            {
                    if (recent[i]!=NULL)
                        fprintf(fp,"%s\n",recent[i]);
                    else
                        fprintf(fp,"####\n");
            }
        fclose(fp);
}
void freeRecent(char *recent[])
{
        int i;
         for (i=0;i<RECENT_NUM;i++)
        {
                if (recent[i]!=NULL)
                        free(recent[i]);
        }       
}


