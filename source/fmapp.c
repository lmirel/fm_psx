#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

//#define _FPS

#include <io/pad.h>
#include <osk_input.h>
#define SUCCESS 	1
#define FAILED	 	0

#include <tiny3d.h>
#include <libfont.h>

// font 0: 224 chr from 32 to 255, 16 x 32 pix 2 bit depth
#include "font.h"

// font 1: 224 chr from 32 to 255, 16 x 32 pix 2 bit depth
#include "font_b.h"

// font 2: 255 chr from 0 to 254, 8 x 8 pix 1 bit depth
extern unsigned char msx[];

#include "console.h"

#include "ff.h"
#include "fflib.h"
#include "fm.h"
#include "util.h"
#include "fsutil.h"
#include "pad.h"

#include "types.h"

#include "ver.h"

typedef struct {
    int device;
    void *dirStruct;
} DIR_ITER;

#include "iosupport.h"
#include "storage.h"
#include <malloc.h>
#include <sys/file.h>
#include <lv2/mutex.h> 
#include <sys/errno.h>

#include <sys/file.h>
#include <ppu-lv2.h>
#include <sys/stat.h>
#include <lv2/sysfs.h>

#include <sysutil/disc.h>
#include <sysutil/sysutil.h>
#include <sysmodule/sysmodule.h>
//app
int fmapp_init (int dt);      //1
//int app_input (int dt);     //2
int fmapp_update (int dt);    //3
int fmapp_render (int dt);    //4
int fmapp_cleanup (int dt);   //5
//
struct fm_panel lp, rp;
//
char fn[256] = {0};
int fddr[8] = {0};
//char fdld[8][256];
char wn[256] = {0};
char dn[256] = {0};
int ffd = -1;
#if 0
static u64 ff_ps3id[8] = {
	0x010300000000000AULL, 0x010300000000000BULL, 0x010300000000000CULL, 0x010300000000000DULL,
	0x010300000000000EULL, 0x010300000000000FULL, 0x010300000000001FULL, 0x0103000000000020ULL 
	};
#if 0
static int dev_fd[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
#include "storage.h"
int sdopen (int fd)
{
	static device_info_t disc_info;
	disc_info.unknown03 = 0x12345678; // hack for Iris Manager Disc Less
	disc_info.sector_size = 0;
	//int ret = sys_storage_get_device_info(ff_ps3id[fd], &disc_info);
    int ret = sys_storage_open(ff_ps3id[fd], &dev_fd[fd]);
    if (0 == ret)
        sys_storage_close(dev_fd[fd]);
    return ret;
}
#else
//tools
FRESULT scan_files2 (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    FDIR dir;
    UINT i = 0;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) 
    {
        for (;;) 
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
                break;  /* Break on error or end of dir */
            i++;
            if (i > 7)
                break;
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                snprintf (fdld[i], 255, "/%s", fno.fname);
                //sprintf(&path[i], "/%s", fno.fname);
                //res = scan_files(path);                    /* Enter the directory */
                //if (res != FR_OK) break;
                //path[i] = 0;
            } 
            else 
            {                                       /* It is a file. */
                snprintf (fdld[i], 255, "%s", fno.fname);
                //printf("%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    return res;
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    FDIR dir;
    static FILINFO fno;

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK)
    {
        //NPrintf("Listing path contents for '%s'\n", path);
        for (;;)
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
            {
                f_closedir(&dir);
                return res;  /* Break on error or end of dir */
            }
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                //NPrintf("/%s\n", fno.fname);
                snprintf (dn, 255, "%s%s", path, fno.fname);
                snprintf (wn, 255, "%s%s/fatfs.tst", path, fno.fname);
                //
                fm_panel_add (&lp, fno.fname, 1, 0);
            } 
            else 
            {                                       /* It is a file. */
                //NPrintf("%s\n", fno.fname);
                snprintf (fn, 255, "%s%s", path, fno.fname);
                fm_panel_add (&lp, fno.fname, 0, 0);
            }
        }
        f_closedir(&dir);
    }
    else
    {
        ;//NPrintf("!unable to open path '%s' result %d\n", path, res);
    }
    return res;
}

int sdopen2 (int i)
{
    FDIR fdir;
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    snprintf(lbuf, 10, "%d:/", i);
    int ret = f_mount(&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
    {
        return ret;
    }
    ret = f_opendir (&fdir, lbuf);
    NPrintf("%d f_opendir %s drive %d ssize %d csize %d\n", ret, lbuf, i, fs.ssize, fs.csize);
    if (ret == FR_OK)
        f_closedir (&fdir);
    f_mount(0, lbuf, 0);                    /* Mount the default drive */
    //
    return ret;
}
#endif
//
#endif

#if 0
fdisk -l /dev/sdb1
Disk /dev/sdb1: 223.6 GiB, 240055746560 bytes, 468858880 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0xf4f4f4f4

Device      Boot      Start        End    Sectors  Size Id Type
/dev/sdb1p1      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p2      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p3      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
/dev/sdb1p4      4109694196 8219388391 4109694196  1.9T f4 SpeedStor
fsck.exfat /dev/sdb1
exfatfsck 1.2.5
Checking file system on /dev/sdb1.
File system version           1.0
Sector size                 512 bytes
Cluster size                128 KB
Volume size                 224 GB
Used space                   21 MB
Available space             224 GB
#endif
int fs_info(int i)
{
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    FDIR fdir;
    snprintf(lbuf, 10, "%d:/", i);
    int ret = f_mount(&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
        return ret;
    ret = f_opendir (&fdir, lbuf);
    if (ret == FR_OK)
    {
        FATFS *fsp = fdir.obj.fs;
        NPrintf("fs on '%s' drive: %d, type: %d\n", lbuf, i, fsp->fs_type);
        NPrintf("sector size: %d, cluster size [sectors]: %d, size of an FAT [sectors]: %d\n", fsp->ssize, fsp->csize, fsp->fsize);
        NPrintf("number of FAT entries (number of clusters + 2): %d, number of FATs (1 or 2): %d\n", fsp->n_fatent, fsp->n_fats);
        NPrintf("last cluster: %d, free clusters: %d\n", fsp->last_clst, fsp->free_clst);
        unsigned long capa = fsp->n_fatent / 1024;
        capa *= fsp->ssize;
        capa /= 1024;
        capa *= fsp->csize;
        capa /= 1024;
        NPrintf("capacity: %luGB\n", capa);
        f_closedir (&fdir);
    }
    //
    f_mount(NULL, lbuf, 0);                    /* UnMount the default drive */
    //
    return FR_OK;
}

int sdopen (int i)
{
    //FDIR fdir;
    char lbuf[10];
    FATFS fs;     /* Ponter to the filesystem object */
    snprintf (lbuf, 10, "%d:/", i);
    int ret = f_mount (&fs, lbuf, 0);                    /* Mount the default drive */
    if (ret != FR_OK)
        return ret;
    if (ffd == -1)
        ffd = i;
    lp.path = strdup (lbuf);
    //ret = scan_files(lbuf); //f_opendir (&fdir, lbuf);
    //
    f_mount(NULL, lbuf, 0);                    /* UnMount the default drive */

    if (ret == FR_OK)
    {
        struct fm_file *ptr = lp.entries;
        NPrintf("Listing path contents for '%s'\n", lp.path);
        while (ptr != NULL)
        {
            NPrintf("%s%c\n", ptr->name, ptr->dir?'/':' ');
            ptr = ptr->next;
        }
        fs_info (i);
    }
    //
    return ret;
}
#if 0
int fatfs_init()
{
    fflib_init();
    //
    int k; for (k = 0; k < 8; k++)
    {
        fdld[k][0] = '\0';
        fddr[k] = fflib_attach (k, ff_ps3id[k], 1);
        NPrintf("open drive %d result %d for 0x%llx\n", k, fddr[k], (long long unsigned int)ff_ps3id[k]);
        if (fddr[k] == FR_OK)
            sdopen (k);
        else
            fflib_detach (k);
    }
    //
    return 0;
}
#endif
#ifdef LIBBUILD
extern void LoadTexture();
#else
void LoadTexture()
{
    u32 * texture_mem = tiny3d_AllocTexture(64*1024*1024); // alloc 64MB of space for textures (this pointer can be global)    

    u32 * texture_pointer; // use to asign texture space without changes texture_mem

    if(!texture_mem) return; // fail!

    texture_pointer = texture_mem;

    ResetFont();
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font  , (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) font_b, (u8 *) texture_pointer, 32, 255, 16, 32, 2, BIT0_FIRST_PIXEL);
    texture_pointer = (u32 *) AddFontFromBitmapArray((u8 *) msx   , (u8 *) texture_pointer,  0, 254,  8,  8, 1, BIT7_FIRST_PIXEL);

    // here you can add more textures using 'texture_pointer'. It is returned aligned to 16 bytes
}
#endif

#if 0
//file write perf
int file_write_perf (int idx)
{
    time_t timer, timere;
    struct tm * timed;
    FATFS fs0;          /* Work area (filesystem object) for logical drives */
    FIL fdst;           /* File objects */
    #define MSZ (1024*1024)
    #define FWR (MSZ*1024)/* 1GB as slices of 1MB */
    #define BSZ (3*MSZ)
    BYTE *buffer = NULL;//[BSZ];   /* File copy buffer */
    FRESULT fr;         /* FatFs function common result code */
    UINT bw;            /* File read/write count */
    char fn[25];
    snprintf (fn, 25, "%d:/file_write.bin", idx);
    /* Register work area for each logical drive */
    f_mount (&fs0, fn, 0);
    //test writing 1G
    /* Create destination file on the drive 0 */
    fr = f_open (&fdst, fn, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr)
    {
        NPrintf ("!failed to create file '%s'\n", fn);
        return (int)fr;
    }
    /* Copy buffer to destination */
    buffer = malloc (BSZ);
    if (buffer == NULL)
    {
        NPrintf ("!failed to allocate buffer of %dbytes\n", BSZ);
        return FR_INT_ERR;
    }
    //
    NPrintf ("writing %dMB to file '%s'.. please wait\n", FWR/MSZ, fn);
    //
    time (&timer);
    timed = localtime (&timer);
    NPrintf ("writing %d blocks of %dMB: ", FWR/BSZ, BSZ/MSZ);
    time (&timer);
    int k; for (k = 0; k < FWR/BSZ; k++)
    {
        fr = f_write (&fdst, buffer, BSZ, &bw);            /* Write it to the destination file */
        if (fr || bw < BSZ)
            break; /* error or disk full */
        //usleep (100000);
        NPrintf ("..%d ", k + 1);
        //if (app_input(0) & PAD_CI_MASK)
        //    break;
        DbgDraw ();
        tiny3d_Flip ();
    }
    time (&timere);
    /* Close open files */
    f_close (&fdst);
    NPrintf ("done.\n");
    int mbps = (k*BSZ)/(timere - timer);
    if (mbps > MSZ)
        NPrintf ("wrote %dMB to file '%s' in %dsec (%dMBps) bs %dbytes\n", (k*BSZ)/MSZ, fn, (timere - timer), mbps/MSZ, BSZ);
    else if (mbps > 1024)
        NPrintf ("wrote %dKB to file '%s' in %dsec (%dKBps) bs %dbytes\n", k*BSZ/1024, fn, (timere - timer), mbps/1024, BSZ);
    //
    FILINFO fno;
    if (f_stat (fn, &fno) == FR_OK)
        NPrintf ("FS: '%s' size: %luMB\n", fn, fno.fsize/MSZ);
    else
        NPrintf ("FS: '%s' size check failed!\n", fn);
    //test reading
    /* Open source file on the drive 1 */
    fr = f_open (&fdst, fn, FA_READ);
    if (fr == FR_OK)
    {
        NPrintf ("reading from file '%s'.. please wait\n", fn);
        //
        time (&timer);
        NPrintf ("reading blocks of %dMB: ", BSZ/MSZ);
        /* Copy source to destination */
        for (k = 0;;k++)
        {
            fr = f_read (&fdst, buffer, BSZ, &bw);  /* Read a chunk of source file */
            if (fr || bw == 0)
                break; /* error or eof */
            NPrintf ("..%d ", k + 1);
            //if (app_input(0) & PAD_CI_MASK)
            //    break;
            DbgDraw ();
            tiny3d_Flip ();
        }
        time (&timere);
        /* Close open files */
        f_close (&fdst);
        NPrintf ("done.\n");
        mbps = (k*BSZ)/(timere - timer);
        if (mbps > MSZ)
            NPrintf ("read %dMB from file '%s' in %dsec (%dMBps) bs %dbytes\n", (k*BSZ)/MSZ, fn, (timere - timer), mbps/MSZ, BSZ);
        else if (mbps > 1024)
            NPrintf ("read %dKB to file '%s' in %dsec (%dKBps) bs %dbytes\n", k*BSZ/1024, fn, (timere - timer), mbps/1024, BSZ);
    }
    //
    free (buffer);
    /* Unregister work area prior to discard it */
    f_mount (NULL, fn, 0);
    return (int)fr;
}
//create file
//f_write > create_chain > find_bitmap
int file_read(char *fname);
int file_create (char *fname)
{
    FATFS fs;      /* Work area (filesystem object) for logical drives */
    FIL fdst;      /* File objects */
    BYTE buffer[] = "ThiS is A TeSt FiLe FoR wRiTiNg.\n\0";   /* File copy buffer */
    FRESULT fr;    /* FatFs function common result code */
    UINT bw;       /* File read/write count */

    /* Register work area for each logical drive */
    char dn[5];
    snprintf(dn, 4, "%.3s", fname);
    NPrintf("mounting drive '%s'\n", dn);
    NPrintf("writing to file '%s':\n", fname);
    f_mount(&fs, dn, 0);

    /* Create destination file on the drive 0 */
    fr = f_open(&fdst, fname, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr)
    {
        NPrintf("!failed creating the file '%s' result %d\n", fname, fr);
        /* Unregister work area prior to discard it */
        f_mount(0, dn, 0);
        return (int)fr;
    }
    /* Copy source to destination */
    fr = f_write(&fdst, buffer, sizeof(buffer), &bw);            /* Write it to the destination file */
    if (fr != FR_OK || bw != sizeof(buffer))
        NPrintf("!failed writing to file '%s' result %d wrote %d bytes vs %d\n", fname, fr, bw, sizeof(buffer));
    /* Close open files */
    f_close(&fdst);

    /* Unregister work area prior to discard it */
    f_mount(0, dn, 0);
    //read test
    file_read (fname);
    //
    return (int)fr;
}
//
int file_new(char *fname)
{
    if(!fname || !*fname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("File create test");
    DbgMess("Press o/circle to return");
    //
    file_create (fname);
    //
    //int btn = 0;
    //
    while(1)
    {
        //2 input
        //btn = app_input(0);
        //if (btn & PAD_CI_MASK)
        //    break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}

//dir contents
int dir_read (char *dname)
{
    FRESULT res;
    FDIR dir;
    static FILINFO fno;
    /* Register work area to the default drive */
    FATFS fs;   /* Work area (filesystem object) for logical drive */
    char drn[5];
    snprintf(drn, 4, "%.3s", dname);
    NPrintf("mounting drive '%s'\n", drn);
    NPrintf("listing directory '%s':\n", dname);
    f_mount(&fs, drn, 0);

    res = f_opendir(&dir, dname);                       /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            FRESULT res1 = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
            {
                f_closedir(&dir);
                return res;  /* Break on error or end of dir */
            }
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                NPrintf("/%s\n", fno.fname);
                snprintf (dn, 255, "%s%s/", dname, fno.fname);
            } 
            else 
            {                                       /* It is a file. */
                NPrintf("%s\n", fno.fname);
                snprintf (fn, 255, "%s%s", dname, fno.fname);
            }
        }
        f_closedir(&dir);
    }
    else
    {
        NPrintf("!unable to open path '%s' result %d\n", dname, res);
    }
    //
    f_mount(0, drn, 0);
    //
    return res;
}

int dir_run(char *dname)
{
    if(!dname || !*dname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("DIR contents");
    DbgMess("Press o/circle to return");
    //
    dir_read (dname);
    //
    //int btn = 0;
    //
    while(1)
    {
        //2 input
        //btn = app_input(0);
        //if (btn & PAD_CI_MASK)
        //   break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}

//file contents
int file_read(char *fname)
{
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    FATFS fs;   /* Work area (filesystem object) for logical drive */

    /* Register work area to the default drive */
    char dn[5];
    snprintf(dn, 4, "%.3s", fname);
    NPrintf("mounting drive '%s'\n", dn);
    NPrintf("reading file '%s':\n", fname);
    f_mount(&fs, dn, 0);

    /* Open a text file */
    fr = f_open(&fil, fname, FA_READ);
    if (fr)
    {
        NPrintf("!failed opening the file '%s' result %d\n", fname, fr);
        //
        f_mount(0, dn, 0);
        return (int)fr;
    }

    /* Read every line and display it */
    while (f_gets(line, sizeof (line), &fil)) 
    {
        NPrintf(line);
    }

    /* Close the file */
    f_close(&fil);
    //
    f_mount(0, dn, 0);
    //
    return 0;
}

int file_run(char *fname)
{
    if(!fname || !*fname)
        return 0;
    //debug console
    initConsole ();
    DbgHeader("File contents");
    DbgMess("Press o/circle to return");
    //
    file_read (fname);
    //
    //int btn = 0;
    //
    while(1)
    {
        //2 input
        //btn = app_input(0);
        //if (btn & PAD_CI_MASK)
        //    break;
        //3
		//4
        DbgDraw();
        tiny3d_Flip();
    }
    return 0;
}
char sp[] = "fat1:/";
char sp2[] = "sys:/";
#endif

//app
struct fm_panel *app_active_panel ()
{
    if (lp.active)
        return &lp;
    return &rp;
}

struct fm_panel *app_inactive_panel ()
{
    if (lp.active)
        return &rp;
    return &lp;
}
//restore app state after other module was executed
int _fmapp_restore (char init)
{
    if (init)
        initConsole ();
    //DbgHeader("FATFS EXFAT Example");
    //DbgMess("Press o/circle to exit");
    NPrintf("\n");
    if (*fn)
        NPrintf("press triangle to list contents of file '%s'\n", fn);
    if (*wn)
        NPrintf("press cross to create a test file '%s'\n", wn);
    if (*dn)
        NPrintf("press rectangle to list contents of dir '%s'\n", dn);
    if (ffd != -1)
        NPrintf("press start for write/read test on drive '%d:/'\n", ffd);
    return 0;
}//1st

int fmapp_init (int dt)
{
    fm_panel_init (&lp, 0, 0, PANEL_W*8, PANEL_H*8, 1);
    fm_panel_init (&rp, PANEL_W*8, 0, PANEL_W*8, PANEL_H*8, 0);
    //debug console
    initConsole ();
    //fatfs test
    //fatfs_init ();
    fflib_init();
    //
    tiny3d_Init (1024*1024);
    //
	ioPadInit (7);
    //
	// Load texture
    LoadTexture ();
    //
    //DbgHeader("FATFS EXFAT Example");
    //
    fm_panel_scan (&lp, NULL);
    fm_panel_scan (&rp, NULL);
    //
    fm_status_set ("use LEFT and RIGHT to navigate as well as CROSS and CIRCLE", 0, 0xeeeeeeFF);
    fm_status_set ("use L1 and R1 to switch active tab", 1, 0x00ff00FF);
    fm_status_set ("use RECTANGLE to copy content", 2, 0xffff00FF);
    fm_status_set ("use TRIANGLE to erase content", 3, 0xff0000FF);
    //
    return 1;
}
//2nd input - skip, we read in app_update
//3rd
// --automount logic, every X update: X = 60fps * 4sec = 240 frames
#define AUTOMOUNT_FREQ  (3 * 60)
static int amount_k = 0;
int fmapp_update(int dat)
{
    //2 input
    ps3pad_read ();
    //mount monitoring logic: every 4 sec
    if (!app_active_panel ()->path)
    {
        if (amount_k <= 0)
        {
            //probe rootfs devices
            if (rootfs_probe ())
            {
                fm_panel_scan (&lp, NULL);
                fm_panel_scan (&rp, NULL);
            }
            amount_k = AUTOMOUNT_FREQ;
        }
        amount_k --;
    }
    //go up
    if (NPad (BUTTON_CIRCLE))
    {
        //fm_panel_exit (app_active_panel ());
        if (fm_panel_exit (app_active_panel ()))
        {
            //really quit?
            if (1 == YesNoDialog ("Quit the file manager?"))
                return -1;
        }
    }
    //activate left panel
    else if (NPad (BUTTON_L1))
    {
        lp.active = 1;
        rp.active = 0;
    }
    //activate right panel
    else if (NPad (BUTTON_R1))
    {
        lp.active = 0;
        rp.active = 1;
    }
    //scroll panel up
    else if (APad (BUTTON_UP))
    {
        fm_panel_scroll (app_active_panel (), FALSE);
    }
    //scroll panel dn
    else if (APad (BUTTON_DOWN))
    {
        fm_panel_scroll (app_active_panel (), TRUE);
    }
    //enter dir
    else if (NPad (BUTTON_RIGHT))
    {
        fm_panel_enter (app_active_panel ());
    }
    //exit dir
    else if (NPad (BUTTON_LEFT))
    {
        fm_panel_exit (app_active_panel ());
    }
    //rename
    else if (NPad (BUTTON_START))
    {
        char sp[CBSIZE];
        struct fm_panel *ps = app_active_panel ();
        if (ps->path && ps->current)
        {
            //todo: check if we're allowed to rename item
            if(Get_OSK_String("Rename", sp, CBSIZE) == SUCCESS)
            {
                //rename
                char lp[CBSIZE];
                snprintf (lp, CBSIZE, "rename %s to %s", sp, ps->current->name);
                fm_status_set (lp, 1, 0xffeeeeFF);
                //
                fm_job_rename (ps->path, ps->current->name, sp);
                //reload for content refresh
                fm_panel_reload (ps);
                fm_panel_locate (ps, sp);
            }
        }
    }
    //new dir
    else if (NPad (BUTTON_SELECT))
    {
        char sp[CBSIZE];
        struct fm_panel *ps = app_active_panel ();
        if (ps->path)
        {
            //todo: check if we're allowed to create dir?!
            //fm_panel_exit (app_active_panel ());
            if(Get_OSK_String("New folder", sp, 255) == SUCCESS)
            {
                //new dir
                char lp[CBSIZE];
                snprintf (lp, CBSIZE, "new dir %s", sp);
                fm_status_set (lp, 1, 0xffeeeeFF);
                fm_job_newdir (ps->path, sp);
                //reload for content refresh
                fm_panel_reload (ps);
                fm_panel_locate (ps, sp);
            }
        }
    }
    //cross - action: enter dir
    else if (NPad (BUTTON_CROSS))
    {
        fm_panel_enter (app_active_panel ());
    }
    //files delete
    else if (NPad (BUTTON_TRIANGLE))
    {
        char sp[CBSIZE];
        struct fm_panel *ps = app_active_panel ();
        if (ps->path)
        {
            snprintf (sp, CBSIZE, "%s/%s", ps->path, ps->current->name);
            fm_job_delete (sp, &fmapp_render);
            //reload for content refresh
            fm_panel_reload (ps);
        }
    }
    //files copy
    else if (NPad (BUTTON_SQUARE))
    {
        char sp[CBSIZE];
        char dp[CBSIZE];
        struct fm_panel *ps = app_active_panel ();
        struct fm_panel *pd = app_inactive_panel ();
        if (ps->path && pd->path)
        {
            snprintf (sp, CBSIZE, "%s/%s", ps->path, ps->current->name);
            snprintf (dp, CBSIZE, "%s/", pd->path);
            fm_job_copy (sp, dp, &fmapp_render);
            //reload inactive panel for content refresh
            fm_panel_reload (app_inactive_panel ());
        }
    }
    //
    return 0;
}
//4th
int fmapp_render(int dat)
{
    #if 1
    /* DRAWING STARTS HERE */
    tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
        
    // Enable alpha Test
    tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

    // Enable alpha blending.
    tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
        TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
        TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
    #endif
    // change to 2D context ( virtual size of the screen is 848.0 x 512.0)
    fm_panel_draw (&lp);
    fm_panel_draw (&rp);
    //
    fm_status_draw (dat);
    //
#ifdef _FPS
    char sfps[8];
    snprintf (sfps, 7, "%dfps", fps_update ());
    SetFontColor (0xff0000ff, 0x00000000);
    SetFontAutoCenter (0);
    DrawString (800, 0, sfps);
#endif
#ifdef SWVER
    char swver[8];
    snprintf (swver, 7, "%s", SWVER);
    SetFontColor (0xff0000ff, 0x00000000);
    SetFontAutoCenter (0);
    DrawString (800, 504, swver);
#endif
    //
    tiny3d_Flip ();

    return 1;
}
//5th
int fmapp_cleanup(int dat)
{
#ifndef LIBBUILD
    ioPadEnd();
#endif
    return 1;
}

s32 fmapp_run()
{
    //1 init
	fmapp_init (0);
    _fmapp_restore (0);
	// Ok, everything is setup. Now for the main loop.
	while(1) 
    {
        //3
        if (fmapp_update (0) == -1)
            break;
		//4
        //cls ();
        fmapp_render (0);
	}
    //5
    fmapp_cleanup (0);
    //
    return 0;
}
