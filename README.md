# fm_psx
simple file manager for PS3
<br>
<br>
usage:

- L1 and R1 change active/current panel to left(L1) or right(R1)
- cross and circle, right and left used to navigate in and out of directories
- up and down scroll current selection- rectangle for copy files/dirs
- triangle for delete of copy/dirs
- START rename file/dir
- SELECT create dir
<br>

todos:

- sanity checks
- available storage check and report
- other confirmation dialogs on rename/delete
- contextual menu(?!)

later todos:
+ split the framework into file management lib and manager
+ write a file management lib that will handle system, ntfs/ext3 and exfat access to files:
- use path names which will include filesystem specification:
* for system: /dev_hdd0/, /dev_bdvd/, /app_home/, /host_root/
* for ntfs and/or ext3: /ntfs0:/, /ext0:/
* for ex/fat: /fat0:/
* others..
- store FS/file/dir specification in a pointer (e.g. PS3FP) while allowing direct access to its attributes (e.g. fd / file descriptor) for flexibility
- will need to support at mininum ps3_fopen, ps3_fclose, ps3_fseek, ps3_f2sectors, ps3_diropen, ps3_dirnext, ps3_dirclose, etc
- will need to handle detection/scanning of portable storage (with transparent mounting/unmounting)


!BE EXTRA CAREFUL WITH SYSTEM FILES/DIRS!
<br>
USE AT YOUR OWN RISK!

# building
use the opensource toolchain and libs from here:
- https://github.com/bucanero/ps3toolchain
- https://github.com/bucanero/PSL1GHT
- https://github.com/Estwald/PSDK3v2/tree/master/libraries-src/Tiny3D

additional libraries can be found here but should not be required
- https://github.com/bucanero/psl1ght-libs
