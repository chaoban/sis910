/**
* this is a test file for WMA Decoder.
* 1. copy a wma file to /dev/hda/1; the file name in this test is "111.wma"
* 2. copile this test file;
* 3. run this test app on eCos i386 target;
* 4. a "wma2.wav" file will created in /dev/hda/1;
*/

#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <cyg/posix/sys/time.h>
#include <cyg/fileio/fileio.h>
#include <unistd.h>
#include <cyg/io/aud/adecctrl.h>
#include <assert.h>
#include <pkgconf/devs_aud_wma.h>

extern struct AUD_tModule AUD_ModuleWMA;

char	gDstWavFile[256] = {0};
static int decodeWma(char *path);

#define LOG_PRINT 1

int main(void)
{
	char		srcPath[256] = {0};
	//printf(("**** mount hda1 at first *****.\n"));
	mount("/dev/hda/1", "/", "fatfs");
	
#if 0	// for one file test
	strcpy(srcPath, "/a32/01-01.wma");
	strcpy(gDstWavFile, "/wav/a32.wav");
	decodeWma(srcPath);
#else	//for batch file decoder test
	struct stat     statbuf;
	struct dirent   *dirp;
	DIR             *dp;
	int             ret;
	char* 	wmaDir = "/wma";
	char* 	wavDir = "/wav";
	
	char *ptr = srcPath;
	strcpy(ptr, wmaDir);
	dp = opendir(ptr);
	assert(dp);

	while ((dirp = readdir(dp)) != NULL) 
	{
		if (strcmp(dirp->d_name, ".") == 0 ||
			strcmp(dirp->d_name, "..") == 0)
			continue;        /* ignore dot and dot-dot */

		strcpy(srcPath, wmaDir);
		ptr = srcPath + strlen(srcPath);
		*ptr++ = '/';
		*ptr = 0;
		strcpy(ptr, dirp->d_name);   /* append name after slash */
		printf("source wma file is %s\n", srcPath);

		strcpy(gDstWavFile, wavDir);
		ptr = gDstWavFile + strlen(gDstWavFile);
		*ptr++ = '/';
		*ptr = 0;
		strcpy(ptr, dirp->d_name);
		ptr = gDstWavFile + strlen(gDstWavFile) - 3;
		*ptr = 0;
		strcpy(ptr, "wav");
		printf("dest wav file is %s\n", gDstWavFile);
		
		if (stat(srcPath, &statbuf) < 0) /* stat error */
		{
			printf("%s stat error.\n", srcPath);
			continue;
		}

		ptr = srcPath + strlen(srcPath) - 3;
		if(S_ISREG(statbuf.st_mode) && strcasecmp(ptr, "wma") == 0)
		{
			decodeWma(srcPath);
		}
	}
#endif
	
	umount("/");
	
	return 0;
}

int decodeWma(char * path)
{
	int status = -1, tid = 0;
	FILE *wmaFid;
	struct stat st;
	AUD_tCtrlBlk wmaCtrl;

#if LOG_PRINT
	char logPath[256] = "/log/";
	FILE *logFid;
	long timeLen = 0;
	int channels = 0;
	struct timeval tvStart, tvEnd;
	struct timezone tz;
	float avg = 0.00;
	
	strcpy(logPath + strlen(logPath), path + strlen(logPath));
	strcpy(logPath + strlen(logPath) - 3, "log");
	//printf("log file is %s\n", logPath);
	logFid = fopen(logPath, "w+b");
#endif

	printf("*** begin to decode %s. ***\n", path);
	tid = stat(path, &st);	
	wmaFid = fopen(path, "rb");
	ABV_init(&wmaCtrl.Abv, st.st_size, st.st_size);
	ABV_write(wmaFid);
	printf("ABV_init OK\n");

	AUD_ModuleWMA.Init(&wmaCtrl);
	AUD_ModuleWMA.GetAttr(&wmaCtrl);

#if LOG_PRINT
	gettimeofday(&tvStart, &tz); // get the start time of decoder
#endif

	do
	{
		status = AUD_ModuleWMA.Decode(&wmaCtrl);
	}while(!status);

#if LOG_PRINT
	gettimeofday(&tvEnd, &tz); // get the end time of decoder

	timeLen = (tvEnd.tv_sec - tvStart.tv_sec) * 1000000;
	timeLen += tvEnd.tv_usec - tvStart.tv_usec;
	
	fprintf(logFid, "\n\n*********** log start *******************\n");
	fprintf(logFid, "Title: [eCos]Run WMA Decoder get %ld microsecond\n", timeLen);
	//fprintf(logFid, "Description: CPU/MCLK/ECLK:Intel P3, 800M, Mem: 256M\n");
	fprintf(logFid, "Description: CPU/MCLK/ECLK:Intel P4, 2.8G, Mem: 512M\n");
	fprintf(logFid, "\t 1. wma file size: %d\n", st.st_size);
	fprintf(logFid, "\t 2. wma file channels: %d\n", wmaCtrl.Attr.NumCh);
	fprintf(logFid, "\t 3. wma file byte rate: %d\n", wmaCtrl.Attr.ByteRate);
	fprintf(logFid, "\t 4. wma file SampleFreq: %d\n", wmaCtrl.Attr.SampleFreq);
	fprintf(logFid, "\t 5. total frames: %d\n", status);
	avg = timeLen/1000000.0;
	avg = status/avg;
	fprintf(logFid, "\t 6. time used: %ld microsecond, average %.2f fps\n", timeLen, avg);

	fprintf(logFid, "\n -------------------------------------     Environment Description     ------------------------------------\n");
	//fprintf(logFid, "OS: eCos \tCPU: Intel P3 800M \tSystem Mem.: 256MSD \n");
	fprintf(logFid, "OS: eCos \tCPU: Intel P4 2.8G \tSystem Mem.: 512M \n");
	fprintf(logFid, "Driver ver.:\tModel WMA\n");
	
	fprintf(logFid, "*********** log end ********************\n");

	fclose(logFid);
#endif
	
	AUD_ModuleWMA.Cleanup(&wmaCtrl);

	ABV_exit();
	fclose(wmaFid);
	printf("*** decode end ***\n");
}

