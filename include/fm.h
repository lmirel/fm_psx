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
    char *path;                 //current path - full path, including drive identifier
    struct fm_file *entries;    //entries
    struct fm_file *current;    //current entry/selection
    //
    int x, y, w, h;             //position+dimentions
    char active;                //panel is active or not
};

int fm_panel_init (struct fm_panel *p, int x, int y, int w, int h, char act);
int fm_panel_draw (struct fm_panel *p);

int fm_panel_scroll (struct fm_panel *p, int dn);

//add file/dir entry to panel
int fm_panel_add (struct fm_panel *p, char *fn, char dir, unsigned long fsz);

//remove file/dir entry from panel
int fm_panel_del (struct fm_panel *p, char *fn);

int fm_fname_get (struct fm_file *link, int cw, char *out);
