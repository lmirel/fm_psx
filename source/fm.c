//fm.c

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include "fm.h"
void NPrintf(const char* fmt, ...);

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
        return 0;
    }
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
