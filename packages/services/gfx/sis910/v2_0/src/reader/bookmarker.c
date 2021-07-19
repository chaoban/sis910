#include "bookmarker.h"
#include <assert.h>
/////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////
int initBookMarker(BMarker bm[],char * filepath)
{
        int i;
        char *p,q;
        char *tmp=(char *)malloc(128*sizeof(char));
        assert(tmp);
        int num;
        int index=-1;
        FILE *fp=NULL;
         for (i=0;i<BOOKMARKER_NUM;i++)
        {
                bm[i].text=NULL;
                bm[i].path=NULL;
                bm[i].posi=0;
        }
        //read the file
        if (filepath==NULL) return index;
        fp=fopen(filepath,"r"); 
        if (fp==NULL) return index;
        fgets(tmp,10,fp);
        num=atoi(tmp);
        fgets(tmp,10,fp);
        index=atoi(tmp);        
        i=0;
        printf("---init bookmarker num=%d,index=%d",num,index);
        while (num){
                    fgets(tmp,100,fp);
                    if (strstr(tmp,"####")!=0) break;
                    p=strtok(tmp,"$$");
                    if (p!=NULL){
                            bm[i].text=(char *)malloc((strlen(p)+1)*sizeof(char));
                            assert(bm[i].text);
                            strcpy(bm[i].text,p);
                        }
                    p=strtok(NULL,"$$");
                    if (p!=NULL){
                            bm[i].path=(char *)malloc((strlen(p)+1)*sizeof(char));
                            assert(bm[i].path);
                            strcpy(bm[i].path,p);                
                        }
                    p=strtok(NULL,"$$");
                    if (p!=NULL)
                            bm[i].posi=atoi(p);
                    num--;
                    i++;
                    if (i==BOOKMARKER_NUM) break;

            }
        free(tmp);
        fclose(fp);
        return index;
}

int addToBM( BMarker bm[], int index,const char * text,const char * path, int posi)
{       
        index++;
        if (index==BOOKMARKER_NUM)
                index=0;
        if (bm[index].text==NULL)
            {
                bm[index].text=(char *)malloc((strlen(text)+1)*sizeof(char));
                strcpy(bm[index].text,text);
            }
        else
            {
                bm[index].text=(char *)realloc(bm[index].text,(strlen(text)+1)*sizeof(char));
                strcpy(bm[index].text,text);
            }
        if (bm[index].path==NULL)
            {
                bm[index].path=(char *)malloc((strlen(path)+1)*sizeof(char));
                strcpy(bm[index].path,path);
            }
        else
            {
                bm[index].path=(char *)realloc(bm[index].path,(strlen(path)+1)*sizeof(char));
                strcpy(bm[index].path,path);
            }        
        bm[index].posi=posi;
        return index;
}
//add to listbox
void addBMToListBox(int index,BMarker bm[],TN_WIDGET *widget)
{   
        int i;
        char *tmp;
        char nm[5];
        if (widget==NULL) return;
        for (i=0;i<BOOKMARKER_NUM;i++)
            {
                if (index<0) index+=BOOKMARKER_NUM;
                if (bm[index].text!=NULL){
                          //EPRINTF(("List:%s\n",recent[index]));
                          sprintf(nm,"%d ",i+1);
                          tmp=(char *)malloc((strlen(bm[index].text)+6)*sizeof(char));
                          assert(tmp);
                          strcpy(tmp,nm);
                          strcat(tmp,bm[index].text);
                          tnAddItemToListBox(widget,tmp,0);
                          free(tmp);
                    }
                index--;
            }        
}

void saveBookmarker(BMarker bm[],int index,char * file)
{
        FILE *fp=NULL;
        int i;
        //index <0 then the list is empty, don't save
        if (index<0) return;
        fp=fopen(file,"w");
        if (fp==NULL) return;
        fprintf(fp,"%d\n",BOOKMARKER_NUM);
        //fputc('\n');
        fprintf(fp,"%d\n",index);
        for (i=0;i<BOOKMARKER_NUM;i++)
            {
                    if ((bm[i].text!=NULL)&&(bm[i].path!=NULL))
                        fprintf(fp,"%s$$%s$$%d\n",bm[i].text,bm[i].path,bm[i].posi);
                    else
                        fprintf(fp,"####\n");
            }
        fclose(fp);
}
void freeBM(BMarker bm[])
{
        int i;
         for (i=0;i<BOOKMARKER_NUM;i++)
        {
                if (bm[i].path!=NULL)
                        free(bm[i].path);
                if (bm[i].text!=NULL)
                        free(bm[i].text);     
        }       
}


