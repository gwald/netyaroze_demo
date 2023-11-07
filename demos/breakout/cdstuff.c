
#define DFILE		3

#define VH_ADDR		0x80090000		// where sound images go in memory
#define VB_ADDR		0x800a0000
#define SEQ_ADDR	0x80110000



typedef struct {
	char	fname[64];
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;


static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\GOGO.SEQ;1", (void *)SEQ_ADDR, 0},
};


/********

		in general:

		FILE_INFO dataFiles[NUMBER_DATA_FILES] = {

		{ "\\path\\filename.ext", (void*) HASH_DEFINED_ADDRESS, 0},

													};

*******/



	// this CD-file-access code taken straight from other samples
void DatafileSearch (void)
{
	int i, j;

	for (i = 0; i < DFILE; i++) 
		{	   
		for (j = 0; j < 10; j++) 
			{	
			if (CdSearchFile(&(dfile[i].finfo), dfile[i].fname) != 0)
				break;		
			else
				printf("%s not found.\n", dfile[i].fname);
			}
		}
}




   
void DatafileRead (void)
{
	int i, j;
	int cnt;

	for (i = 0; i < DFILE; i++) 
		{		
		for (j = 0; j < 10; j++) 
			{	
			CdReadFile(dfile[i].fname, dfile[i].addr,dfile[i].finfo.size);

			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	

			if (cnt == 0)
				break;	
			}
		}
}


