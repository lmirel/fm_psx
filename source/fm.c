//fm.c

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include <tiny3d.h>
#include <libfont.h>

#include "fm.h"
#include "util.h"
#include "fsutil.h"
#include "console.h"

//status message
static char *s_msg[STATUS_H] = {NULL, NULL, NULL, NULL};
//status message text color
static int c_msg[STATUS_H] = {-1, -1, -1, -1};
//set status message for intex idx
int fm_status_set (char * sm, int idx, int col)
{
    if (idx < 0 || idx > STATUS_H - 1)
        return -1;
    if (s_msg[idx])
        free (s_msg[idx]);
    if (sm)
        s_msg[idx] = strdup (sm);
    else
        s_msg[idx] = NULL;
    c_msg[idx] = col;
    return 0;
}

int fm_status_draw (int dat)
{
    SetCurrentFont (2);
    SetFontSize (8, 8);
    //title - current path
    SetFontAutoCenter (0);
    int i;
    for (i = 0; i < STATUS_H; i++)
    {
        if (c_msg[i] != -1)
            SetFontColor (c_msg[i], 0x00000000);
        if (s_msg[i])
            DrawString (0, (PANEL_H + i) * 8, s_msg[i]);
    }
    return 0;
}

//enter current dir
int fm_panel_enter (struct fm_panel *p)
{
    //can't enter file
    if (!p->current->dir)
        return -1;
    //move deeper
    char np[256];
    if (p->path && *p->path)
        snprintf (np, 255, "%s/%s", p->path, p->current->name);
    else
        snprintf (np, 255, "%s", p->current->name);
    return fm_panel_scan (p, np);
    //
    return 0;
}

//exit current dir
int fm_panel_exit (struct fm_panel *p)
{
    //can't return from here on root with no FS
    if (!p->path)
        return -1;
    //
    char np[256];
    char *lp = strrchr (p->path, '/');
    if (lp)
    {
        //is this already on root?
        if (*(lp + 1) == '\0')
            //exit to rootfs
            return fm_panel_scan (p, NULL);
        *lp = 0;
    }
    else
        //exit to rootfs
        return fm_panel_scan (p, NULL);
    //
    snprintf (np, 255, "%s", p->path);
    return fm_panel_scan (p, np);
}

int fm_panel_clear (struct fm_panel *p)
{
    struct fm_file *ptr;
    for (ptr = p->entries; ptr != NULL; ptr = p->entries)
    {
        p->entries = ptr->next;
        if (ptr->name)
            free (ptr->name);
        free (ptr);
    }
    p->current = NULL;
    p->current_idx = -1;
    //
    p->files = 0;
    p->dirs = 0;
    p->fsize = 0;
    //
    return 0;
}

int fm_job_clear (struct fm_job *job)
{
    struct fm_file *ptr;
    for (ptr = job->entries; ptr != NULL; ptr = job->entries)
    {
        job->entries = ptr->next;
        if (ptr->name)
            free (ptr->name);
        free (ptr);
    }
    //
    if (job->spath)
        free (job->spath);
    job->spath = NULL;
    if (job->dpath)
        free (job->dpath);
    job->dpath = NULL;
    //
    job->files = 0;
    job->dirs = 0;
    job->fsize = 0;
    //
    return 0;
}

int fm_job_list (char *path)
{
    struct fm_job fmjob;
    struct fm_job *job = &fmjob;
    //
    job->spath = strdup (path);
    job->dpath = NULL;
    job->stype = FS_TNONE;
    job->dtype = FS_TNONE;
    //
    job->entries = NULL;
    //
    job->files = 0;
    job->dirs = 0;
    job->fsize = 0;
    //
    fs_job_scan (job);
    char lp[256];
    snprintf (lp, 256, "job scan %dfiles, %ddirs, %llubytes", job->files, job->dirs, job->fsize);
    fm_status_set (lp, 0, 0xeeeeeeFF);
    //
    struct fm_file *ptr;
    for (ptr = job->entries; ptr != NULL; ptr = ptr->next)
    {
        NPrintf ("job %d> %08lu> %s\n", ptr->dir, ptr->size, ptr->name);
    }
    //
    fm_job_clear (job);
    //
    return 0;
}

int fm_job_add (struct fm_job *p, char *fn, char dir, unsigned long fsz)
{
    // Allocate memory for new node;
    struct fm_file *link = (struct fm_file*) malloc (sizeof (struct fm_file));
    if (!link)
        return -1;
    //
    link->name = strdup (fn);
    link->dir = dir;
    link->size = fsz;
    //
    link->prev = NULL;
    link->next = NULL;
    NPrintf ("fm_job_add %d>%s\n", dir, fn);
    //stats
    if (fsz > 0)
        p->fsize += fsz;
    if (dir)
        p->dirs++;
    else
        p->files++;
    // If head is empty, create new list
    if (p->entries == NULL)
    {
        p->entries = link;
    }
    else
    {
        struct fm_file *current = p->entries;
        // move to the end of the list
        while (current->next != NULL)
            current = current->next;
        // Insert link at the end of the list
        current->next = link;
        link->prev = current;
    }
    //
    return 0;
}

int fm_panel_scan (struct fm_panel *p, char *path)
{
    if (p->path)
        free (p->path);
    if (path)
        p->path = strdup (path);
    else
        p->path = NULL;
    //cleanup
    fm_panel_clear (p);
        
    //
    return fs_path_scan (p);
}

int fm_panel_init (struct fm_panel *p, int x, int y, int w, int h, char act)
{
    p->x = x;
    p->y = y;
    p->w = w;
    p->h = h;
    //
    p->active = act;
    p->entries = NULL;
    p->current = NULL;
    p->current_idx = -1;
    p->path = NULL;
    //
    p->files = 0;
    p->dirs = 0;
    p->fsize = 0;
    p->fs_type = FS_TNONE;
    //
    return 0;
}

int fm_panel_draw (struct fm_panel *p)
{
    static char fname[53];
    int wh = p->h/8 - 2;    //scroll rows: panel height - 2 rows
    int se = 0;             //skipped entries
    //
    if (p->active == TRUE)
        DrawRect2d (p->x, p->y, 0, p->w, p->h, 0xb4b4b4ff);
    else
        DrawRect2d (p->x, p->y, 0, p->w, p->h, 0x787878ff);
    //draw panel content: 56 lines - 1 for dir path, 1 for status - 54
    int k;
    SetCurrentFont (2);
    SetFontSize (8, 8);
    //title - current path
    SetFontColor (0x0000ffff, 0x00000000);
    SetFontAutoCenter (0);
    if (p->path)
        snprintf (fname, 51, "%s", p->path);
    else
        snprintf (fname, 51, "%s", "[root]");
    DrawString (p->x, p->y, fname);
    //
    SetFontColor (0x000000ff, 0x00000000);
    SetFontAutoCenter (0);
    //
    struct fm_file *ptr = p->entries;
    //do we need to skip entries on listing?
    for (se = p->current_idx - wh + 1; se > 0 && ptr != NULL; ptr = ptr->next, se--)
        ;//skip some entries
    for (k = 0; k < wh && ptr != NULL; k++, ptr = ptr->next)
    {
        //draw current item
        if (p->current == ptr)
        {
            DrawRect2d (p->x, p->y + 8 + k * 8, 0, p->w, 8, 0x787878ff);
            //
            fname[0] = '>';
            fm_fname_get (ptr, 51, fname + 1);
            DrawString (p->x, p->y + 8 + k * 8, fname);
        }
        else
        {
            if (ptr->selected)
            {
                fname[0] = '*';
                fm_fname_get (ptr, 51, fname + 1);
                DrawString (p->x, p->y + 8 + k * 8, fname);
            }
            else
            {
                fm_fname_get (ptr, 51, fname);
                DrawString (p->x + 8, p->y + 8 + k * 8, fname);
            }
        }
        //file size - to the right side of the name
        if (!ptr->dir)
        {
            if (ptr->size > GBSZ)
                snprintf (fname, 7, "%4luGB", ptr->size / GBSZ);
            else if (ptr->size > MBSZ)
                snprintf (fname, 7, "%4luMB", ptr->size / MBSZ);
            else
                snprintf (fname, 7, "%4luKB", ptr->size / KBSZ);
            DrawString (p->x + 8 + (46 * 8), p->y + 8 + k * 8, fname);
        }
    }
    //status - size, files, dirs
    SetFontColor (0x0000ffff, 0x00000000);
    SetFontAutoCenter (0);
    if (p->path)
    {
        int bw = snprintf (fname, 51, "%d dirs, %d files - ", p->dirs, p->files);
        if (p->fsize > GBSZ)
            snprintf (fname + bw, 51 - bw, "%llu GB", p->fsize / GBSZ);
        else if (p->fsize > MBSZ)
            snprintf (fname + bw, 51 - bw, "%llu MB", p->fsize / MBSZ);
        else
            snprintf (fname + bw, 51 - bw, "%llu KB", p->fsize / KBSZ);
        DrawString (p->x, p->y + wh * 8 + 8, fname);
    }
    //
    return 0;
}

static int add_before (struct fm_panel *p, struct fm_file *link, struct fm_file *next)
{
    /* 4. Make prev of new node as prev of next_node */
    link->prev = next->prev;  
  
    /* 5. Make the prev of next_node as new_node */
    next->prev = link;  
  
    /* 6. Make next_node as next of new_node */
    link->next = next;  
  
    /* 7. Change next of new_node's previous node */
    if (link->prev != NULL)  
        link->prev->next = link;  
    /* 8. If the prev of new_node is NULL, it will be 
       the new head node */
    else
        (p->entries) = link; 
    return 0;
}

int fm_fname_get (struct fm_file *link, int cw, char *out)
{
    *out = 0;
    if (!link)
        return -1;
    if (link->dir)
        snprintf (out, cw, "/%s", link->name);
    else
        snprintf (out, cw, "%s", link->name);
    return 0;
}

int fm_panel_scroll (struct fm_panel *p, int dn)
{
    if (dn)
    {
        if (p->current->next != NULL)
        {
            p->current = p->current->next;
            p->current_idx++;
        }
        else
            return -1;
    }
    else
    {
        if (p->current->prev != NULL)
        {
            p->current = p->current->prev;
            p->current_idx--;
        }
        else
            return -1;
    }
    return 0;
}

int fm_panel_add (struct fm_panel *p, char *fn, char dir, unsigned long fsz)
{
    // Allocate memory for new node;
    struct fm_file *link = (struct fm_file*) malloc (sizeof (struct fm_file));
    if (!link)
        return -1;
    //
    link->name = strdup (fn);
    link->dir = dir;
    link->size = fsz;
    //
    link->selected = FALSE;
    //
    link->prev = NULL;
    link->next = NULL;
    //NPrintf ("fm_panel_add %s dir %d\n", fn, dir);
    //stats
    if (fsz > 0)
        p->fsize += fsz;
    if (dir)
        p->dirs++;
    else
        p->files++;
    // If head is empty, create new list
    if (p->entries == NULL)
    {
        p->entries = link;
    }
    else
    {
        struct fm_file *current = p->entries;
    #if 1
        if (dir)
        {
            while (current->next != NULL && current->dir == 1 && strcmp (current->name, fn) < 0)
                current = current->next;
            //don't add after file
            if (current->next == NULL && current->dir)
            {
                current->next = link;
                link->prev = current;
            }
            else
            {
                //NPrintf ("fm_panel_add before %s\n", current->name);
                add_before (p, link, current);
            }
        }
        else
        {
            //skip dirs
            while (current->next != NULL && current->dir == TRUE)
                current = current->next;
            //compare only with files
            if (!current->dir && strcmp (current->name, fn) < 0)
                while (current->next != NULL && strcmp (current->name, fn) < 0)
                    current = current->next;
            //
            if (strcmp (current->name, fn) > 0)
            {
                //NPrintf ("fm_panel_add before %s\n", current->name);
                add_before (p, link, current);
            }
            else
            //if (current->next == NULL)
            {
                current->next = link;
                link->prev = current;
            }
        }
    #else
        // move to the end of the list
        while (current->next != NULL)
            current = current->next;
        // Insert link at the end of the list
        current->next = link;
        link->prev = current;
    #endif
    }
    //set current item
    p->current = p->entries;
    p->current_idx = 0;
    //
    return 0;
}

int fm_panel_del (struct fm_panel *p, char *fn)
{
    struct fm_file *pre_node;

    if(p->entries == NULL)
    {
        //printf("Linked List not initialized");
        return -1;
    } 
    struct fm_file *current = p->entries;
    pre_node = current;
    while (current->next != NULL && strcmp (current->name, fn) != 0) 
    {
        pre_node = current;
        current = current->next;
    }        

    if (strcmp (current->name, fn) == 0)
    {
        pre_node->next = pre_node->next->next;
        if (pre_node->next != NULL)
        {          // link back
            pre_node->next->prev = pre_node;
        }
        if (p->entries == current)
            p->entries = NULL;
        free (current->name);
        free (current);
    }
    return 0;
}
