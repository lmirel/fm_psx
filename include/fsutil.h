//fsutil.h
#ifndef __FSUTIL__
#define __FSUTIL__

int fs_path_scan (struct fm_panel *p);

int fs_job_scan (struct fm_job *p);

int fs_get_fstype (char *path, int *npo);

#endif
