//fm.h - simple file manager

struct fm_file {
    char *name;
    char dir;
    unsigned long size;
    //
    struct fm_file *prev;
    struct fm_file *next;
};

struct fm_panel {
    char *cpath;
    struct fm_file *entries;
};

//add file/dir entry to panel
int fm_panel_add (struct fm_panel *p, char *fn, char dir, unsigned long fsz);

//remove file/dir entry from panel
int fm_panel_del (struct fm_panel *p, char *fn);
