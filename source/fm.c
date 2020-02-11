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

void NPrintf(const char* fmt, ...);

int fm_panel_init (struct fm_panel *p, int x, int y, int w, int h, char act)
{
    p->x = x;
    p->y = y;
    p->w = w;
    p->h = h;
    p->active = act;
    p->entries = NULL;
    p->current = NULL;
    p->path = NULL;
    //
    return 0;
}

int fm_panel_draw (struct fm_panel *p)
{
    static char fname[53];
    //
    if (p->active)
        DrawRect2d (p->x, p->y, 0, p->w, p->h, 0xb4b4b4ff);
    else
        DrawRect2d (p->x, p->y, 0, p->w, p->h, 0x787878ff);
    //draw panel content: 56 lines - 1 for dir path, 1 for status - 54
    int k;
    SetFontColor (0x000000ff, 0x00000000);
    SetFontAutoCenter (0);
    SetCurrentFont (2);
    SetFontSize (8, 8);
    struct fm_file *ptr = p->entries;
    for (k = 0; k < 54 && ptr != NULL; k++, ptr = ptr->next)
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
            //
            fm_fname_get (ptr, 51, fname);
            DrawString (p->x + 8, p->y + 8 + k * 8, fname);
        }
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
            p->current = p->current->next;
        else
            return -1;
    }
    else
    {
        if (p->current->prev != NULL)
            p->current = p->current->prev;
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
    link->prev = NULL;
    link->next = NULL;
    NPrintf ("fm_panel_add %s dir %d\n", fn, dir);

    // If head is empty, create new list
    if (p->entries == NULL)
    {
        p->entries = link;
        //
        if (p->current == NULL)
            p->current = link;
        return 0;
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
                NPrintf ("fm_panel_add before %s\n", current->name);
                add_before (p, link, current);
            }
        }
        else
        {
            //skip dirs
            while (current->next != NULL && current->dir == 1)
                current = current->next;
            //compare only with files
            if (!current->dir && strcmp (current->name, fn) < 0)
                while (current->next != NULL && strcmp (current->name, fn) < 0)
                    current = current->next;
            //
            if (strcmp (current->name, fn) > 0)
            {
                NPrintf ("fm_panel_add before %s\n", current->name);
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
    if (p->current == NULL)
        p->current = link;
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
