//fsutil.c

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include <sysutil/osk.h>
#include <sysutil/sysutil.h>
#include <sys/memory.h>
#include <ppu-lv2.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <lv2/sysfs.h>

#include "ff.h"
#include "fm.h"
#include "fsutil.h"
#include "console.h"

//sys fs
#define FS_S_IFMT 0170000
#define FS_S_IFDIR 0040000
#define DT_DIR 1

/*
sysFsGetFreeSize("/dev_hdd0/", &blockSize, &freeSize);
sprintf(filename, "/dev_usb00%c/", 47+find_device);
sysFsGetFreeSize(filename, &blockSize, &freeSize);
if(find_device==11) sprintf(filename, "/dev_bdvd");

//dir
	sysFSDirent dir;
    DIR_ITER *pdir = NULL;

    if(is_ntfs) {pdir = ps3ntfs_diropen(path); if(pdir) ret = 0; else ret = -1; }
    else ret=sysLv2FsOpenDir(path, &dfd);

    while ((!is_ntfs && !sysLv2FsReadDir(dfd, &dir, &read)) 
        || (is_ntfs && ps3ntfs_dirnext(pdir, dir.d_name, &st) == 0)) {
    
	if(is_ntfs) ps3ntfs_dirclose(pdir); else sysLv2FsCloseDir(dfd);
//file
    ret = sysLv2FsOpen(path, 0, &fd, S_IRWXU | S_IRWXG | S_IRWXO, NULL, 0);
                        ret = sysLv2FsOpen(temp_buffer, SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd, 0777, NULL, 0);
            if(flags & CPY_FILE1_IS_NTFS) {fd = ps3ntfs_open(path, O_RDONLY, 0);if(fd < 0) ret = -1; else ret = 0;}

                    fd2 = ps3ntfs_open(path2, O_WRONLY | O_CREAT | O_TRUNC, 0);if(fd2 < 0) ret = -1; else ret = 0;
                    ret = sysLv2FsOpen(path2, SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &fd2, 0777, NULL, 0);
                    sysLv2FsChmod(path2, FS_S_IFMT | 0777);

                {ret = ps3ntfs_write(v->fd, v->mem, v->size); v->readed = (u64) ret; if(ret>0) ret = 0;}
            else ret=sysLv2FsWrite(v->fd, v->mem, v->size, &v->readed);

            if(flags & ASYNC_NTFS) ps3ntfs_close(v->fd); else sysLv2FsClose(v->fd);

 */

int fs_path_scan (struct fm_panel *p)
{
    if (!p->path)
        return -1;
    //scan FAT/ExFAT path
    if (strncmp (p->path, "fat", 3) == 0)
    {
        p->fs_type = FS_TFAT;
        NPrintf ("scanning fat path %s\n", p->path);
        return fat_scan_path (p);
    }
    //scan EXT path
    else if (strncmp (p->path, "ext", 3) == 0)
    {
        p->fs_type = FS_TEXT;
        return 1;//ext_scan_path (p);
    }
    //scan NTFS path
    else if (strncmp (p->path, "ntfs", 4) == 0)
    {
        p->fs_type = FS_TNTFS;
        return 1;//ntfs_scan_path (p);
    }
    //scan sys path
    else
    {
        p->fs_type = FS_TSYS;
        NPrintf ("scanning sys path %s\n", p->path);
        return sys_scan_path (p);
    }
    return 0;
}

int sys_scan_path (struct fm_panel *p)
{
    char lp[256];
	int dfd;
	u64 read;
	sysFSDirent dir;
    char *lpath = p->path + 4;  //from 'sys:/' to '/'
    int res = sysLv2FsOpenDir (lpath, &dfd);
    if (res)
    {
        NPrintf ("!failed sysLv2FsOpenDir path %s, res %d\n", p->path, res);
		return res;
    }
    for (; !sysLv2FsReadDir (dfd, &dir, &read); )
    {
		if (!read)
			break;
		if (!strcmp (dir.d_name, ".") || !strcmp (dir.d_name, ".."))
			continue;
        //
        if (dir.d_type & DT_DIR)
        {
            fm_panel_add (p, dir.d_name, 1, 0);
        }
        else
        {
            snprintf (lp, 255, "%s/%s", lpath, dir.d_name);
            sysFSStat stat;
            res = sysLv2FsStat (lp, &stat);
            NPrintf ("sysLv2FsStat for '%s', res %d\n", lp, res);
            if (res >= 0)
                fm_panel_add (p, dir.d_name, 0, stat.st_size);
            else
                fm_panel_add (p, dir.d_name, 0, -1);
        }
    }
    sysLv2FsCloseDir (dfd);
    //
    return 0;
}

int fat_scan_path (struct fm_panel *p)
{
    char lp[256];
    FRESULT res;
    FDIR dir;
    static FILINFO fno;
    FATFS fs;     /* Ponter to the filesystem object */
    char *lpath = p->path + 3;
    res = f_mount (&fs, lpath, 0);                    /* Mount the default drive */
    if (res != FR_OK)
        return res;
    //strip the 'fat' preffix on path from 'fat0:/' to '0:/'
    res = f_opendir (&dir, lpath);                       /* Open the directory */
    NPrintf ("scanning fat path %s, res %d\n", lpath, res);
    //
    if (res == FR_OK)
    {
        for (;;)
        {
            FRESULT res1 = f_readdir (&dir, &fno);                   /* Read a directory item */
            if (res1 != FR_OK || fno.fname[0] == 0) 
                break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) 
            {                    /* It is a directory */
                fm_panel_add (p, fno.fname, 1, 0);
            } 
            else 
            {                                       /* It is a file. */
                snprintf (lp, 255, "%s/%s", lpath, fno.fname);
                NPrintf ("FAT f_stat for '%s', res %d\n", lp, res);
                if (f_stat (lp, &fno) == FR_OK)
                    fm_panel_add (p, fno.fname, 0, fno.fsize);
                else
                    fm_panel_add (p, fno.fname, 0, -1);
            }
        }
        f_closedir (&dir);
    }
    else
    {
        ;//DPrintf("!unable to open path '%s' result %d\n", path, res);
    }
    f_mount (NULL, lpath, 0);                    /* UnMount the default drive */
    //
    return res;
}
