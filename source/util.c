//util.c
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <sysutil/msg.h>
#include <sysutil/sysutil.h>

#include "util.h"
#include "pad.h"

#define S_PI 3.14159265f
#define D_PI 6.28318531f


int NPad (int btn)
{
	if (new_pad & btn)
		return 1;
	return 0;
}

int PPad (int btn)
{
	if (old_pad & btn)
        return 1;
	return 0;
}

#define PPAD_SKIP   5   //skip old pad report every 5 calls to reduce speed
int APad (int btn)
{
    static int pps = PPAD_SKIP;
	if (NPad(btn))
    {
        pps = PPAD_SKIP;
		return 1;
    }
	if (PPad(btn))
    {
        if (pps--)
            return 0;
        pps = PPAD_SKIP;
        return 1;
    }
	return 0;
}

void DrawRect2d (float x, float y, float z, float w, float h, u32 rgba)
{
    tiny3d_SetPolygon (TINY3D_QUADS);
    tiny3d_VertexPos (x, y, z);
    tiny3d_VertexColor (rgba);
    tiny3d_VertexPos (x + w, y, z);
    tiny3d_VertexPos (x + w, y + h, z);
    tiny3d_VertexPos (x, y + h, z);
    tiny3d_End ();
}

#if 0
unsigned long get_millis ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (unsigned long)((unsigned long long)tv.tv_sec * CLK_TCK +
                    (unsigned long long)tv.tv_usec * (1000000L / CLK_TCK));
}
#endif

int fps_update ()
{
    static time_t times = 0, timee;
    static int fpsv = -1;
    static int fpsh = 0;
    //
    if (times == 0)
        time (&times);
    time (&timee);
    //
    if (timee > times)
    {
        fpsh = fpsv;
        times = timee;
        fpsv = -1;
    }
    fpsv++;
    //
    return fpsh;
}

msgType mdialogyesno = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO  | MSG_DIALOG_DEFAULT_CURSOR_NO;
msgType mdialogyesno2 = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON;
msgType mdialogyesno3 = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO;

msgType mdialogok = MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK;

msgType mdialog = MSG_DIALOG_NORMAL | MSG_DIALOG_DISABLE_CANCEL_ON;

volatile int dialog_action = 0;

void my_dialog(msgButton button, void *userdata)
{
    switch(button)
    {
        case MSG_DIALOG_BTN_YES:
            dialog_action = 1;
            break;
        case MSG_DIALOG_BTN_NO:
        case MSG_DIALOG_BTN_ESCAPE:
        case MSG_DIALOG_BTN_NONE:
            dialog_action = 2;
            break;
        default:
            break;
    }
}

void my_dialog2(msgButton button, void *userdata)
{
    switch(button)
    {
        case MSG_DIALOG_BTN_OK:
        case MSG_DIALOG_BTN_ESCAPE:
        case MSG_DIALOG_BTN_NONE:
            dialog_action = 1;
            break;
        default:
            break;
    }
}

void wait_dialog()
{
    while(!dialog_action)
    {
        sysUtilCheckCallback();
        tiny3d_Flip();
    }

    msgDialogAbort();
    usleep(100000);
}

void DrawDialogOKTimer(char * str, float milliseconds)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogok, str, my_dialog2, (void*) 0x0000aaab, NULL );
    msgDialogClose(milliseconds);

    wait_dialog();
}


void DrawDialogOK(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogok, str, my_dialog2, (void*) 0x0000aaab, NULL );

    wait_dialog();
}


void DrawDialogTimer(char * str, float milliseconds)
{
    dialog_action = 0;

    msgDialogOpen2(mdialog, str, my_dialog2, (void*) 0x0000aaab, NULL );
    msgDialogClose(milliseconds);

    wait_dialog();
}

int DrawDialogYesNoTimer(char * str, float milliseconds)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno, str, my_dialog, (void*)  0x0000aaaa, NULL );
    msgDialogClose(milliseconds);

    wait_dialog();

    return dialog_action;
}

int YesNoDialog(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno, str, my_dialog, (void*)  0x0000aaaa, NULL );

    wait_dialog();

    return dialog_action;
}

int DrawDialogYesNoDefaultYes(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno3, str, my_dialog, (void*)  0x0000aaaa, NULL );

    wait_dialog();

    return dialog_action;
}

int DrawDialogYesNoTimer2(char * str, float milliseconds)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno2, str, my_dialog, (void*)  0x0000aaaa, NULL );
    msgDialogClose(milliseconds);

    wait_dialog();

    return dialog_action;
}

int DrawDialogYesNo2(char * str)
{
    dialog_action = 0;

    msgDialogOpen2(mdialogyesno2, str, my_dialog, (void*)  0x0000aaaa, NULL );

    wait_dialog();

    return dialog_action;
}

//progress bar dialog box
static volatile int progress_action = 0;

static msgType mdialogprogress = MSG_DIALOG_SINGLE_PROGRESSBAR | MSG_DIALOG_MUTE_ON | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_BKG_INVISIBLE;
static msgType mdialogprogress2 = MSG_DIALOG_DOUBLE_PROGRESSBAR | MSG_DIALOG_MUTE_ON;

static void progress_callback(msgButton button, void *userdata)
{
    switch(button)
    {
        //case MSG_DIALOG_BTN_OK:
        case MSG_DIALOG_BTN_YES:
            progress_action = 1;
            break;
        case MSG_DIALOG_BTN_NO:
        case MSG_DIALOG_BTN_ESCAPE:
            progress_action = 2;
            break;
        case MSG_DIALOG_BTN_NONE:
        case MSG_DIALOG_BTN_INVALID:
            progress_action = -1;
            break;
        default:
            break;
    }
}

int DrawProgressDialogTest (char *progress_bar_title)
{
    progress_action = 0;

    msgDialogOpen2(mdialogprogress2, progress_bar_title, progress_callback, (void *) 0xadef0045, NULL);

    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, " ");
    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, " ");
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);

    sysUtilCheckCallback(); tiny3d_Flip();
    //todo job
    msgDialogAbort();
    return 0;
}

static u32 prev_prc0 = 0;
static u32 prev_prc1 = 0;
void ProgressBarUpdate(u32 cprc, const char *msg)
{
    if (msg)
    {
        msgDialogProgressBarSetMsg (MSG_PROGRESSBAR_INDEX0, msg);
    }
    if (cprc > prev_prc0)
    {
        msgDialogProgressBarInc (MSG_PROGRESSBAR_INDEX0, cprc - prev_prc0);
        prev_prc0 = cprc;
    }
    else if (cprc < prev_prc0)
    {
        msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
        prev_prc0 = 0;
    }
    sysUtilCheckCallback();
    tiny3d_Flip();
}

void ProgressBar2Update(u32 cprc, const char *msg)
{
    if (msg)
    {
        msgDialogProgressBarSetMsg (MSG_PROGRESSBAR_INDEX1, msg);
    }
    if (cprc > prev_prc1)
    {
        msgDialogProgressBarInc (MSG_PROGRESSBAR_INDEX1, cprc - prev_prc1);
        prev_prc1 = cprc;
    }
    else if (cprc < prev_prc1)
    {
        msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);
        prev_prc1 = 0;
    }
    //
    sysUtilCheckCallback();
    tiny3d_Flip();
}

void SingleProgressBarDialog(char *caption)
{
    progress_action = 0;

    msgDialogOpen2(mdialogprogress, caption, progress_callback, (void *) 0xadef0044, NULL);

    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);

    sysUtilCheckCallback();
    tiny3d_Flip();
}

void DoubleProgressBarDialog(char *caption)
{
    progress_action = 0;
    prev_prc0 = 0;
    prev_prc1 = 0;

    msgDialogOpen2(mdialogprogress2, caption, progress_callback, (void *) 0xadef0042, NULL);

    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, "");
    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, "");
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);

    //2
    sysUtilCheckCallback();
    tiny3d_Flip();

    //3
    //Update message and progress
    //msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0, (u32) percent);
    //msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, msg);
    //msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1, (u32) percent);
    //msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, msg);
    //sysUtilCheckCallback();
    //tiny3d_Flip();

    //4: 1 = OK, YES; 2 = NO/ESC/CANCEL; -1 = NONE
    //if(ProgressBarActionGet() == 2) break;

    //5
    //msgDialogAbort();
}

int ProgressBarActionGet ()
{
    return progress_action;
}

#if 0
static int my_game_copy(char *path, char *path2)
{

    progress_action2 = 0;

    bar1_countparts = 0.0f;
    bar2_countparts = 0.0f;

    msgDialogOpen2(mdialogprogress2, progress_bar_title, progress_callback, (void *) 0xadef0045, NULL);

    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0, " ");
    msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, " ");
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);

    sysUtilCheckCallback(); tiny3d_Flip();

    global_device_bytes = 0;

    if(fast_copy_async(path, path2, 1) < 0) {abort_copy = 665;  msgDialogAbort(); return FAILED;}

    int ret = _my_game_copy(path, path2);

    int ret2 = fast_copy_process();

    fast_copy_async(path, path2, 0);

    msgDialogAbort();

    if(ret < 0 || ret2 < 0) return FAILED;

    return SUCCESS;
}
//
/***********************************************************************************************************/
/* msgDialog                                                                                               */
/***********************************************************************************************************/


static msgType mdialogprogress =   MSG_DIALOG_SINGLE_PROGRESSBAR | MSG_DIALOG_MUTE_ON;
static msgType mdialogprogress2 =   MSG_DIALOG_DOUBLE_PROGRESSBAR | MSG_DIALOG_MUTE_ON;

static volatile int progress_action = 0;

static void progress_callback(msgButton button, void *userdata)
{
    switch(button)
    {
        case MSG_DIALOG_BTN_OK:
            progress_action = 1;
            break;
        case MSG_DIALOG_BTN_NO:
        case MSG_DIALOG_BTN_ESCAPE:
            progress_action = 2;
            break;
        case MSG_DIALOG_BTN_NONE:
            progress_action = -1;
            break;
        default:

            break;
    }
}

static void update_bar(u32 cpart)
{
    msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0, (u32) cpart);
    sysUtilCheckCallback(); tiny3d_Flip();
}

static void update_bar2(u32 cpart)
{
    msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1, (u32) cpart);
    sysUtilCheckCallback(); tiny3d_Flip();
}

static void single_bar(char *caption)
{
    progress_action = 0;

    msgDialogOpen2(mdialogprogress, caption, progress_callback, (void *) 0xadef0044, NULL);

    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);

    sysUtilCheckCallback();tiny3d_Flip();
}

static float progress_0 = 0.0f;

static void double_bar(char *caption)
{
    progress_action = 0;

    msgDialogOpen2(mdialogprogress2, caption, progress_callback, (void *) 0xadef0042, NULL);

    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
    msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);

    progress_0 = 0.0f;
    sysUtilCheckCallback();tiny3d_Flip();
}

                        single_bar("Deleting...");

                        float parts = 100.0f / (float) files;
                        float cpart = 0;

                        for(n = 0; n < nentries; n++)
                        {
                            if(!(entries[n].d_type & IS_MARKED)) continue; // skip no marked
                            if(progress_action == 2) break;

                            cpart += parts;
                            if(cpart >= 1.0f)
                            {
                                update_bar((u32) cpart);
                            ..
                        sysUtilCheckCallback(); tiny3d_Flip();
                        msgDialogAbort();
                        usleep(250000);

//
            msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1); write_progress = 0;
            msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1, getfilename_part(fast_files[current_fast_file_r].pathw));

                msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0, (u32) bar1_countparts);
                bar1_countparts-= (float) ((u32) bar1_countparts);
            }

                msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1, (u32) bar2_countparts);

#endif

