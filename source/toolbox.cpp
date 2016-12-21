#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <fastmath.h>
#include <stddef.h>
#include <netdb.h>
#include <stdbool.h>

#include <np.h>
#include <np/drm.h>

#include <sys/synchronization.h>
#include <sys/spu_initialize.h>
#include <sys/ppu_thread.h>
#include <sys/return_code.h>
#include <sys/sys_time.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/deci3.h>
#include <sys/timer.h>
#include <sys/paths.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/vm.h>

#include <cell/control_console.h>
#include <cell/http.h>
#include <cell/gcm.h>
#include <cell/sysmodule.h>
#include <cell/font.h>
#include <cell/fontFT.h>
#include <cell/dbgfont.h>
#include <cell/cell_fs.h>

#include <cell/rtc.h>
#include <cell/rtc/rtcsvc.h>

#include <cell/pad.h>
#include <cell/keyboard.h>

#include <cell/codec/pngdec.h>
#include <cell/codec/jpgdec.h>
#include <cell/codec.h>

#include <sysutil/sysutil_msgdialog.h>
#include <sysutil/sysutil_oskdialog.h>
#include <sysutil/sysutil_syscache.h>
#include <sysutil/sysutil_sysparam.h>

#include <sysutil/sysutil_common.h>
#include <sysutil/sysutil_screenshot.h>
#include <sysutil/sysutil_bgmplayback.h>
#include <sysutil/sysutil_gamecontent.h>

#include <netex/libnetctl.h>
#include <netex/errno.h>
#include <netex/net.h>

#include <arpa/inet.h>
#include <netinet/in.h>

bool is_cobra = false;

//SYS_PROCESS_PARAM(1200, 0x100000)

sys_process_param_t __sys_process_param SYS_PROCESS_PARAM_SECTION = {
		sizeof(sys_process_param_t),
		SYS_PROCESS_PARAM_MAGIC,
		SYS_PROCESS_PARAM_VERSION_330_0,
		0x00340001ULL,
		1200,
		0x100000,
		SYS_PROCESS_PARAM_MALLOC_PAGE_SIZE_1M,	 //_1M
		SYS_PROCESS_PARAM_PPC_SEG_DEFAULT,
		(uint32_t) &__sys_process_crash_dump_param};



#define STR_APP_NAME "Rebug Toolbox"
#define STR_APP_ID	 "RBGTLBOX2"
#define STR_APP_VER	 "02.02.11"





//#include "syscall8.h"
#include "common.h"
u64 HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_355;
u64 NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_355;
u64 SYSCALL_TABLE			= SYSCALL_TABLE_355;

#include "syscall36.h"

#include "storage.h"
#include "graphics.h"
#include "fonts.h"

#include "flash_os_area.h"

#include "ftp.h"
#include "openftp/ftp_filesystem.h"

#define FB(x) ((x)*1920*1080*4)	// 1 video frame buffer
#define MB(x) ((x)*1024*1024)	// 1 MB
#define KB(x) ((x)*1024)		// 1 KB

#define SYSCALL_PEEK	6
#define SYSCALL_POKE	7

#define UNMOUNT	0
#define MOUNT	1
#define OFF		0
#define ON		1

#define SYSCALL8_OPCODE_GET_VERSION   0x7000
#define SYSCALL8_OPCODE_GET_VERSION2   0x7001


// LANGUAGE 

static char STR_LANGUAGE[130][128]={{0}};
static char STR_LANGCODE[130][2]={{0}};
int lang_N;
u8 lang=0;
char lang_path[130][128];

char STR_LANG[50] = "Language";
char STR_LANGDESC[50] = "Choose a language";
char STR_DISABLE[50] = "Disabled";
char STR_ENABLE[50] = "Enabled";
char STR_NO[50] = "No";
char STR_SYSTEM[50] = "System";
char STR_SELECTOR[50] = "Selector";
char STR_LV1PATCH[50] = "LV1 Patches";
char STR_DTC[50] = "DEX/CEX";
char STR_UTLS[50] = "Utilities";
char STR_EIDRK[150] = "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR";
char STR_SCRSHOT[130] = "Saving screenshot to";
char STR_DONE[50] = "Done!";
char STR_SHTDWN[130] = "Shutting down...";
char STR_TRYRESTART[130] = "Trying to restart...";
char STR_NOTFOUND[50] = "was not found!";
char STR_PETBOOTINST[130] = "Do you want to install Petitboot on VFLASH/NAND Region 5?";
char STR_PETBOOTOK[130] = "Petitboot has been installed.. Enjoy OtherOS.";
char STR_PETBOOTERROR[150] = "ERROR: Could not find Petitboot..VFLASH Detected.\n\nPlease put file named [dtbimage.ps3.bin] on USB device and try again.";
char STR_RESIZE5[130] = "Do you want to resize VFLASH/NAND Region 5 to allow OtherOS?";
char STR_RESIZE5OK[130] = "SUCCESS: Your PS3 needs to reboot for changes to take effect.";
char STR_RESIZE5ERROR[130] = "FAILED: resizing VFLASH/NAND Region 5 is not completed.";
char STR_FLASHOK[130] = "SUCCESS: Your PS3 needs to reboot for changes to take effect.";
char STR_FLASHERROR[50] = "FAILED!!!";
char STR_GAMEOSFLG[130] = "Do you want to set GameOS boot flag?";
char STR_GAMEOSFLGOK[130] = "Flag is already set.";
char STR_SYSINF[130] = "System information";
char STR_SYSINFDESC[130] = "Display information about your PS3\xE2\x84\xA2 system.";
char STR_QUIT[50] = "Quit";
char STR_QUITDESC1[50] = "Quit";
char STR_QUITDESC2[150] = "and return to XMB\xE2\x84\xA2 screen";
char STR_RESTSYS[50] = "Restart System";
char STR_RESTSYSDESC1[50] = "Close";
char STR_RESTSYSDESC2[130] = "and restart your PLAYSTATION\xC2\xAE\x33 System";
char STR_RESTSYS_SUB1[130] = "Soft (LV2 Reboot Only)";
char STR_RESTSYS_SUB2[130] = "Full (On/Off Cycle)";
char STR_BOOTOOS[50] = "Boot OtherOS";
char STR_BOOTOOSDESC[130] = "Select LV1 patches and reboot into OtherOS";
char STR_BOOTOOS_SUB1[130] = "Boot (LV1 patches: Apply All)";
char STR_BOOTOOS_SUB2[130] = "Boot (LV1 patches: Use current)";
char STR_TRNOFF[50] = "Turn Off System";
char STR_TRNOFFDESC[130] = "Shutdown your PLAYSTATION\xC2\xAE\x33 System";
char STR_QA[50] = "Toggle QA Flag";
char STR_QADESC[130] = "Enable or disable QA flag functions.";
char STR_NOTAV[50] = "Not Available";
char STR_TOGREC[130] = "Toggle Recovery Mode";
char STR_TOGRECDESC[130] = "Enable or disable recovery mode on next reboot.";
char STR_TOGPM[130] = "Toggle Product Mode";
char STR_TOGPMDESC[130] = "Enable or disable product mode (FSM).";
char STR_LOADLV2[130] = "Load LV2 Kernel";
char STR_LOADLV2DESC[130] = "Load lv2_kernel.self.[KERNEL_NAME] from USB or /dev_hdd0.";
char STR_BUT[130] = "Button Assignment";
char STR_BUTDESC[130] = "Sets which buttons are used for Accept/Enter and Cancel/Back.";
char STR_CIRCLE[50] = "Circle is [Accept]";
char STR_CROSS[50] = "Cross is [Accept]";
char STR_XREG[130] = "Backup/Restore xRegistry";
char STR_XREGDESC[130] = "Backup or Restore the PS3 system settings from USB.";
char STR_BACKUP[50] = "Backup";
char STR_RESTORE[50] = "Restore";
char STR_RESIZEVFLASH[130] = "Resize VFLASH/NAND Regions";
char STR_RESIZEVFLASHDESC[130] = "Resize VFLASH/NAND Regions 5 to allow OtherOS.";
char STR_INSTPETBOOT[50] = "Install petitboot";
char STR_INSTPETBOOTDESC[130] = "Install Petitboot to VFLASH/NAND Regions 5 from USB.";
char STR_GAMEOSBOOTFLG[130] = "Set GameOS Boot Flag";
char STR_GAMEOSBOOTFLGDESC[130] = "fixes issue loading PS2 titles if OtherOS is installed.";
char STR_PKG[130] = "Create \x22Packages\x22 Folder";
char STR_PKGDESC[130] = "Create /dev_hdd0/packages folder.";
char STR_SAVLV1[130] = "Export Hypervisor LV1 Memory";
char STR_SAVLV1DESC[130] = "Save LV1 memory to a file.";
char STR_SAVLV2[130] = "Export GameOS LV2 Memory";
char STR_SAVLV2DESC[130] = "Save LV2 memory to a file.";
char STR_SAVFLASH[130] = "Export Flash to File";
char STR_SAVFLASHDESC[130] = "Backup your current NOR/NAND flash to /dev_usb000.";
char STR_SAVEIDRK[50] = "Dump eid root key";
char STR_SAVEIDRKDESC[130] = "Dump eid root key at dev_hdd0/game/RBGTLBOX2/USRDIR/eid_root_key.";
char STR_PS3ID[50] = "Change Active PS3ID";
char STR_PS3IDDESC[130] = "Spoof IDPS in LV2 memory.";
char STR_USE[50] = "Use";
char STR_SM[50] = "System Mode";
char STR_SMDESC[130] = "Switch between Normal and REBUG modes.";
char STR_XMBOM[50] = "XMB Operation Mode";
char STR_XMBOMDESC[130] = "Switch between Retail and Debug XMB.";
char STR_MENU[50] = "Debug Menu Type";
char STR_MENUDESC[130] = "Switch between CEX QA and DEX Debug menu.";
char STR_TOGCFW[130] = "Toggle XMB CFW settings";
char STR_TOGCFWDESC[130] = "Enable or Disable XMB CFW settings v0.1a (MOD)";
char STR_TOGCOB[50] = "Toggle COBRA Mode";
char STR_TOGCOBDESC[130] = "Enable or disable COBRA Mode on next reboot.";
char STR_TOGPS2[50] = "Toggle PS2 Emulator";
char STR_TOGPS2DESC[130] = "PS2Emu swap to use Original PS2 Emu files.";
char STR_TOGWM[50] = "Toggle webMAN";
char STR_TOGWMDESC[130] = "Enable or disable integrated webMAN on next reboot.";
char STR_NORBG[130] = "REBUG Selector Functions Not Available";
char STR_NORBGDESC[130] = "Please install REBUG REX/D-REX Firmware to access the Selector Functions.";
char STR_PATCHLV1[130] = "LV1 Peek/Poke Support";
char STR_PATCHLV1DESC[130] = "Patch unused LV1 syscalls 182 and 183.";
char STR_LV2MEMPRO[130] = "LV2 Memory Protection";
char STR_LV2MEMPRODESC[130] = "Patch Shutdown on LV2 modification.";
char STR_HTAB[130] = "HTAB Mapping With Write Protection";
char STR_HTABDESC[130] = "Enable or Disable protected HTAB mapping.";
char STR_INDI[130] = "Access to all INDI Info Manager Services";
char STR_INDIDESC[130] = "Enable or Disable INDI access.";
char STR_EEPROM[130] = "Update Manager EEPROM Write Access";
char STR_EEPROMDESC[130] = "Patch Update Manager access to EEPROM.";
char STR_DISPATCH[130] = "Dispatch Manager Access";
char STR_DISPATCHDESC[130] = "Patch Dispatch Manager access to all services.";
char STR_ENCDEC[130] = "Allow ENCDEC IOCTL Command 0x85";
char STR_ENCDECDESC[130] = "Enable IOCTL Command 0x85";
char STR_SYSMAN[130] = "System Manager Ability Mask of GameOS";
char STR_SYSMANDESC[130] = "Allow access to all system manager services.";
char STR_ALLPKG[130] = "Extract all PKG Types";
char STR_ALLPKGDESC[130] = "Allow Update Manager to extract all PKG types.";
char STR_LPAR[130] = "Full Access for Repo Nodes in Any LPAR";
char STR_LPARDESC[130] = "Allow create, modify, delete for repository nodes.";
char STR_LV1SPE[130] = "LV1 Full Access to MFC_SR1 SPE Register";
char STR_LV1SPEDESC[130] = "Allow all-bit access to the register.";
char STR_LV1DABR[130] = "LV1 Access to set Data Break Points";
char STR_LV1DABRDESC[130] = "Enable LV1 access with lv1_set_dabr().";
char STR_LV1GART[130] = "LV1 Access to GPU GART Memory";
char STR_LV1GARTDESC[130] = "Allow LV1 to use 4KB IO page size.";
char STR_LV1KEYS[130] = "Storage Manager ENCDEC Keys Access";
char STR_LV1KEYSDESC[130] = "Allow Storage Manager to clear ENCDEC keys.";
char STR_LV1ACL[130] = "Skip all ACL Checks";
char STR_LV1ACLDESC[130] = "Enable skipping of ACL checks for all storage devices.";
char STR_LV1GO[130] = "Initial GuestOS Loader";
char STR_LV1GODESC[130] = "Enable GuestOS mode 1 for GameOS.";
char STR_SWPLV2K[50] = "Swap LV2 Kernel";
char STR_SWPLV2KDESC[130] = "Switch between retail and debug kernels.";
char STR_REWTID[130] = "Rewrite Target ID in Flash";
char STR_REWTIDDESC[130] = "Convert PS3 to cex/dex if valid flash dump/eid_root_key is provided.";
char STR_NOCEXDEX[130] = "CEX/DEX Functions Not Available";
char STR_NOCEXDEXDESC[130] = "Install REBUG REX EDITION to access these functions.";
char STR_SELECT[50] = ": Select";
char STR_HOLDQUITREB[130] = ": Hold to Quit and Reboot";
char STR_HOLDQUIT[50] = ": Hold to Quit";
char STR_FREEHDD[130] = "Free HDD space";
char STR_NUMFW[50] = "PS3\xE2\x84\xA2 System: Firmware";
char STR_IP[50] = "IP Address";
char STR_QUITXMB[50] = "Quit to XMB?";
char STR_CHANGEIDPSOK1[130] = "Successfully applied";
char STR_CHANGEIDPSOK2[130] = "changes to LV2 memory.\n\nCurrent target ID set to";
char STR_CHANGEIDPSERROR[130] = "Error occurred while searching for IDPS";
char STR_SWP2DEXOK[130] = "LV2 Kernel successfully changed to DEX/Debug.";
char STR_SWP2CEXOK[130] = "LV2 Kernel successfully changed to CEX/Retail.";
char STR_SWPERROR[130] = "Incompatible firmware version or an error has occurred.";
char STR_NEEDRESTSYS[130] = "System restart is required. Press OK to reboot";
char STR_WEIDERROR1[130] = "Error occured while accessing flash";
char STR_WEIDERROR2[100] = "The file you provided is not compatible with your system!";
char STR_WEIDRUSURE1[130] = "You are about to update your PS3\xE2\x84\xA2 Region/Target ID!\n\nRegion/Target ID: ";
char STR_WEIDRUSURE2[25] = "ARE YOU SURE?";
char STR_WEIDRUSURE3[100] = "You are about to change your PS3\xE2\x84\xA2 Region/Target ID!\n\nCurrent ID:";
char STR_WEIDRUSURE4[50] = "New ID:";
char STR_WEIDRRUUSURE[200] = "WARNING:\n\nThis operation is DANGEROUS and should be carried out ONLY if you are CONFIDENT!\n\nA R E   Y O U   S U R E ?";
char STR_WEIDOK1[200] = "Flash EID0 region successfully overwritten (192 bytes).\n\nPlease quit to XMB, turn-off and turn-on your system!";
char STR_WEIDOK2[200] = "Flash EID0 region successfully overwritten (192 bytes).\n\nTo apply region change, please quit to XMB, turn-off and turn-on your system!";
char STR_WEIDERROR3[130] = "Error writing to flash EID0!";
char STR_WEIDERROR4[130] = "Error accessing FLASH device!";
char STR_WTDMSG1[130] = "Your firmware and current mode";
char STR_WTDMSG2[300] = "allows you to dump eid_root_key.\nYou can dump it and convert from DEX to CEX and vice versa with this toolbox.\nIf you have your root key placed in /dev_hdd0/game/RBGTLBOX2/USRDIR/eid_root_key, then you will be greeted with a choice now.";
char STR_WTDMSG3[130] = "if you have your root key at /dev_hdd0/game/RBGTLBOX2/USRDIR/eid_root_key than you will be greeted with a choice now.";
char STR_WTDMSG4[130] = "Found eid_root_key, do you want automatic conversion from";
char STR_WTDMSG5[130] = "Found true DEX\nCEX swap would change region id to USA(0x84)";
char STR_WTDMSG6[130] = "Error occurred.\n\nPlease verify your eid_root_key, Your key may have been corrupted or someone else's";
char STR_WTDMSG7[130] = "Please provide the following file and try again";
char STR_WTDMSG8[300] = "Flashing the whole NOR is disabled.\n\nName your NOR dump properly to enable FLASHING ONLY THE EID0 SECTION.\n\nInput [*.EID0.NORBIN] must be 16MB and also be from YOUR console.\nOnly 192 bytes will be written to flash (EID0).";
char STR_WTDMSG9[300] = "Flashing the whole NAND is disabled.\n\nName your NAND dump properly to enable FLASHING ONLY THE EID0 SECTION.\n\nInput [*.EID0.NANDBIN] must be 239MB or 256MB and also be from YOUR console.\nOnly 192 bytes will be written to flash (EID0).";
char STR_MAINNORBG[130] = "Your system is not running on compatible REBUG configuration!\n\nSome options will not be available.";
char STR_MAINNOCOB[130] = "Your system is not running on compatible REBUG COBRA configuration!\n\nToggling COBRA will not be available.";
char STR_MAINNOFW[130] = "Cannot detect firmware version, press OK to exit.";
char STR_MAINERRFLASH[130] = "Cannot access FLASH device, press OK to exit.";
char STR_SCRSHTSAVED[130] = "Screenshot successfully saved as";
char STR_SAVXREG[130] = "Do you want to backup /dev_flash2/etc/xRegistry.sys to /dev_usb000?";
char STR_SAVXREGOK[130] = "Backup complete!\nxRegistry.sys backed up to /dev_usb000";
char STR_SAVXREGERR1[130] = "ERROR!\nxRegistry.sys was not backed up";
char STR_SAVXREGERR2[130] = "ERROR!\nThere is not a storage device connected to dev_usb000";
char STR_SAVXREGERR3[130] = "ERROR: /dev_flash2/etc/xRegistry.sys was not found!";
char STR_RESXREG[130] = "Do you want to restore xRegistry.sys from /dev_usb000?";
char STR_RESXREGOK[130] = "Restore complete!\nxRegistry.sys restored from /dev_usb000";
char STR_RESXREGERR1[130] = "ERROR!\nxRegistry.sys was not restored";
char STR_RESXREGERR2[130] = "ERROR: /dev_usb000/xRegistry.sys was not found!";
char STR_MAKPKGOK[130] = "Folder [/dev_hdd0/packages] created successfully.";
char STR_MAKPKGERR[130] = "Cannot create [/dev_hdd0/packages] folder!";
char STR_LOADLV2KERR1[130] = "Invalid LV2 Kernel file selected!";
char STR_LOADLV2K[130] = "Do you want to load the selected LV2 kernel and soft reboot?";
char STR_LOADLV2KNOSPACE[130] = "There is not enough free space in /dev_flash!";
char STR_LOADLV2KCHKLV1[130] = "Checking LV1, please wait...";
char STR_LOADLV2KERR2[130] = "LV2 Kernel: Operation failed.";
char STR_BOOTOOSMSG1[130] = "Do you want to change the boot order, enable all LV1 patches and load OtherOS?";
char STR_BOOTOOSMSG2[130] = "Do you want to change the boot order and load OtherOS?";
char STR_BOOTOOSERR1[130] = "OtherOS: Operation failed.";
char STR_SWPKERR[130] = "This function is not available in CEX mode.\n\nPlease flash a DEX NOR/NAND first.";
char STR_DMPFLSH[130] = "Do you want to export FLASH memory to a file?";
char STR_DMPFLSHERR[130] = "Please insert a USB drive (/dev_usb000) with at least 256MB free and try again!";
char STR_EXPLVMSG1[130] = "Do you want to export GameOS LV2 memory to a file?";
char STR_EXPLVMSG2[130] = "Do you want to export HV LV1 memory to a file?";
char STR_EXPLVWAIT[30] = "Please wait...";
char STR_EXPLVOK[130] = "GameOS memory exported successfully to file";
char STR_EXPLVERR[130] = "GameOS memory export failed!\n\nCannot create";
char STR_EXPLVMSG3[130] = "Exporting HyperVisor (LV1) memory to file";
char STR_EXPLVMSG5[130] = "Exporting GameOS memory to file";
char STR_EXPLVMSG4[130] = "Please wait, it may take about 5 minutes...";
char STR_EXPLVOK2[130] = "Hypervisor memory exported successfully to file";
char STR_EXPLVERR2[130] = "Hypervisor memory export failed!\n\nCannot create";
char STR_MODNORM[130] = "[NORMAL MODE] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_TRYAGAIN[130] = "FAILED: PLEASE TRY AGAIN";
char STR_MODNORMALRD[130] = "[NORMAL MODE] IS ALREADY SET!!";
char STR_MODREB[130] = "[REBUG MODE] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_MODREBALRD[130] = "[REBUG MODE] IS ALREADY SET!";
char STR_WMDIS[130] = "WebMAN is disabled, System will auto-reboot when you quit.";
char STR_WMENA[130] = "WebMAN is enabled. webMAN will be loaded on next boot.\n\nMake sure to enable COBRA after enabling webMAN";
char STR_MODRET[130] = "[RETAIL XMB] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_MODRETERR1[130] = "PLEASE TRY [DEBUG XMB]";
char STR_MODRETERR2[130] = "UNAVAILABLE IN [NORMAL MODE]";
char STR_MODDEB[130] = "[DEBUG XMB] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_MODDEBERR1[130] = "PLEASE TRY [RETAIL XMB]";
char STR_MODDEBERR2[130] = "UNAVAILABLE IN [NORMAL MODE]";
char STR_MODDEBSQA[130] = "[DEBUG SETTINGS: MENU CEX QA] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_MODDEBSQAOK[130] = "[DEBUG SETTINGS: MENU CEX QA] is now active.";
char STR_MODDEBSQAERR1[130] = "[DEBUG SETTINGS: MENU CEX QA] FAILED";
char STR_MODDEBSQAERR2[130] = "TRY [DEBUG SETTINGS: MENU DEBUG]";
char STR_MODDEBSDEB[130] = "[DEBUG SETTINGS: MENU DEBUG] is now active.\n\nSystem will auto-reboot when you quit.";
char STR_MODDEBSDEBOK[130] = "[DEBUG SETTINGS: MENU DEBUG] is now active.";
char STR_MODDEBSDEBERR1[130] = "[DEBUG SETTINGS: MENU DEBUG] FAILED";
char STR_MODDEBSDEBERR2[130] = "TRY [DEBUG SETTINGS: MENU CEX QA]";
char STR_MODCOBENA[50] = "COBRA Mode enabled.";
char STR_MODCOBDIS[50] = "COBRA Mode disabled.";
char STR_ERRFLASH[130] = "Error in flash, reinstall firmware to fix this";
char STR_PS2ERR1[130] = "No PS2Emu files found on HDD or USB(dev_usb000).";
char STR_PS2CONT[130] = "Continue to copy PS2Emu files from USB(dev_usb000)?";
char STR_PS2COP[130] = "PS2Emu files copied from USB!";
char STR_PS2SWPOK[130] = "PS2Emu swapped!\nReboot for changes to take effect.";
char STR_XMBCFWDIS[130] = "XMB CFW settings MOD is Disabled. The plugin will be unloaded on next boot.";
char STR_XMBCFWENA[130] = "XMB CFW settings MOD is Enabled. The feature will be available via Network Column on XMB.";
char STR_FLASHSET[130] = "Flag is already set.";
char STR_1[50] = "Data saved as:";
char STR_2[50] = "Processed";
char STR_3[50] = "MB in";
char STR_4[50] = "min";
char STR_5[50] = "MB/s";
char STR_READFLASHERROR1[130] = "Error occurred while accessing data!\n\nSectors read: ";
char STR_READFLASHERROR2[30] = "\nTotal sectors: ";
char STR_TOGHOSTINF[130] = "Toggle Host Information on XMB [DEX]";
char STR_TOGHOSTINFDESC[130] = "Enable or Disable Host Information on XMB";

int lang_pos, fh;
int exist(char *path);

uint32_t get_xreg_value(char *key, u32 default_value)
{
	int reg = -1;
	u32 reg_value = default_value;
	u16 off_string, len_data, len_string;
	u64 read, pos, off_val_data;
	CellFsStat stat;
	char string[256];

	if(cellFsOpen("/dev_flash2/etc/xRegistry.sys", CELL_FS_O_RDONLY, &reg, NULL, 0) != CELL_FS_SUCCEEDED || reg == -1)
	{
		return reg_value;
	}

	cellFsStat("/dev_flash2/etc/xRegistry.sys", &stat);

	u64 entry_name = 0x10000;

	while(true)
	{
	//// Data entries ////
		//unk
		entry_name += 2;

		//relative entry offset
		cellFsLseek(reg, entry_name, 0, &pos);
		cellFsRead(reg, &off_string, 2, &read);
		entry_name += 4;

		//data lenght
		cellFsLseek(reg, entry_name, 0, &pos);
		cellFsRead(reg, &len_data, 2, &read);
		entry_name += 3;

		//data
		off_val_data = entry_name;
		entry_name += len_data + 1;

	//// String Entries ////
		off_string += 0x12;

		//string length
		cellFsLseek(reg, off_string, 0, &pos);
		cellFsRead(reg, &len_string, 2, &read);
		off_string += 3;

		//string
		memset(string, 0, sizeof(string));
		cellFsLseek(reg, off_string, 0, &pos);
		cellFsRead(reg, string, len_string, &read);

		//Find language
		if(strcmp(string, key) == 0)
		{
			cellFsLseek(reg, off_val_data, 0, &pos);
			cellFsRead(reg, &reg_value, 4, &read);
			break;
		}

		if(off_string == 0xCCDD || entry_name >= stat.st_size) break;
	}

	cellFsClose(reg);

	return reg_value;
}

static bool language(const char *file_str, char *default_str)
{
	uint64_t siz = 0;
	uint8_t i;
	int f=0;
	char temp[1];

	bool do_retry=true;

	if (fh) f=fh; //file is already open
    else
    {
		if (cellFsOpen(lang_path[lang], CELL_FS_O_RDONLY, &f, NULL, 0) != CELL_FS_SUCCEEDED) return false;

		fh = f;

 retry:
		cellFsLseek(f, lang_pos, CELL_FS_SEEK_SET, &siz);
	}

	do {
		cellFsRead(f, (void *)&temp, 0x01, &siz);
		lang_pos++;
		for(i=0; i < strlen(file_str); i++)
		{
			if (temp[0] != file_str[i]) break;
			else if (i==strlen(file_str)-1)
			{
				while(siz && temp[0] != '{') {
					cellFsRead(f, (void *)&temp, 0x01, &siz);
					lang_pos++;
				}
				
				int str_len = 0;
				while(siz) {
					cellFsRead(f, (void *)&temp, 0x01, &siz);
					lang_pos++;
					if (temp[0] == '}') {
						default_str[str_len] = NULL;
						return true;
					}
					
					
					if (temp[0] == 92) { 
						cellFsRead(f, (void *)&temp, 0x01, &siz);
						lang_pos++;
						if (temp[0] == 'n') {
							default_str[str_len] = '\n';
							str_len++;
						} 
						else if (temp[0] == 'r') {
							default_str[str_len] = '\r';
							str_len++;
						} 
						else if (temp[0] == 'x') {
							cellFsRead(f, (void *)&temp, 0x01, &siz);
							lang_pos++;
							uint8_t val=0, val_u=0;
							sscanf(temp, "%hhX", &val);
							val = val*0x10;
							cellFsRead(f, (void *)&temp, 0x01, &siz);
							lang_pos++;
							sscanf(temp, "%hhX", &val_u);
							val += val_u;
							default_str[str_len] = val;
							str_len++;
						}
						else {
							default_str[str_len] = 92;
							str_len++;
							default_str[str_len] = temp[0];
							str_len++;
						}
						
					} else {
						default_str[str_len] = temp[0];
						str_len++;
					}
					
				}
			}
			else
			{
				cellFsRead(f, (void *)&temp, 0x01, &siz);
				lang_pos++;
			}
		}
	} while(siz != 0);

	if(do_retry) {do_retry=false; lang_pos=0; goto retry;}

	return true;
}

void get_all_language()
{
	char LOCPath[128];
	char TXTPath[128];
	int fd;
	
	
	strcpy(STR_LANGUAGE[0], "English") ;
	sprintf(LOCPath, "/dev_hdd0/game/%s/USRDIR/loc", STR_APP_ID);
	
	if(cellFsOpendir(LOCPath, &fd) == CELL_FS_SUCCEEDED) {
		
		CellFsDirent entry; u64 read_e;
	
		while(cellFsReaddir(fd, &entry, &read_e) == 0 && read_e > 0) {
			
			sprintf(TXTPath, "%s/%s", LOCPath, entry.d_name);
			
			lang_pos=fh=0;
			
			if(cellFsOpen(TXTPath, CELL_FS_O_RDONLY, &fh, NULL, 0) == CELL_FS_SUCCEEDED) {
				if( language("STR_LANGUAGE", STR_LANGUAGE[lang_N]) == true) {
					language("STR_LANGCODE", STR_LANGCODE[lang_N]);
					strcpy(lang_path[lang_N], TXTPath);
					lang_N++;
				}
				cellFsClose(fh);
			}
		}
		cellFsClosedir(fd);
	}

}

/*
	0x0=German
	0x1=English (US)
	0x2=Spanish
	0x3=French
	0x4=Italian
	0x5=Dutch
	0x6=Portuguese (Por)
	0x7=Russian
	0x8=Japanese
	0x9=Korean
	0xA=Chinese (traditional)
	0xB=Chinese (simplified)
	0xC=Finnish
	0xD=Swedish
	0xE=Danish
	0xF=Norwegian
	0x10=Polish
	0x11=Portuguese (Bra)
	0x12=English (UK)
*/

void load_activ()
{
	FILE *fp;
	char activ[128];
	char data[130]={0};
	int i;
	
	sprintf(activ, "/dev_hdd0/game/%s/USRDIR/loc/activ.txt", STR_APP_ID);
	
	fp = fopen(activ, "r");
	
	if( fp != NULL ) {
		fgets(data, 128, fp);
		fclose(fp);
		for(i=0; i < lang_N ;i++) {
			if( strcmp(data, STR_LANGUAGE[i]) == 0) {
				lang = i;
				return;
			}
		}
	} 
	else {
		uint32_t lang_xreg = get_xreg_value((char*)"/setting/system/language", 1);
		
		for(i=0; i < lang_N ;i++) {
			if((u8) STR_LANGCODE[i][0] == lang_xreg) {
				lang = i;
				return;
			}
		}
	}
	
	lang = 0;
	
}

void save_activ()
{
	FILE *fp;
	char activ[128];
	sprintf(activ, "/dev_hdd0/game/%s/USRDIR/loc/activ.txt", STR_APP_ID);
	fp = fopen(activ, "w");
	if(fp==NULL) return;
	fputs(STR_LANGUAGE[lang], fp);
	fclose(fp);
}

static void update_language(void)
{
	
	lang_pos=fh=0;
	
	if( language("STR_DISABLE", STR_DISABLE) == false ) return ;
	
	language("STR_ENABLE", STR_ENABLE);
	language("STR_NO", STR_NO);
	language("STR_SYSTEM", STR_SYSTEM);
	language("STR_SELECTOR", STR_SELECTOR);
	language("STR_LV1PATCH", STR_LV1PATCH);
	language("STR_DTC", STR_DTC);
	language("STR_UTLS", STR_UTLS);
	language("STR_EIDRK", STR_EIDRK);
	language("STR_SCRSHOT", STR_SCRSHOT);
	language("STR_DONE", STR_DONE);
	language("STR_SHTDWN", STR_SHTDWN);
	language("STR_TRYRESTART", STR_TRYRESTART);
	language("STR_NOTFOUND", STR_NOTFOUND);
	language("STR_PETBOOTINST", STR_PETBOOTINST);
	language("STR_PETBOOTOK", STR_PETBOOTOK);
	language("STR_PETBOOTERROR", STR_PETBOOTERROR);
	language("STR_RESIZE5", STR_RESIZE5);
	language("STR_RESIZE5OK", STR_RESIZE5OK);
	language("STR_RESIZE5ERROR", STR_RESIZE5ERROR);
	language("STR_FLASHOK", STR_FLASHOK);
	language("STR_FLASHERROR", STR_FLASHERROR);
	language("STR_GAMEOSFLG", STR_GAMEOSFLG);
	language("STR_GAMEOSFLGOK", STR_GAMEOSFLGOK);
	language("STR_SYSINF", STR_SYSINF);
	language("STR_SYSINFDESC", STR_SYSINFDESC);
	language("STR_QUIT", STR_QUIT);
	language("STR_QUITDESC1", STR_QUITDESC1);
	language("STR_QUITDESC2", STR_QUITDESC2);
	language("STR_RESTSYS", STR_RESTSYS);
	language("STR_RESTSYSDESC1", STR_RESTSYSDESC1);
	language("STR_RESTSYSDESC2", STR_RESTSYSDESC2);
	language("STR_RESTSYS_SUB1", STR_RESTSYS_SUB1);
	language("STR_RESTSYS_SUB2", STR_RESTSYS_SUB2);
	language("STR_BOOTOOS", STR_BOOTOOS);
	language("STR_BOOTOOSDESC", STR_BOOTOOSDESC);
	language("STR_BOOTOOS_SUB1", STR_BOOTOOS_SUB1);
	language("STR_BOOTOOS_SUB2", STR_BOOTOOS_SUB2);
	language("STR_TRNOFF", STR_TRNOFF);
	language("STR_TRNOFFDESC", STR_TRNOFFDESC);
	language("STR_QA", STR_QA);
	language("STR_QADESC", STR_QADESC);
	language("STR_NOTAV", STR_NOTAV);
	language("STR_TOGREC", STR_TOGREC);
	language("STR_TOGRECDESC", STR_TOGRECDESC);
	language("STR_TOGPM", STR_TOGPM);
	language("STR_TOGPMDESC", STR_TOGPMDESC);
	language("STR_LOADLV2", STR_LOADLV2);
	language("STR_LOADLV2DESC", STR_LOADLV2DESC);
	language("STR_BUT", STR_BUT);
	language("STR_BUTDESC", STR_BUTDESC); 
	language("STR_CIRCLE", STR_CIRCLE);
	language("STR_CROSS", STR_CROSS);
	language("STR_XREG", STR_XREG);
	language("STR_XREGDESC", STR_XREGDESC);
	language("STR_BACKUP", STR_BACKUP);
	language("STR_RESTORE", STR_RESTORE);
	language("STR_RESIZEVFLASH", STR_RESIZEVFLASH);
	language("STR_RESIZEVFLASHDESC", STR_RESIZEVFLASHDESC);
	language("STR_INSTPETBOOT", STR_INSTPETBOOT);
	language("STR_INSTPETBOOTDESC", STR_INSTPETBOOTDESC);
	language("STR_GAMEOSBOOTFLG", STR_GAMEOSBOOTFLG);
	language("STR_GAMEOSBOOTFLGDESC", STR_GAMEOSBOOTFLGDESC);
	language("STR_PKG", STR_PKG);
	language("STR_PKGDESC", STR_PKGDESC);
	language("STR_SAVLV1", STR_SAVLV1);
	language("STR_SAVLV1DESC", STR_SAVLV1DESC);
	language("STR_SAVLV2", STR_SAVLV2);
	language("STR_SAVLV2DESC", STR_SAVLV2DESC);
	language("STR_SAVFLASH", STR_SAVFLASH);
	language("STR_SAVFLASHDESC", STR_SAVFLASHDESC);
	language("STR_SAVEIDRK", STR_SAVEIDRK);
	language("STR_SAVEIDRKDESC", STR_SAVEIDRKDESC);
	language("STR_PS3ID", STR_PS3ID);
	language("STR_PS3IDDESC", STR_PS3IDDESC);
	language("STR_USE", STR_USE);
	language("STR_SM", STR_SM);
	language("STR_SMDESC", STR_SMDESC);
	language("STR_XMBOMDESC", STR_XMBOMDESC);
	language("STR_MENU", STR_MENU);
	language("STR_MENUDESC", STR_MENUDESC);
	language("STR_TOGCFW", STR_TOGCFW);
	language("STR_TOGCFWDESC", STR_TOGCFWDESC);
	language("STR_TOGCOB", STR_TOGCOB);
	language("STR_TOGCOBDESC", STR_TOGCOBDESC);
	language("STR_TOGPS2", STR_TOGPS2);
	language("STR_TOGPS2DESC", STR_TOGPS2DESC);
	language("STR_TOGWM", STR_TOGWM);
	language("STR_TOGWMDESC", STR_TOGWMDESC);
	language("STR_NORBG", STR_NORBG);
	language("STR_NORBGDESC", STR_NORBGDESC); 
	language("STR_PATCHLV1", STR_PATCHLV1); 
	language("STR_PATCHLV1DESC", STR_PATCHLV1DESC); 
	language("STR_LV2MEMPRO", STR_LV2MEMPRO); 
	language("STR_LV2MEMPRODESC", STR_LV2MEMPRODESC); 
	language("STR_HTAB", STR_HTAB); 
	language("STR_HTABDESC", STR_HTABDESC); 
	language("STR_INDI", STR_INDI);
	language("STR_INDIDESC", STR_INDIDESC);
	language("STR_EEPROM", STR_EEPROM);
	language("STR_EEPROMDESC", STR_EEPROMDESC);
	language("STR_DISPATCH", STR_DISPATCH);
	language("STR_DISPATCHDESC", STR_DISPATCHDESC);
	language("STR_ENCDEC", STR_ENCDEC);
	language("STR_ENCDECDESC", STR_ENCDECDESC);
	language("STR_SYSMAN", STR_SYSMAN);
	language("STR_SYSMANDESC", STR_SYSMANDESC);
	language("STR_ALLPKG", STR_ALLPKG);
	language("STR_ALLPKGDESC", STR_ALLPKGDESC);
	language("STR_LPAR", STR_LPAR);
	language("STR_LPARDESC", STR_LPARDESC);
	language("STR_LV1SPE", STR_LV1SPE);
	language("STR_LV1SPEDESC", STR_LV1SPEDESC);
	language("STR_LV1DABR", STR_LV1DABR);
	language("STR_LV1DABRDESC", STR_LV1DABRDESC);
	language("STR_LV1GART", STR_LV1GART);
	language("STR_LV1GARTDESC", STR_LV1GARTDESC);
	language("STR_LV1KEYS", STR_LV1KEYS);
	language("STR_LV1KEYSDESC", STR_LV1KEYSDESC);
	language("STR_LV1ACL", STR_LV1ACL);
	language("STR_LV1ACLDESC", STR_LV1ACLDESC);
	language("STR_LV1GO", STR_LV1GO);
	language("STR_LV1GODESC", STR_LV1GODESC);
	language("STR_SWPLV2K", STR_SWPLV2K);
	language("STR_SWPLV2KDESC", STR_SWPLV2KDESC);
	language("STR_REWTID", STR_REWTID);
	language("STR_REWTIDDESC", STR_REWTIDDESC);
	language("STR_NOCEXDEX", STR_NOCEXDEX);
	language("STR_NOCEXDEXDESC", STR_NOCEXDEXDESC);
	language("STR_SELECT", STR_SELECT);
	language("STR_HOLDQUITREB", STR_HOLDQUITREB);
	language("STR_HOLDQUIT", STR_HOLDQUIT);
	language("STR_FREEHDD", STR_FREEHDD);
	language("STR_NUMFW", STR_NUMFW);
	language("STR_IP", STR_IP);
	language("STR_QUITXMB", STR_QUITXMB);
	language("STR_CHANGEIDPSOK1", STR_CHANGEIDPSOK1);
	language("STR_CHANGEIDPSOK2", STR_CHANGEIDPSOK2);
	language("STR_CHANGEIDPSERROR", STR_CHANGEIDPSERROR);
	language("STR_SWP2DEXOK", STR_SWP2DEXOK);
	language("STR_SWP2CEXOK", STR_SWP2CEXOK);
	language("STR_SWPERROR", STR_SWPERROR);
	language("STR_NEEDRESTSYS", STR_NEEDRESTSYS);
	language("STR_WEIDERROR1", STR_WEIDERROR1);
	language("STR_WEIDERROR2", STR_WEIDERROR2);
	language("STR_WEIDRUSURE1", STR_WEIDRUSURE1);
	language("STR_WEIDRUSURE2", STR_WEIDRUSURE2);
	language("STR_WEIDRUSURE3", STR_WEIDRUSURE3);
	language("STR_WEIDRUSURE4", STR_WEIDRUSURE4);
	language("STR_WEIDRRUUSURE", STR_WEIDRRUUSURE);
	language("STR_WEIDOK1", STR_WEIDOK1);
	language("STR_WEIDOK2", STR_WEIDOK2);
	language("STR_WEIDERROR3", STR_WEIDERROR3);
	language("STR_WEIDERROR4", STR_WEIDERROR4);
	language("STR_WTDMSG1", STR_WTDMSG1);
	language("STR_WTDMSG2", STR_WTDMSG2);
	language("STR_WTDMSG3", STR_WTDMSG3);
	language("STR_WTDMSG4", STR_WTDMSG4);
	language("STR_WTDMSG5", STR_WTDMSG5);
	language("STR_WTDMSG6", STR_WTDMSG6);
	language("STR_WTDMSG7", STR_WTDMSG7);
	language("STR_WTDMSG8", STR_WTDMSG8);
	language("STR_WTDMSG9", STR_WTDMSG9);
	language("STR_MAINNORBG", STR_MAINNORBG);
	language("STR_MAINNOCOB", STR_MAINNOCOB);
	language("STR_MAINNOFW", STR_MAINNOFW);
	language("STR_MAINERRFLASH", STR_MAINERRFLASH);
	language("STR_SCRSHTSAVED", STR_SCRSHTSAVED);
	language("STR_SAVXREG", STR_SAVXREG);
	language("STR_SAVXREGOK", STR_SAVXREGOK);
	language("STR_SAVXREGERR1", STR_SAVXREGERR1);
	language("STR_SAVXREGERR2", STR_SAVXREGERR2);
	language("STR_SAVXREGERR3", STR_SAVXREGERR3);
	language("STR_RESXREG", STR_RESXREG);
	language("STR_RESXREGOK", STR_RESXREGOK);
	language("STR_RESXREGERR1", STR_RESXREGERR1);
	language("STR_RESXREGERR2", STR_RESXREGERR2);
	language("STR_MAKPKGOK", STR_MAKPKGOK);
	language("STR_MAKPKGERR", STR_MAKPKGERR);
	language("STR_LOADLV2KERR1", STR_LOADLV2KERR1);
	language("STR_LOADLV2K", STR_LOADLV2K);
	language("STR_LOADLV2KNOSPACE", STR_LOADLV2KNOSPACE);
	language("STR_LOADLV2KCHKLV1", STR_LOADLV2KCHKLV1);
	language("STR_LOADLV2KERR2", STR_LOADLV2KERR2);
	language("STR_BOOTOOSMSG1", STR_BOOTOOSMSG1);
	language("STR_BOOTOOSMSG2", STR_BOOTOOSMSG2);
	language("STR_BOOTOOSERR1", STR_BOOTOOSERR1);
	language("STR_SWPKERR", STR_SWPKERR);
	language("STR_DMPFLSH", STR_DMPFLSH);
	language("STR_DMPFLSHERR", STR_DMPFLSHERR);
	language("STR_EXPLVMSG1", STR_EXPLVMSG1);
	language("STR_EXPLVMSG2", STR_EXPLVMSG2);
	language("STR_EXPLVWAIT", STR_EXPLVWAIT);
	language("STR_EXPLVOK", STR_EXPLVOK);
	language("STR_EXPLVERR", STR_EXPLVERR);
	language("STR_EXPLVMSG3", STR_EXPLVMSG3);
	language("STR_EXPLVMSG5", STR_EXPLVMSG5);
	language("STR_EXPLVMSG4", STR_EXPLVMSG4);
	language("STR_EXPLVOK2", STR_EXPLVOK2);
	language("STR_EXPLVERR2", STR_EXPLVERR2);
	language("STR_MODNORM", STR_MODNORM);
	language("STR_TRYAGAIN", STR_TRYAGAIN);
	language("STR_MODNORMALRD", STR_MODNORMALRD);
	language("STR_MODREB", STR_MODREB);
	language("STR_MODREBALRD", STR_MODREBALRD);
	language("STR_WMDIS", STR_WMDIS);
	language("STR_WMENA", STR_WMENA);
	language("STR_MODRET", STR_MODRET);
	language("STR_MODRETERR1", STR_MODRETERR1);
	language("STR_MODRETERR2", STR_MODRETERR2);
	language("STR_MODDEB", STR_MODDEB);
	language("STR_MODDEBERR1", STR_MODDEBERR1);
	language("STR_MODDEBERR2", STR_MODDEBERR2);
	language("STR_MODDEBSQA", STR_MODDEBSQA);
	language("STR_MODDEBSQAOK", STR_MODDEBSQAOK);
	language("STR_MODDEBSQAERR1", STR_MODDEBSQAERR1);
	language("STR_MODDEBSQAERR2", STR_MODDEBSQAERR2);
	language("STR_MODDEBSDEB", STR_MODDEBSDEB);
	language("STR_MODDEBSDEBOK", STR_MODDEBSDEBOK);
	language("STR_MODDEBSDEBERR1", STR_MODDEBSDEBERR1);
	language("STR_MODDEBSDEBERR2", STR_MODDEBSDEBERR2);
	language("STR_MODCOBENA", STR_MODCOBENA);
	language("STR_MODCOBDIS", STR_MODCOBDIS);
	language("STR_ERRFLASH", STR_ERRFLASH);
	language("STR_PS2ERR1", STR_PS2ERR1);
	language("STR_PS2CONT", STR_PS2CONT);
	language("STR_PS2COP", STR_PS2COP);
	language("STR_PS2SWPOK", STR_PS2SWPOK);
	language("STR_XMBCFWDIS", STR_XMBCFWDIS);
	language("STR_XMBCFWENA", STR_XMBCFWENA);
	language("STR_FLASHSET", STR_FLASHSET);
	language("STR_LANG", STR_LANG);
	language("STR_LANGDESC", STR_LANGDESC);
    language("STR_1", STR_1);
	language("STR_2", STR_2);
	language("STR_3", STR_3);
	language("STR_4", STR_4);
	language("STR_5", STR_5);
	language("STR_READFLASHERROR1", STR_READFLASHERROR1);
	language("STR_READFLASHERROR2", STR_READFLASHERROR2);
	language("STR_TOGHOSTINF", STR_TOGHOSTINF);
	language("STR_TOGHOSTINFDESC", STR_TOGHOSTINFDESC);
	
	if(fh) {cellFsClose(fh); lang_pos=fh=0;}
}


static int sys_get_version(uint32_t *version)
{
 system_call_2(8, SYSCALL8_OPCODE_GET_VERSION, (uint64_t)(uint32_t)version);
 return (int)p1;
}

static int sys_get_version2(uint16_t *version)
{
 system_call_2(8, SYSCALL8_OPCODE_GET_VERSION2, (uint64_t)(uint32_t)version);
 return (int)p1;
}

int cobra_get_version(uint16_t *cobra_version, uint16_t *ps3_version)
{
 uint32_t version1;
 uint16_t version2;
 int ret;

 ret = sys_get_version(&version1);
 if (ret != 0)
  return ret;

 if (cobra_version && sys_get_version2(&version2) == 0)
 {
  *cobra_version = version2;
 }
 else if (cobra_version)
 {
  switch (version1&0xFF)
  {
   case 1:
    *cobra_version = 0x0102;
   break;

   case 2:
    *cobra_version = 0x0200;
   break;

   case 3:
    *cobra_version = 0x0300;
   break;

   case 4:
    *cobra_version = 0x0310;
   break;

   case 5:
    *cobra_version = 0x0320;
   break;

   case 6:
    *cobra_version = 0x0330;
   break;

   case 7:
    *cobra_version = 0x0400;
   break;

   default:
    *cobra_version = 0x0410;
   break;
  }
 }

 if (ps3_version)
 {
  *ps3_version = ((version1>>8)&0xFFFF);

  if (*ps3_version == 0x0000)
  {
   *ps3_version = 0x0341;
  }
 }

 return 0;
}

u32 COL_XMB_CLOCK=0xffd0d0d0;
u32 COL_XMB_COLUMN=0xf0e0e0e0;
u32 COL_XMB_TITLE=0xf0e0e0e0;
u32 COL_XMB_SUBTITLE=0xf0909090;
u8	XMB_SPARK_SIZE=4;
u32 XMB_SPARK_COLOR=0xffffff00;

//#define USE_DEBUG 1

#ifdef USE_DEBUG
// logger
void logger_init();
void net_send(const char *__format, ...);
	#define MM_LOG(...) net_send(__VA_ARGS__);
#else
	#define MM_LOG(...) {}//printf(__VA_ARGS__);
#endif

#define printf(...) MM_LOG(__VA_ARGS__);

typedef uint64_t u64;

//memory info
typedef struct {
	uint32_t total;
	uint32_t avail;
} _meminfo;
_meminfo meminfo;

bool reset_settings=0;
u8 reset_mode=0;
int bounce=0;

CellGcmSurface main_surface[2];
void *color_base_addr;
volatile u32 frame_index = 0;
u32 video_buffer;
int V_WIDTH, V_HEIGHT;//, _V_WIDTH, _V_HEIGHT;
u8 video_mode=1;
int vert_indx=0, vert_texture_indx=0;
void flip(void);

system_time_t double_click_timer=0;

// for network and file_copy buffering
#define BUF_SIZE				(3 * 1024 * 1024)

//folder copy
#define MAX_FAST_FILES			1
#define MAX_FAST_FILE_SIZE		(3 * 1024 * 1024) //used x3 = 9MB

#define MEMORY_CONTAINER_SIZE_KB	( 2 * 1024 * 1024) // for OSK

enum {
	CALLBACK_TYPE_INITIALIZE = 0,
	CALLBACK_TYPE_REGIST_1,
	CALLBACK_TYPE_FINALIZE
};

static sys_memory_container_t memory_container;

void pokeq( uint64_t addr, uint64_t val);
uint64_t peekq(uint64_t addr);
uint64_t peek_lv1_cobra(uint64_t addr);

void save_options();
void shutdown_system(u8 mode);

u8 rex_compatible=1;
u8 rebug_compatible=1;
u8 cobra_compatible=1;
u8 rebug_checked=0;
u8 cobra_checked=0;
u8 auto_reboot=0;
u8 otheros=0;
u8 reboot=0;
u8 lv2_kernel=0;  // selected
u8 lv2_kernels=0; // total
//u8 dump_mode = 0;

#define MAX_LV2_KERNELS 10
typedef struct
{
	char 	name[128];
	char 	path[512];
}
lv2_files;
lv2_files lv2_kernel_files[MAX_LV2_KERNELS];

void export_lv(u8 _mode);
void create_packages();
void load_lv2_kernel(u8 _sel);
void boot_otherOS(u8 _mode);
void dump_flash();
void swap_kernel();
void check_settings();
void apply_settings(char *option, int val, u8 _forced);
void change_lv1_um(u8 val);
void change_lv1_dm(u8 val);
void set_xReg();
void set_xo();

u64 idps0=0;
u64 idps1=0;
u8 get_idps(u8 _eid);
void set_idps(u8 _val);


u8 get_eid();

void write_to_device(u64 device, u32 _flags, char *_path);

u8 in_xmb_mode();
//void mip_texture( uint8_t *buffer_to, uint8_t *buffer_from, uint32_t width, uint32_t height, int scaleF);
//void blur_texture(uint8_t *buffer_to, uint32_t width, uint32_t height, int x, int y,  int wx, int wy, uint32_t c_BRI, int use_grayscale, int iterations, int p_range);

void print_label(float x, float y, float scale, uint32_t color, char *str1p, float weight, float slant, int font);
void print_label_ex(float x, float y, float scale, uint32_t color, char *str1p, float weight, float slant, int font, float hscale, float vscale, int centered);
void flush_ttf(uint8_t *buffer, uint32_t _V_WIDTH, uint32_t _V_HEIGHT);

void show_sysinfo();

static u32 get_crc(char *_path);

void file_copy(char *path, char *path2, int progress);

int load_png_texture(u8 *data, char *name, uint16_t _DW);
void change_opacity(u8 *buffer, int delta, u32 size);
//void change_bri(u8 *buffer, int delta, u32 size);

void read_flash(u64 device, u32 _flags, char *iso_path);

void exit_app();
void quit_app();
void draw_stars();
void draw_popups();
void load_xmb_bg();

void draw_whole_xmb(u8 mode);
void draw_xmb_bg();
void draw_xmb_clock(u8 *buffer, const int _xmb_icon);
void draw_xmb_icon_text(int _xmb_icon);
void draw_xmb_bare(u8 _xmb_icon, u8 _all_icons, bool recursive, int _sub_level);
void init_xmb_icons();
void init_slider();

void redraw_column_texts(int _xmb_icon);
void reset_xmb();
void free_all_buffers();
void free_text_buffers();

void draw_status_bar(u8 win);
void init_misc_icons();

volatile bool is_decoding_jpg=0;
//void load_jpg_threaded(int _xmb_icon, int cn);

volatile bool is_decoding_png=0;
//void load_png_threaded(int _xmb_icon, int cn);

//static void jpg_thread_entry( uint64_t arg );
//sys_ppu_thread_t jpgdec_thr_id;

//static void png_thread_entry( uint64_t arg );
//sys_ppu_thread_t pngdec_thr_id;

const int32_t misc_thr_prio  = 1600;
const size_t app_stack_size  = 65536;

volatile u8 is_any_xmb_column=0;
volatile u8 drawing_xmb=0;

volatile float angle=0.f;
u8 a_dynamic=0;
u8 a_dynamic2=0;

bool debug_mode=false;
u8 background_type=0;

bool side_menu_open=false;
u8 side_menu_color_indx=6;
u8 top_menu_color_indx=6;
u32 side_menu_color[13]={0x54524a00, 0x231d7c00, 0x7c1d2a00, 0x2a7c1d00, 0xcd480600, 0x571d7c00, 0x30303000,
						 0x3961be00, 0xbe393900, 0x1d457c00, 0x06599400, 0x06943f00, 0x06599400};
u32 top_menu_color[13]={0x54524a00, 0x231d7c00, 0x7c1d2a00, 0x2a7c1d00, 0xcd480600, 0x571d7c00, 0x30303000,
						 0x3961be00, 0xbe393900, 0x1d457c00, 0x06599400, 0x06943f00, 0x30303000};

int sub_menu_open=0;
int pb_step=429;

int repeat_init_delay=20;
int repeat_key_delay=4;
int repeat_counter1=repeat_init_delay; //wait before repeat
int repeat_counter2=repeat_key_delay; // repeat after pause

u8 repeat_counter3=1; // accelerate repeat (multiplier)
float repeat_counter3_inc=0.f;

int repeat_counter1_t[7]; //wait before repeat
int repeat_counter2_t[7]; // repeat after pause
u8 repeat_counter3_t[7];
float repeat_counter3_inc_t[7];

bool key_repeat=0;
bool key_repeat_t[7];

static char time_result[16];
static char cat_result[512];
u64 seconds_clock=0;
bool xmb_legend_drawn=0;
bool xmb_info_drawn=0;
bool use_analog=0;
bool join_copy=0;
u8 xmb_settings_sel=0;

char d1[512], d2[512], df[512];

long long int last_refresh=0;
long long int last_refresh2=0;

bool screen_saver_mode=0;
char status_info[256];

int dim=0, dimc=0;
int c_opacity=0xff, c_opacity_delta=-1;
int c_opacity2=0xff;
int b_box_opaq= 0xf8;
int b_box_step= -4;

float c_firmware=3.55f;

bool use_symlinks=0;
u8 ftp_service=0;
volatile u8 ftp_clients=0;
bool http_active=false;

time_t time_start;

//FONTS
typedef struct SampleRenderTarget {
	CellFontRenderer      Renderer;
	CellFontRenderSurface Surface;
}SampleRenderWork;

static const CellFontLibrary* freeType;
static Fonts_t* fonts;
static SampleRenderWork RenderWork;

int legend_y=760, legend_h=96, last_selected, rnd;

char current_version[25]=STR_APP_VER;
char current_version_NULL[30];

u16 dox_width=256;
u16 dox_height=256;

u16 dox_cross_x=15;
u16 dox_cross_y=14;
u16 dox_cross_w=34;
u16 dox_cross_h=34;

u16 dox_circle_x=207;
u16 dox_circle_y=14;
u16 dox_circle_w=34;
u16 dox_circle_h=34;


u16 dox_triangle_x=80;
u16 dox_triangle_y=14;
u16 dox_triangle_w=34;
u16 dox_triangle_h=34;

u16 dox_square_x=143;
u16 dox_square_y=14;
u16 dox_square_w=36;
u16 dox_square_h=34;

u16 dox_start_x=12;
u16 dox_start_y=106;
u16 dox_start_w=42;
u16 dox_start_h=36;

u16 dox_select_x=72;
u16 dox_select_y=108;
u16 dox_select_w=50;
u16 dox_select_h=34;

u16 dox_ls_x=132;
u16 dox_ls_y=70;
u16 dox_ls_w=56;
u16 dox_ls_h=56;

u16 dox_rs_x=196;
u16 dox_rs_y=196;
u16 dox_rs_w=58;
u16 dox_rs_h=56;

u16 dox_pad_x=7;
u16 dox_pad_y=200;
u16 dox_pad_w=53;
u16 dox_pad_h=53;

u16 dox_l1_x=130;
u16 dox_l1_y=143;
u16 dox_l1_w=60;
u16 dox_l1_h=24;

u16 dox_r1_x=194;
u16 dox_r1_y=143;
u16 dox_r1_w=60;
u16 dox_r1_h=24;

u16 dox_l2_x=2;
u16 dox_l2_y=143;
u16 dox_l2_w=62;
u16 dox_l2_h=24;

u16 dox_r2_x=66;
u16 dox_r2_y=143;
u16 dox_r2_w=62;
u16 dox_r2_h=24;

u16 dox_l3_x=68;
u16 dox_l3_y=196;
u16 dox_l3_w=58;
u16 dox_l3_h=56;

u16 dox_r3_x=132;
u16 dox_r3_y=196;
u16 dox_r3_w=58;
u16 dox_r3_h=56;

//white circle
u16 dox_rb1u_x=192;
u16 dox_rb1u_y=70;
u16 dox_rb1u_w=32;
u16 dox_rb1u_h=31;

//white circle selected
u16 dox_rb1s_x=192;
u16 dox_rb1s_y=101;
u16 dox_rb1s_w=32;
u16 dox_rb1s_h=31;

//gray circle
u16 dox_rb2u_x=224;
u16 dox_rb2u_y=70;
u16 dox_rb2u_w=31;
u16 dox_rb2u_h=31;

//gray circle selected
u16 dox_rb2s_x=224;
u16 dox_rb2s_y=101;
u16 dox_rb2s_w=31;
u16 dox_rb2s_h=31;

//selection circle
u16 dox_rb3s_x=177;
u16 dox_rb3s_y=40;
u16 dox_rb3s_w=31;
u16 dox_rb3s_h=30;

//attention sign
u16 dox_att_x=1;
u16 dox_att_y=65;
u16 dox_att_w=44;
u16 dox_att_h=39;

//white arrow
u16 dox_arrow_w_x=44;
u16 dox_arrow_w_y=41;
u16 dox_arrow_w_w=44;
u16 dox_arrow_w_h=44;

//black arrow
u16 dox_arrow_b_x=87;
u16 dox_arrow_b_y=58;
u16 dox_arrow_b_w=44;
u16 dox_arrow_b_h=44;

static int unload_modules();
void draw_text_stroke(float x, float y, float size, u32 color, const char *str);

char app_path[32];
char app_temp[128];
char app_usrdir[64];

char app_img[96];
char app_homedir[64];

char options_bin[128];

int date_format=0; // 1=MM/DD/YYYY, 2=YYYY/MM/DD
int time_format=1; // 0=12h 1=24h
int progress_bar=1;
int dim_setting=5; //5 seconds to dim titles
int ss_timeout=2;
int sao_timeout=1;
int ss_timer=0;

int ss_timer_last=0;
int clear_activity_logs=1;

int lock_display_mode=-1;

char fm_func[32];
int xmb_sparks=1;
int xmb_popup=1;
u8 confirm_with_x=1;
u8 show_temp=2, show_temp2=0;

volatile int scale_icon_h=0;

#define	BUTTON_SELECT		(1<<0)
#define	BUTTON_L3			(1<<1)
#define	BUTTON_R3			(1<<2)
#define	BUTTON_START		(1<<3)
#define	BUTTON_UP			(1<<4)
#define	BUTTON_RIGHT		(1<<5)
#define	BUTTON_DOWN			(1<<6)
#define	BUTTON_LEFT			(1<<7)
#define	BUTTON_L2			(1<<8)
#define	BUTTON_R2			(1<<9)
#define	BUTTON_L1			(1<<10)
#define	BUTTON_R1			(1<<11)
#define	BUTTON_TRIANGLE		(1<<12)
#define	_BUTTON_CIRCLE		(1<<13)
#define	_BUTTON_CROSS		(1<<14)
#define	BUTTON_SQUARE		(1<<15)

#define	BUTTON_PAUSE		(1<<16)
#define	BUTTON_RED			(1<<17)

u16 BUTTON_CROSS =	_BUTTON_CROSS;
u16 BUTTON_CIRCLE=	_BUTTON_CIRCLE;

volatile u8 init_finished=0;
volatile u8 app_shutdown=0;
volatile u8 restart_request;
volatile bool unload_called=0;
volatile bool initAIO_called=0;

//volatile bool canDraw=true;

//char userBG[64];
static char auraBG[128];
char avchdBG[64];
char blankBG[64];
char playBG[64];
char legend[64];
char xmbicons[64];
//char xmbicons2[64];

char xmbbg[64];
int  xmbbg_user_w=1920;
int  xmbbg_user_h=1080;

int abort_rec=0;

	u8 *text_bmp=NULL;
	u8 *text_bmpS=NULL;
	u8 *text_bmpUBG=NULL;

	u8 *text_USB=NULL;
	u8 *text_HDD=NULL;
	u8 *text_BLU_1=NULL;
	u8 *text_NET_6=NULL;
	u8 *text_OFF_2=NULL;
	u8 *text_FMS=NULL;

	u8 *text_DOX=NULL;
	u8 *text_MSG=NULL;
	u8 *text_INFO=NULL;

	u8 *text_CFC_3=NULL;
	u8 *text_SDC_4=NULL;
	u8 *text_MSC_5=NULL;
	u8 *text_bmpUPSR=NULL;
	u8 *text_bmpIC;
	u8 *text_TEMP;
	u8 *text_DROPS;
	u8 *text_SLIDER;
	u8 *text_legend;
	u8 *text_DEVS;
	u8 *text_TEXTS;
	u8 *text_FONT;

	u8* BORDER_TL=NULL;
	u8* BORDER_BL=NULL;
	u8* BORDER_TR=NULL;
	u8* BORDER_BR=NULL;
	u8* BORDER_LT=NULL;
	u8* BORDER_RB=NULL;
	u8* BORDER_SS=NULL;

	u8* BORDER_TL2=NULL;
	u8* BORDER_BL2=NULL;
	u8* BORDER_TR2=NULL;
	u8* BORDER_BR2=NULL;
	u8* BORDER_LT2=NULL;
	u8* BORDER_RB2=NULL;
	u8* BORDER_SS2=NULL;

	u8 win_opened=0;
	u16	win_number=0;

	u8* BORDER_PATH_BOX_A=NULL;
	u8* BORDER_PATH_BOX_I=NULL;

	//u8* text_TASKBAR=NULL;

	u8 *text_PBOX;
	u8 *text_PBOX1;

	u8 *text_CBOX;
	u8 *text_GBOX;

	u8 *xmb_col;
	u8 *xmb_clock;

	u8 *xmb_icon_home	=	NULL;
	u8 *xmb_icon_refresh=	NULL;
	u8 *xmb_icon_off	=	NULL;
	u8 *xmb_icon_info	=	NULL;
	u8 *xmb_icon_util	=	NULL;

	u8 *xmb_icon_globe	=	NULL;
	u8 *xmb_icon_help	=	NULL;
	u8 *xmb_icon_quit	=	NULL;
	u8 *xmb_icon_star	=	NULL;
	u8 *xmb_icon_star_small = NULL;
	u8 *xmb_icon_blu_small = NULL;
	u8 *xmb_icon_net_small = NULL;

	u8 *xmb_icon_retro	=	NULL;
	u8 *xmb_icon_ftp	=	NULL;
	u8 *xmb_icon_folder	=	NULL;
	u8 *xmb_icon_usb	=	NULL;
	u8 *xmb_icon_psx	=	NULL;
	u8 *xmb_icon_ps2	=	NULL;
	u8 *xmb_icon_blend	=	NULL;
	u8 *xmb_icon_psp	=	NULL;
	u8 *xmb_icon_psp2	=	NULL;
	u8 *xmb_icon_dvd	=	NULL;
	u8 *xmb_icon_bdv	=	NULL;

	u8 *xmb_icon_psx_n	=	NULL;
	u8 *xmb_icon_ps2_n	=	NULL;
	u8 *xmb_icon_psp_n	=	NULL;
	u8 *xmb_icon_dvd_n	=	NULL;
	u8 *xmb_icon_bdv_n	=	NULL;


	u8 *xmb_icon_desk	=	NULL;
	u8 *xmb_icon_hdd	=	NULL;
	u8 *xmb_icon_blu	=	NULL;
	u8 *xmb_icon_blu_n	=	NULL;
	u8 *xmb_icon_tool	=	NULL;
	u8 *xmb_icon_note	=	NULL;
	u8 *xmb_icon_film	=	NULL;
	u8 *xmb_icon_photo	=	NULL;
	u8 *xmb_icon_update	=	NULL;
	u8 *xmb_icon_usb_update=NULL;
	u8 *xmb_icon_logo	=	NULL;

	u8 *xmb_icon_ss		=	NULL;
	u8 *xmb_icon_showtime=	NULL;
	u8 *xmb_icon_theme	=	NULL;
	u8 *xmb_icon_arrow	=	NULL;


FILE *fpV;
int do_move=0;

bool pp_enabled=true; //peek/poke

u8 bdisk_mode=0;
u8 dex_mode=0;
u8 dex_flash=0;

int payload=0;
char payloadT[2];

int      portNum = -1;

//OSK
CellOskDialogCallbackReturnParam OutputInfo;
CellOskDialogInputFieldInfo inputFieldInfo;
uint16_t Result_Text_Buffer[128 + 1];
int enteredCounter = 0;
char new_file_name[1024];

 u8 mount_dev_blind=0;

 u8 animation=3;
 float overscan=0.0f;
 bool is_remoteplay=0;


#define MAX_LIST 960

#define MAX_LIST_OPTIONS 128
typedef struct
{
	u32		color;
	char 	label[64];
	char 	value[512];
}
t_opt_list;
t_opt_list opt_list[MAX_LIST_OPTIONS];
u8 opt_list_max=0;

int open_dd_menu_xmb(char *_caption, int _width, t_opt_list *list, int _max, int _x, int _y, int _max_entries);
int open_side_menu(int _top, int sel);

#define MAX_PANE_SIZE 500
typedef struct
{
	u8 	type; //0-dir 1-file
	char 	name[128];
	char 	path[512];
	char	entry[20]; // __0+name for dirs and __1+name for files - used for sorting dirs first
	int64_t	size;
	time_t 	time;
	mode_t  mode;
	u8		selected;
}
t_dir_pane;
int max_dir_l=0;
int max_dir_r=0;

#define MAX_PANE_SIZE_BARE 500
typedef struct
{
	char 	name[128];
	char 	path[512];
}
t_dir_pane_bare;


volatile int draw_legend=1;

int file_counter=0; // to count files
int abort_copy=0; // abort process

typedef struct
{
	char 	label[256];
    float	x;
	float	y;
	float	scale;
	float	weight;
	float	slant;
	u8		font;
	float	hscale;
	float	vscale;
	u8		centered;
	float	cut;
	uint32_t color;
}
ttf_labels;

ttf_labels ttf_label[512];
int max_ttf_label=0;

int mode_list=0;
u32 forcedevices=0xffff;


u8 fm_sel=0;
u8 fm_sel_old=15;


int cover_mode=8, user_font=4;

u8 mm_locale=0;
u8 mui_font=4; // font for multilingual user interface

int net_available=0;
union CellNetCtlInfo net_info;
int net_avail=1;

int copy_file_counter=0;

int lastINC=0, lastINC3=0, lastINC2=0;

using namespace cell::Gcm;

static char xmb_columns [10] [32];

uint8_t padLYstick=0, padLXstick=0, padRYstick=0, padRXstick=0;

double mouseX=0.5f, mouseY=0.5f, mouseYD=0.0000f, mouseXD=0.0000f, mouseYDR=0.0000f, mouseXDR=0.0000f, mouseYDL=0.0000f, mouseXDL=0.0000f;
uint8_t xDZ=30, yDZ=30;
uint8_t xDZa=30, yDZa=30;

float offY=0.0f, BoffY=0.0f, offX=0.0f, incZ=0.7f, BoffX=0.0f, slideX=0.0f;

static void *host_addr;

int load_texture(u8 *data, char *name, uint16_t dw);

time_t rawtime;
struct tm * timeinfo;

int xmb_bg_show=0;
#define XMB_BG_COUNTER 50
volatile int xmb_bg_counter=XMB_BG_COUNTER;
volatile int xmb_bg_loaded=0;
char xmb_bg_path[128];

#define MAX_STARS 128
typedef struct
{
    u16		x;
	u16		y;
	u8		bri;
	u8		size;
}
stars_def;
stars_def stars[MAX_STARS];

// text_bmpUPSR
#define PSV_TEXT_WIDTH 720
#define PSV_TEXT_HEIGHT 74
#define XMB_TEXT_WIDTH 912
#define XMB_TEXT_HEIGHT 74
#define MAX_XMB_TEXTS (1920 * 1080 * 2) / (XMB_TEXT_WIDTH * XMB_TEXT_HEIGHT)
typedef struct __xmbtexts
{
	bool used;
	u8	*data; //pointer to image
}
xmbtexts __attribute__((aligned(8)));
xmbtexts xmb_txt_buf[MAX_XMB_TEXTS];
int xmb_txt_buf_max=0;

// text_bmpUBG
#define XMB_THUMB_WIDTH 416
#define XMB_THUMB_HEIGHT 414
#define MAX_XMB_THUMBS (1920 * 1080 * 1) / (XMB_THUMB_WIDTH * XMB_THUMB_HEIGHT)
typedef struct
{
	int used;
	int column;
	u8	*data; //pointer to image
}
xmbthumbs;
xmbthumbs xmb_icon_buf[MAX_XMB_THUMBS];
int xmb_icon_buf_max=0;

#define MAX_XMB_OPTIONS 16
typedef struct __xmbopt
{
	char label[36];
	char value[4];

}
xmbopt;// __attribute__((aligned(8)));


#define MAX_XMB_MEMBERS 100
typedef struct __xmbmem //1624 bytes per member
{
	u8		type;	// 0 Device/Folder
					// 1 PS3 Game				(structure)
					// 2 AVCHD/Blu-ray Video	(structure) (from Game List)

					// 3 Showtime Video			(file)
					// 4 Music
					// 5 Image

					// 6 Function
					// 7 Setting

	u8		status; // 0 Pending, 1 Loading, 2 Loaded
	bool	is_checked;
	int		game_id; //pointer to menu_list[id]
	u8		game_split;
	u32		game_user_flags;
	char	name[192];
	char	subname[128];
	u8		option_size;
	u8		option_selected;
	char	optionini[34];
	xmbopt	option[MAX_XMB_OPTIONS];
	int		data; //index in pointer array to text image stripe (xmbtexts) xmb_txt_buf
	int		icon_buf;
	u8		*icon; //pointer to icon image
	u16		iconw;
	u16		iconh;
	char	file_path[384]; //path to entry file
	char	icon_path[384]; //path to entry icon
}
xmbmem;// __attribute__((aligned(16)));

#define MAX_XMB_ICONS (6)
typedef struct
{
	u8		init;
    volatile u16		size;
	u16		first;
	u8		*data;
	char	name[32];
	u8		group;	// Bits 0-4: for genre/emulators: genres/retro_groups
					// Bits 7-5: for alphabetic grouping (alpha_groups)

	xmbmem	member[MAX_XMB_MEMBERS+3];
}
xmb_def;
xmb_def xmb[MAX_XMB_ICONS]; //xmb[0] - browse column

volatile u8 xmb_icon=1;
volatile u8 xmb0_icon=1;
u8 xmb_icon_last=2;
u16 xmb_icon_last_first=0;
int xmb_slide=0;
int xmb_slide_y=0;
int xmb_slide_step=0;
int xmb_slide_step_y=0;

u8 xmb_sublevel=0;
int xmb0_slide_y=0;
int xmb0_slide_step_y=0;
int xmb0_slide=0;
int xmb0_slide_step=0;

void draw_xmb_icons(xmb_def *_xmb, const int _xmb_icon, int _xmb_x_offset, int _xmb_y_offset, const bool _recursive, int sub_level, int _bounce);

void mod_xmb_member(xmbmem *_member, u16 _size, char *_name, char *_subname);

int message_clock=0;
volatile bool check_for_popups=0;

float angle_dynamic(float min, float max)   // angle global var changes each flip (0 to 359 degrees)
											// 3.6 degrees per flip
{
	if(angle<180.f)
		return (float)((angle/180.f) * (max-min))+min;
	else
		return (float)(((360.f/angle) - 1.f) * (max-min))+min;
}

char *tmhour(int _hour)
{
	int th=_hour;
	if(time_format) th=_hour;//sprintf(time_result, "%2d", _hour);
	else
	{
		if(_hour>11) th=_hour-12;
		if(!th) th=12;
	}
	sprintf(time_result, "%2d", th);
	return time_result;
}

char *string_cat(char* str1, const char* str2)
{
	cat_result[0]=0;
	strcat(cat_result, str1);
	strcat(cat_result, str2);
	return cat_result;
}

u8 in_xmb_mode()
{
	return 1;
}

void set_xo()
{
	if(confirm_with_x)
	{
		BUTTON_CROSS =	_BUTTON_CROSS;	 //(1<<12)
		BUTTON_CIRCLE=	_BUTTON_CIRCLE;	 //(1<<13)
		dox_cross_x=15;
		dox_cross_y=14;
		dox_cross_w=34;
		dox_cross_h=34;

		dox_circle_x=207;
		dox_circle_y=14;
		dox_circle_w=34;
		dox_circle_h=34;
	}
	else
	{
		BUTTON_CROSS =	_BUTTON_CIRCLE;
		BUTTON_CIRCLE=	_BUTTON_CROSS;

		dox_circle_x=15;
		dox_circle_y=14;
		dox_circle_w=34;
		dox_circle_h=34;

		dox_cross_x=207;
		dox_cross_y=14;
		dox_cross_w=34;
		dox_cross_h=34;

	}
}



static void callback_aio(CellFsAio *aio, CellFsErrno err, int id, uint64_t size)
{
	(void) id;
    if (err == CELL_FS_SUCCEEDED) {
        aio->offset+=size;
        aio->user_data=0;
    } else {
        aio->user_data=3;
    }
}

static void initAIO()
{
	if(initAIO_called) return;
	initAIO_called=1;

	char m_point[16];
	int dir_fd;
	uint64_t nread;
	CellFsDirent entryF;

	if(cellFsOpendir((char*) "/", &dir_fd)==CELL_FS_SUCCEEDED)// && dir_fd)
	{
		while(cellFsReaddir(dir_fd, &entryF, &nread)==CELL_FS_SUCCEEDED)
		{
			if(nread==0) break;

			sprintf(m_point, "/%s", entryF.d_name);
			if( (strcmp(entryF.d_name,"host_root") || (!strcmp(entryF.d_name,"host_root") && dex_mode==1)) && strncmp(entryF.d_name, "pvd_usb", 7) && strncmp(entryF.d_name, "dev_hdd", 7))
				cellFsAioInit(m_point);
		}
		cellFsClosedir(dir_fd);
	}
	initAIO_called=0;
}

int sys8_disable_all = 0;


int is_cobra_based(void)
{
    uint32_t version = 0x99999999;

    if (sys_get_version(&version) < 0)
        return 0;

    if (version != 0x99999999) // If value changed, it is cobra
        return 1;

    return 0;

}


static void scan_for_lv2_kernels(char* path)
{
	if(lv2_kernels>=(MAX_LV2_KERNELS-1)) return;
	int dir_fd;
	uint64_t nread;
	CellFsDirent entryF;

	if(cellFsOpendir(path, &dir_fd)==CELL_FS_SUCCEEDED)
	{
		while(cellFsReaddir(dir_fd, &entryF, &nread)==CELL_FS_SUCCEEDED)
		{
			if(nread==0) break;

			if(strstr(entryF.d_name, "lv2_kernel.self"))
			{
				if(lv2_kernels<(MAX_LV2_KERNELS-1))
				{
					sprintf(lv2_kernel_files[lv2_kernels].path, "%s/%s", path, entryF.d_name);
					if(!strcmp(entryF.d_name, "lv2_kernel.self"))
						snprintf(lv2_kernel_files[lv2_kernels].name, 32, "%s (%s)", entryF.d_name, path);
					else
					{
						snprintf(lv2_kernel_files[lv2_kernels].name, 32, "%s (%s)", entryF.d_name+15, path);
						for(u8 m=0; m<strlen(lv2_kernel_files[lv2_kernels].name)-1; m++)
							if(lv2_kernel_files[lv2_kernels].name[m]=='_' || lv2_kernel_files[lv2_kernels].name[m]=='.')
							lv2_kernel_files[lv2_kernels].name[m]=' ';
					}
					lv2_kernels++;
				}
			}
		}
		cellFsClosedir(dir_fd);
	}
}

/*****************************************************/
/* DIALOG                                            */
/*****************************************************/
#define CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF	(0<<7)
#define CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON	(1<<7)
#define CELL_MSGDIALOG_TYPE_PROGRESSBAR_DOUBLE	(2<<12)

int osk_dialog=0;
int osk_open=0;
volatile int dialog_ret=0;

u32 type_dialog_yes_no = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO
					   | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO;

u32 type_dialog_yes_back = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK
					   | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK;


u32 type_dialog_ok = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK
				   | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON| CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK;

u32 type_dialog_no = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON;
u32 type_dialog_back = CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF;

static void dialog_fun1( int button_type, void * )
{

	switch ( button_type ) {
	case CELL_MSGDIALOG_BUTTON_YES:
		dialog_ret=1;
		break;
	case CELL_MSGDIALOG_BUTTON_NO:
	case CELL_MSGDIALOG_BUTTON_NONE:
		dialog_ret=2;
		break;

	case CELL_MSGDIALOG_BUTTON_ESCAPE:
		dialog_ret=3;
		break;

	default:
		break;
	}
}
static void dialog_fun2( int button_type, void * )
{

	switch ( button_type ) {
		case CELL_MSGDIALOG_BUTTON_OK:
		case CELL_MSGDIALOG_BUTTON_NONE:
		dialog_ret=1;
		break;

	case CELL_MSGDIALOG_BUTTON_ESCAPE:
		dialog_ret=3;
		break;

	default:
		break;
	}
}

//void trap() { __asm__ ("trap");}

u32 new_pad=0, old_pad=0;

void pad_reset();
static int pad_read( void );

void wait_dialog_simple()
{

	while(!dialog_ret)
	{

		sys_timer_usleep(1668);
		flip();
	}

	ss_timer=0;
	ss_timer_last=time(NULL);
	cellMsgDialogClose(60.0f);
	setRenderColor();
}

void wait_dialog()
{
	if(cover_mode==5) {wait_dialog_simple();return;}

	while(!dialog_ret)
	{

		if(init_finished)
		{
			draw_whole_xmb(1);
		}
		else
			flip();
	}

	cellMsgDialogClose(60.0f);
	setRenderColor();
	pad_reset();
	ss_timer=0;
	ss_timer_last=time(NULL);
}


u64 is_size(char *path)
{
	struct CellFsStat s;
	if(cellFsStat(path, &s)==CELL_FS_SUCCEEDED)
		return s.st_size;
	else
		return 0;
}


int exist(char *path)
{
	struct stat p_stat;
	return (stat(path, &p_stat)>=0);
}


int exist_c(const char *path)
{
	struct stat p_stat;
	return (stat(path, &p_stat)>=0);
}

int rndv(int maxval)
{
	return (int) ((float)rand() * ((float)maxval / (float)RAND_MAX));
}

void get_date(char *_prefix)
{
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	sprintf(_prefix, "%04d%02d%02d-%02d%02d%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	return;
}

void flipc(int _fc)
{
	int flipF;
	for(flipF = 0; flipF<_fc; flipF++)
	{
		sys_timer_usleep(3336);

		flip();
	}
}

void open_please_wait()
{
	cellMsgDialogAbort();
	dialog_ret=0; cellMsgDialogOpen2( type_dialog_no, (const char*) "Please wait...", dialog_fun2, (void*)0x0000aaab, NULL );
	flipc(120);
	dialog_ret=0;
}

void get_free_memory()
{
	system_call_1(352, (uint64_t) &meminfo);
}


u32 new_pad_t[7];
u32 old_pad_t[7];
uint8_t old_status=0;
uint8_t old_status_k=0;
u8 pad_num=0;
u8 active_pads=0;
int last_pad=-1;
static u32 old_info[7];
static CellPadActParam _CellPadActParam[1];
bool use_motor=false;

u8 read_pad_info();
u8 read_pad_info_browse();

void pad_reset()
{
	for(int n=0;n<7;n++)
	{
		new_pad_t[n]=0;
		old_pad_t[n]=0;
		old_info [n]=0;
	}
	new_pad=0; old_pad=0;
}

static void pad_motor( u8 m1, u8 m2)
{
		_CellPadActParam[0].motor[0]=m1;
		_CellPadActParam[0].motor[1]=m2;
		_CellPadActParam[0].reserved[0]=0;
		_CellPadActParam[0].reserved[1]=0;
		_CellPadActParam[0].reserved[2]=0;
		_CellPadActParam[0].reserved[3]=0;
		_CellPadActParam[0].reserved[4]=0;
		_CellPadActParam[0].reserved[5]=0;
		for(u8 n=0;n<7;n++)
			cellPadSetActDirect(n, &_CellPadActParam[0]);
		sys_timer_usleep(250*1000);
		use_motor=false;
}

	u16 padSensorX;
	//u16 padSensorY;
	u16 padSensorZ;
	//u16 padSensorG;

static int pad_read( void )
{

static CellKbInfo info;
static CellKbData kdata;

	ss_timer=(time(NULL)-ss_timer_last);

	u32 padd;
	u32 dev_type=0;
	key_repeat=0;
	if(pad_num>6) pad_num=0;
	key_repeat_t[pad_num]=0;

	static CellPadData databuf;
	CellPadInfo2 infobuf;



uint32_t old_info_k = 0;

//check for keyboard input

		if(cellKbSetReadMode (0, CELL_KB_RMODE_INPUTCHAR) != CELL_KB_OK) goto read_mouse;
		if(cellKbSetCodeType (0, CELL_KB_CODETYPE_ASCII)  != CELL_KB_OK) goto read_mouse;
		if(cellKbGetInfo (&info) != CELL_KB_OK) goto read_mouse;

		if((info.info & CELL_KB_INFO_INTERCEPTED) &&
		   (!(old_info_k & CELL_KB_INFO_INTERCEPTED))){
			old_info_k = info.info;
		}else if((!(info.info & CELL_KB_INFO_INTERCEPTED)) &&
				 (old_info_k & CELL_KB_INFO_INTERCEPTED)){
			old_info_k = info.info;
		}
        if (info.status[0] == CELL_KB_STATUS_DISCONNECTED) goto read_mouse;
		if (cellKbRead (0, &kdata)!=CELL_KB_OK) goto read_mouse;
        if (kdata.len == 0) goto read_mouse;

//		sprintf(mouseInfo, "Keyboard: Buttons : %02X %c", kdata.keycode[0], kdata.keycode[0]);

        old_status_k = info.status[0];

		padd = 0;
		if(kdata.keycode[0]==0x8050) padd = padd | (1<<7); //LEFT
		else if(kdata.keycode[0]==0x804F) padd = padd | (1<<5); //RIGHT
		else if(kdata.keycode[0]==0x8052) padd = padd | (1<<4); //UP
		else if(kdata.keycode[0]==0x8051) padd = padd | (1<<6); //DOWN

		else if(kdata.keycode[0]==0xC050) padd = padd | (1<<7); //LEFT (NUM BLOCK)
		else if(kdata.keycode[0]==0xC04F) padd = padd | (1<<5); //RIGHT
		else if(kdata.keycode[0]==0xC052) padd = padd | (1<<4); //UP
		else if(kdata.keycode[0]==0xC051) padd = padd | (1<<6); //DOWN

		else if(kdata.keycode[0]==0x400A || kdata.keycode[0]==0x000A) padd = padd | (1<<14); //ENTER
		else if(kdata.keycode[0]==0x8029) padd = padd | (1<<12); //ESC->TRIANGLE

		else if(kdata.keycode[0]==0x8043) padd = padd | (1<<2); //F10 = R3
		else if(kdata.keycode[0]==0x8040 || kdata.keycode[0]==0x8039) padd = padd | (1<<1); //F7 = L3

		goto pad_out;

// check for mouse input

read_mouse:
	// no mouse support :)

	if ( cellPadGetInfo2(&infobuf) != 0 )
	{
		old_pad_t[pad_num] = new_pad_t[pad_num] = 0;
		old_pad = new_pad = 0;
		return 1;
	}

	active_pads=0;
	for(int n=0;n<7;n++)
		if ( infobuf.port_status[n] == CELL_PAD_STATUS_CONNECTED ) active_pads++;

	for(int n=pad_num;n<7;n++)
		if ( infobuf.port_status[n] == CELL_PAD_STATUS_CONNECTED ) {pad_num=n; goto pad_ok;}
	for(int n=0;n<pad_num;n++)
		if ( infobuf.port_status[n] == CELL_PAD_STATUS_CONNECTED ) {pad_num=n; goto pad_ok;}

	old_pad_t[pad_num] = new_pad_t[pad_num] = 0;
	old_pad = new_pad = 0;
	pad_num++;
	return 1;


pad_ok:

	if((infobuf.system_info & CELL_PAD_INFO_INTERCEPTED) && (!(old_info[pad_num] & CELL_PAD_INFO_INTERCEPTED)))
	{
		old_info[pad_num] = infobuf.system_info;
	}
	else
		if((!(infobuf.system_info & CELL_PAD_INFO_INTERCEPTED)) && (old_info[pad_num] & CELL_PAD_INFO_INTERCEPTED))
		{
			old_info[pad_num] = infobuf.system_info;
			old_pad_t[pad_num] = new_pad_t[pad_num] = 0;
			goto pad_repeat;
		}

	if (cellPadGetDataExtra( pad_num, &dev_type, &databuf ) != CELL_PAD_OK)
	{
		old_pad_t[pad_num] = new_pad_t[pad_num] = 0;
		old_pad=new_pad = 0;
		pad_num++;
		return 1;
	}

	if (databuf.len == 0)
	{
		new_pad_t[pad_num] = 0;
		goto pad_repeat;
	}

	padd = ( databuf.button[2] | ( databuf.button[3] << 8 ) ); //digital buttons
	padRXstick = databuf.button[4]; // right stick
	padRYstick = databuf.button[5];
	padLXstick = databuf.button[6]; // left stick
	padLYstick = databuf.button[7];

	if(screen_saver_mode)
	{
		cellPadSetPortSetting( pad_num, CELL_PAD_SETTING_SENSOR_ON); //CELL_PAD_SETTING_PRESS_ON |

		padSensorX = databuf.button[20];
		//padSensorY = databuf.button[21];
		padSensorZ = databuf.button[22];
		//padSensorG = databuf.button[23];
		if(!screen_saver_mode)
		{
			if(padSensorX>404 && padSensorX<424) padd|=BUTTON_L1;
			if(padSensorX>620 && padSensorX<640) padd|=BUTTON_R1;
		}
	}
	else
		cellPadSetPortSetting( pad_num, 0);

	if(dev_type==CELL_PAD_DEV_TYPE_BD_REMOCON)
	{
		if(databuf.button[25]==0x0b || databuf.button[25]==0x32) padd|=(1 << 14); //map BD remote [enter] and [play] to [X]
		else if(databuf.button[25]==0x81) padd|=BUTTON_RED;		//RED -> quit
		else if(databuf.button[25]==0x82) padd|=BUTTON_L2	 | BUTTON_R2;		//GREEN -> screensaver
	}

	if(cover_mode!=5 && !use_analog)
	{
		if(padLYstick<2) padd=padd | BUTTON_UP;   if(padLYstick>253) padd=padd | BUTTON_DOWN;
		if(padLXstick<2) padd=padd | BUTTON_LEFT; if(padLXstick>253) padd=padd | BUTTON_RIGHT;

		if(padRYstick<2) padd=padd | BUTTON_UP;   if(padRYstick>253) padd=padd | BUTTON_DOWN;
		if(padRXstick<2) padd=padd | BUTTON_LEFT; if(padRXstick>253) padd=padd | BUTTON_RIGHT;
	}

	mouseYD=0.0f; mouseXD=0.0f;
	mouseYDL=0.0f; mouseXDL=0.0f;
	mouseYDR=0.0f; mouseXDR=0.0f;
	//deadzone: x=100 y=156 (28 / 10%)

	if(padRXstick<=(128-xDZa)){
		mouseXD=(float)(((padRXstick+xDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseXDR=mouseXD;}

	if(padRXstick>=(128+xDZa)){
		mouseXD=(float)(((padRXstick-xDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseXDR=mouseXD;}

	if(padRYstick<=(128-yDZa)){
		mouseYD=(float)(((padRYstick+yDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseYDR=mouseYD;}

	if(padRYstick>=(128+yDZa)){
		mouseYD=(float)(((padRYstick-yDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseYDR=mouseYD;}


	if(padLXstick<=(128-xDZa)){
		mouseXD=(float)(((padLXstick+xDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseXDL=mouseXD;}

	if(padLXstick>=(128+xDZa)){
		mouseXD=(float)(((padLXstick-xDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseXDL=mouseXD;}

	if(padLYstick<=(128-yDZa)){
		mouseYD=(float)(((padLYstick+yDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseYDL=mouseYD;}

	if(padLYstick>=(128+yDZa)){
		mouseYD=(float)(((padLYstick-yDZa-128.0f))/(11000.0f/active_pads));//*(1.f-overscan);
		mouseYDL=mouseYD;}

pad_out:

	new_pad_t[pad_num] = padd & (~old_pad_t[pad_num]);
	old_pad_t[pad_num] = padd;

	if(new_pad_t[pad_num]==0 && old_pad_t[pad_num]==0) goto pad_repeat;

	c_opacity_delta=16;	dimc=0; dim=1;
	b_box_opaq= 0xfe;
	b_box_step= -4;

	ss_timer=(time(NULL)-ss_timer_last);
	ss_timer_last=time(NULL);
	a_dynamic2=0;

pad_repeat:

	key_repeat_t[pad_num]=0;

	if(last_pad!=-1)
	{
		repeat_counter1_t[last_pad] = repeat_counter1;
		repeat_counter2_t[last_pad] = repeat_counter2;
		repeat_counter3_t[last_pad] = repeat_counter3;
		repeat_counter3_inc_t[last_pad] = repeat_counter3_inc;
	}

	last_pad=-1;

	if(new_pad_t[pad_num]==0 && old_pad_t[pad_num]!=0)
	{
		repeat_counter1_t[pad_num]--;
		if(repeat_counter1_t[pad_num]<=0)
		{
			repeat_counter1_t[pad_num]=0;
			repeat_counter2_t[pad_num]--;
			if(repeat_counter2_t[pad_num]<=0)
			{
				if(repeat_counter3_inc_t[pad_num]<3.f) repeat_counter3_inc_t[pad_num]+=0.02f;
				repeat_counter3_t[pad_num]+=(int)repeat_counter3_inc_t[pad_num];
				repeat_counter2_t[pad_num]=repeat_key_delay;
				new_pad_t[pad_num]=old_pad_t[pad_num];
				ss_timer=0;
				ss_timer_last=time(NULL);
				xmb_bg_counter=XMB_BG_COUNTER;
			}
			key_repeat_t[pad_num]=1;
			last_pad=pad_num;
		}
	}
	else
	{
		if(!active_pads) active_pads=1;
		repeat_counter1_t[pad_num]=repeat_init_delay/active_pads;
		repeat_counter2_t[pad_num]=repeat_key_delay/active_pads;
		repeat_counter3_t[pad_num]=1;
		repeat_counter3_inc_t[pad_num]=0.f;
	}

	repeat_counter1 = repeat_counter1_t[pad_num];
	repeat_counter2 = repeat_counter2_t[pad_num];
	repeat_counter3 = repeat_counter3_t[pad_num];
	repeat_counter3_inc = repeat_counter3_inc_t[pad_num];

	key_repeat=key_repeat_t[pad_num];
	new_pad=new_pad_t[pad_num];
	old_pad=old_pad_t[pad_num];
	pad_num++;
	if(last_pad!=-1) pad_num=last_pad;

	return 1;
}

void screen_saver()
{
		pad_read();
		c_opacity_delta=0; new_pad=0; old_pad=0;
		int initial_skip=0;

		screen_saver_mode=1;
		while(!restart_request && !app_shutdown) {


			if(ftp_clients<2)
			{
				for(int n=0; n<MAX_STARS; n++)
				{
					int move_star= rndv(10);

					{
						draw_square(((float)stars[n].x/1920.0f-0.5f)*2.0f, (0.5f-(float)stars[n].y/1080.0f)*2.0f, (stars[n].size/1920.f), (stars[n].size/1080.f), 0.0f,
							(0xffffff00 | stars[n].bri)
							);

						stars[n].x+=(int) ( ( 512.f - (float)padSensorX) * 0.20f );
						stars[n].y-=(int) ( ( 500.f - (float)padSensorZ) * 0.15f );

						if(move_star>6 || move_star<1) stars[n].bri-=(4*(rndv(4)+1));
						if(stars[n].x>1919 || stars[n].y>1079 || stars[n].x<1 || stars[n].y<1 || stars[n].bri<(4*(rndv(4)+1)))
						{
							stars[n].x=rndv(1920);
							stars[n].y=rndv(1080);
							stars[n].bri=rndv(222)&0xf0;
							stars[n].size=rndv(XMB_SPARK_SIZE)+1;
						}
					}

				}

				if(show_temp>=2 && time(NULL)&0x20) draw_xmb_clock(xmb_clock, 0);

				setRenderColor();
			}

			flip();
			sys_ppu_thread_yield();

			initial_skip++;
			new_pad=0; old_pad=0;

			pad_read();
			if (( (new_pad || old_pad) && initial_skip>150)) {// || c_opacity_delta==16 // || www_running!=www_running_old
				new_pad=0;  break;
				}
			if(ss_timer>=(sao_timeout*3600) && sao_timeout) shutdown_system(0);

		}
		ss_timer=0;
		ss_timer_last=time(NULL);
		c_opacity=0xff; c_opacity2=0xff;
		screen_saver_mode=0;
}

u64 get_temperature(u32 _dev, u32 *_temp)
{
	system_call_2(383, (u64)_dev, (u64)_temp);
	return_to_user_prog(u64);
}

static uint64_t syscall_837(const char *device, const char *format, const char *point, u32 a, u32 b, u32 c, void *buffer, u32 len)
{
	system_call_8(837, (u64)device, (u64)format, (u64)point, a, b, c, (u64)buffer, len);
	return_to_user_prog(uint64_t);
}

static uint64_t syscall_838(const char *device)
{
	system_call_3(838, (u64)device, 0, 1);
	return_to_user_prog(uint64_t);
}

void mountpoints(u8 _mode)
{
	if(_mode==MOUNT)
	{
		syscall_837("CELL_FS_IOS:BDVD_DRIVE", "CELL_FS_ISO9660", "/dev_bdvd", 0, 1, 0, 0, 0);
		syscall_837("CELL_FS_IOS:BDVD_DRIVE", "CELL_FS_ISO9660", "/dev_ps2disc", 0, 1, 0, 0, 0);
	}
	else
	{
		syscall_838("/dev_bdvd");
		syscall_838("/dev_ps2disc");
	}
}

uint8_t np_pool[131072];
//#define PSP_EMU_KLIC {{0x2A, 0x6A, 0xFB, 0xCF, 0x43, 0xD1, 0x57, 0x9F, 0x7D, 0x73, 0x87, 0x41, 0xA1, 0x3B, 0xD4, 0x2E}}

void launch_self_npdrm(char *_self, char *_param)
{
	char* launchargv[2];
	memset(launchargv, 0, sizeof(launchargv));
	char self[256];
	sprintf(self, "%s", _self);
	int len = strlen(_param);
	launchargv[0] = (char*)malloc(len + 1); strcpy(launchargv[0], _param);
	launchargv[1] = NULL;

	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP);
	sceNpInit(131072, np_pool);
	SceNpDrmKey *k_licensee = NULL;
	unload_modules();
	sceNpDrmProcessExitSpawn2( k_licensee, (const char*) self,
		(const char**)launchargv,
		NULL, NULL, 0, 64, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
	sceNpTerm();
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_NP);
	_Exitspawn((const char*)self, (char* const*)launchargv, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}

void launch_self_npdrm2(char *_self)
{
	char self[256];
	sprintf(self, "%s", _self);
	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP);
	sceNpInit(131072, np_pool);
	SceNpDrmKey *k_licensee = NULL;
	unload_modules();
	sceNpDrmProcessExitSpawn2( k_licensee, (const char*) self,
		NULL,
		NULL, NULL, 0, 64, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
	sceNpTerm();
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_NP);
	_Exitspawn((const char*)self, NULL, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}

void launch_self_npdrm3(char *_self, char *_param, char *_param2)
{
	char* launchargv[3];
	memset(launchargv, 0, sizeof(launchargv));
	char self[256];
	sprintf(self, "%s", _self);

	int len = strlen(_param);
	launchargv[0] = (char*)malloc(len + 1); strcpy(launchargv[0], _param);

	len = strlen(_param2);
	launchargv[1] = (char*)malloc(len + 1); strcpy(launchargv[1], _param2);

	launchargv[2] = NULL;

	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP);
	sceNpInit(131072, np_pool);
	SceNpDrmKey *k_licensee = NULL;
	unload_modules();
	sceNpDrmProcessExitSpawn2( k_licensee, (const char*) self,
		(const char**)launchargv,
		NULL, NULL, 0, 64, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
	sceNpTerm();
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_NP);

	_Exitspawn((const char*)self, (char* const*)launchargv, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}

void launch_self(char *_self, char *_param)
{
	launch_self_npdrm(_self, _param);

	char* launchargv[2];
	memset(launchargv, 0, sizeof(launchargv));
	char self[256];
	sprintf(self, "%s", _self);
	int len = strlen(_param);
	launchargv[0] = (char*)malloc(len + 1); strcpy(launchargv[0], _param);
	launchargv[1] = NULL;
	unload_modules();
	_Exitspawn((const char*)self, (char* const*)launchargv, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}

void launch_self2(char *_self)
{
	launch_self_npdrm2(_self);
	char self[256];
	sprintf(self, "%s", _self);
	unload_modules();
	_Exitspawn((const char*)self, NULL, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}

void launch_self3(char *_self, char *_param, char *_param2)
{
	launch_self_npdrm3(_self, _param, _param2);

	char* launchargv[3];
	memset(launchargv, 0, sizeof(launchargv));
	char self[256];
	sprintf(self, "%s", _self);

	int len = strlen(_param);
	launchargv[0] = (char*)malloc(len + 1); strcpy(launchargv[0], _param);

	len=strlen(_param2);
	launchargv[1] = (char*)malloc(len + 1); strcpy(launchargv[1], _param2);

	launchargv[2] = NULL;
	unload_modules();
	_Exitspawn((const char*)self, (char* const*)launchargv, NULL, NULL, 0, 1001, SYS_PROCESS_PRIMARY_STACK_SIZE_512K);
	exit(0);
}


u64 get_free_drive_space(char *_path)
{
	if(strlen(_path)<6) return 0;
	char tmp_path[1024];
	if(_path[0]!='/')
		sprintf(tmp_path, "/%s/", _path);
	else
		sprintf(tmp_path, "%s/", _path);
	tmp_path[strchr((tmp_path+1), '/')-tmp_path]=0;
	uint32_t blockSize;
	uint64_t freeSize;
	cellFsGetFreeSize(tmp_path, &blockSize, &freeSize);
	return (u64)( ((uint64_t)blockSize * freeSize));
}


/****************************************************/
/* FTP SECTION                                      */
/****************************************************/

u64 mount_dev_flash()
{
	u64 ret2 = syscall_837("CELL_FS_IOS:BUILTIN_FLSH1", "CELL_FS_FAT", "/dev_rebug", 0, 0, 0, 0, 0);
	cellFsAioInit((char*)"/dev_rebug");
	return ret2;
}

u64 unmount_dev_flash()
{
	u64 ret2=0;
	{ret2= syscall_838("/dev_blind");}
	{ret2 = syscall_838("/dev_wflash");}
	{ret2 = syscall_838("/dev_rwflash");}
	return ret2;
}
/*
void dump_root_key()
{
    if(c_firmware==3.55f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_355.self");
    }
    else
    if(c_firmware==4.21f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_421.self");
    }
    else
    if(c_firmware==4.21f && dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_421d.self");
    }
    else
    if(c_firmware==4.46f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_446.self");
    }
    else
    if(c_firmware==4.65f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_465.self");
    }
    else
    if(c_firmware==4.66f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_465.self");
    }
    else
    if(c_firmware==4.70f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_470.self");
    }
    else
    if(c_firmware==4.70f && dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_470d.self");
    }
    else
    if(c_firmware==4.75f && !dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_475.self");
    }
    else
    if(c_firmware==4.75f && dex_mode)
    {
        dialog_ret=0;
        cellMsgDialogOpen2( type_dialog_yes_no, "Do you want to dump your eid root key and reboot?\n\nThe eid_root_key will be created in /dev_hdd0/game/RBGTLBOX2/USRDIR", dialog_fun1, (void*)0x0000aaaa, NULL );
        wait_dialog();
        if(dialog_ret==1)
            launch_self2((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_475d.self");
    }

}
*/
void dump_root_key()
{
    char version[8];

    if(c_firmware==3.55f && !dex_mode) strcpy(version, "355");   else
    if(c_firmware==4.21f && !dex_mode) strcpy(version, "421");   else
    if(c_firmware==4.21f &&  dex_mode) strcpy(version, "421d");  else
    if(c_firmware==4.46f && !dex_mode) strcpy(version, "446");   else
    if(c_firmware==4.65f && !dex_mode) strcpy(version, "465");   else
    if(c_firmware==4.66f && !dex_mode) strcpy(version, "465");   else
    if(c_firmware==4.70f && !dex_mode) strcpy(version, "470");   else
    if(c_firmware==4.70f &&  dex_mode) strcpy(version, "470d");  else
    if(c_firmware==4.75f && !dex_mode) strcpy(version, "475");   else
    if(c_firmware==4.75f &&  dex_mode) strcpy(version, "475d");  else
	if(c_firmware==4.76f && !dex_mode) strcpy(version, "475");   else
	if(c_firmware==4.76f &&  dex_mode) strcpy(version, "475d");  else
	if(c_firmware==4.78f && !dex_mode) strcpy(version, "475");   else
    if(c_firmware==4.78f &&  dex_mode) strcpy(version, "475d");  else
	if(c_firmware==4.80f && !dex_mode) strcpy(version, "480");   else
    if(c_firmware==4.80f &&  dex_mode) strcpy(version, "480d");  else
	if(c_firmware==4.81f && !dex_mode) strcpy(version, "475");   else
    if(c_firmware==4.81f &&  dex_mode) strcpy(version, "481d");  else	return;

    char rkdumper[64];
    sprintf(rkdumper, "/dev_hdd0/game/RBGTLBOX2/USRDIR/root_key_%s.self", version);

    if(!exist((char*)rkdumper)) return;

    dialog_ret=0;
    cellMsgDialogOpen2( type_dialog_yes_no, STR_EIDRK, dialog_fun1, (void*)0x0000aaaa, NULL );
    wait_dialog();

    if(dialog_ret==1)
        launch_self2((char*)rkdumper);
}
/*******************/
/* Control Console */


CellConsoleInputProcessorResult _cellConsolePeek
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation;
	char peek_addr[32]; peek_addr[0]=0;
	uint64_t peekA=0;

      if (sscanf(pcInput, "%*s %s", peek_addr)==1) {
		  peekA=strtoull(peek_addr, NULL, 16)+0x8000000000000000ULL;
  		  if(peekA>0x80000000007ffff8ULL) return CELL_CONSOLE_INPUT_PROCESSED;
			cellConsolePrintf(uiConnection, "peek(0x80000000%08X): 0x%08X%08X\n", peekA, (peekq(peekA)>>32), peekq(peekA));
      } else {
            cellConsolePrintf(uiConnection,
				"Usage: peek <u32 address>\n\npeek 2f8011\n\n");
      }
      cellConsolePrintf(uiConnection, "\n> ");
      return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsolePeekL
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation;
	char peek_addr[32]; peek_addr[0]=0;
	uint64_t peekA=0; int n=0;
	uint64_t _val=0;
	unsigned char _vals[9];

      if (sscanf(pcInput, "%*s %s", peek_addr)==1) {
		for(n=0; n<256; n+=8)
		  {
		peekA=strtoull(peek_addr, NULL, 16)+0x8000000000000000ULL + (uint64_t) n;
		if(peekA>0x80000000007ffff8ULL) break;
		_val=peekq(peekA);
		for(u8 m=0;m<8;m++)
		  {
			_vals[7-m]=(_val>>(m*8))&0xff; if(_vals[7-m]<0x20 || _vals[7-m]>0x7f) _vals[7-m]='.';

		  }
		cellConsolePrintf(uiConnection, "peek(0x80000000%08X): 0x%08X%08X | %s\n", peekA, (_val>>32), _val, _vals);
		  }

      } else {
            cellConsolePrintf(uiConnection,
				"Usage: peekl <u32 address>\n\npeek 2f8011\n\n");
      }
      cellConsolePrintf(uiConnection, "\n> ");
      return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsolePoke
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation;
	char peek_addr[32]; peek_addr[0]=0;
	char poke_val[32]; poke_val[0]=0;
	uint64_t peekA=0, pokeA=0;

      if (sscanf(pcInput, "%*s %s %s", peek_addr, poke_val)==2) {
			peekA=strtoull(peek_addr, NULL, 16)+0x8000000000000000ULL;
			if(peekA>0x80000000007ffff8ULL) return CELL_CONSOLE_INPUT_PROCESSED;
			cellConsolePrintf(uiConnection, "peek(0x80000000%08X): 0x%08X%08X\n", peekA, (peekq(peekA)>>32), peekq(peekA));

  			pokeA=strtoull(poke_val, NULL, 16);
			pokeq(peekA, pokeA);
			peekA=strtoull(peek_addr, NULL, 16)+0x8000000000000000ULL;
			cellConsolePrintf(uiConnection, "poke(0x80000000%08X)= 0x%08X%08X\n", peekA, (peekq(peekA)>>32), peekq(peekA));

      } else {
            cellConsolePrintf(uiConnection,
				"Usage: poke <u32 address> <u64 value>\n\npoke 2f8011 1020304050607080\n\n");
      }
      cellConsolePrintf(uiConnection, "\n> ");
      return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsolePeekLV1
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation;
	char peek_addr[32]; peek_addr[0]=0;
	uint64_t peekA=0; int n=0;
	uint64_t _val=0;
	unsigned char _vals[9];

      if (sscanf(pcInput, "%*s %s", peek_addr)==1) {
		for(n=0; n<256; n+=8)
		  {
		peekA=strtoull(peek_addr, NULL, 16) + (uint64_t) n;
		_val = peek_lv1_cobra(peekA);
		for(u8 m=0;m<8;m++)
		  {
			_vals[7-m]=(_val>>(m*8))&0xff; if(_vals[7-m]<0x20 || _vals[7-m]>0x7f) _vals[7-m]='.';

		  }
		cellConsolePrintf(uiConnection, "peeklv1(0x00000000%08X): 0x%08X%08X | %s\n", peekA, (_val>>32), _val, _vals);
		  }

      } else {
            cellConsolePrintf(uiConnection,
				"Usage: peeklv1 <u32 address>\n\npeeklv1 2f8011\n\n");
      }
      cellConsolePrintf(uiConnection, "\n> ");
      return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsolePokeLV1
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation;
	char peek_addr[32]; peek_addr[0]=0;
	char poke_val[32]; poke_val[0]=0;
	uint64_t peekA=0, pokeA=0;

      if (sscanf(pcInput, "%*s %s %s", peek_addr, poke_val)==2) {
			peekA=strtoull(peek_addr, NULL, 16);
  			pokeA=strtoull(poke_val, NULL, 16);

			cellConsolePrintf(uiConnection, "peeklv1(0x00000000%08X): 0x%08X%08X\n", peekA, (peek_lv1_cobra(peekA)>>32), peek_lv1_cobra(peekA));
			poke_lv1(peekA, pokeA);
			cellConsolePrintf(uiConnection, "pokelv1(0x00000000%08X)= 0x%08X%08X\n", peekA, (peek_lv1_cobra(peekA)>>32), peek_lv1_cobra(peekA));

      } else {
            cellConsolePrintf(uiConnection,
				"Usage: pokelv1 <u32 address> <u64 value>\n\npokelv1 2f8011 1020304050607080\n\n");
      }
      cellConsolePrintf(uiConnection, "\n> ");
      return CELL_CONSOLE_INPUT_PROCESSED;
}


CellConsoleInputProcessorResult _cellConsoleQuitXMB
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation; (void)uiConnection; (void)iContinuation; (void) pcInput;
    cellConsolePrintf(uiConnection, "\nShutting down...\n");
	//unload_modules();
    cellConsolePrintf(uiConnection, "Done!\n\n");
	exit_app();

    return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsoleSS
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation; (void)uiConnection; (void)iContinuation; (void) pcInput;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	char video_mem[64], scm[128];
	if(dex_mode==1)
		sprintf(video_mem, "%s/%04d%02d%02d-%02d%02d%02d-SCREENSHOT.RAW", app_usrdir, timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	else
		sprintf(video_mem, "/dev_hdd0/%04d%02d%02d-%02d%02d%02d-SCREENSHOT.RAW", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	sprintf(scm, "\n%s: [%s]...\n", STR_SCRSHTSAVED, video_mem);
    cellConsolePrintf(uiConnection, scm);
	FILE *fpA;
	remove(video_mem);
	fpA = fopen ( video_mem, "wb" );
	uint64_t c_pos=0;
	for(c_pos=0;c_pos<video_buffer;c_pos+=4){
		fwrite((uint8_t*)(color_base_addr)+c_pos+1, 3, 1, fpA);
	}
	fclose(fpA);
	sprintf(scm, "%s\n\n> ", STR_DONE);
    cellConsolePrintf(uiConnection, scm);


    return CELL_CONSOLE_INPUT_PROCESSED;
}

CellConsoleInputProcessorResult _cellConsoleRESTART
				(unsigned int uiConnection,
				const char *pcInput,
				void *pvDummy,
				int iContinuation) {

	(void) pvDummy; (void) iContinuation; (void)uiConnection; (void)iContinuation; (void) pcInput;
	char str[128];
	sprintf(str, "\n%s\n", STR_SHTDWN);
    cellConsolePrintf(uiConnection, str);
	sprintf(str, "%s\n", STR_TRYRESTART);
	cellConsolePrintf(uiConnection, str);
	restart_request=1;

    return CELL_CONSOLE_INPUT_PROCESSED;
}


/****************************************************/
/* MODULES SECTION                                  */
/****************************************************/

static int load_modules()
{
	int ret;

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
	if(ret != CELL_OK) return ret;

	ret = cellSysmoduleLoadModule( CELL_SYSMODULE_IO );
	if (ret != CELL_OK) return ret;

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_JPGDEC);
	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
	CellScreenShotSetParam  screenshot_param = {0, 0, 0, 0};
	screenshot_param.photo_title = "Screenshot";
	screenshot_param.game_title = "Screenshots";
	screenshot_param.game_comment = current_version_NULL;
	cellScreenShotSetParameter (&screenshot_param);
	cellScreenShotSetOverlayImage(app_homedir,	(char*) "ICON0.PNG", 50, 850);
	cellScreenShotEnable();

	cellSysmoduleLoadModule( CELL_SYSMODULE_SYSUTIL_GAME );

	cellKbInit(1);

	ret = cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
	if (ret < 0) return ret;

	cellNetCtlInit();
	net_available = (sys_net_initialize_network()==0);

#ifdef USE_DEBUG
	logger_init();
#endif

	cellConsoleInit();
	//cellConsoleNetworkInitialize();
	cellConsoleNetworkServerInit(8080);

/*
cellConsoleInputProcessorAdd("resetbd",
				"Unmount/mount dev_bdvd",
			      "", 0,
			      _cellConsoleResetBDVD);

cellConsoleInputProcessorAdd("panic",
				"LV2 panic/reload",
			      "", 0,
			      _cellConsolePanic);

cellConsoleInputProcessorAdd("debug",
				"Enable disable debug messages",
			      "on|off", 0,
			      _cellConsoleDebug);
*/

cellConsoleInputProcessorAdd("pokelv1",
				"Write value to LV1 memory",
			      "<u32 address> <u64 value>", 0,
			      _cellConsolePokeLV1);

cellConsoleInputProcessorAdd("peeklv1",
				"Read value from LV1 memory",
			      "<u32 address>", 0,
			      _cellConsolePeekLV1);

cellConsoleInputProcessorAdd("poke",
				"Write value to LV2 memory",
			      "<u32 address> <u64 value>", 0,
			      _cellConsolePoke);

cellConsoleInputProcessorAdd("peekl",
				"Read 256 values from LV2 memory",
			      "<u32 address>", 0,
			      _cellConsolePeekL);

cellConsoleInputProcessorAdd("peek",
				"Read value from LV2 memory",
			      "<u32 address>", 0,
			      _cellConsolePeek);

cellConsoleInputProcessorAdd("quit",
				"Quit and exit to XMB",
			      "", 0,
			      _cellConsoleQuitXMB);

cellConsoleInputProcessorAdd("screenshot",
				"Save current screen as RAW (RGB) image in /dev_hdd0",
			      "", 0,
			      _cellConsoleSS);

cellConsoleInputProcessorAdd("restart",
				"Restart",
			      "", 0,
			      _cellConsoleRESTART);


		ret = Fonts_LoadModules();
		if ( ret != CELL_OK ) {
			return CELL_OK;
		}

		fonts = Fonts_Init();
		if ( fonts ) {
			ret = Fonts_InitLibraryFreeType( &freeType );
			if ( ret == CELL_OK ) {
				ret = Fonts_OpenFonts( freeType, fonts, app_usrdir );
				if ( ret == CELL_OK ) {
					ret = Fonts_CreateRenderer( freeType, 0, &RenderWork.Renderer );
					if ( ret == CELL_OK ) {
						return CELL_OK;
					}

					Fonts_CloseFonts( fonts );
				}
				Fonts_EndLibrary( freeType );
			}
			Fonts_End();
			fonts = (Fonts_t*)0;
		}

	return ret;
}

static int unload_modules()
{
#ifdef USE_DEBUG
	MM_LOG("Unload modules called...\n");
#endif
	if(unload_called) return 0;
	unload_called=1;

	app_shutdown=true;
	init_finished=0;

	if(!reset_settings)
	{
		save_options();
	}


	cellPadEnd();

	cellSysmoduleUnloadModule(CELL_SYSMODULE_NET);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_GCM_SYS);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_IO);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_JPGDEC);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_GAME);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);

	cellSysmoduleUnloadModule( CELL_SYSMODULE_SYSUTIL_SCREENSHOT );

	Fonts_CloseFonts( fonts );
	Fonts_DestroyRenderer( &RenderWork.Renderer );
	Fonts_EndLibrary( freeType );
	Fonts_End();
	Fonts_UnloadModules();

	if(memory_container) sys_memory_container_destroy( memory_container );
	return 0;
}





/****************************************************/
/* PNG SECTION                                      */
/****************************************************/


typedef struct CtrlMallocArg
{
	u32 mallocCallCounts;

} CtrlMallocArg;


typedef struct CtrlFreeArg
{
	u32 freeCallCounts;

} CtrlFreeArg;

void *png_malloc(u32 size, void * a)
{
    CtrlMallocArg *arg;
	arg = (CtrlMallocArg *) a;
	arg->mallocCallCounts++;
	return memalign(16,size+16);
}


static int png_free(void *ptr, void * a)
{
    CtrlFreeArg *arg;
  	arg = (CtrlFreeArg *) a;
  	arg->freeCallCounts++;
	free(ptr);
	return 0;
}

/*
void *jpg_malloc(u32 size, void * a)
{
    CtrlMallocArg *arg;
	arg = (CtrlMallocArg *) a;
	arg->mallocCallCounts++;
	return memalign(16,size+16);
}


static int jpg_free(void *ptr, void * a)
{
    CtrlFreeArg *arg;
  	arg = (CtrlFreeArg *) a;
  	arg->freeCallCounts++;
	free(ptr);
	return 0;
}*/

int map_rsx_memory(u8 *buffer, size_t buf_size)
{
	int ret;
	u32 offset;
	ret = cellGcmMapMainMemory(buffer, buf_size, &offset);
	if(CELL_OK != ret ) return ret;
	return 0;
}

volatile int png_w=0, png_h=0, png_w2, png_h2;
volatile int jpg_w=0, jpg_h=0;
volatile int png_w_th=0, png_h_th=0;

/*
int load_jpg_texture_th(u8 *data, char *name, uint16_t _DW)
{

	int ret=-1, ok=-1;
	jpg_w=0; jpg_h=0;

    CellJpgDecMainHandle     mHandle;
    CellJpgDecSubHandle      sHandle;

    CellJpgDecInParam        inParam;
    CellJpgDecOutParam       outParam;

    CellJpgDecSrc            src;
    CellJpgDecOpnInfo        opnInfo;
    CellJpgDecInfo           info;

    CellJpgDecDataOutInfo    dOutInfo;
    CellJpgDecDataCtrlParam  dCtrlParam;

    CellJpgDecThreadInParam  InParam;
    CellJpgDecThreadOutParam OutParam;

	CtrlMallocArg               MallocArg;
	CtrlFreeArg                 FreeArg;

    float                    downScale;


    MallocArg.mallocCallCounts  = 0;
    FreeArg.freeCallCounts      = 0;

//	InParam.spuThreadEnable   = CELL_JPGDEC_SPU_THREAD_DISABLE;
	InParam.spuThreadEnable   = CELL_JPGDEC_SPU_THREAD_ENABLE;
	InParam.ppuThreadPriority = 1600;
	InParam.spuThreadPriority = 250;
	InParam.cbCtrlMallocFunc  = jpg_malloc;
	InParam.cbCtrlMallocArg   = &MallocArg;
	InParam.cbCtrlFreeFunc    = jpg_free;
	InParam.cbCtrlFreeArg     = &FreeArg;

    if(cellJpgDecCreate(&mHandle, &InParam, &OutParam) == CELL_OK)
	{
            src.srcSelect  = CELL_JPGDEC_FILE;
            src.streamPtr  = NULL;
            src.streamSize = 0;

            src.fileName   = name;
            src.fileOffset = 0;
            src.fileSize   = 0;

            src.spuThreadEnable = CELL_JPGDEC_SPU_THREAD_ENABLE;
//			src.spuThreadEnable = CELL_JPGDEC_SPU_THREAD_DISABLE;

            int ret_dec_open = cellJpgDecOpen(mHandle, &sHandle, &src, &opnInfo);
            if(ret_dec_open == CELL_OK)
			{
				if(cellJpgDecReadHeader(mHandle, sHandle, &info) == CELL_OK)
				{
					if(scale_icon_h)
					{
						if( ((float)info.imageHeight / (float)XMB_THUMB_HEIGHT) > ((float)info.imageWidth / (float) XMB_THUMB_WIDTH))
							downScale=(float)info.imageHeight / (float)(XMB_THUMB_HEIGHT);
						else
							downScale=(float)info.imageWidth / (float) (XMB_THUMB_WIDTH);
					}
					else
					{

						if(info.imageWidth>1920 || info.imageHeight>1080)
						{
							if( ((float)info.imageWidth / 1920) > ((float)info.imageHeight / 1080 ) ){
								downScale = (float)info.imageWidth / 1920.f;
							}else{
								downScale = (float)info.imageHeight / 1080.f;
							}
						}
						else
							downScale=1.f;

						if(strstr(name, "/HDAVCTN/BDMT_O1.jpg")!=NULL || strstr(name, "/BDMV/META/DL/HDAVCTN_O1.jpg")!=NULL) downScale = (float) (info.imageWidth / 320.f);
					}

					if( downScale <= 1.f ){
						inParam.downScale = 1;
					}else if( downScale <= 2.f ){
						inParam.downScale = 2;
					}else if( downScale <= 4.f ){
						inParam.downScale = 4;
					}else{
						inParam.downScale = 8;
					}

					if(downScale>8.0f)
					{
						jpg_w=0; jpg_h=0;
						goto leave_jpg_th;

					}

					inParam.commandPtr       = NULL;
					inParam.method           = CELL_JPGDEC_FAST;
					inParam.outputMode       = CELL_JPGDEC_TOP_TO_BOTTOM;
					inParam.outputColorSpace = CELL_JPG_RGBA;
					inParam.outputColorAlpha = 0xfe;

					ret = cellJpgDecSetParameter(mHandle, sHandle, &inParam, &outParam);

					if(ret == CELL_OK)
					{
						if(scale_icon_h && inParam.downScale)
							dCtrlParam.outputBytesPerLine = (int) ((info.imageWidth/inParam.downScale) * 4);
						else
							dCtrlParam.outputBytesPerLine = _DW * 4;

						ret = cellJpgDecDecodeData(mHandle, sHandle, data, &dCtrlParam, &dOutInfo);

						if((ret == CELL_OK) && (dOutInfo.status == CELL_JPGDEC_DEC_STATUS_FINISH))
						{
							jpg_w= outParam.outputWidth;
							jpg_h= outParam.outputHeight;
							ok=0;
						}
					}
				}
			}

leave_jpg_th:
            if(ret_dec_open == CELL_OK) cellJpgDecClose(mHandle, sHandle);
		    cellJpgDecDestroy(mHandle);
	} //decoder create
	//MM_LOG("load_JPG_texture_th: %i x %i (%s)\n", jpg_w, jpg_h, name);
	scale_icon_h=0;
	return ret;
}

int load_jpg_texture(u8 *data, char *name, uint16_t _DW)
{
	scale_icon_h=0;
	while(is_decoding_jpg || is_decoding_png){ sys_timer_usleep(3336); cellSysutilCheckCallback();}
	is_decoding_jpg=1;
	int ret=-1, ok=-1;
	png_w=0; png_h=0;

    CellJpgDecMainHandle     mHandle;
    CellJpgDecSubHandle      sHandle;

    CellJpgDecInParam        inParam;
    CellJpgDecOutParam       outParam;

    CellJpgDecSrc            src;
    CellJpgDecOpnInfo        opnInfo;
    CellJpgDecInfo           info;

    CellJpgDecDataOutInfo    dOutInfo;
    CellJpgDecDataCtrlParam  dCtrlParam;

    CellJpgDecThreadInParam  InParam;
    CellJpgDecThreadOutParam OutParam;

	CtrlMallocArg               MallocArg;
	CtrlFreeArg                 FreeArg;

    float                    downScale;
    bool                     unsupportFlag;

    MallocArg.mallocCallCounts  = 0;
    FreeArg.freeCallCounts      = 0;

//	InParam.spuThreadEnable   = CELL_JPGDEC_SPU_THREAD_DISABLE;
	InParam.spuThreadEnable   = CELL_JPGDEC_SPU_THREAD_ENABLE;
	InParam.ppuThreadPriority = 1001;
	InParam.spuThreadPriority = 250;
	InParam.cbCtrlMallocFunc  = jpg_malloc;
	InParam.cbCtrlMallocArg   = &MallocArg;
	InParam.cbCtrlFreeFunc    = jpg_free;
	InParam.cbCtrlFreeArg     = &FreeArg;

    if(cellJpgDecCreate(&mHandle, &InParam, &OutParam) == CELL_OK)
	{
            src.srcSelect  = CELL_JPGDEC_FILE;
            src.fileName   = name;
            src.fileOffset = 0;
            src.fileSize   = 0;
            src.streamPtr  = NULL;
            src.streamSize = 0;

            src.spuThreadEnable = CELL_JPGDEC_SPU_THREAD_ENABLE;
//			src.spuThreadEnable = CELL_JPGDEC_SPU_THREAD_DISABLE;

			unsupportFlag = false;
            if(cellJpgDecOpen(mHandle, &sHandle, &src, &opnInfo) == CELL_OK)
			{
                ret = cellJpgDecReadHeader(mHandle, sHandle, &info);
				if(info.jpegColorSpace == CELL_JPG_UNKNOWN){
					unsupportFlag = true;
				}

				if(ret == CELL_OK)
				{
					if(scale_icon_h)
					{
	//					if(info.imageHeight>info.imageWidth)
						if( ((float)info.imageHeight / (float)XMB_THUMB_HEIGHT) > ((float)info.imageWidth / (float) XMB_THUMB_WIDTH))
							downScale=(float)info.imageHeight / (float)(XMB_THUMB_HEIGHT);
						else
							downScale=(float)info.imageWidth / (float) (XMB_THUMB_WIDTH);
					}
					else
					{

						if(info.imageWidth>1920 || info.imageHeight>1080){
							if( ((float)info.imageWidth / 1920) > ((float)info.imageHeight / 1080 ) ){
								downScale = (float)info.imageWidth / 1920.f;
							}else{
								downScale = (float)info.imageHeight / 1080.f;
							}
						}
						else
							downScale=1.f;

						if(strstr(name, "/HDAVCTN/BDMT_O1.jpg")!=NULL || strstr(name, "/BDMV/META/DL/HDAVCTN_O1.jpg")!=NULL) downScale = (float) (info.imageWidth / 320.f);

					}

						if( downScale <= 1.f ){
							inParam.downScale = 1;
						}else if( downScale <= 2.f ){
							inParam.downScale = 2;
						}else if( downScale <= 4.f ){
							inParam.downScale = 4;
						}else{
							inParam.downScale = 8;
						}

						if(downScale>8.0f)
						{
							png_w=0;
							png_h=0;
							cellJpgDecClose(mHandle, sHandle);
							goto leave_jpg;

						}


					inParam.commandPtr       = NULL;
					inParam.method           = CELL_JPGDEC_FAST;//CELL_JPGDEC_QUALITY
					inParam.outputMode       = CELL_JPGDEC_TOP_TO_BOTTOM;
					inParam.outputColorSpace = CELL_JPG_RGBA;
			//		if(scale_icon_h)
			//          inParam.outputColorAlpha = 0x80;
			//		else
						inParam.outputColorAlpha = 0xfe;
					ret = cellJpgDecSetParameter(mHandle, sHandle, &inParam, &outParam);
				}

				if(ret == CELL_OK)
				{
	//				if( _DW<1920 )
						if(scale_icon_h && inParam.downScale)
							dCtrlParam.outputBytesPerLine = (int) ((info.imageWidth/inParam.downScale) * 4);
						else
							dCtrlParam.outputBytesPerLine = _DW * 4;

	//				else
	//		            dCtrlParam.outputBytesPerLine = 1920 * 4;
	//                memset(data, 0, sizeof(data));

					ret = cellJpgDecDecodeData(mHandle, sHandle, data, &dCtrlParam, &dOutInfo);

					if((ret == CELL_OK) && (dOutInfo.status == CELL_JPGDEC_DEC_STATUS_FINISH))
						{
						png_w= outParam.outputWidth;
						png_h= outParam.outputHeight;
						ok=0;
						}
				}
				cellJpgDecClose(mHandle, sHandle);
            }

leave_jpg:

		ret = cellJpgDecDestroy(mHandle);
	} //decoder create

	scale_icon_h=0;
	is_decoding_jpg=0;
	//MM_LOG("load_JPG_texture   : %i x %i (%s)\n", png_w, png_h, name);
	return ret;
}

int load_png_texture_th(u8 *data, char *name)//, uint16_t _DW)
{
	png_w_th= png_h_th= 0;

	int  ret_file, ret, ok=-1;

	CellPngDecMainHandle        mHandle;
	CellPngDecSubHandle         sHandle;

	CellPngDecThreadInParam 	InParam;
	CellPngDecThreadOutParam 	OutParam;

	CellPngDecSrc 		        src;
	CellPngDecOpnInfo 	        opnInfo;
	CellPngDecInfo 		        info;

	CellPngDecDataOutInfo 	    dOutInfo;
	CellPngDecDataCtrlParam     dCtrlParam;
	CellPngDecInParam 	        inParam;
	CellPngDecOutParam 	        outParam;

	CtrlMallocArg               MallocArg;
	CtrlFreeArg                 FreeArg;

	int ret_png=-1;

//	InParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_DISABLE;
	InParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_ENABLE;
	InParam.ppuThreadPriority = 1600;
	InParam.spuThreadPriority = 200;
	InParam.cbCtrlMallocFunc  = png_malloc;
	InParam.cbCtrlMallocArg   = &MallocArg;
	InParam.cbCtrlFreeFunc    = png_free;
	InParam.cbCtrlFreeArg     = &FreeArg;


	ret_png= ret= cellPngDecCreate(&mHandle, &InParam, &OutParam);

//	memset(data, 0x00, sizeof(data)); //(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4)

	if(ret_png == CELL_OK)
	{

			memset(&src, 0, sizeof(CellPngDecSrc));
			src.srcSelect     = CELL_PNGDEC_FILE;
			src.fileName      = name;

//			src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_DISABLE;
			src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_ENABLE;

			ret_file=ret = cellPngDecOpen(mHandle, &sHandle, &src, &opnInfo);

			if(ret == CELL_OK)
			{
				ret = cellPngDecReadHeader(mHandle, sHandle, &info);
			}

			if(ret == CELL_OK && ( info.imageWidth <=1920))//+ info.imageHeight <= 3000)) //1920+1080
			{
				inParam.commandPtr        = NULL;
				inParam.outputMode        = CELL_PNGDEC_TOP_TO_BOTTOM;
				inParam.outputColorSpace  = CELL_PNGDEC_RGBA;
				inParam.outputBitDepth    = 8;
				inParam.outputPackFlag    = CELL_PNGDEC_1BYTE_PER_1PIXEL;

				if((info.colorSpace == CELL_PNGDEC_GRAYSCALE_ALPHA) || (info.colorSpace == CELL_PNGDEC_RGBA) || (info.chunkInformation & 0x10))
					inParam.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
				else
					inParam.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;

//				if(use_png_alpha)
//					inParam.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
//				else
					inParam.outputColorAlpha  = 0xff;

//				inParam.outputColorAlpha  = 0x00;


				ret = cellPngDecSetParameter(mHandle, sHandle, &inParam, &outParam);
			}
			else
				ret=-1;

			if(ret == CELL_OK)
			{
					dCtrlParam.outputBytesPerLine =  info.imageWidth * 4;//_DW * 4;
					ret = cellPngDecDecodeData(mHandle, sHandle, data, &dCtrlParam, &dOutInfo);

//					sys_timer_usleep(500);

					if((ret == CELL_OK) && (dOutInfo.status == CELL_PNGDEC_DEC_STATUS_FINISH))
					{
						png_w_th= outParam.outputWidth;
						png_h_th= outParam.outputHeight;
						ok=0;
					}
			}

			if(ret_file==0)	ret = cellPngDecClose(mHandle, sHandle);

			ret = cellPngDecDestroy(mHandle);
	}

	return ok;
}
*/


int load_png_texture(u8 *data, char *name, uint16_t _DW)
{
	while(is_decoding_jpg || is_decoding_png){ sys_timer_usleep(3336); cellSysutilCheckCallback();}
	is_decoding_png=1;
	int  ret_file, ret, ok=-1;
	png_w= png_h= 0;

	CellPngDecMainHandle        mHandle;
	CellPngDecSubHandle         sHandle;

	CellPngDecThreadInParam 	InParam;
	CellPngDecThreadOutParam 	OutParam;

	CellPngDecSrc 		        src;
	CellPngDecOpnInfo 	        opnInfo;
	CellPngDecInfo 		        info;

	CellPngDecDataOutInfo 	    dOutInfo;
	CellPngDecDataCtrlParam     dCtrlParam;
	CellPngDecInParam 	        inParam;
	CellPngDecOutParam 	        outParam;

	CtrlMallocArg               MallocArg;
	CtrlFreeArg                 FreeArg;

	int ret_png=-1;

	InParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_DISABLE;
//	InParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_ENABLE;
	InParam.ppuThreadPriority = 1001;
	InParam.spuThreadPriority = 250;
	InParam.cbCtrlMallocFunc  = png_malloc;
	InParam.cbCtrlMallocArg   = &MallocArg;
	InParam.cbCtrlFreeFunc    = png_free;
	InParam.cbCtrlFreeArg     = &FreeArg;


	ret_png= ret= cellPngDecCreate(&mHandle, &InParam, &OutParam);

//	memset(data, 0x00, sizeof(data)); //(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4)

	if(ret_png == CELL_OK)
	{

		memset(&src, 0, sizeof(CellPngDecSrc));
		src.srcSelect     = CELL_PNGDEC_FILE;
		src.fileName      = name;

		src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_DISABLE;
//			src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_ENABLE;

		ret_file=ret = cellPngDecOpen(mHandle, &sHandle, &src, &opnInfo);

		if(ret == CELL_OK)
		{
			ret = cellPngDecReadHeader(mHandle, sHandle, &info);
		}

		if(ret == CELL_OK && (_DW >= info.imageWidth && _DW*info.imageHeight<=2073600))// <= 3000))
		{
			inParam.commandPtr        = NULL;
			inParam.outputMode        = CELL_PNGDEC_TOP_TO_BOTTOM;
			inParam.outputColorSpace  = CELL_PNGDEC_RGBA;
			inParam.outputBitDepth    = 8;
			inParam.outputPackFlag    = CELL_PNGDEC_1BYTE_PER_1PIXEL;

			if((info.colorSpace == CELL_PNGDEC_GRAYSCALE_ALPHA) || (info.colorSpace == CELL_PNGDEC_RGBA) || (info.chunkInformation & 0x10))
				inParam.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
			else
				inParam.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;

//				if(use_png_alpha)
//					inParam.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
//				else
				inParam.outputColorAlpha  = 0xff;

//				inParam.outputColorAlpha  = 0x00;


			ret = cellPngDecSetParameter(mHandle, sHandle, &inParam, &outParam);
		}
		else
			ret=-1;

		if(ret == CELL_OK)
		{
			dCtrlParam.outputBytesPerLine = _DW * 4;
			ret = cellPngDecDecodeData(mHandle, sHandle, data, &dCtrlParam, &dOutInfo);

//				sys_timer_usleep(500);

			if((ret == CELL_OK) && (dOutInfo.status == CELL_PNGDEC_DEC_STATUS_FINISH))
				{
				png_w= outParam.outputWidth;
				png_h= outParam.outputHeight;
				ok=0;
				}
		}

		if(ret_file==0)	ret = cellPngDecClose(mHandle, sHandle);

		ret = cellPngDecDestroy(mHandle);

	}

	//InParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_DISABLE;

//	use_png_alpha=0;
	is_decoding_png=0;
	//MM_LOG("load_PNG_texture   : %i x %i (%s)\n", png_w, png_h, name);
return ok;
}

int load_texture(u8 *data, char *name, uint16_t dw)
{

//	if(strstr(name, ".jpg")!=NULL || strstr(name, ".JPG")!=NULL || strstr(name, ".jpeg")!=NULL || strstr(name, ".JPEG")!=NULL)
//		load_jpg_texture( data, name, dw);
//	else
	if(strstr(name, ".png")!=NULL || strstr(name, ".PNG")!=NULL)
	{
		load_png_texture( data, name, dw);
	}
	return 0;

}
/****************************************************/
/* syscalls                                         */
/****************************************************/


void pokeq( uint64_t addr, uint64_t val)
{
	if(c_firmware!=3.55f && c_firmware!=3.41f && c_firmware!=3.15f && c_firmware!=4.21f && c_firmware!=4.30f && c_firmware!=4.31f && c_firmware!=4.40f && c_firmware!=4.41f && c_firmware!=4.46f && c_firmware!=4.50f && c_firmware!=4.53f &&
	   c_firmware!=4.55f && c_firmware!=4.60f && c_firmware!=4.65f && c_firmware!=4.66f && c_firmware!=4.70f && c_firmware!=4.75f && c_firmware!=4.76f && c_firmware!=4.78f && c_firmware!=4.80f) return;

	if(!pp_enabled) return;
	system_call_2(SYSCALL_POKE, addr, val);
}

uint64_t peekq(uint64_t addr)
{
	system_call_1(SYSCALL_PEEK, addr);
	return_to_user_prog(uint64_t);
}

uint64_t peek_lv1_cobra(uint64_t addr)
{
	if(!is_cobra) return peek_lv1(addr); //OLD
    //if(!is_cobra || addr <= 0x1000 || addr >= 0xA000) return peek_lv1(addr); // NEW BYPASS FOR SYSCALL8
	system_call_1(11, addr);
	return_to_user_prog(uint64_t);
}

static void sysutil_callback( uint64_t status, uint64_t param, void * userdata )
{
	(void)param;
	(void)userdata;
	int ret=0;
//	if(status) 		MM_LOG("sysutil_callback status: %i\n", status);
	switch(status)
	{

	case CELL_SYSUTIL_REQUEST_EXITGAME:
		//MM_LOG("REQUEST_EXITGAME signal received!\n");
		if(!reset_settings)
		{
			save_options();
		}
		app_shutdown=true;

		if(memory_container) sys_memory_container_destroy( memory_container );
		exit_app();
		break;

	case CELL_SYSUTIL_OSKDIALOG_LOADED:
		break;

	case CELL_SYSUTIL_OSKDIALOG_INPUT_CANCELED:
		osk_dialog=-1;
		enteredCounter=0;
		osk_open=0;
		ret = cellOskDialogAbort();
		ret = cellOskDialogUnloadAsync(&OutputInfo);
		break;

	case CELL_SYSUTIL_OSKDIALOG_FINISHED:
		if(osk_dialog!=-1) osk_dialog=1;
		ret = cellOskDialogUnloadAsync(&OutputInfo);
		break;

	case CELL_SYSUTIL_OSKDIALOG_UNLOADED:
		sys_memory_container_destroy( memory_container );
		break;

	case CELL_SYSUTIL_DRAWING_BEGIN:
	case CELL_SYSUTIL_DRAWING_END:
		break;

	case CELL_SYSUTIL_BGMPLAYBACK_PLAY:
		break;

	case CELL_SYSUTIL_BGMPLAYBACK_STOP:

	case CELL_SYSUTIL_OSKDIALOG_INPUT_ENTERED:
		ret = cellOskDialogGetInputText( &OutputInfo );
		break;

	case CELL_SYSUTIL_OSKDIALOG_INPUT_DEVICE_CHANGED:
		if(param == CELL_OSKDIALOG_INPUT_DEVICE_KEYBOARD ){
//			ret = cellOskDialogSetDeviceMask( CELL_OSKDIALOG_DEVICE_MASK_PAD );
		}
		break;

	default:
		break;
	}
}

void draw_box( uint8_t *buffer_to, uint32_t width, uint32_t height, int x, int y, uint32_t border_color)
{
	u32 line = width * 4;
	u32 pos_to_border = ( y * line) + (x * 4), cline=0;
	u32 lines=0;
	unsigned char* bt;

	for(lines=0; lines<(height); lines++)
	{
		for(cline=0; cline<(line); cline+=4)
		{
			bt = (uint8_t*)(buffer_to) + pos_to_border + cline;
			*(uint32_t*)bt = border_color;
		}
		pos_to_border+=line;
	}

}

void put_texture( uint8_t *buffer_to, uint8_t *buffer_from, uint32_t width, uint32_t height, int from_width, int x, int y, int border, uint32_t border_color)
{
	int row	 = from_width * 4;
	int line = 1920 * 4;
	uint32_t pos_to = ( y * line) + (x * 4), cline=0;
	uint32_t pos_to_border = ( (y-border) * line) + ((x-border) * 4);
	uint32_t pos_from = 0;
	uint32_t lines=0;
	unsigned char* bt;

	if(border)
	{
		for(lines=0; lines<(height+(border*2)); lines++)
		{
			for(cline=0; cline<((width+border*2)*4); cline+=4)
			{
				bt = (uint8_t*)(buffer_to) + pos_to_border + cline;
				*(uint32_t*)bt = border_color;
			}
			pos_to_border+=line;
		}
	}

	for(lines=0; lines<height; lines++)
	{

		memcpy(buffer_to + pos_to, buffer_from + pos_from, width * 4);
		pos_from+=row;
		pos_to+=line;
	}

}

void put_texture_with_alpha( uint8_t *buffer_to, uint8_t *buffer_from, uint32_t _width, uint32_t _height, int from_width, int x, int y, int border, uint32_t border_color)
{
	int row	 = from_width * 4;
	int line = 1920 * 4;
	uint32_t pos_to = ( y * line) + (x * 4), cline=0;
	uint32_t pos_to_border = ( (y-border) * line) + ((x-border) * 4);
	uint32_t pos_from = 0;
	uint32_t lines=0;
	uint32_t c_pixel_N_R, c_pixel_N_G, c_pixel_N_B;
	uint32_t c_pixelR, c_pixelG, c_pixelB, c_pixel_N_A, c_pixel;
	uint32_t width=_width;
	uint32_t height=_height;
	unsigned char* bt;
	unsigned char* btF;
	if( (x+width) > 1920) width=(1920-x);
	if( (y+height) > 1080) height=(1080-y);

	if(border)
	{
		for(lines=0; lines<(height+(border*2)); lines++)
		{
			for(cline=0; cline<((width+border*2)*4); cline+=4)
			{
				bt = (uint8_t*)(buffer_to) + pos_to_border + cline;
				*(uint32_t*)bt = border_color;
			}
			pos_to_border+=line;
		}
	}

	for(lines=0; lines<height; lines++)
	{
		for(cline=0; cline<((width)*4); cline+=4)
		{
			btF = (uint8_t*)(buffer_from) + pos_from + cline;
			bt = (uint8_t*)(buffer_to) + pos_to + cline;

			c_pixel = *(uint32_t*)btF;
			c_pixel_N_A = (c_pixel    ) & 0xff;

			if(c_pixel_N_A)
			{

				float d_alpha  = (c_pixel_N_A / 255.0f);
				float d_alpha1 = 1.0f-d_alpha;
				c_pixel_N_R = (int)(buffer_from[pos_from + cline + 0] * d_alpha);
				c_pixel_N_G = (int)(buffer_from[pos_from + cline + 1] * d_alpha);
				c_pixel_N_B = (int)(buffer_from[pos_from + cline + 2] * d_alpha);

				c_pixelR = (int)(buffer_to[pos_to + cline + 0] * d_alpha1) + c_pixel_N_R;
				c_pixelG = (int)(buffer_to[pos_to + cline + 1] * d_alpha1) + c_pixel_N_G;
				c_pixelB = (int)(buffer_to[pos_to + cline + 2] * d_alpha1) + c_pixel_N_B;

				//keep the higher alpha
				*(uint32_t*)bt = ((buffer_to[pos_to + cline + 3]>(c_pixel_N_A) ? buffer_to[pos_to + cline + 3] : (c_pixel_N_A) )) | (c_pixelR<<24) | (c_pixelG<<16) | (c_pixelB<<8);
			}
		}

		pos_from+=row;
		pos_to+=line;
	}

}

void put_texture_with_alpha_gen( uint8_t *buffer_to, uint8_t *buffer_from, uint32_t _width, uint32_t _height, int from_width, u16 to_width, int x, int y)
{
	int row	 = from_width * 4;
	int line = to_width * 4;
	uint32_t pos_to = ( y * line) + (x * 4), cline=0;
	uint32_t pos_from = 0;
	uint32_t lines=0;
	uint32_t c_pixel_N_R, c_pixel_N_G, c_pixel_N_B;
	uint32_t c_pixelR, c_pixelG, c_pixelB, c_pixel_N_A, c_pixel;
	uint32_t width=_width;
	uint32_t height=_height;
	unsigned char* bt;
	unsigned char* btF;
	if( (x+width) > to_width) width=(to_width-x);

	for(lines=0; lines<height; lines++)
	{
		for(cline=0; cline<((width)*4); cline+=4)
		{
			btF = (uint8_t*)(buffer_from) + pos_from + cline;
			bt = (uint8_t*)(buffer_to) + pos_to + cline;

			c_pixel = *(uint32_t*)btF;
			c_pixel_N_A = (c_pixel    ) & 0xff;

			if(c_pixel_N_A)
			{

				float d_alpha  = (c_pixel_N_A / 255.0f);
				float d_alpha1 = 1.0f-d_alpha;
				c_pixel_N_R = (int)(buffer_from[pos_from + cline + 0] * d_alpha);
				c_pixel_N_G = (int)(buffer_from[pos_from + cline + 1] * d_alpha);
				c_pixel_N_B = (int)(buffer_from[pos_from + cline + 2] * d_alpha);

				c_pixelR = (int)(buffer_to[pos_to + cline + 0] * d_alpha1) + c_pixel_N_R;
				c_pixelG = (int)(buffer_to[pos_to + cline + 1] * d_alpha1) + c_pixel_N_G;
				c_pixelB = (int)(buffer_to[pos_to + cline + 2] * d_alpha1) + c_pixel_N_B;

				//keep the higher alpha
				*(uint32_t*)bt = ((buffer_to[pos_to + cline + 3]>(c_pixel_N_A) ? buffer_to[pos_to + cline + 3] : (c_pixel_N_A) )) | (c_pixelR<<24) | (c_pixelG<<16) | (c_pixelB<<8);
			}
		}

		pos_from+=row;
		pos_to+=line;
	}

}

void gray_texture( uint8_t *buffer_to, uint32_t width, uint32_t height, int step)
{
	uint32_t cline=0;
	uint16_t c_pixel;
	int line=0;
	(void) step;
	(void) line;

	for(cline=0; cline<(width*height*4); cline+=4)
	{
		/*if(step){
			line++;
			if(line>=width) {
				line=0;
				memset(buffer_to + cline, 0, width*4);
				cline+=width*4;
				continue;
			}
		}*/
		c_pixel = buffer_to[cline];
		c_pixel+= buffer_to[cline + 1];
		c_pixel+= buffer_to[cline + 2];
		memset(buffer_to + cline, (uint8_t) (c_pixel/3), 3);
	}
}

/*
void mip_texture( uint8_t *buffer_to, uint8_t *buffer_from, uint32_t width, uint32_t height, int scaleF)
{
	uint32_t pos_to = 0, pos_from = 0, cline=0, scale, cscale;
	uint32_t lines=0;

	if(scaleF<0)
	{
		scale=(-1)*scaleF;
		for(lines=0; lines<height; lines+=scale)
		{
			pos_from = lines * width * 4;
			for(cline=0; cline<(width*4); cline+=(4*scale))
			{
				memcpy(buffer_to + pos_to, buffer_from + pos_from + cline, 4);
				pos_to+=4;
			}
		}
	}
	else
	{
		scale=scaleF;

		for(lines=0; lines<height; lines++)
		{
			pos_from = lines * width * 4;
			for(cline=0; cline<(width*4); cline+=4)
			{
				for(cscale=0; cscale<scale; cscale++)
				{
					memcpy(buffer_to + pos_to, buffer_from + pos_from + cline, 4);
					pos_to+=4;
				}
			}

			for(cscale=0; cscale<(scale-1); cscale++)
			{
				memcpy(buffer_to + pos_to, buffer_to + pos_to - width * scale * 4, width * scale * 4);
				pos_to+=width * scale * 4;
			}
		}

	}

}


void blur_texture(uint8_t *buffer_to, uint32_t width, uint32_t height, int x, int y,  int wx, int wy, uint32_t c_BRI, int use_grayscale, int iterations, int p_range)
{

	int p_step = 4 * p_range;
	int row	 = width * p_step;

	int line = width * 4;
	uint32_t pos_to=0;
	int lines=0, cline=0, iter=0;
	(void) height;
	uint32_t c_pixel, c_pixelR, c_pixelG, c_pixelB, c_pixelR_AVG, c_pixelG_AVG, c_pixelB_AVG;
	int use_blur=1;

	if(iterations==0) {use_blur=0; iterations=1;}

	for(iter=0; iter<iterations; iter++)
	{
	pos_to = ( y * line) + (x * 4);

	for(lines=0; lines<wy; lines++)
	{

		for(cline=0; cline<(wx*4); cline+=4)
		{

			if(lines>=p_range && cline>=p_range && lines<(wy-p_range) && cline<((wx-p_range)*4))
			{

			if(use_blur)
			{
			// box blur
				// get RGB values for all surrounding pixels
				// to create average for blurring
				c_pixelB = buffer_to[pos_to + cline + 0 + p_step];
				c_pixelG = buffer_to[pos_to + cline + 1 + p_step];
				c_pixelR = buffer_to[pos_to + cline + 2 + p_step];

				c_pixelB+= buffer_to[pos_to + cline + 0 - p_step];
				c_pixelG+= buffer_to[pos_to + cline + 1 - p_step];
				c_pixelR+= buffer_to[pos_to + cline + 2 - p_step];

				c_pixelB+= buffer_to[pos_to + cline + 0 - row];
				c_pixelG+= buffer_to[pos_to + cline + 1 - row];
				c_pixelR+= buffer_to[pos_to + cline + 2 - row];

				c_pixelB+= buffer_to[pos_to + cline + 0 + row];
				c_pixelG+= buffer_to[pos_to + cline + 1 + row];
				c_pixelR+= buffer_to[pos_to + cline + 2 + row];

				c_pixelB+= buffer_to[pos_to + cline + 0 - row - p_step];
				c_pixelG+= buffer_to[pos_to + cline + 1 - row - p_step];
				c_pixelR+= buffer_to[pos_to + cline + 2 - row - p_step];

				c_pixelB+= buffer_to[pos_to + cline + 0 + row + p_step];
				c_pixelG+= buffer_to[pos_to + cline + 1 + row + p_step];
				c_pixelR+= buffer_to[pos_to + cline + 2 + row + p_step];

				c_pixelB+= buffer_to[pos_to + cline + 0 - row + p_step];
				c_pixelG+= buffer_to[pos_to + cline + 1 - row + p_step];
				c_pixelR+= buffer_to[pos_to + cline + 2 - row + p_step];

				c_pixelB+= buffer_to[pos_to + cline + 0 + row - p_step];
				c_pixelG+= buffer_to[pos_to + cline + 1 + row - p_step];
				c_pixelR+= buffer_to[pos_to + cline + 2 + row - p_step];

				// average values
				c_pixelB_AVG=((uint8_t) (c_pixelB/8));
				c_pixelG_AVG=((uint8_t) (c_pixelG/8));
				c_pixelR_AVG=((uint8_t) (c_pixelR/8));
			}
				else //no blur
				{
					c_pixelB_AVG = buffer_to[pos_to + cline + 0];
					c_pixelG_AVG = buffer_to[pos_to + cline + 1];
					c_pixelR_AVG = buffer_to[pos_to + cline + 2];
				}


				if(c_BRI>0) // increase brightnes by percent (101+=1%+)
				{
					c_pixelB_AVG=(uint32_t) (c_pixelB_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelB_AVG>0xff) c_pixelB_AVG=0xff;
					c_pixelG_AVG=(uint32_t) (c_pixelG_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelG_AVG>0xff) c_pixelG_AVG=0xff;
					c_pixelR_AVG=(uint32_t) (c_pixelR_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelR_AVG>0xff) c_pixelR_AVG=0xff;

				}

				if(use_grayscale)
				{
					// greyscale + box blur
					c_pixel = c_pixelB_AVG + c_pixelG_AVG + c_pixelR_AVG;
					memset(buffer_to + pos_to + cline + 0, (uint8_t) (c_pixel/3), 3);
				}
				else
				{
					buffer_to[pos_to + cline	]= c_pixelB_AVG;
					buffer_to[pos_to + cline + 1]= c_pixelG_AVG;
					buffer_to[pos_to + cline + 2]= c_pixelR_AVG;
				}
			}
			else
			{
				c_pixelB_AVG = buffer_to[pos_to + cline + 0];
				c_pixelG_AVG = buffer_to[pos_to + cline + 1];
				c_pixelR_AVG = buffer_to[pos_to + cline + 2];
				if(c_BRI>0) // increase brightnes by percent (101+=1%+)
				{
					c_pixelB_AVG=(uint32_t) (c_pixelB_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelB_AVG>0xff) c_pixelB_AVG=0xff;
					c_pixelG_AVG=(uint32_t) (c_pixelG_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelG_AVG>0xff) c_pixelG_AVG=0xff;
					c_pixelR_AVG=(uint32_t) (c_pixelR_AVG*(((float)c_BRI)/100.0f) ); if(c_pixelR_AVG>0xff) c_pixelR_AVG=0xff;

				}

				if(use_grayscale)
				{
					// greyscale + box blur
					c_pixel = c_pixelB_AVG + c_pixelG_AVG + c_pixelR_AVG;
					memset(buffer_to + pos_to + cline + 0, (uint8_t) (c_pixel/3), 3);
				}
				else
				{	buffer_to[pos_to + cline	]= c_pixelB_AVG;
					buffer_to[pos_to + cline + 1]= c_pixelG_AVG;
					buffer_to[pos_to + cline + 2]= c_pixelR_AVG;
				}

			}


			if(use_grayscale && !use_blur)
			{
				// convert to grayscale only
				c_pixel = buffer_to[pos_to + cline + 0];
				c_pixel+= buffer_to[pos_to + cline + 1];
				c_pixel+= buffer_to[pos_to + cline + 2];
				if(c_BRI>0)
					{ if(c_pixel>(c_BRI*3))
					c_pixel-=(c_BRI*3); else c_pixel=0; }
				memset(buffer_to + pos_to + cline, (uint8_t) (c_pixel/3), 3);
			}


			// keep alpha
			// memset(buffer_to + pos_to + cline + 3, buffer_to[pos_to + cline + 3], 1);
			//if(sub_menu_open)
			//buffer_to[pos_to + cline + 3] = 0x80;
		}

		pos_to+=line;
	}
	}//iterations
}
*/


//FONTS


void print_label(float x, float y, float scale, uint32_t color, char *str1p, float weight, float slant, int ufont)
{
	if(max_ttf_label<512)
	{
		ttf_label[max_ttf_label].x = x;
		ttf_label[max_ttf_label].y = y;
		ttf_label[max_ttf_label].scale = scale;
		ttf_label[max_ttf_label].color = color;
		ttf_label[max_ttf_label].weight = weight;
		ttf_label[max_ttf_label].slant = slant;
		ttf_label[max_ttf_label].font = ufont;
		ttf_label[max_ttf_label].hscale = 1.0f;
		ttf_label[max_ttf_label].vscale = 1.0f;
		ttf_label[max_ttf_label].centered = 0;
		ttf_label[max_ttf_label].cut = 0.0f;

		sprintf(ttf_label[max_ttf_label].label, "%s", str1p);
		max_ttf_label++;
	}
}

void print_label_width(float x, float y, float scale, uint32_t color, char *str1p, float weight, float slant, int ufont, float hscale, float vscale, int centered, float cut)
{
	if(max_ttf_label<512)
	{
		ttf_label[max_ttf_label].x = x;
		ttf_label[max_ttf_label].y = y;
		ttf_label[max_ttf_label].scale = scale;
		ttf_label[max_ttf_label].color = color;
		ttf_label[max_ttf_label].weight = weight;
		ttf_label[max_ttf_label].slant = slant;
		ttf_label[max_ttf_label].font = ufont;
		ttf_label[max_ttf_label].hscale = hscale;
		ttf_label[max_ttf_label].vscale = vscale;
		ttf_label[max_ttf_label].centered = centered;
		ttf_label[max_ttf_label].cut = cut;

		sprintf(ttf_label[max_ttf_label].label, "%s", str1p);
		max_ttf_label++;
	}
}

void print_label_ex(float x, float y, float scale, uint32_t color, char *str1p, float weight, float slant, int ufont, float hscale, float vscale, int centered)
{
	if(max_ttf_label<512)
	{
		ttf_label[max_ttf_label].x = x;
		ttf_label[max_ttf_label].y = y;
		ttf_label[max_ttf_label].scale = scale;
		ttf_label[max_ttf_label].color = color;
		ttf_label[max_ttf_label].weight = weight;
		ttf_label[max_ttf_label].slant = slant;
		ttf_label[max_ttf_label].font = ufont;
		ttf_label[max_ttf_label].hscale = hscale;
		ttf_label[max_ttf_label].vscale = vscale;
		ttf_label[max_ttf_label].centered = centered;
		ttf_label[max_ttf_label].cut = 0.0f;
		sprintf(ttf_label[max_ttf_label].label, "%s", str1p);
		max_ttf_label++;
	}
}

void flush_ttf(uint8_t *buffer, uint32_t _V_WIDTH, uint32_t _V_HEIGHT)
{
	if(!max_ttf_label) return;
		uint32_t color;
		CellFontRenderer* renderer;
		CellFontRenderSurface* surf;
		CellFont Font[1];
		CellFont* cf;
		int fn;

//		uint8_t *buffer = NULL;
//		buffer=(uint8_t*)(color_base_addr)+video_buffer*(c_frame_index);

		surf     = &RenderWork.Surface;
		cellFontRenderSurfaceInit( surf,
		                           buffer, _V_WIDTH*4, 4,
		                           _V_WIDTH, _V_HEIGHT );

		cellFontRenderSurfaceSetScissor( surf, 0, 0, _V_WIDTH, _V_HEIGHT );
		renderer = &RenderWork.Renderer;
		fn = FONT_SYSTEM_SERIF;

		if(user_font==1 || user_font>9) fn = FONT_SYSTEM_GOTHIC_JP;
		else if (user_font==2) fn = FONT_SYSTEM_GOTHIC_LATIN;
		else if (user_font==3) fn = FONT_SYSTEM_SANS_SERIF;
		else if (user_font==4) fn = FONT_SYSTEM_SERIF;
/*		else if (user_font>4 && user_font<10) fn=user_font+5;
		else if (user_font>14 && user_font<20) fn=user_font;
*/
		int ret;

		if(ttf_label[0].font!=0) fn=ttf_label[0].font;
		ret	= Fonts_AttachFont( fonts, fn, &Font[0] );

		if ( ret == CELL_OK ) cf = &Font[0];
		else                  cf = (CellFont*)0;

		if ( cf ) {
			static float textScale = 1.00f;
			static float weight = 1.00f;
			static float slant = 0.00f;
			float surfW = (float)_V_WIDTH;
			float surfH = (float)_V_HEIGHT;
			float textW;// = surfW;
			float textH;// = surfH;
			float step = 0.f;
			float lineH, baseY;
			float w;
			textW = surfW * textScale;
			textH = surfH * textScale;
			uint8_t* utf8Str0;
			float scale, scaley, x, y;
			int cl=0;

			for(cl=0; cl<max_ttf_label; cl++)
			{
				if(cl==0) Fonts_BindRenderer( cf, renderer );
				slant  = ttf_label[cl].slant;
				weight = ttf_label[cl].weight;

				scale  = 30.0f * ttf_label[cl].scale * (surfW/1920.0f) * ttf_label[cl].hscale;
				scaley = 30.0f * ttf_label[cl].scale * (surfH/1080.0f) * ttf_label[cl].vscale;

				Fonts_SetFontScale( cf, scale );
				Fonts_SetFontEffectWeight( cf, weight );
				Fonts_SetFontEffectSlant( cf, slant );
				ret = Fonts_GetFontHorizontalLayout( cf, &lineH, &baseY );

				utf8Str0 = (uint8_t*) ttf_label[cl].label;
				x = ttf_label[cl].x;
				y = ttf_label[cl].y;
				color = ttf_label[cl].color; //0x80ffffff;//

				if ( ret == CELL_OK ) {

					w = Fonts_GetPropTextWidth( cf, utf8Str0, scale, scaley, slant, step, NULL, NULL );

					if(ttf_label[cl].centered==1) x=(ttf_label[cl].x - (w/2.0f)/surfW);
					else if(ttf_label[cl].centered==2) x=(ttf_label[cl].x - (w)/surfW); //right justified


					if ( ( (w+(x*surfW)) > textW) && (ttf_label[cl].cut==0.0f) && ttf_label[cl].centered!=2) {
						float ratio;

						scale = Fonts_GetPropTextWidthRescale( scale, w, (textW-(x*surfW)), &ratio );
						w     *= ratio;
						//baseY *= ratio;
						//lineH *= ratio;
						step  *= ratio;
						if(ttf_label[cl].centered==1) x=(ttf_label[cl].x - (w/2.0f)/surfW);
						else if(ttf_label[cl].centered==2) x=(ttf_label[cl].x - (w)/surfW); //right justified

					}
					else if ( (ttf_label[cl].cut>0.0f) && (w>((int)(ttf_label[cl].cut*(float)_V_WIDTH))) ) {
						float ratio;

						scale = Fonts_GetPropTextWidthRescale( scale, w, ((ttf_label[cl].cut*(float)_V_WIDTH)), &ratio );
						w     *= ratio;
						//baseY *= ratio;
						//lineH *= ratio;
						step  *= ratio;

						if(ttf_label[cl].centered==1) x=(ttf_label[cl].x - (w/2.0f)/surfW);
						else if(ttf_label[cl].centered==2) x=(ttf_label[cl].x - (w)/surfW); //right justified

					}

					if(!in_xmb_mode()) Fonts_RenderPropText( cf, surf, (int)(x*surfW)+1, (int)(y*surfH)+1, utf8Str0, scale, scaley, slant, step, 0xff000000 ); // && cover_mode!=5
					if(in_xmb_mode()) Fonts_RenderPropText( cf, surf, (int)(x*surfW)+2, (int)(y*surfH)+2, utf8Str0, scale, scaley, slant, step, 0x10101010 );
					// &&
					Fonts_RenderPropText( cf, surf, (int)(x*surfW), (int)(y*surfH), utf8Str0, scale, scaley, slant, step, color );//(color & 0x00ffffff)

				}
			}
					Fonts_UnbindRenderer( cf );

			Fonts_DetachFont( cf );
		}

	max_ttf_label=0;
}


/****************************************************/
/* UTILS                                            */
/****************************************************/

void fix_perm_recursive(const char* start_path)
{
	new_pad=0; old_pad=0;
	if(abort_rec==1) return;

    int dir_fd;
    uint64_t nread;
    char f_name[CELL_FS_MAX_FS_FILE_NAME_LENGTH+1];
    CellFsDirent dir_ent;
    CellFsErrno err;
    cellFsChmod(start_path, 0777);


	flip();
    if (cellFsOpendir(start_path, &dir_fd) == CELL_FS_SUCCEEDED)
    {
        cellFsChmod(start_path, 0777);
        while (1) {
			pad_read();
			if ( (old_pad & BUTTON_CIRCLE) || (old_pad & BUTTON_TRIANGLE) || dialog_ret==3) { abort_rec=1; new_pad=0; old_pad=0; break; } //
            err = cellFsReaddir(dir_fd, &dir_ent, &nread);
            if (nread != 0) {
                if (!strcmp(dir_ent.d_name, ".") || !strcmp(dir_ent.d_name, ".."))
                    continue;

                sprintf(f_name, "%s/%s", start_path, dir_ent.d_name);

                if (dir_ent.d_type == CELL_FS_TYPE_DIRECTORY)
                {
                    cellFsChmod(f_name, CELL_FS_S_IFDIR | 0777);
                    fix_perm_recursive(f_name);
					if(abort_rec==1) break;
                }
                else if (dir_ent.d_type == CELL_FS_TYPE_REGULAR)
                {
                    cellFsChmod(f_name, 0666);
                }
            } else {
                break;
            }
        }
        err = cellFsClosedir(dir_fd);
    }
}


static u32 get_crc(char *_path)
{
	FILE *fp;
	u32 crc=0;
	fp = fopen(_path, "rb");
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		uint32_t len = ftell(fp);
		unsigned char *mem = NULL;
		mem= (unsigned char *) memalign(16, len+16);
		fseek(fp, 0, SEEK_SET);
		fread((void *) mem, len, 1, fp);
		fclose(fp);
		uint32_t crc_c;
		for(crc_c=0; crc_c<len; crc_c++) crc+=mem[crc_c];
		free(mem);
	}
#ifdef USE_DEBUG
	MM_LOG("libfs CRC = [%s] 0x%xULL\n", _path, crc);
#endif
	return crc;
}

static double get_system_version(void)
{
	FILE *fp;
	float base=3.55f;
	fp = fopen("/dev_flash/vsh/etc/version.txt", "rb");
	if (fp != NULL) {
		char bufs[1024];
		fgets(bufs, 1024, fp);
		fclose(fp);
		base = strtod(bufs + 8, NULL); // this is either the spoofed or actual version
	}
	else
	{
		//if(peekq(0x80000000002EFE20ULL)==0x4445580000000000ULL) {dex_mode=2; return 3.55f;}
		//if(peekq(0x8000000000302D88ULL)==0x4445580000000000ULL) {dex_mode=2; return 4.21f;}
		//dex_mode=1;
		return 0.0f;
	}

	//if(peekq(0x8000000000302D88ULL)==0x4445580000000000ULL) {dex_mode=2; return 4.21f;}

	float base2=base;

	u32 crc=get_crc((char*) "/dev_flash/sys/external/libfs.sprx");
	if(crc==0x416bbaULL) base=3.15f; else // ignore spoofers by crcing libfs
	if(crc==0x41721eULL) base=3.41f; else // ofw   3.41
	if(crc==0x419d7eULL) base=3.41f; else // rebug 3.41.3 , rogero 3.55 3.7a
	if(crc==0x41655eULL) base=3.55f; else // kmeaw 3.55
	if(crc==0x4133c3ULL) base=3.55f; else // rebug 3.55.2
	//0x4133c3ULL rebug 3.55.2 original
	//0x40ca20ULL rebug 3.55.2 aio mod
	if(	   crc==0x4092ffULL
		|| crc==0x4109deULL
		|| crc==0x40e554ULL //kmeaw mod
		|| crc==0x40ca20ULL // rebug mod
		|| crc==0x40baa9ULL // 341ofw mod
		|| crc==0x45A8E9ULL // 421cfw mod
		)
	{
		base=3.55f;	  // kmeaw 3.55 with modded AIO
		if(crc==0x45A8E9ULL)
			base=4.21f;
		else
		if(crc==0x40baa9ULL)
			base=3.41f;
	}
	if(dex_mode) return base2;

	return base;
}

/****************************************************/
/* FILE UTILS                                       */
/****************************************************/

void file_copy(char *path, char *path2, int progress)
{
	if(path[0]!='/' || path2[0]!='/' || (strstr(path2, "/dev_bdvd")!=NULL)) return;

		uint64_t _copy_global_bytes=0x00ULL;
		uint64_t _global_device_bytes=0x00ULL;

		if(progress)
		{
			//flipc(30);
			dialog_ret=0;
		}

		char rdr[512];
		time_start=time(NULL);
		int fs;
		int fd;
		uint64_t fsiz = 0;
		uint64_t msiz = 0;
		sprintf(rdr, "%s", path);
		int seconds=0;
		off64_t perc = 0;
		int new_perc, delta;
		int idw0;

		cellFsOpen(path, CELL_FS_O_RDONLY, &fs, NULL, 0);
		cellFsLseek(fs, 0, CELL_FS_SEEK_END, &msiz);
		cellFsClose(fs);

		uint64_t chunk = BUF_SIZE;
		if(msiz<chunk && msiz>0) chunk=msiz;

		abort_copy=0;

		cellFsOpen(rdr, CELL_FS_O_RDONLY, &fs, NULL, 0);

		copy_file_counter=1;
		_copy_global_bytes=msiz;

		unsigned char* buf = (unsigned char*) memalign(128, chunk*2);

		u8   split_part=0;
		bool split_mode=(strstr(path2, "/dev_hdd")==NULL);
		u64  split_segment=0;
		u64 split_limit=0xFFFF0000ULL;

		bool is_iso=(strstr(path, ".iso")!=NULL || strstr(path, ".ISO")!=NULL);

		if(split_mode && ( (is_size(path))>=split_limit ) )
		{
			if(is_iso)
				sprintf(rdr, "%s.%i", path2, split_part);
			else
				sprintf(rdr, "%s.666%02i", path2, split_part);
		}
		else
			sprintf(rdr, "%s", path2);

		CellFsAio aiow0;
		initAIO();

		if(progress) dialog_ret=0;
		pad_read();
		cellFsOpen(rdr, CELL_FS_O_CREAT|CELL_FS_O_WRONLY|CELL_FS_O_TRUNC, &fd, NULL, 0); //CELL_FS_O_CREAT|CELL_FS_O_RDWR|CELL_FS_O_TRUNC

		unsigned char *rbuf[2];
		rbuf[0]=buf;
		rbuf[1]=buf+chunk;
		u8 c_buf=0;

		aiow0.fd = fd;
		aiow0.offset = 0;
		aiow0.buf = rbuf[c_buf];
		aiow0.user_data = 0;

		while(fsiz < msiz && abort_copy==0)
		{
			if((fsiz+chunk) > msiz)
				chunk = (msiz-fsiz);

			if(cellFsRead(fs, (void *)rbuf[c_buf], chunk, NULL)!=CELL_FS_SUCCEEDED)	{abort_copy=1;break;}

			while(1)
			{
				if(aiow0.user_data==0)						// can write - initiate aioWrite
				{
					//memcpy(buf2w, buf, chunk);
					aiow0.size = chunk;
					aiow0.user_data=1;
					aiow0.buf = rbuf[c_buf];
					if(cellFsAioWrite(&aiow0, &idw0, callback_aio)!=CELL_FS_SUCCEEDED) {abort_copy=1; break;}
					c_buf=1-c_buf;
					break;
				}
				else
				{
					if(aiow0.user_data==1)					// aioWrite still writing - check callbacks
						{cellSysutilCheckCallback();}
					else
						{abort_copy=1; break;}				// write failed - abort
				}
			}

			if(aiow0.user_data==3) {abort_copy=1; break;}

			fsiz = fsiz + chunk;
			_global_device_bytes+=chunk;
			split_segment+=chunk;
			if(fsiz>=msiz) break;

			if(split_mode && ( (split_segment+chunk)>=split_limit))
			{
				if(aiow0.user_data==1) // wait for final chunk to be written
				while(1)
				{
					if(aiow0.user_data==0) {break;}
					if(aiow0.user_data==3) {abort_copy=1; break;}
					cellSysutilCheckCallback();
				}
				cellFsClose(fd);
				cellFsChmod(rdr, 0666);
				split_part++;
				split_segment=0;
				if(is_iso)
					sprintf(rdr, "%s.%i", path2, split_part);
				else
					sprintf(rdr, "%s.666%02i", path2, split_part);
				remove(rdr);
				cellFsOpen(rdr, CELL_FS_O_CREAT|CELL_FS_O_RDWR|CELL_FS_O_TRUNC, &fd, NULL, 0);
				aiow0.fd = fd;
			}

			seconds= (int) (time(NULL)-time_start);
			new_perc = (_global_device_bytes * 100) / ( _copy_global_bytes+1);
			delta = new_perc - perc;

			if ( progress && dialog_ret) {abort_copy=1; new_pad=0; old_pad=0; break;}
		}

		if(aiow0.user_data==1) // wait for final chunk to be written
		while(1)
		{
			if(aiow0.user_data==0) {break;}
			if(aiow0.user_data==3) {abort_copy=1; break;}
			cellSysutilCheckCallback();
		}
		cellFsClose(fd);

		cellFsClose(fs);

		cellFsChmod(rdr, 0666);
		if( _global_device_bytes != _copy_global_bytes) abort_copy=1;
		if(abort_copy==1) remove(path2);
		if(progress!=0)
		{
			cellMsgDialogAbort();//sys_timer_usleep(100000); flip();
		}
		free(buf);
		dialog_ret=0;
		ss_timer=0;
		ss_timer_last=time(NULL);
}


/*
void open_osk(int for_what, char *init_text)
{
	char orig[512];

	if(for_what==1) sprintf(orig, (const char*) "Rename [%s] to:", init_text);
	if(for_what==2)	sprintf(orig, "%s", (const char*) "CREATE NEW FOLDER - Enter name for the new folder:");
	if(for_what==3)	sprintf(orig, "%s", init_text);
	if(for_what==4)	sprintf(orig, "%s", init_text);

    wchar_t my_message[((strlen(orig) + 1)*2)];
    mbstowcs(my_message, orig, (strlen(orig) + 1));

    wchar_t INIT_TEXT[((strlen(init_text) + 1)*2)];
    mbstowcs(INIT_TEXT, init_text, (strlen(init_text) + 1));
	if(for_what==2 || for_what==3) INIT_TEXT[0]=0;

	inputFieldInfo.message = (uint16_t*)my_message;
	inputFieldInfo.init_text = (uint16_t*)INIT_TEXT;
	inputFieldInfo.limit_length = 128;

	CellOskDialogPoint pos;
	pos.x = 0.0; pos.y = 0.5;
	int32_t LayoutMode = CELL_OSKDIALOG_LAYOUTMODE_X_ALIGN_CENTER;

	CellOskDialogParam dialogParam;

	if(for_what==3)
	{
	inputFieldInfo.limit_length = 4;
	cellOskDialogSetKeyLayoutOption (CELL_OSKDIALOG_10KEY_PANEL);
	cellOskDialogAddSupportLanguage (CELL_OSKDIALOG_PANELMODE_PASSWORD | CELL_OSKDIALOG_PANELMODE_NUMERAL);

	dialogParam.allowOskPanelFlg = ( CELL_OSKDIALOG_PANELMODE_NUMERAL | CELL_OSKDIALOG_PANELMODE_PASSWORD);
	dialogParam.firstViewPanel = CELL_OSKDIALOG_PANELMODE_NUMERAL;

	}
	else
	{
	cellOskDialogSetKeyLayoutOption (CELL_OSKDIALOG_10KEY_PANEL | CELL_OSKDIALOG_FULLKEY_PANEL);
	cellOskDialogAddSupportLanguage (CELL_OSKDIALOG_PANELMODE_ALPHABET | CELL_OSKDIALOG_PANELMODE_NUMERAL |
		CELL_OSKDIALOG_PANELMODE_ENGLISH |
		CELL_OSKDIALOG_PANELMODE_DEFAULT |
		CELL_OSKDIALOG_PANELMODE_SPANISH |
		CELL_OSKDIALOG_PANELMODE_FRENCH |
		CELL_OSKDIALOG_PANELMODE_RUSSIAN |
		CELL_OSKDIALOG_PANELMODE_JAPANESE |
		CELL_OSKDIALOG_PANELMODE_CHINA_TRADITIONAL);

	dialogParam.allowOskPanelFlg = (CELL_OSKDIALOG_PANELMODE_ALPHABET | CELL_OSKDIALOG_PANELMODE_NUMERAL |
		CELL_OSKDIALOG_PANELMODE_ENGLISH |
		CELL_OSKDIALOG_PANELMODE_DEFAULT |
		CELL_OSKDIALOG_PANELMODE_SPANISH |
		CELL_OSKDIALOG_PANELMODE_FRENCH |
		CELL_OSKDIALOG_PANELMODE_RUSSIAN |
		CELL_OSKDIALOG_PANELMODE_JAPANESE |
		CELL_OSKDIALOG_PANELMODE_CHINA_TRADITIONAL);
	dialogParam.firstViewPanel = CELL_OSKDIALOG_PANELMODE_ALPHABET_FULL_WIDTH;
	}

	cellOskDialogSetLayoutMode( LayoutMode );

	dialogParam.controlPoint = pos;
	dialogParam.prohibitFlgs = CELL_OSKDIALOG_NO_RETURN;
	cellOskDialogSetInitialInputDevice(CELL_OSKDIALOG_INPUT_DEVICE_PAD );
	osk_dialog=0;
	if(sys_memory_container_create( &memory_container, MEMORY_CONTAINER_SIZE_KB)==CELL_OK)
	{
		cellOskDialogLoadAsync(memory_container, &dialogParam, &inputFieldInfo);
		osk_open=for_what;
	}
	else
		osk_open=0;

};
*/

//petiboot
#define VFLASH5_DEV_ID								0x100000500000001ull
#define VFLASH5_SECTOR_SIZE						0x200ull
#define VFLASH5_SECTORS								0xc000ull
#define VFLASH5_HEADER_SECTORS				0x2ull
#define VFLASH5_OS_DB_AREA_SECTORS		0x2ull

#define PETITBOOT_FILENAME		"dtbImage.ps3.bin"
#define PETITBOOT_FILENAME2	    "dtbImage.ps3.bin.minimal"


void install_petiboot()
{
#define MIN(a, b)	((a) <= (b) ? (a) : (b))

	bool found=false;
	char _path[64];

	for(int i = 0; i < 99; i++)
	{
	 if(is_nor()) sprintf(_path, "/dev_usb%03i/" PETITBOOT_FILENAME, i); else sprintf(_path, "/dev_usb%03i/" PETITBOOT_FILENAME2, i);
	 if(exist(_path)) {found=true; break;}
	}

	if(!found)
	{
	 cellMsgDialogAbort();
	 if(is_nor()) sprintf(_path, PETITBOOT_FILENAME " %s", STR_NOTFOUND); else sprintf(_path, PETITBOOT_FILENAME2 " %s", STR_NOTFOUND);
	 dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, (const char*) _path, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	 return;
	}

	uint32_t dev_handle;
	FILE *fp;
	int file_size, file_sectors, start_sector, sector_count;
	struct storage_device_info info;
	uint8_t buf[VFLASH5_SECTOR_SIZE * 16];
	struct os_area_header *hdr;
	struct os_area_params *params;
	struct os_area_db *db;
	uint32_t unknown2;
	int result;

	dev_handle = 0;
	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_yes_no, STR_PETBOOTINST, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog();
	if(dialog_ret!=1)  return;


	open_please_wait();

	fp = fopen(_path, "r");
	if (!fp) goto done;

	result = fseek(fp, 0, SEEK_END);
	if (result) goto done;

	file_size = ftell(fp);
	file_sectors = (file_size + VFLASH5_SECTOR_SIZE - 1) / VFLASH5_SECTOR_SIZE;

	if (file_sectors > 0xBFFC) goto done;


	result = fseek(fp, 0, SEEK_SET);
	if (result) goto done;


	result = lv2_storage_get_device_info(VFLASH5_DEV_ID, &info);
	if (result) goto done;

	if (info.capacity < (VFLASH5_HEADER_SECTORS + VFLASH5_OS_DB_AREA_SECTORS + file_sectors)) goto done;

	result = lv2_storage_open(VFLASH5_DEV_ID, &dev_handle);
	if (result) goto done;

	/* write os header and db area */

	start_sector = 0;
	sector_count = VFLASH5_HEADER_SECTORS + VFLASH5_OS_DB_AREA_SECTORS;

	memset(buf, 0, sizeof(buf));
	hdr = (struct os_area_header *) buf;
	params = (struct os_area_params *) (buf + OS_AREA_SEGMENT_SIZE);
	db = (struct os_area_db *) (buf + VFLASH5_HEADER_SECTORS * OS_AREA_SEGMENT_SIZE);

	strncpy((char *) hdr->magic, HEADER_MAGIC, sizeof(hdr->magic));
	hdr->version = HEADER_VERSION;
	hdr->db_area_offset = VFLASH5_HEADER_SECTORS; /* in sectors */
	hdr->ldr_area_offset = VFLASH5_HEADER_SECTORS + VFLASH5_OS_DB_AREA_SECTORS; /* in sectors */
	hdr->ldr_format = HEADER_LDR_FORMAT_RAW; /* we do not use gzip format !!! */
	hdr->ldr_size = file_size;

	params->boot_flag = PARAM_BOOT_FLAG_GAME_OS;
	params->num_params = 0;

	db->magic = DB_MAGIC;
	db->version = DB_VERSION;
	db->index_64 = 24;
	db->count_64 = 57;
	db->index_32 = 544;
	db->count_32 = 57;
	db->index_16 = 836;
	db->count_16 = 57;

	result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
	if (result) {
		goto done;
	}

	start_sector += VFLASH5_HEADER_SECTORS + VFLASH5_OS_DB_AREA_SECTORS;

	while (file_sectors)
	{
		sector_count = MIN(file_sectors, 16);

		result = fread(buf, 1, sector_count * VFLASH5_SECTOR_SIZE, fp);
		if (result < 0) goto done;

		result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
		if (result) goto done;

		sys_timer_usleep(10000);

		file_sectors -= sector_count;
		start_sector += sector_count;
	}
		cellMsgDialogAbort();

	if(fp) fclose(fp);

	lv2_storage_close(dev_handle);
	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_PETBOOTOK, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	return;

done:

	if(fp) fclose(fp);

	lv2_storage_close(dev_handle);

	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_PETBOOTERROR, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();

	return;

#undef MIN
}
//petiboot end

//setup vflash/nand region 5
#define PARTITION_TABLE_MAGIC1												0x000000000face0ffull
#define PARTITION_TABLE_MAGIC2												0x00000000deadfaceull

/* VFLASH */

#define VFLASH_DEV_ID																	0x100000000000001ull
#define VFLASH_SECTOR_SIZE														0x200ull
#define VFLASH_START_SECTOR														0x0ull
#define VFLASH_SECTOR_COUNT														0x2ull
#define VFLASH_FLAGS																	0x6ull

#define VFLASH_PARTITION_TABLE_6ND_REGION_OFFSET			0x270ull
#define VFLASH_6TH_REGION_NEW_SECTOR_COUNT						0xc000ull

#define VFLASH_PARTITION_TABLE_7TH_REGION_OFFSET			0x300ull
#define VFLASH_7TH_REGION_NEW_START_SECTOR						0x7fa00ull

/* NAND FLASH */

#define FLASH_DEV_ID																	0x100000000000001ull
#define FLASH_SECTOR_SIZE															0x200ull
#define FLASH_START_SECTOR														0x7600ull
#define FLASH_SECTOR_COUNT														0x2ull
#define NFLASH_FLAGS																		0x6ull

#define FLASH_PARTITION_TABLE_6ND_REGION_OFFSET				0x270ull
#define FLASH_6TH_REGION_NEW_START_SECTOR							0x73a00ull
#define FLASH_6TH_REGION_NEW_SECTOR_COUNT							0x4200ull

#define FLASH_PARTITION_TABLE_7TH_REGION_OFFSET				0x300ull
#define FLASH_7TH_REGION_NEW_START_SECTOR							0x77c00ull
#define FLASH_7TH_REGION_NEW_SECTOR_COUNT							0x200ull

#define FLASH_REGION_LPAR_AUTH_ID											0x1070000002000001ull /* GameOS LPAR auth id */
#define FLASH_REGION_ACL															0x3ull

/* is_vflash_on */
static int is_vflash_on(void){
	uint8_t flag;

	lv2_ss_get_cache_of_flash_ext_flag(&flag);

	return !(flag & 0x1);
}

/* setup_vflash */
static void setup_vflash(void){
	uint32_t dev_handle;
	int start_sector, sector_count;
	uint32_t unknown2;
	uint8_t buf[VFLASH_SECTOR_SIZE * VFLASH_SECTOR_COUNT];
	uint64_t *ptr;
	int result;

	dev_handle = 0;

	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_yes_no, STR_RESIZE5, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog();
        if(dialog_ret!=1)  {lv2_storage_close(dev_handle); return;}


	result = lv2_storage_open(VFLASH_DEV_ID, &dev_handle);
	if (result) {
		goto done;
	}

	start_sector = VFLASH_START_SECTOR;
	sector_count = VFLASH_SECTOR_COUNT;

	result = lv2_storage_read(dev_handle, 0, start_sector, sector_count, buf, &unknown2, VFLASH_FLAGS);
	if (result) {
		goto done;
	}

	sys_timer_usleep(10000);

	/* check partition table magic */

	if ((*((uint64_t *) buf + 2) != PARTITION_TABLE_MAGIC1) ||
		(*((uint64_t *) buf + 3) != PARTITION_TABLE_MAGIC2)) {
		goto done;
	}

	/* patch sector count of VFLASH 6th region */

	ptr = (uint64_t *) (buf + VFLASH_PARTITION_TABLE_6ND_REGION_OFFSET + 0x8ull);

	*ptr = VFLASH_6TH_REGION_NEW_SECTOR_COUNT;

	/* patch start sector of VFLASH 7th region */

	ptr = (uint64_t *) (buf + VFLASH_PARTITION_TABLE_7TH_REGION_OFFSET);

	*ptr = VFLASH_7TH_REGION_NEW_START_SECTOR;

	result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, VFLASH_FLAGS);
	if (result) {
		goto done;
	}

	sys_timer_usleep(10000);

	lv2_storage_close(dev_handle);

	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_RESIZE5OK, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();

	return;

done:

	lv2_storage_close(dev_handle);

	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_RESIZE5ERROR, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();

	return;
}

/* setup_flash */
static void setup_flash(void)
{
	uint32_t dev_handle;
	int start_sector, sector_count;
	uint32_t unknown2;
	uint8_t buf[FLASH_SECTOR_SIZE * FLASH_SECTOR_COUNT];
	uint64_t *ptr;
	int result;

	dev_handle = 0;

	result = lv2_storage_open(FLASH_DEV_ID, &dev_handle);
	if (result) {
		goto done;
	}

	start_sector = FLASH_START_SECTOR;
	sector_count = FLASH_SECTOR_COUNT;

	result = lv2_storage_read(dev_handle, 0, start_sector, sector_count, buf, &unknown2, NFLASH_FLAGS);
	if (result) {
		goto done;
	}

	sys_timer_usleep(10000);

	/* check partition table magic */

	if ((*((uint64_t *) buf + 2) != PARTITION_TABLE_MAGIC1) ||
		(*((uint64_t *) buf + 3) != PARTITION_TABLE_MAGIC2)) {
		goto done;
	}

	/* patch FLASH 6th region */

	ptr = (uint64_t *) (buf + FLASH_PARTITION_TABLE_6ND_REGION_OFFSET);

	*ptr++ = FLASH_6TH_REGION_NEW_START_SECTOR;
	*ptr++ = FLASH_6TH_REGION_NEW_SECTOR_COUNT;
	*ptr++ = FLASH_REGION_LPAR_AUTH_ID;
	*ptr++ = FLASH_REGION_ACL;

	/* patch FLASH 7th region */

	ptr = (uint64_t *) (buf + FLASH_PARTITION_TABLE_7TH_REGION_OFFSET);

	*ptr++ = FLASH_7TH_REGION_NEW_START_SECTOR;
	*ptr++ = FLASH_7TH_REGION_NEW_SECTOR_COUNT;
	*ptr++ = FLASH_REGION_LPAR_AUTH_ID;
	*ptr++ = FLASH_REGION_ACL;

	result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, NFLASH_FLAGS);
	if (result) {
		goto done;
	}

	sys_timer_usleep(10000);

	lv2_storage_close(dev_handle);

	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_FLASHOK, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();

	return;

done:

	lv2_storage_close(dev_handle);

	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_FLASHERROR, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();

	return;
}

/* main */
void resize_vflash()
{
	int vflash_on;

	vflash_on = is_vflash_on();

	if (vflash_on) {
		setup_vflash();
	} else {
		setup_flash();
	}

	return;
}

int parse_ini()
{
    char line [ 256 ];

    FILE *fp;

    if(!exist( (char*) "/dev_hdd0/game/RBGTLBOX2/USRDIR/options.bin")) return 0;

    fp = fopen ( (char*) "/dev_hdd0/game/RBGTLBOX2/USRDIR/options.bin", "r" );

    if( fp != NULL )
    {

        while ( fgets ( line, sizeof line, fp ) != NULL ) /* read a line */
        {
            if(line[0]==35) continue;

            if(strstr (line,"confirm_with_x=0")!=NULL) {confirm_with_x=0; set_xo();}
            if(strstr (line,"confirm_with_x=1")!=NULL) {confirm_with_x=1; set_xo();}
        }

        fclose ( fp );
    }

    return 0;
}


void clean_up()
{
	char cleanup[48]=" ";
	if(clear_activity_logs==1)
	{
		sprintf(cleanup, "/dev_hdd0/vsh/pushlist/patch.dat"); remove(cleanup);
		sprintf(cleanup, "/dev_hdd0/vsh/pushlist/game.dat"); remove(cleanup);
		for(int n2=0;n2<20;n2++) {sprintf(cleanup, "/dev_hdd0/home/000000%02i/etc/boot_history.dat", n2); remove(cleanup);}
	}
	for(int n2=0;n2<20;n2++) {
		sprintf(cleanup, "/dev_hdd0/home/000000%02i", n2); cellFsChmod(cleanup, CELL_FS_S_IFDIR | 0777);
		sprintf(cleanup, "/dev_hdd0/home/000000%02i/savedata", n2); cellFsChmod(cleanup, CELL_FS_S_IFDIR | 0777);
		}
	sprintf(cleanup, "%s", "/dev_hdd0/game"); cellFsChmod(cleanup, CELL_FS_S_IFDIR | 0777);
}

void draw_xmb_bg()
{
	set_texture( text_bmp, 1920, 1080);
	display_img(0, 0, 1920, 1080, xmbbg_user_w, xmbbg_user_h, 0.9f, 1920, 1080);
}

void load_xmb_bg()
{
	memset(text_bmp, 0, 8294400);
	load_texture(text_bmp, xmbbg, 1920);
	xmbbg_user_w=png_w;
	xmbbg_user_h=png_h;
}

void draw_xmb_title(u8 *buffer, xmbmem *member, int cn, u32 col1, u32 col2, u8 _xmb_col, float xsize)
{
		memset(buffer, 0, XMB_TEXT_WIDTH*XMB_TEXT_HEIGHT*4); //flush_ttf(buffer, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT);

		if(_xmb_col!=1 && member[cn].option_size) //settings
			print_label_ex( 0.99f, 0.2f, 0.48f, col1, member[cn].option[member[cn].option_selected].label, 1.04f, 0.0f, 0, 3.0f, 23.0f, 2);

		print_label_ex( 0.000f, 0.52f, 0.87f, col2, member[cn].subname, 1.02f, 0.0f, 0, (1920.f/(float)XMB_TEXT_WIDTH), 15.5f, 0); //3.0f
		print_label_ex( 0.000f, 0.02f, 1.10f, col1, member[cn].name, 1.04f, 0.0f, 0, (1920.f/(float)XMB_TEXT_WIDTH)+xsize, 15.0f, 0);//3.0f

		flush_ttf(buffer, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT);
}

void mod_xmb_member(xmbmem *_member, u16 size, char *_name, char *_subname)
{
	snprintf(_member[size].name, sizeof(_member[size].name), "%s", _name);
	_member[size].name[sizeof(_member[size].name)]=0;
	snprintf(_member[size].subname, sizeof(_member[size].subname), "%s", _subname);
	_member[size].subname[sizeof(_member[size].subname)]=0;
}

void add_xmb_member(xmbmem *_member, volatile u16 *_size, char *_name, char *_subname,
		/*type*/u8 _type, /*status*/u8 _status, /*icon*/u8 *_data, u16 _iconw, u16 _iconh)
{
	if( (*_size)>=(MAX_XMB_MEMBERS-1)  ) return;

	u16 size=(*_size);
	_member[size].type		=_type;
	_member[size].status	=_status;

	snprintf(_member[size].name, sizeof(_member[size].name), "%s", _name);
	_member[size].name[sizeof(_member[size].name)-1]=0;
	snprintf(_member[size].subname, sizeof(_member[size].subname), "%s", _subname);
	_member[size].subname[sizeof(_member[size].subname)-1]=0;

	_member[size].option_size=0;
	_member[size].option_selected=0;

	_member[size].icon  =_data;
	_member[size].icon_buf = -1;
	_member[size].iconw =_iconw;
	_member[size].iconh =_iconh;

	(*_size)++;
}

void add_xmb_suboption(xmbopt *_option, u8 *_size, u8 _type, char *_label, char *_value)
{
	(void) _type;
	if((*_size)>=MAX_XMB_OPTIONS) return;
	u8 size=(*_size);
	sprintf(_option[size].label, "%s", _label);
	sprintf(_option[size].value, "%s", _value);
	(*_size)++;
}

void add_xmb_option(xmbmem *_member, volatile u16 *_size, char *_name, char *_subname, char *_optionini)
{
	if((*_size)>=(MAX_XMB_MEMBERS-1)) return;

	u16 size=(*_size);
	_member[size].type		=  7;//option
	_member[size].status	=  2;//loaded

	snprintf(_member[size].name, sizeof(_member[size].name), "%s", _name);
	_member[size].name[sizeof(_member[size].name)-1]=0;
	snprintf(_member[size].subname, sizeof(_member[size].subname), "%s", _subname);
	_member[size].subname[sizeof(_member[size].subname)-1]=0;

	_member[size].option_size=0;
	_member[size].option_selected=0;
	snprintf(_member[size].optionini, sizeof(_member[size].option), "%s", _optionini);
	_member[size].subname[sizeof(_member[size].option)-1]=0;

	_member[size].data=-1;
	_member[size].icon  = xmb_icon_tool;
	_member[size].icon_buf = -1;
	_member[size].iconw = 128;
	_member[size].iconh = 128;

	(*_size)++;
}

void free_text_buffers()
{
	for(int n=0; n<MAX_XMB_TEXTS; n++)
	{
		xmb_txt_buf[n].used=0;
		xmb_txt_buf[n].data=text_bmpUPSR+(n*XMB_TEXT_WIDTH*XMB_TEXT_HEIGHT*4);
	}
	xmb_txt_buf_max=0;

	for(int c=0; c<MAX_XMB_ICONS; c++)
		for(int n=0; n<xmb[c].size; n++) xmb[c].member[n].data=-1;

}

void free_all_buffers()
{
	while(is_decoding_jpg || is_decoding_png){ sys_timer_usleep(3336); cellSysutilCheckCallback();}
	int n;
	for(n=0; n<MAX_XMB_THUMBS; n++) xmb_icon_buf[n].used=-1;
}

void reset_xmb()
{
	for(int n=0; n<MAX_XMB_ICONS; n++)
	{
		xmb[n].size=0;
		xmb[n].first=0;
		xmb[n].init=0;
		xmb[n].data=text_FMS+(n*65536);
	}
	//xmb[8].data=xmb_icon_retro;
	//xmb[9].data=text_FMS+(8*65536);
}

int find_free_buffer(const int _col)
{
	(void) _col;
	int n;
	for(n=0; n<MAX_XMB_THUMBS; n++)
	{
		if(xmb_icon_buf[n].used==-1) return n;
	}

	free_all_buffers();
	return 0;
}


// Draws the cross MM bar (XMMB)
void draw_xmb_icons(xmb_def *_xmb, const int _xmb_icon_, int _xmb_x_offset, int _xmb_y_offset, const bool _recursive, int sub_level, int _bounce)
{
	int _xmb_icon = _xmb_icon_;

	int first_xmb=_xmb_icon-2;
	int xpos, _xpos;
	u8 subicons = (sub_level!=-1);
	if(sub_level<0) sub_level=0;
	_xpos=-90+_xmb_x_offset - (200*sub_level);
	int ypos=0, tw=0, th=0;
	u16 icon_x=0;
	u16 icon_y=0;
	int mo_of=0;
	float mo_of2=0.0f;
	bool one_done=false;
	int bounce_step=0;
	int nx, ny, nw, nh;

	if(_xmb_icon>3 && _xmb_x_offset>0) {first_xmb--; _xpos-=200;}

	for(int n=first_xmb; n<MAX_XMB_ICONS; n++)
	{
		_xpos+=200;
		xpos = _xpos;

		_xmb_icon = _xmb_icon_;
		if(_xmb_x_offset>=100 && _xmb_icon>1 && !subicons) {_xmb_icon--; free_all_buffers();}
		if(_xmb_x_offset<=-100 && _xmb_icon<MAX_XMB_ICONS-1 && !subicons) {_xmb_icon++; free_all_buffers();}

		if(n<1) continue;
		if(sub_level && n!=xmb_icon) continue;

		if(n<=3) set_texture(_xmb[n].data, 128, 128); //icon
		if(n==4) set_texture(_xmb[5].data, 128, 128); //icon
		if(n==5) set_texture(xmb_icon_util, 128, 128); //icon

		mo_of=abs((int)(_xmb_x_offset*0.18f));
		if(_xmb[_xmb_icon].first>=_xmb[_xmb_icon].size) _xmb[_xmb_icon].first=0;
		if(n==_xmb_icon_)
		{
			/*if(egg) // :)
				display_img_angle(xpos-(36-mo_of)/2, 230-(36-mo_of), 164-mo_of, 164-mo_of, 128, 128, 0.8f, 128, 128, angle);
			else*/
				display_img(xpos-(36-mo_of)/2, 230-(36-mo_of) - _bounce, 164-mo_of, 164-mo_of, 128, 128, 0.8f, 128, 128);
			set_texture(xmb_col, 300, 30); //column name
			display_img(xpos-86, 340 - _bounce, 300, 30, 300, 30, 0.7f, 300, 30);

			if(_xmb[_xmb_icon].size>0 && subicons && !(key_repeat && ( (old_pad & BUTTON_LEFT) || (old_pad & BUTTON_RIGHT)) && (xmb_icon!=1 && xmb_icon!=MAX_XMB_ICONS-1)) && (abs(_xmb_x_offset)<100 || _xmb_icon != _xmb_icon_))
			{
				xpos = _xpos;
				if(_xmb_x_offset>=100 && !subicons) xpos-=200;
				if(_xmb_x_offset<=-100 && !subicons) xpos+=200;

				int cn;
				int cn3=1;
				int first_xmb_mem = _xmb[_xmb_icon].first;
				int cnmax=3;
				if(_xmb[_xmb_icon].first>2 && _xmb_y_offset>0) {first_xmb_mem--; cn3--;}

				for(int m=0;m<4;m++) // make it pleasureable to watch while loading column
				{
					if(m==1)
					{
						cn3=0;
						first_xmb_mem = _xmb[_xmb_icon].first-1;
						cnmax=1;
					}

					if(m==2)
					{
						cn3=-1;
						first_xmb_mem = _xmb[_xmb_icon].first-2;
						cnmax=0;
					}

					if(m==3)
					{
						cn3=3;
						first_xmb_mem = _xmb[_xmb_icon].first+2;
						cnmax=8;
					}

					if(_xmb[_xmb_icon].first>2 && _xmb_y_offset>0) {first_xmb_mem--; cn3--;}

					for(cn=first_xmb_mem; (cn<_xmb[_xmb_icon].size && cn3<cnmax); cn++)
					{

						cn3++;
						if(cn<0) continue;
						if(sub_level && cn3!=2) continue;

						tw=_xmb[_xmb_icon].member[cn].iconw; th=_xmb[_xmb_icon].member[cn].iconh;
						if(tw<320 && th<176 && tw!=128 && th!=128) {tw*=2; th*=2;}
						if(tw>320 || th>176)
						{
							if(tw>th) {th= (int)((float)th/((float)tw/320.f)); tw=320;}
							else {tw= (int)((float)tw/((float)th/176.f)); th=176;}
							if(tw>320) {th= (int)((float)th/((float)tw/320.f));	tw=320;}
							if(th>176) {tw= (int)((float)tw/((float)th/176.f));	th=176;}
						}

						if(cn3!=2) {tw/=2; th/=2;}
						else
						{
							tw=(int) ( (float)tw*(  (1.f+(float)_bounce/90.f) ) );
							th=(int) ( (float)th*(  (1.f+(float)_bounce/90.f) ) );
						}

						mo_of2=2.f-(abs(_xmb_y_offset)/90.0f);

						if( (_xmb_y_offset!=0) )
						{
							if( (_xmb_y_offset>0 && cn3==1) || (_xmb_y_offset<0 && cn3==3) )
							{
								tw=_xmb[_xmb_icon].member[cn].iconw; th=_xmb[_xmb_icon].member[cn].iconh;
								if(tw<320 && th<176 && tw!=128 && th!=128) {tw*=2; th*=2;}
								if(tw>320 || th>176)
								{
									if(tw>th) {th= (int)((float)th/((float)tw/320.f)); tw=320;}
									else {tw= (int)((float)tw/((float)th/176.f)); th=176;}
									if(tw>320) {th= (int)((float)th/((float)tw/320.f));	tw=320;}
									if(th>176) {tw= (int)((float)tw/((float)th/176.f));	th=176;}
								}
								tw=(int)(tw/mo_of2); th=(int)(th/mo_of2);
							}
							else if( (_xmb_y_offset!=0 && cn3==2))
							{
								tw=_xmb[_xmb_icon].member[cn].iconw; th=_xmb[_xmb_icon].member[cn].iconh;
								if(tw<320 && th<176 && tw!=128 && th!=128) {tw*=2; th*=2;}
								if(tw>320 || th>176)
								{
									if(tw>th) {th= (int)((float)th/((float)tw/320.f)); tw=320;}
									else {tw= (int)((float)tw/((float)th/176.f)); th=176;}
									if(tw>320) {th= (int)((float)th/((float)tw/320.f));	tw=320;}
									if(th>176) {tw= (int)((float)tw/((float)th/176.f));	th=176;}
								}
								tw=(int)(tw/(3.f-mo_of2)); th=(int)(th/(3.f-mo_of2));
							}
						}

						if(cn3<1) ypos=cn3*90+_xmb_y_offset - _bounce;
						else if(cn3==1) ypos=cn3*90 - _bounce + ( (_xmb_y_offset>0) ? (int)(_xmb_y_offset*3.566f) : (_xmb_y_offset) );
						else if(cn3==2) {ypos = 411 - _bounce + ( (_xmb_y_offset>0) ? (int)(_xmb_y_offset*2.377f) : (int)(_xmb_y_offset*3.566f) );}
						else if(cn3==3) ypos=(cn3-3)*90 + 625 + _bounce + ( (_xmb_y_offset>0) ? _xmb_y_offset : (int)(_xmb_y_offset*2.377f) );
						else if(cn3 >3) ypos=(cn3-3)*90 + 625 + _bounce + _xmb_y_offset;

						if(sub_level>0)
						{
							icon_x=xpos+80+tw/2;
							icon_y=ypos+th/2-15;
							set_texture(xmb_icon_arrow+((int)(angle*0.0388f))*3600, 30, 30); //pulsing back arrow
							display_img(icon_x, icon_y, 30, 30, 30, 30, 0.4f, 30, 30);
						}

						if(_xmb[_xmb_icon].member[cn].data==-1 && _xmb_x_offset==0 && !one_done)
						{
							one_done=true;
							if(xmb_txt_buf_max>=MAX_XMB_TEXTS) {redraw_column_texts(_xmb_icon); xmb_txt_buf_max=0;}
							_xmb[_xmb_icon].member[cn].data=xmb_txt_buf_max;
							draw_xmb_title(xmb_txt_buf[xmb_txt_buf_max].data, _xmb[_xmb_icon].member, cn, COL_XMB_TITLE, COL_XMB_SUBTITLE, _xmb_icon, 0.f);
							xmb_txt_buf_max++;
						}

						if(_xmb[_xmb_icon].member[cn].data!=-1 && ((ss_timer<dim_setting && dim_setting) || _xmb[_xmb_icon].first==cn || dim_setting==0) && abs(_xmb_x_offset)<100)
						{
							a_dynamic=(int)angle_dynamic(160, 255);
							if(cn3==2 && _xmb_x_offset==0 && _xmb_y_offset==0 && _xmb[_xmb_icon].member[cn].data!=-1)
							{
								if(abs(a_dynamic-a_dynamic2)>5)
								{
									a_dynamic2=a_dynamic;
									draw_xmb_title(text_TEXTS, _xmb[_xmb_icon].member, cn, ((COL_XMB_TITLE & 0x00ffffff) | (a_dynamic<<24)), COL_XMB_SUBTITLE, _xmb_icon, (float)(a_dynamic-160.f)/2000.f);
								}
								set_texture(text_TEXTS, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT); //text

							}
							else
								set_texture(xmb_txt_buf[_xmb[_xmb_icon].member[cn].data].data, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT); //text

							if(bounce) bounce_step=bounce*5/3+10; else bounce_step=0;

							if(_xmb_icon!=6 && _xmb_icon!=7)
								display_img(xpos+((_xmb_icon==9)?(230+bounce_step):(128+tw/2)), ypos+th/2-XMB_TEXT_HEIGHT/2, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT, 0.5f, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT); //(int)(XMB_TEXT_WIDTH*(1.f-abs((float)_xmb_x_offset)/200.f))
							else
								display_img(xpos+128+tw/2, ypos+th/2-XMB_TEXT_HEIGHT/2, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT, 0.5f, XMB_TEXT_WIDTH, XMB_TEXT_HEIGHT); //(int)(XMB_TEXT_WIDTH*(1.f-abs((float)_xmb_x_offset)/200.f))
						}

						/*if((_xmb[_xmb_icon].member[cn].status==1 || _xmb[_xmb_icon].member[cn].status==0) && !_recursive && !key_repeat)
						{
							if(_xmb[_xmb_icon].member[cn].status==0)
							{
								_xmb[_xmb_icon].member[cn].status=1;
								xmb_icon_buf_max=find_free_buffer(_xmb_icon);
								xmb_icon_buf[xmb_icon_buf_max].used=cn;
								xmb_icon_buf[xmb_icon_buf_max].column=_xmb_icon;

								_xmb[_xmb_icon].member[cn].icon = xmb_icon_buf[xmb_icon_buf_max].data;
								_xmb[_xmb_icon].member[cn].icon_buf=xmb_icon_buf_max;
							}

	//						load_png_partial( _xmb[_xmb_icon].member[cn].icon, _xmb[_xmb_icon].member[cn].icon_path, _xmb[_xmb_icon].member[cn].iconw, _xmb[_xmb_icon].member[cn].iconh/2, 0);
							if(_xmb_icon==5 || _xmb_icon==3 || _xmb_icon==8)
							{
								if(_xmb_icon!=3 && (strstr(_xmb[_xmb_icon].member[cn].icon_path,".png")!=NULL || strstr(_xmb[_xmb_icon].member[cn].icon_path,".PNG")!=NULL))
									load_png_threaded( _xmb_icon, cn);
								else
									load_jpg_threaded( _xmb_icon, cn);
							}
							else
							{
								if(strstr(_xmb[_xmb_icon].member[cn].icon_path,".JPG")!=NULL || strstr(_xmb[_xmb_icon].member[cn].icon_path,".jpg")!=NULL || strstr(_xmb[_xmb_icon].member[cn].icon_path,".mp3")!=NULL || strstr(_xmb[_xmb_icon].member[cn].icon_path,".MP3")!=NULL)
									load_jpg_threaded( _xmb_icon, cn);
								else
									load_png_threaded( _xmb_icon, cn);
							}
						}*/
						if(_xmb[_xmb_icon].member[cn].status==1 || (_xmb[_xmb_icon].member[cn].status==0 && (_recursive || key_repeat)) )
						{
							tw=128; th=128;
							if(cn3!=2) {tw/=2; th/=2;}
							icon_x=xpos+64-tw/2;
							icon_y=ypos;

							set_texture(_xmb[0].data, 128, 128); //icon
							display_img_angle(icon_x, icon_y, tw, th, 128, 128, 0.5f, 128, 128, angle);

						}

						if(_xmb[_xmb_icon].member[cn].status==2)
						{
							icon_x=xpos+64-tw/2;
							icon_y=ypos;

							set_texture(_xmb[_xmb_icon].member[cn].icon, _xmb[_xmb_icon].member[cn].iconw, _xmb[_xmb_icon].member[cn].iconh);
							display_img(icon_x, icon_y,	tw,	th,	tw,	th,	0.5f, tw,	th);
						}

					}
				}
			}

		}
		else
		{
			/*if(egg)
			{
				if(n==xmb_icon-1 && _xmb_x_offset>0) display_img_angle(xpos-(mo_of)/2, 230-(mo_of), 128+mo_of, 128+mo_of, 128, 128, 0.0f, 128, 128, angle);
				else if(n==xmb_icon+1 && _xmb_x_offset<0) display_img_angle(xpos-(mo_of)/2, 230-(mo_of), 128+mo_of, 128+mo_of, 128, 128, 0.0f, 128, 128, angle);
				else display_img_angle(xpos, 230, 128, 128, 128, 128, 0.0f, 128, 128, angle);
			}
			else */

			nx=xpos-(mo_of)/2;
			ny=230-(mo_of) - _bounce;
			nw=128+mo_of;
			nh=128+mo_of;
			if(!(n==xmb_icon-1 && _xmb_x_offset>0) && !((n==xmb_icon+1 && _xmb_x_offset<0)))
			{
				nx=xpos;
				ny=230 - _bounce;
				nw=128;
				nh=128;
			}

			display_img(nx, ny, nw, nh, 128, 128, 0.0f, 128, 128);
		}

	}

	if(is_any_xmb_column)
	{
		if(time(NULL)&1)
		{
			set_texture(_xmb[is_any_xmb_column].data, 128, 128);
			display_img(1834, 74, 64, 64, 128, 128, 0.6f, 128, 128);
		}
		set_texture(xmb_icon_help, 128, 128);
		display_img_angle(1770, 74, 64, 64, 128, 128, 0.6f, 128, 128, angle);
	}

	if(key_repeat && !_xmb_x_offset && _xmb_y_offset!=0 && _xmb[_xmb_icon].first>1 && _xmb[_xmb_icon].first<(_xmb[_xmb_icon].size-2) && _xmb[_xmb_icon].size>10 && _xmb[_xmb_icon].first<_xmb[_xmb_icon].size)
	{
		set_texture(text_SLIDER, 64, 1);
		display_img(300, 0, 64, 1080, 64, 1080, -0.4f, 64, 1080);
		set_texture(xmb_icon_arrow, 30, 30);
		display_img_angle(334, (int)(177.f+(((float)_xmb[_xmb_icon].first+1.f)/(float)_xmb[_xmb_icon].size)*798.f), 30, 30, 30, 30, -0.45f, 30, 30, 45.f);
	}
}

void draw_xmb_bare(u8 _xmb_icon, u8 _all_icons, bool recursive, int _sub_level)
{


	draw_stars();
	draw_xmb_bg();
	draw_xmb_clock(xmb_clock, (_all_icons!=2 ? _xmb_icon : -1));

	if(_all_icons==1) draw_xmb_icons(xmb, _xmb_icon, xmb_slide, xmb_slide_y, recursive, _sub_level, 0);
	else if(_all_icons==2) draw_xmb_icons(xmb, _xmb_icon, xmb_slide, xmb_slide_y, recursive, -1, 0);
	flip();
}



void change_opacity(u8 *buffer, int delta, u32 size)
{
	u32 pixel;
	u32 delta2;
	if(delta>0)
	{
		for(u32 fsr=0; fsr<size; fsr+=4)
		{
			pixel=*(uint32_t*) ((uint8_t*)(buffer)+fsr);
			delta2 = ((u32)((float)(pixel&0xff)*(1.0f+(float)delta/100.f)));
			if(delta2>0xff) delta2=0xff;
			pixel= (pixel & 0xffffff00) | delta2;
			*(uint32_t*) ((uint8_t*)(buffer)+fsr)= pixel;
		}
	}
	else
	{
		for(u32 fsr=0; fsr<size; fsr+=4)
		{
			pixel=*(uint32_t*) ((uint8_t*)(buffer)+fsr);
			delta2 = ((u32)((float)(pixel&0xff)*((float)abs(delta)/100.f)));
			if(delta2>0xff) delta2=0xff;
			pixel= (pixel & 0xffffff00) | delta2;
			*(uint32_t*) ((uint8_t*)(buffer)+fsr)= pixel;
		}
	}

}

void set_gameos_flag()
{
	uint32_t dev_handle;
	int start_sector, sector_count, result;
	struct storage_device_info info;
	uint8_t buf[VFLASH5_SECTOR_SIZE * 16];
	struct os_area_header *hdr;
	struct os_area_params *params;
	uint32_t unknown2;

	dev_handle = 0;

	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_yes_no, STR_GAMEOSFLG, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog();
	if(dialog_ret!=1)
		goto done;

	result = lv2_storage_get_device_info(VFLASH5_DEV_ID, &info);
	if (result) {
		goto done;
	}

	if (info.capacity < VFLASH5_HEADER_SECTORS) {
		goto done;
	}

	result = lv2_storage_open(VFLASH5_DEV_ID, &dev_handle);
	if (result) {
		goto done;
	}

	/* write os header and params */

	start_sector = 0;
	sector_count = VFLASH5_HEADER_SECTORS;

	memset(buf, 0, sizeof(buf));
	hdr = (struct os_area_header *) buf;
	params = (struct os_area_params *) (buf + OS_AREA_SEGMENT_SIZE);

	result = lv2_storage_read(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
	if (result) {
		goto done;
	}

	if (strncmp((const char *) hdr->magic, HEADER_MAGIC, sizeof(hdr->magic))) {
		goto done;
	}

	if (hdr->version != HEADER_VERSION) {
		goto done;
	}

	if (params->boot_flag != PARAM_BOOT_FLAG_GAME_OS) {
		params->boot_flag = PARAM_BOOT_FLAG_GAME_OS;

		result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
		if (result) {
			goto done;
		}
	}
	else
	{
	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_ok, STR_GAMEOSFLGOK, dialog_fun2, (void*)0x0000aaab, NULL );
	wait_dialog_simple();
	goto done;
	}

	result = lv2_storage_close(dev_handle);

	lv2_sm_shutdown(0x8201, NULL, 0);

done:
	lv2_storage_close(dev_handle);
}

int open_side_menu(int _top, int sel)
{
	side_menu_open=true;
	int _width=600;
	int _height=1080;

	u8 *text_LIST = text_FONT;

	while(xmb_bg_loaded==1) { sys_timer_usleep(3336); cellSysutilCheckCallback();}
	// create opacity 'gradient' left-to-right from ~94% to ~18%, RGB: #4a5254
	for(int fsr2=0; fsr2<_height; fsr2++)
	{
		for(int fsr=4; fsr<_width; fsr++)
			*(uint32_t*) ( (u8*)(text_LIST)+((fsr+fsr2*_width)*4 ))=side_menu_color[side_menu_color_indx]|( 0xf0-(int)( 240.f*((float)fsr/1.15f)/(float)_width) );

		*(uint32_t*) ( (u8*)(text_LIST)+((0+fsr2*_width)*4 ))=side_menu_color[side_menu_color_indx]|0xf0;
		*(uint32_t*) ( (u8*)(text_LIST)+((1+fsr2*_width)*4 ))=0x747474f0;
		*(uint32_t*) ( (u8*)(text_LIST)+((2+fsr2*_width)*4 ))=0x949494f0;
		*(uint32_t*) ( (u8*)(text_LIST)+((3+fsr2*_width)*4 ))=0x646464f0;
	}

	// print menu entries
	max_ttf_label=0;
	u32 _color;
	for(int n=0; n<opt_list_max; n++)
	{
		if(opt_list[n].color)
		{
			_color=opt_list[n].color;
			print_label_ex( 0.134f, 0.0008f+(float)(n*40.f+(float)_top)/1080.f, 1.2f, 0xffe5e5e5, opt_list[n].label+1, 1.04f, 0.0f, mui_font, 0.68f/((float)(_width/1920.f)), (0.7f)/((float)(_height/1080.f)), 0);
		}
		else
			_color=0xf0e5e5e5;
		print_label_ex( 0.133f, (float)(n*40.f+(float)_top)/1080.f, 1.2f, (opt_list[n].label[0]!=' ' ? 0xff000000 : _color), opt_list[n].label+1, 1.04f, 0.0f, mui_font, 0.68f/((float)(_width/1920.f)), (0.7f)/((float)(_height/1080.f)), 0);
	}
	flush_ttf(text_LIST, _width, _height);
	bounce=0;
	bool to_bounce=0;

	for(int fsr=1860; fsr>1320; fsr-=60) // slide in the side menu
	{
		if(to_bounce) bounce+=10*(1+video_mode);
		draw_whole_xmb(0);
		set_texture(text_LIST, _width, _height);	display_img(fsr, 0, 1920-fsr, _height, _width, _height, -0.3f, _width, _height);
		flip();
	}

	for(int n=sel;n<opt_list_max;n++)
	{
		if(opt_list[n].label[0]==' ') {sel=n; break;}
	}
	if(sel>=opt_list_max) return -1;

	while(1)
	{
		xmb_bg_counter=XMB_BG_COUNTER;
		ss_timer_last=time(NULL)-300;
		repeat_counter1=repeat_init_delay;
		pad_read();
		key_repeat=0;
		if ( (new_pad & BUTTON_TRIANGLE) || (new_pad & BUTTON_CIRCLE) ) {sel=-1; break;}
		if ( (new_pad & BUTTON_CROSS) && opt_list[sel].label[0]==' ')  break;


		if ( (new_pad & BUTTON_L1) || (new_pad & BUTTON_L2) || (new_pad & BUTTON_LEFT))  {sel=-2; break;}
		if ( (new_pad & BUTTON_R1) || (new_pad & BUTTON_R2) || (new_pad & BUTTON_RIGHT)) {sel=-3; break;}

		if ( (new_pad & BUTTON_DOWN))
		{
			for(int n=0;n<15;n++)
			{
				sel++;
				if(sel>=opt_list_max) sel=0;
				if(opt_list[sel].label[0]==' ') break;
			}
		}

		if ( (new_pad & BUTTON_UP))
		{
			for(int n=0;n<15;n++)
			{
				sel--;
				if(sel<0) sel=opt_list_max-1;
				if(opt_list[sel].label[0]==' ') break;
			}
		}

		draw_whole_xmb(0);
		set_texture(text_LIST, _width, _height);	display_img(1320, 0, _width, _height, _width, _height, -0.3f, _width, _height);

		if(opt_list[sel].label[0]==' ')
		{
			set_texture(xmb_icon_arrow+((int)(angle*0.0388f))*3600, 30, 30); //pulsing back arrow
			display_img_angle(1352, sel*40+_top+1, 30, 30, 30, 30, -0.4f, 30, 30, 45.f);
		}
		flip();
	}

	for(int fsr=1320; fsr<=1860; fsr+=60) // slide out
	{
		if(to_bounce) bounce-=10*(1+video_mode);

		draw_whole_xmb(0);
		set_texture(text_LIST, _width, _height);
		display_img(fsr, 0, 1920-fsr, _height, _width, _height, -0.3f, _width, _height);
		flip();
	}
	bounce=0;
	side_menu_open=false;
	if(sel!=-1)
	{
		draw_whole_xmb(0);
		flip();
	}
	return sel;
}

/*
int open_dd_menu_xmb(char *_caption, int _width, t_opt_list *list, int _max, int _x, int _y, int _max_entries)
{
	(void) _x;
	(void) _y;

	u8 _menu_font=17;
	float y_scale=0.85f;
	if(mm_locale) {_menu_font=mui_font; y_scale=0.7f;}

	if(_max_entries>16) _max_entries=16;
	u8 *text_LIST = NULL;
	text_LIST = text_FONT;
	int line_h = 26;
	int _height = 315;//(_max_entries+5) * line_h;
	if(_max>8) line_h=(int)(208.f/(float(_max)));
	int last_sel=-1;
	int first=0;
	int sel=0;
	char filename[1024];

	sprintf(filename, "%s/LBOX.PNG", app_img);
	load_texture(text_LIST+756000, filename, 600);
	mip_texture(text_LIST+756000, text_LIST+756000, 600, 630, -2);
	change_opacity(text_LIST+756000, 40, 300*315*4);
	xmb_bg_show=0;
	xmb_bg_counter=XMB_BG_COUNTER;

	while(1)
	{

		xmb_bg_show=0;
		xmb_bg_counter=XMB_BG_COUNTER;

		pad_read();
		if ( (new_pad & BUTTON_TRIANGLE) || (new_pad & BUTTON_CIRCLE) || (new_pad & BUTTON_CROSS) )
		{
			for(int sx=(int)(mouseX*1920.f); sx<2520; sx+=90)
			{

				draw_whole_xmb(0);
				set_texture(text_LIST, _width, _height);
				display_img(sx, (int)(mouseY*1080.f), _width, _height, _width, _height, -0.3f, _width, _height);

				flip();
			}
			if ( (new_pad & BUTTON_CROSS) )  return sel; else return -1;
		}

		if ( (new_pad & BUTTON_DOWN))
		{
			sel++;
			if(sel>=_max) sel=0;
			first=sel-_max_entries+2;
			if(first<0) first=0;
		}

		if ( (new_pad & BUTTON_UP))
		{
			sel--;
			if(sel<0) sel=_max-1;
			first=sel-_max_entries+2;
			if(first<0) first=0;
		}

		if(last_sel!=sel)
		{
			memcpy(text_LIST, text_LIST+756000, 756000);
			max_ttf_label=0;
			print_label_ex( 0.53f, 0.05f, 0.62f, COL_XMB_COLUMN, _caption, 1.04f, 0.0f, 0, 1.0f/((float)(_width/1920.f)), (1.2f)/((float)(_height/1080.f)), 1);
			flush_ttf(text_LIST, _width, _height);

			for(int n=first; (n<(first+_max_entries-1) && n<_max); n++)
			{
				if(n==sel)
					print_label_ex( 0.055f, ((float)((n-first+2.2f)*line_h)/(float)_height)-0.007f, 1.2f, 0xf0e0e0e0, list[n].label, 1.04f, 0.0f, _menu_font, 0.68f/((float)(_width/1920.f)), (y_scale)/((float)(_height/1080.f)), 0);
				else
					print_label_ex( 0.120f, ((float)((n-first+2.2f)*line_h)/(float)_height), 0.85f, COL_XMB_SUBTITLE, list[n].label, 1.04f, 0.0f, _menu_font, 0.8f/((float)(_width/1920.f)), (y_scale+0.1f)/((float)(_height/1080.f)), 0 );
				flush_ttf(text_LIST, _width, _height);
			}

			print_label_ex( 0.6f, ((float)(_height-line_h*2)/(float)_height)+0.0326f, 1.5f, 0xf0c0c0c0, (char*) "Confirm", 1.04f, 0.0f, _menu_font, 0.5f/((float)(_width/1920.f)), 0.5f/((float)(_height/1080.f)), 0);
			flush_ttf(text_LIST, _width, _height);
			put_texture_with_alpha_gen( text_LIST, text_DOX+(dox_cross_x	*4 + dox_cross_y	* dox_width*4), dox_cross_w,	dox_cross_h,	dox_width, _width, (int)((0.6f*_width)-dox_cross_w-5), _height-line_h*2+4);
			if(last_sel==-1)
			{
				for(int sx=1920; sx>(int)(mouseX*1920.f); sx-=60)
				{

					draw_whole_xmb(0);
					set_texture(text_LIST, _width, _height);
					display_img(sx, (int)(mouseY*1080.f), _width, _height, _width, _height, -0.3f, _width, _height);

					flip();
				}
			}
			last_sel=sel;
		}



		draw_whole_xmb(0);

		set_texture(text_LIST, _width, _height);
		display_img((int)(mouseX*1920.f), (int)(mouseY*1080.f), _width, _height, _width, _height, -0.3f, _width, _height);

		flip();

		mouseX+=mouseXD; mouseY+=mouseYD;
		if(mouseX>0.995f) {mouseX=0.995f;mouseXD=0.0f;} if(mouseX<0.0f) {mouseX=0.0f;mouseXD=0.0f;}
		if(mouseY>0.990f) {mouseY=0.990f;mouseYD=0.0f;} if(mouseY<0.0f) {mouseY=0.0f;mouseYD=0.0f;}

	}

	return -1;
}
*/


void save_options()
{
	seconds_clock=0;
	remove(options_bin);
	FILE *fpA;
	fpA = fopen ( options_bin, "w" );
	if(fpA!=NULL)
	{
		fprintf(fpA, "confirm_with_x=%i\r\n", confirm_with_x);

//		fprintf(fpA, "%i\n", );

		fclose( fpA);
	}

}

void shutdown_syscall();
void shutdown_syscall()
{
system_call_4(379,0x1100,0,0,0);
}

void add_home_column()
{
		char str[128];
		
		sprintf(xmb[1].name, "%s", xmb_columns[1]); xmb[1].first=0; xmb[1].size=0;

		add_xmb_member(xmb[1].member, &xmb[1].size, STR_SYSINF, STR_SYSINFDESC,
				/*type*/6, /*status*/2, /*icon*/xmb_icon_info, 128, 128);
		
		sprintf(str, "%s %s %s", STR_QUITDESC1, STR_APP_NAME, STR_QUITDESC2);
		add_xmb_member(xmb[1].member, &xmb[1].size, STR_QUIT, str,
				/*type*/6, /*status*/2, /*icon*/xmb_icon_quit, 128, 128);

		u8 col=1;
		
		add_xmb_option(xmb[col].member, &xmb[col].size, STR_LANG, STR_LANGDESC, (char*) "lang");
		char nb[2];
		int i;
		for(i=0; i < lang_N ; i++) {
			sprintf(nb, "%d", i);
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_LANGUAGE[i],			nb);
		}
		xmb[col].member[xmb[col].size-1].option_selected=lang;
		xmb[col].member[xmb[col].size-1].icon=xmb[0].data;
		
		
		sprintf(str, "%s %s %s", STR_RESTSYSDESC1, STR_APP_NAME, STR_RESTSYSDESC2);
		add_xmb_option(xmb[col].member, &xmb[col].size, STR_RESTSYS, str, (char*) "Reboot");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NO,			(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_RESTSYS_SUB1,			(char*)"1");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_RESTSYS_SUB2,			(char*)"2");
		xmb[col].member[xmb[col].size-1].option_selected=reboot;
		xmb[col].member[xmb[col].size-1].icon=xmb[0].data;


		add_xmb_option(xmb[col].member, &xmb[col].size, STR_BOOTOOS, STR_BOOTOOSDESC, (char*)"otheros");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NO			 ,				(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_BOOTOOS_SUB1,				(char*)"1");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_BOOTOOS_SUB2,				(char*)"2");
		xmb[col].member[xmb[col].size-1].option_selected=otheros;
		xmb[col].member[xmb[col].size-1].icon=xmb[4].data;

		add_xmb_member(xmb[1].member, &xmb[1].size, STR_TRNOFF, STR_TRNOFFDESC,
				/*type*/6, /*status*/2, /*icon*/xmb_icon_off, 128, 128);
}

u8 rebug_mode=0;
u8 xmb_mode=0;
u8 menu_mode=0;
u8 cobra_mode=0;
u8 swap_emu=0;
u8 gameos_flag=0;
u8 webman_mode=0;
u8 cfw_settings=0;
u8 wmlp=0;
u8 xmb_plugin=0;

u8 lv1_pp=0;
u8 lv1_sm=0;
u8 lv1_lv2=0;
u8 lv1_htab=0;
u8 lv1_indi=0;
u8 lv1_um=0;
u8 lv1_dm=0;

u8 lv1_enc=0;
u8 lv1_smgo=0;
u8 lv1_core=0;
u8 lv1_bank=0;
u8 lv1_pkg=0;
u8 lv1_lpar=0;
u8 lv1_spe=0;
u8 lv1_dabr=0;
u8 lv1_gart=0;
u8 lv1_keys=0;
u8 lv1_acl=0;
u8 lv1_go=0;

u8 util_qa=0;
u8 util_recovery=0;
u8 util_prodmode=0;
u8 util_idps=0;
u8 util_xReg=0;

void parse_settings()
{
	seconds_clock=0;
	char oini[32];
	int val=0;
	for(int m=1;m<MAX_XMB_ICONS; m++)
	{
		for(int n=0;n<xmb[m].size;n++ )
		{
			if(!xmb[m].member[n].option_size) continue;
			sprintf(oini, "%s", xmb[m].member[n].optionini);
			val=(int)strtol(xmb[m].member[n].option[xmb[m].member[n].option_selected].value, NULL, 10);

				 if(!strcmp(oini, "rebug_mode"))		rebug_mode		=val;
			else if(!strcmp(oini, "xmb_mode"))			xmb_mode		=val;
			else if(!strcmp(oini, "menu_mode"))			menu_mode		=val;
			else if(!strcmp(oini, "cobra_mode"))		cobra_mode		=val;
			else if(!strcmp(oini, "swap_emu"))		swap_emu		=val;
			else if(!strcmp(oini, "webman_mode"))		webman_mode		=val;
			else if(!strcmp(oini, "cfw_settings"))		cfw_settings		=val;
			else if(!strcmp(oini, "wmlp"))				wmlp		=val;
			else if(!strcmp(oini, "xmb_plugin"))		xmb_plugin		=val;
			else if(!strcmp(oini, "confirm_with_x"))	{confirm_with_x	=val; set_xo(); save_options();}
			else if(!strcmp(oini, "gameos_flag"))		gameos_flag		=val;

			else if(!strcmp(oini, "otheros"))			otheros			=val;

			else if(!strcmp(oini, "reboot"))			reboot			=val;

			else if(!strcmp(oini, "lv1_pp"))			lv1_pp			=val;
			else if(!strcmp(oini, "lv1_sm"))			lv1_sm			=val;
			else if(!strcmp(oini, "lv1_lv2"))			lv1_lv2			=val;
			else if(!strcmp(oini, "lv1_htab"))			lv1_htab		=val;
			else if(!strcmp(oini, "lv1_indi"))			lv1_indi		=val;
			else if(!strcmp(oini, "lv1_um"))			lv1_um			=val;
			else if(!strcmp(oini, "lv1_dm"))			lv1_dm			=val;

			else if(!strcmp(oini, "lv1_enc"))			lv1_enc			=val;
			else if(!strcmp(oini, "lv1_smgo"))			lv1_smgo		=val;
			else if(!strcmp(oini, "lv1_core"))			lv1_core		=val;
			else if(!strcmp(oini, "lv1_bank"))			lv1_bank		=val;
			else if(!strcmp(oini, "lv1_pkg"))			lv1_pkg			=val;
			else if(!strcmp(oini, "lv1_lpar"))			lv1_lpar		=val;
			else if(!strcmp(oini, "lv1_spe"))			lv1_spe			=val;
			else if(!strcmp(oini, "lv1_dabr"))			lv1_dabr		=val;
			else if(!strcmp(oini, "lv1_gart"))			lv1_gart		=val;
			else if(!strcmp(oini, "lv1_keys"))			lv1_keys		=val;
			else if(!strcmp(oini, "lv1_acl"))			lv1_acl			=val;
			else if(!strcmp(oini, "lv1_go"))			lv1_go			=val;

			else if(!strcmp(oini, "util_qa"))			util_qa			=val;
			else if(!strcmp(oini, "util_recovery"))		util_recovery	=val;
			else if(!strcmp(oini, "util_prodmode"))		util_prodmode	=val;
			else if(!strcmp(oini, "util_xReg"))         util_xReg       =val;

			else if(!strcmp(oini, "lv2_kernel"))		lv2_kernel		=val;
			
			else if(!strcmp(oini, "lang"))				lang		=val;
		}
	}
}

void add_utilities()
{
	check_settings();
	u8 first=0;
//	Utilities
	u8 col=5;
	sprintf(xmb[col].name, "%s", xmb_columns[col]);
	first=xmb[col].first;
	xmb[col].first=0;
	xmb[col].size=0;

	u8 old_lv1_um=lv1_um;
	u8 old_lv1_dm=lv1_dm;
	u8 to_restore=0;
	if(!lv1_um || !lv1_dm)
	{
		change_lv1_um(1);
		change_lv1_dm(1);
		to_restore=1;
	}

	add_xmb_option(xmb[col].member, &xmb[col].size, (char*)"Toggle QA Flag", (char*)"Enable or disable QA flag functions.",	(char*)"util_qa");
	if(c_firmware==3.55f || c_firmware==4.21f || c_firmware==4.30f || c_firmware==4.31f || c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f  || c_firmware==4.50f || c_firmware==4.53f || c_firmware==4.55f || c_firmware==4.60f || c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f)
	{
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
		xmb[col].member[xmb[col].size-1].option_selected=read_qa_flag();//util_qa;
	}
	else
	{
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NOTAV,			(char*)"0");
		xmb[col].member[xmb[col].size-1].option_selected=0;
	}
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;


	add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGREC, STR_TOGRECDESC,	(char*)"util_recovery");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=read_recover_mode_flag();//util_recovery;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;


	if(c_firmware==3.55f)
	{
        add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGPM, STR_TOGPMDESC,	(char*)"util_prodmode");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
		xmb[col].member[xmb[col].size-1].option_selected=read_product_mode_flag();//util_prodmode;
		xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
	}

	if(to_restore)
	{
		change_lv1_um(old_lv1_um);
		change_lv1_dm(old_lv1_dm);
	}

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LOADLV2, STR_LOADLV2,	(char*)"lv2_kernel");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NO,			(char*)"0");

	lv2_kernels=1;
	scan_for_lv2_kernels((char*)"/dev_flash");
	scan_for_lv2_kernels((char*)"/dev_hdd0");
	scan_for_lv2_kernels(app_usrdir);
	char _path[64];
	for(u8 m=0;m<99;m++)
	{
		sprintf(_path, "/dev_usb%03i", m);
		if(exist(_path)) scan_for_lv2_kernels(_path);
	}
	char _index[10];
	for(u8 m=1; m<lv2_kernels; m++)
	{
		sprintf(_index, "%i", m);
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)lv2_kernel_files[m].name,				_index);
	}
	xmb[col].member[xmb[col].size-1].option_selected=lv2_kernel;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_BUT, STR_BUTDESC, (char*)"confirm_with_x");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_CIRCLE,	(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_CROSS ,	(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=confirm_with_x;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_XREG, STR_XREGDESC,	(char*)"util_xReg");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NO,	(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_BACKUP,	(char*)"1");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_RESTORE,	(char*)"2");
	xmb[col].member[xmb[col].size-1].option_selected=util_xReg;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_RESIZEVFLASH, STR_RESIZEVFLASHDESC ,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_INSTPETBOOT, STR_INSTPETBOOTDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_GAMEOSBOOTFLG, STR_GAMEOSBOOTFLGDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_PKG, STR_PKGDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_SAVLV1, STR_SAVLV1DESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_SAVLV2, STR_SAVLV2DESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

	add_xmb_member(xmb[col].member, &xmb[col].size, STR_SAVFLASH, STR_SAVFLASHDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);


	if( (!dex_mode && (c_firmware==3.55f || c_firmware==4.46f || c_firmware==4.65f || c_firmware==4.66f)) || c_firmware==4.21f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f)
	{
	add_xmb_member(xmb[col].member, &xmb[col].size, STR_SAVEIDRK, STR_SAVEIDRKDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);
	}

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_PS3ID, STR_PS3IDDESC,	(char*)"util_idps");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, STR_NO,			(char*)"0");
	char tid[8];
	u8 cid=get_idps(0);//EID0 TargetID
	if(cid)
	{
		sprintf(tid, "%s EID0: %02X (%s)", STR_USE, cid, (cid==0x82?((char*)"DEX"):((char*)"CEX")));
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)tid,			(char*)"1");
		cid=get_idps(5);//EID5 TargetID
		sprintf(tid, "%s EID5: %02X (%s)", STR_USE, cid, (cid==0x82?((char*)"DEX"):((char*)"CEX")));
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)tid,			(char*)"2");

		/*add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x82 (DEX)",			(char*)"3");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x83 (JAP)",			(char*)"4");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x84 (USA)",			(char*)"5");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x85 (EUR)",			(char*)"6");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x86 (KOR)",			(char*)"7");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x87 (UK)",			(char*)"8");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x88 (MEX)",			(char*)"9");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x89 (AUS)",			(char*)"10");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x8A (ASIA)",		(char*)"11");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x8B (TAI)",			(char*)"12");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x8C (RUS)",			(char*)"13");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x8D (CHI)",			(char*)"14");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Use IDPS: 0x8E (HK)",			(char*)"15");*/
	}
	else
		util_idps=0;
	xmb[col].member[xmb[col].size-1].option_selected=util_idps;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	if(first<xmb[col].size) xmb[col].first=first; else xmb[col].first=0;
}

void add_settings_column()
{
	check_settings();

	u8 first=0;

//	SELECTOR

	u8 col=2;
	sprintf(xmb[col].name, "%s", xmb_columns[col]);
	first=xmb[col].first;
	xmb[col].first=0;
	xmb[col].size=0;

	if(rex_compatible)
	{
		add_xmb_option(xmb[col].member, &xmb[col].size, STR_SM, STR_SMDESC,	(char*)"rebug_mode");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Normal",				(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"REBUG",				(char*)"1");
		xmb[col].member[xmb[col].size-1].option_selected=rebug_mode;
		xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

		if(rebug_mode)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_XMBOM, STR_XMBOMDESC,	(char*)"xmb_mode");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Retail",		(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Debug",		(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=xmb_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}

		add_xmb_option(xmb[col].member, &xmb[col].size, STR_MENU, STR_MENUDESC,	(char*)"menu_mode");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"CEX QA",				(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"DEX",				(char*)"1");
		xmb[col].member[xmb[col].size-1].option_selected=menu_mode;
		xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

		if((c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && cobra_compatible)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGCFW, STR_TOGCFWDESC,	(char*)"cfw_settings");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,		(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,			(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=cfw_settings;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}
		/*
		if((c_firmware==4.81f) && cobra_compatible)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, (char*)"Toggle webMAN LaunchPAD", (char*)"Enable or Disable webMAN MOD LaunchPAD v0.1a",	(char*)"wmlp");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_XC2_DISABLE,		(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_XC2_ENABLE,			(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=wmlp;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}*/		
		
		if( dex_mode && (c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && cobra_compatible)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, (char*)STR_TOGHOSTINF, (char*)STR_TOGHOSTINFDESC,	(char*)"xmb_plugin");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,		(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,			(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=xmb_plugin;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}

		if((c_firmware==4.21f || c_firmware==4.30f || c_firmware==4.31f || c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f || c_firmware==4.50f || c_firmware==4.53f || c_firmware==4.55f ||
		    c_firmware==4.60f || c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && cobra_compatible)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGCOB, STR_TOGCOBDESC,	(char*)"cobra_mode");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=cobra_mode; //cobra_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGPS2, STR_TOGPS2DESC,	(char*)"swap_emu");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Original",			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"COBRA",				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=swap_emu; //ps2_swap;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}

		if(exist((char*)"/dev_flash/vsh/module/webftp_server.sprx") || exist((char*)"/dev_flash/vsh/module/webftp_server.sprx.bak"))
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGWM, STR_TOGWMDESC,	(char*)"webman_mode");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=webman_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}

/*			add_xmb_option(xmb[col].member, &xmb[col].size, (char*)"GameOS boot flag", (char*)"set gameos boot flag to fix ps2 issue",	(char*)"gameos_flag");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Set it",			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Set it",				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=gameos_flag; //cobra_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;*/
		/*if(!dex_mode || dex_mode==2)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, (char*)"Write Access to /dev_flash", (char*)"Mount /dev_flash as /dev_blind with write permissions.",	(char*)"mount_dev_blind");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,				(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,					(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=mount_dev_blind;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_folder;
		}*/

		if(first<xmb[col].size) xmb[col].first=first; else xmb[col].first=0;
	}
	else if(cobra_compatible)
	{
		if((c_firmware==4.21f || c_firmware==4.30f || c_firmware==4.31f || c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f || c_firmware==4.50f || c_firmware==4.53f || c_firmware==4.55f ||
		    c_firmware==4.60f || c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && cobra_compatible)
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGCOB, STR_TOGCOBDESC,	(char*)"cobra_mode");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=cobra_mode; //cobra_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGPS2, STR_TOGPS2DESC,	(char*)"swap_emu");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"Original",			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)"COBRA",				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=swap_emu; //ps2_swap;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}

		if(exist((char*)"/dev_flash/vsh/module/webftp_server.sprx") || exist((char*)"/dev_flash/vsh/module/webftp_server.sprx.bak"))
		{
			add_xmb_option(xmb[col].member, &xmb[col].size, STR_TOGWM, STR_TOGWMDESC,	(char*)"webman_mode");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
			add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
			xmb[col].member[xmb[col].size-1].option_selected=webman_mode;
			xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
		}
	}
	else
		add_xmb_member(xmb[col].member, &xmb[col].size, STR_NORBG, STR_NORBGDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

//	LV1 Patches

	col=3;
	sprintf(xmb[col].name, "%s", xmb_columns[col]);
	first=xmb[col].first;
	xmb[col].first=0;
	xmb[col].size=0;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_PATCHLV1, STR_PATCHLV1DESC,	(char*)"lv1_pp");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_pp;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

/*
	if(c_firmware==4.21f)
	{
		add_xmb_option(xmb[col].member, &xmb[col].size, (char*)"Integriry Check", (char*)"Patch System Manager integriry check.",	(char*)"lv1_sm");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
		add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
		xmb[col].member[xmb[col].size-1].option_selected=lv1_sm;
		xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;
	}
*/
	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV2MEMPRO, STR_LV2MEMPRODESC,	(char*)"lv1_lv2");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_lv2;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_HTAB, STR_HTABDESC,	(char*)"lv1_htab");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_htab;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_INDI, STR_INDIDESC,	(char*)"lv1_indi");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_indi;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_EEPROM, STR_EEPROMDESC,	(char*)"lv1_um");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_um;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_DISPATCH, STR_DISPATCHDESC,	(char*)"lv1_dm");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_dm;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_ENCDEC, STR_ENCDECDESC,	(char*)"lv1_enc");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_enc;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_SYSMAN, STR_SYSMANDESC,	(char*)"lv1_smgo");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_smgo;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;


	add_xmb_option(xmb[col].member, &xmb[col].size, STR_ALLPKG, STR_ALLPKGDESC,	(char*)"lv1_pkg");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_pkg;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LPAR, STR_LPARDESC,	(char*)"lv1_lpar");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_lpar;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1SPE, STR_LV1SPEDESC,	(char*)"lv1_spe");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_spe;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1DABR, STR_LV1DABRDESC,	(char*)"lv1_dabr");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_dabr;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1GART, STR_LV1GARTDESC,	(char*)"lv1_gart");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_gart;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1KEYS, STR_LV1KEYSDESC,	(char*)"lv1_keys");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_keys;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1ACL, STR_LV1ACLDESC,	(char*)"lv1_acl");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_acl;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

    add_xmb_option(xmb[col].member, &xmb[col].size, STR_LV1GO, STR_LV1GODESC,	(char*)"lv1_go");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_DISABLE,			(char*)"0");
	add_xmb_suboption(xmb[col].member[xmb[col].size-1].option, &xmb[col].member[xmb[col].size-1].option_size, 0, (char*)STR_ENABLE,				(char*)"1");
	xmb[col].member[xmb[col].size-1].option_selected=lv1_go;
	xmb[col].member[xmb[col].size-1].icon=xmb_icon_tool;

	if(first<xmb[col].size) xmb[col].first=first; else xmb[col].first=0;


//	CEX/DEX Patches

	col=4;
	sprintf(xmb[col].name, "%s", xmb_columns[col]);
	first=xmb[col].first;
	xmb[col].first=0;
	xmb[col].size=0;
    if(rex_compatible)
	{
		add_xmb_member(xmb[col].member, &xmb[col].size, STR_SWPLV2K, STR_SWPLV2KDESC,
				/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

		add_xmb_member(xmb[col].member, &xmb[col].size, STR_REWTID, STR_REWTIDDESC,
				/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);

		if(first<xmb[col].size) xmb[col].first=first; else xmb[col].first=0;
    }
	else
	add_xmb_member(xmb[col].member, &xmb[col].size, STR_NOCEXDEX, STR_NOCEXDEXDESC,
			/*type*/6, /*status*/2, /*icon*/xmb_icon_tool, 128, 128);
}


void init_slider()
{
		for(int fsr=4; fsr<64; fsr++)
			*(uint32_t*) ( (u8*)(text_SLIDER)+(((63-fsr))*4 ))=side_menu_color[side_menu_color_indx]|( 0xa0-(int)( 160.f*((float)fsr)/64.f) );

		*(uint32_t*) ( (u8*)(text_SLIDER)+((63)*4 ))=side_menu_color[side_menu_color_indx]|0xf0;
		*(uint32_t*) ( (u8*)(text_SLIDER)+((62)*4 ))=0x747474f0;
		*(uint32_t*) ( (u8*)(text_SLIDER)+((61)*4 ))=0x949494f0;
		*(uint32_t*) ( (u8*)(text_SLIDER)+((60)*4 ))=0x646464f0;
}

void init_misc_icons()
{
		reset_xmb();
		load_texture(text_FMS, xmbicons, 128);
//		load_texture(xmb_icon_retro, xmbicons2, 128);
/*
		mip_texture( xmb_icon_star_small, xmb_icon_star, 128, 128, -4);
		mip_texture( xmb_icon_blu_small, xmb_icon_blu, 128, 128, -4);
		//mip_texture( xmb_icon_net_small, xmb[9].data, 128, 128, -2);



		memcpy(xmb_icon_blu_n, xmb_icon_blu, 65536);
		memcpy(xmb_icon_bdv_n, xmb_icon_bdv, 65536);
		memcpy(xmb_icon_dvd_n, xmb_icon_dvd, 65536);
		memcpy(xmb_icon_psx_n, xmb_icon_psx, 65536);
		memcpy(xmb_icon_ps2_n, xmb_icon_ps2, 65536);
		memcpy(xmb_icon_psp_n, xmb_icon_psp, 65536);
		put_texture_with_alpha_gen( xmb_icon_blu_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);
		put_texture_with_alpha_gen( xmb_icon_bdv_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);
		put_texture_with_alpha_gen( xmb_icon_dvd_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);
		put_texture_with_alpha_gen( xmb_icon_psx_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);
		put_texture_with_alpha_gen( xmb_icon_ps2_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);
		put_texture_with_alpha_gen( xmb_icon_psp_n, xmb_icon_net_small, 64, 64, 64, 128, 32, 32);

		sprintf(auraBG, "%s/CMLOGO1.PNG", app_img); load_texture(xmb_icon_logo, auraBG, 128);
*/
}

void init_xmb_icons()
{
		seconds_clock=0;
		xmb_legend_drawn=0;
		xmb_info_drawn=0;
		bounce=0;
		xmb_sublevel=0;
		xmb0_icon=xmb_icon;
		xmb_bg_show=0;
		xmb_bg_counter=XMB_BG_COUNTER;

		init_misc_icons();

		sprintf(auraBG, "%s/ARROW.PNG", app_img); load_texture(xmb_icon_arrow, auraBG, 30);

		sprintf(xmbbg,	"%s/XMBBG.PNG", app_img);
		load_xmb_bg();
		init_slider();

		free_text_buffers();

		for(int n=0; n<MAX_XMB_THUMBS; n++)
		{
			xmb_icon_buf[n].used=-1;
			xmb_icon_buf[n].column=0;
			xmb_icon_buf[n].data=text_bmpUBG+(n*XMB_THUMB_WIDTH*XMB_THUMB_HEIGHT*4);
			memset(xmb_icon_buf[n].data, 0x30, (XMB_THUMB_WIDTH*XMB_THUMB_HEIGHT*4));
		}
		xmb_icon_buf_max=0;

		free_all_buffers();

		add_home_column();
		add_settings_column();
		add_utilities();
		draw_xmb_icon_text(xmb_icon);
}


void draw_xmb_clock(u8 *buffer, const int _xmb_icon)
{
	u32 clock_color=COL_XMB_CLOCK;

	if(_xmb_icon) seconds_clock=0;

	if( (time(NULL)-seconds_clock)>5  &&  show_temp==1 ) goto skip2;
	if( (time(NULL)-seconds_clock)>0  &&  show_temp>=2 )
	{
		show_temp2++;
		show_temp2&=15;
		goto skip2;
	}

		if( (time(NULL)-seconds_clock)<30) goto clock_text;
skip2:
		if(_xmb_icon) seconds_clock=0; else seconds_clock=time(NULL);

		char xmb_date[32];
		time ( &rawtime ); timeinfo = localtime ( &rawtime );

		if(date_format==0)	sprintf(xmb_date, "%d/%d %s:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, tmhour(timeinfo->tm_hour), timeinfo->tm_min); //, timeinfo->tm_sec
		else sprintf(xmb_date,"%d/%d %s:%02d", timeinfo->tm_mon+1, timeinfo->tm_mday, tmhour(timeinfo->tm_hour), timeinfo->tm_min);

		max_ttf_label=0;

		if(show_temp)
		{
			char xmb_temp[64];
			u32 t1=0, t2=0;
			get_temperature(0, &t1); // 3E030000 -> 3E.03'C -> 62.(03/256)'C
			get_temperature(1, &t2);
			t1=t1>>24;
			t2=t2>>24;

			if(show_temp==1)
			{
				print_label_ex( 0.5f, 0.0f, 0.9f, COL_XMB_CLOCK, xmb_date, 1.04f, 0.0f, 2, 6.40f, 18.0f, 1);
				sprintf(xmb_temp, "CPU: %i°C RSX: %i°C", t1, t2);
				if(t1>79 || t2>79) clock_color=0x800000e0;
				print_label_ex( 0.5f, 0.5f, 0.9f, clock_color, xmb_temp, 1.04f, 0.0f, 2, 4.50f, 18.0f, 1);
			}
			else
			if(show_temp>=2)
			{
				if(show_temp2<6)
					print_label_ex( 0.5f, 0.0f, 0.9f, COL_XMB_CLOCK, xmb_date, 1.04f, 0.0f, 2, 6.40f, 36.0f, 1);
				else
				{
					if(show_temp==3)
					{
						t1=int(1.8f*(float)t1+32.f);
						t2=int(1.8f*(float)t2+32.f);
					}

					if(show_temp2<11)
					{
						if( (t1>79 && show_temp==2) || (t1>175 && show_temp==3) ) clock_color=0x800000e0;
						if(show_temp==2)
							sprintf(xmb_date, "CPU: %i°C", t1);
						else
							sprintf(xmb_date, "CPU: %i°F", t1);
					}
					else
					{
						if( (t2>79 && show_temp==2) || (t2>175 && show_temp==3) ) clock_color=0x800000e0;
						if(show_temp==2)
							sprintf(xmb_date, "RSX: %i°C", t2);
						else
							sprintf(xmb_date, "RSX: %i°F", t2);
					}
					print_label_ex( 0.5f, 0.0f, 0.9f, clock_color, xmb_date, 1.04f, 0.0f, 2, 6.40f, 36.0f, 1);
				}
			}
		}
		else
			print_label_ex( 0.5f, 0.0f, 0.9f, COL_XMB_CLOCK, xmb_date, 1.04f, 0.0f, 2, 6.40f, 36.0f, 1);

		//draw_legend=1;

		if(_xmb_icon==-1)
		{
			if(time(NULL)&1)
			{
				set_texture(xmb[0].data, 128, 128);
				display_img(1770, 74, 64, 64, 128, 128, -0.1f, 128, 128);
			}
		}
		if(_xmb_icon>0 && _xmb_icon<MAX_XMB_ICONS)
		{
			set_texture(xmb[_xmb_icon].data, 128, 128);
			display_img(1834, 74, 64, 64, 128, 128, -0.1f, 128, 128);
		}

		memset(buffer, 0, 36000); flush_ttf(buffer, 300, 30);
clock_text:
		if(cover_mode!=1)
		{
			set_texture(buffer, 300, 30);
			if(cover_mode!=6)
			{
				if(cover_mode==5)
					display_img(1663, 1031, 300, 30, 300, 30, -0.25f, 300, 30);
				else
					display_img(1520, 90, 300, 30, 300, 30, -0.1f, 300, 30);
			}
			else
				display_img(1520, 1020+bounce/2, 300, 30, 300, 30, -0.25f, 300, 30);
		}
}

void draw_xmb_legend(const int _xmb_icon)
{
	if(xmb_bg_counter>5 || ( (_xmb_icon==1 || _xmb_icon==9) ) || c_opacity2<=0x40 || xmb_slide_step!=0 || xmb_slide_step_y!=0 || xmb_popup==0 || key_repeat) return;

	if(!xmb_legend_drawn)
	{
		u8 _menu_font=2;
		if(mm_locale) _menu_font=mui_font;

		xmb_legend_drawn=1;
		char xmb_text[32]; xmb_text[0]=0;
		for(int fsr=0; fsr<84000; fsr+=4) *(uint32_t*) ((uint8_t*)(text_MSG)+fsr)=0x22222280;

		put_texture_with_alpha_gen( text_MSG, text_DOX+(dox_cross_x	*4 + dox_cross_y	* dox_width*4), dox_cross_w,	dox_cross_h,	dox_width, 300, 8, 1);
		print_label_ex( 0.17f, 0.08f, 0.6f, 0xffd0d0d0, STR_SELECT, 1.00f, 0.0f, _menu_font, 6.40f, 18.0f, 0);

		put_texture_with_alpha_gen( text_MSG, text_DOX+(dox_circle_x	*4 + dox_circle_y	* dox_width*4), dox_circle_w,	dox_circle_h,	dox_width, 300, 8, 35);
		if(auto_reboot)
			print_label_ex( 0.17f, 0.55f, 0.6f, 0xffd0d0d0, STR_HOLDQUITREB, 1.00f, 0.0f, _menu_font, 6.40f, 18.0f, 0);
		else
			print_label_ex( 0.17f, 0.55f, 0.6f, 0xffd0d0d0, STR_HOLDQUIT, 1.00f, 0.0f, _menu_font, 6.40f, 18.0f, 0);

		flush_ttf(text_MSG, 300, 70);
	}
	if(c_opacity2<=0x80) change_opacity(text_MSG, -95, 84000);
	set_texture(text_MSG, 300, 70);
	display_img(1520, 930, 300, 70, 300, 70, -0.2f, 300, 70);
}


void redraw_column_texts(int _xmb_icon)
{
	for(int n=0; n<xmb[_xmb_icon].size; n++) xmb[_xmb_icon].member[n].data=-1;
	add_settings_column();
}

void draw_xmb_icon_text(int _xmb_icon)
{
	xmb_legend_drawn=0;
	xmb_info_drawn=0;
	max_ttf_label=0;
	print_label_ex( 0.5f, 0.0f, 1.0f, COL_XMB_COLUMN, xmb[_xmb_icon].name, 1.04f, 0.0f, mui_font, 4.2f, 25.5f, 1);
	memset(xmb_col, 0, 36000);
	flush_ttf(xmb_col, 300, 30);
	for(int n=0; n<xmb[_xmb_icon].size; n++) xmb[_xmb_icon].member[n].data=-1;
}


void draw_stars()
{
	int right_border=1919;
	if(side_menu_open) right_border=1319;

	for(int n=0; n<MAX_STARS; n++)
	{
		int move_star= rndv(10);
		if(stars[n].x>1319 && side_menu_open) stars[n].x=(int) ((float)stars[n].x*0.6875f);

		draw_square(((float)stars[n].x/1920.f-0.5f)*2.0f, (0.5f-(float)stars[n].y/1080.0f)*2.0f, (stars[n].size/1920.f), (stars[n].size/1080.f), 0.0f, ( (XMB_SPARK_COLOR&0xffffff00) | stars[n].bri));

		if(move_star>6 || move_star<1) stars[n].bri-=(4*(rndv(4)+1));
		if(stars[n].x>right_border || stars[n].y>1079 || stars[n].x<1 || stars[n].y<1 || stars[n].bri<(4*(rndv(4)+1)))
		{
			stars[n].x=rndv(right_border+1);
			if(in_xmb_mode()) stars[n].y=rndv(360)+360;
			else stars[n].y=rndv(1080);
			stars[n].bri=rndv(222)&0xf0;
			stars[n].size=rndv(XMB_SPARK_SIZE)+2;
		}
	}
}

void launch_web_browser(char *start_page)
{
	char browser_self[128];
	sprintf(browser_self, "%s/BROWSER.SELF", app_usrdir);
	if(exist(browser_self))
		launch_self(browser_self, start_page);
}

/****************************************************/
/* MAIN                                             */
/****************************************************/

u8 read_pad_info_browse()
{

	if ( (new_pad & BUTTON_UP) )
	{
		c_opacity_delta=16;	dimc=0; dim=1;

		new_pad=0;
		if(xmb[0].size>1)
		{
			xmb_legend_drawn=0;
			xmb_info_drawn=0;
			xmb_bg_show=0; xmb_bg_counter=XMB_BG_COUNTER;
			if(xmb[0].first==0) {xmb[0].first=xmb[0].size-1; xmb0_slide_y=0; xmb0_slide_step_y=0; }
			else
			{
				if(xmb0_slide_step_y!=0)
				{
					if(xmb0_slide_y >0) { if(xmb[0].first>0) xmb[0].first-=repeat_counter3; xmb0_slide_y=0;}
					if(xmb0_slide_y <0) { if(xmb[0].first<xmb[0].size-1) xmb[0].first+=repeat_counter3; xmb0_slide_y=0;}
					if(xmb[0].first==1 || xmb[0].first>=xmb[0].size) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
					if(xmb[0].first>=xmb[0].size) xmb[0].first=xmb[0].size-1;
				}
				//else
				xmb0_slide_step_y=10;
			}
		}
	}

	if ( (new_pad & BUTTON_DOWN) )
	{
		c_opacity_delta=16;	dimc=0; dim=1;
		new_pad=0;
		if(xmb[0].size>1)
		{
			xmb_legend_drawn=0;
			xmb_info_drawn=0;
			xmb_bg_show=0; xmb_bg_counter=XMB_BG_COUNTER;
			if(xmb[0].first==xmb[0].size-1) {xmb[0].first=0; xmb0_slide_y=0; xmb0_slide_step_y=0;}
			else
			{
				if(xmb0_slide_step_y!=0)
				{
					if(xmb0_slide_y >0) { if(xmb[0].first>0) xmb[0].first-=repeat_counter3; xmb0_slide_y=0;}
					if(xmb0_slide_y <0) { if(xmb[0].first<xmb[0].size-1) xmb[0].first+=repeat_counter3; xmb0_slide_y=0;}
					if(xmb[0].first>=xmb[0].size-2) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
				}

				xmb0_slide_step_y=-10;
				if(xmb[0].first>=xmb[0].size) xmb[0].first=xmb[0].size-1;

			}
		}
	}
	return 0;
}

u8 read_pad_info()
{
	u8 to_return=0;
	if(new_pad & BUTTON_UP)
	{
		c_opacity_delta=16;	dimc=0; dim=1;

		new_pad=0;
		if(xmb[xmb_icon].size>1)
		{
			xmb_legend_drawn=0;
			xmb_info_drawn=0;
			xmb_bg_loaded=0;
			xmb_bg_show=0; xmb_bg_counter=XMB_BG_COUNTER;
			if(xmb[xmb_icon].first==0) {xmb[xmb_icon].first=xmb[xmb_icon].size-1; xmb_slide_y=0; xmb_slide_step_y=0;}
			else
			{
				if(xmb_slide_step_y!=0)
				{
					if(xmb_slide_y >0) { if(xmb[xmb_icon].first>0) xmb[xmb_icon].first-=repeat_counter3; xmb_slide_y=0;}
					if(xmb_slide_y <0) { if(xmb[xmb_icon].first<xmb[xmb_icon].size-repeat_counter3) xmb[xmb_icon].first+=repeat_counter3; xmb_slide_y=0;}
					if(xmb[xmb_icon].first==1 || xmb[xmb_icon].first>=xmb[xmb_icon].size) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
					if(xmb[xmb_icon].first>=xmb[xmb_icon].size) xmb[xmb_icon].first=xmb[xmb_icon].size-1;
				}
				xmb_slide_step_y=10;
			}
		}
	}

	if(new_pad & BUTTON_DOWN)
	{
		c_opacity_delta=16;	dimc=0; dim=1;
		new_pad=0;

		if(xmb[xmb_icon].size>1)
		{
			xmb_legend_drawn=0;
			xmb_info_drawn=0;
			xmb_bg_loaded=0;
			xmb_bg_show=0; xmb_bg_counter=XMB_BG_COUNTER;
			if(xmb[xmb_icon].first==xmb[xmb_icon].size-1) {xmb[xmb_icon].first=0; xmb_slide_y=0; xmb_slide_step_y=0;}
			else
			{
				if(xmb_slide_step_y!=0)
				{
					if(xmb_slide_y >0) { if(xmb[xmb_icon].first>0) xmb[xmb_icon].first-=repeat_counter3; xmb_slide_y=0;}
					if(xmb_slide_y <0) { if(xmb[xmb_icon].first<xmb[xmb_icon].size-repeat_counter3) xmb[xmb_icon].first+=repeat_counter3; xmb_slide_y=0;}
					if(xmb[xmb_icon].first>=xmb[xmb_icon].size-2) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
				}
				xmb_slide_step_y=-10;
				if(xmb[xmb_icon].first>=xmb[xmb_icon].size) xmb[xmb_icon].first=xmb[xmb_icon].size-1;
			}
		}
	}

	if ((new_pad & BUTTON_LEFT))
	{
		xmb_legend_drawn=0;
		xmb_info_drawn=0;
		xmb_bg_loaded=0;
		if(xmb_icon>1)
		{
			if(xmb_slide_step!=0)
			{
				if(xmb_slide >0) {if(xmb_icon>2) xmb_icon--;  free_all_buffers(); xmb_slide=0;draw_xmb_icon_text(xmb_icon);}
				if(xmb_slide <0) {if(xmb_icon<MAX_XMB_ICONS-2) xmb_icon++; free_all_buffers(); xmb_slide=0; draw_xmb_icon_text(xmb_icon);}
				if(xmb_icon!=1) xmb_slide_step=15;
				if(xmb_icon==2) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
			}
			else
				xmb_slide_step=15;
		}
		goto leave_for8;
	}

	if(new_pad & BUTTON_RIGHT)
	{
		xmb_legend_drawn=0;
		xmb_info_drawn=0;
		xmb_bg_loaded=0;
		if(xmb_icon<MAX_XMB_ICONS-1)
		{
			if(xmb_slide_step!=0)
			{
				if(xmb_slide >0) {if(xmb_icon>2) xmb_icon--; free_all_buffers(); xmb_slide=0; draw_xmb_icon_text(xmb_icon);}
				if(xmb_slide <0) {if(xmb_icon<MAX_XMB_ICONS-2) xmb_icon++; free_all_buffers(); xmb_slide=0;draw_xmb_icon_text(xmb_icon);}
				xmb_slide_step=-15;
				if(xmb_icon!=MAX_XMB_ICONS-1) xmb_slide_step=-15;
				if(xmb_icon==MAX_XMB_ICONS-2) {repeat_counter1=120; repeat_counter2=repeat_key_delay;repeat_counter3=1;repeat_counter3_inc=0.f;}
			}
			else
				xmb_slide_step=-15;
		}
		goto leave_for8;
	}


		if((old_pad & BUTTON_SELECT) && (new_pad & BUTTON_R2))
		{
			c_opacity_delta=16;	dimc=0; dim=1;
			new_pad=0; //state_draw=1;
			overscan+=0.01f; if(overscan>0.10f) overscan=0.10f;
			{to_return=1; goto leave_for8;}
		}

		if((old_pad & BUTTON_SELECT) && (new_pad & BUTTON_L2))
		{
			c_opacity_delta=16;	dimc=0; dim=1;
			new_pad=0; //old_pad=0; state_draw=1;
			overscan-=0.01f;if(overscan<0.0f) overscan=0.00f;
			{to_return=1; goto leave_for8;}
		}


leave_for8:
	return to_return;
}


void draw_whole_xmb(u8 mode)
{
	xmb0_icon=xmb_icon;
	drawing_xmb=1;
	if(bounce<0) bounce=0;
	if(mode)
	{
		pad_read();
		read_pad_info();
	}

	xmb_bg_counter--;

	if(xmb_slide_step!=0 || xmb_slide_step_y!=0) xmb_bg_show=0;
	draw_stars();

	draw_xmb_bg();

	if(xmb_slide_step!=0) //xmmb sliding horizontally
	{
		xmb_slide+=xmb_slide_step+xmb_slide_step*video_mode;
			 /*if(xmb_slide == 165)  xmb_slide_step=3; //slow it down before settling
		else if(xmb_slide ==-165)  xmb_slide_step=-3;
		else */ if(xmb_slide == 180)  xmb_slide_step= 2;
		else if(xmb_slide ==-180)  xmb_slide_step=-2;
		else if(xmb_slide >= 200) {xmb_slide_step= 0; free_all_buffers(); xmb_icon--; xmb_slide=0; draw_xmb_icon_text(xmb_icon);}
		else if(xmb_slide <=-200) {xmb_slide_step= 0; free_all_buffers(); xmb_icon++; xmb_slide=0; draw_xmb_icon_text(xmb_icon);}

		if(xmb_icon>MAX_XMB_ICONS-1) xmb_icon=MAX_XMB_ICONS-1;
		else if(xmb_icon<1) xmb_icon=1;
		if(xmb_slide_step==0) xmb_bg_counter=XMB_BG_COUNTER;
	}

	if(xmb_slide_step_y!=0) //xmmb sliding vertically
	{
		xmb_slide_y+=xmb_slide_step_y+xmb_slide_step_y*video_mode;
			 if(xmb_slide_y == 20) xmb_slide_step_y = 5;
		else if(xmb_slide_y ==-20) xmb_slide_step_y =-5;
		else if(xmb_slide_y == 60) xmb_slide_step_y = 2;
		else if(xmb_slide_y ==-60) xmb_slide_step_y =-2;
		else if(xmb_slide_y == 80) xmb_slide_step_y = 1;
		else if(xmb_slide_y ==-80) xmb_slide_step_y =-1;
		else if(xmb_slide_y >= 90) {xmb_slide_step_y= 0; xmb_legend_drawn=0; if(xmb[xmb_icon].first>0) xmb[xmb_icon].first--; xmb_slide_y=0;}
		else if(xmb_slide_y <=-90) {xmb_slide_step_y= 0; xmb_legend_drawn=0; if(xmb[xmb_icon].first<xmb[xmb_icon].size-1) xmb[xmb_icon].first++; xmb_slide_y=0;}
		if(xmb_slide_step_y==0) xmb_bg_counter=XMB_BG_COUNTER;
	}

	draw_xmb_icons(xmb, xmb_icon, xmb_slide, xmb_slide_y, 0, xmb_sublevel, bounce);
	if(xmb_icon==2) xmb_settings_sel=xmb[2].first;

	draw_xmb_clock(xmb_clock, 0);
	draw_xmb_legend(xmb_icon);

	if(mode) flip();

	drawing_xmb=0;
}


void show_sysinfo()
{
		char sys_info[512];

		get_free_memory();

		char line1[128];
		char line2[128];
		char line3[128];
		char line4[128];
		char line5[128];

		char line0[128];
		char line7[128];

        uint16_t cobra_version;
        cobra_get_version(&cobra_version, NULL);

		//if(is_cobra)
		//sprintf(line7, "COBRA: %i.%02X", (cobra_version>>8), (cobra_version & 0xFF));

		#define SC_COBRA_SYSCALL8				(8)
		#define SYSCALL8_OPCODE_GET_MAMBA		0x7FFFULL

		bool is_mamba = false; {system_call_1(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_GET_MAMBA); is_mamba = ((int)p1 ==0x666);}
		char cobra_name[8]; sprintf(cobra_name, "%s", is_mamba ? "MAMBA" : "COBRA");

		if((cobra_version & 0xF) == 0)
			sprintf(line7, "%s: %i.%01X", cobra_name, (cobra_version>>8), (cobra_version & 0xFF) >> 4);
		else
			sprintf(line7, "%s: %i.%02X", cobra_name, (cobra_version>>8), (cobra_version & 0xFF));

		if(cobra_version==0) strcpy(line7, "\0");
		sprintf(line0, STR_NUMFW);

		u64 freeSpace = get_free_drive_space((char*)"/dev_hdd0");
		sprintf(line5, (char*) "%s: %.2f GB", STR_FREEHDD, (double)(freeSpace/1073741824.00f));

		strncpy(line4, current_version, 8); line4[8]=0;
		get_eid();
		//sprintf(line7, "\nCobra:%s", is_cobra?((char*)"Enabled"):((char*)"Disabled"));
		sprintf(line1, (char*) "LV2 Kernel: %s\nTarget Type: %s", dex_mode?((char*)"DEX"):((char*)"CEX"), dex_flash?((char*)"DEX"):((char*)"CEX"));//, (double) ((meminfo.avail+meminfo.total)/1024.0f)
		if(payload==-1) sprintf(line2, "%s %.2f", line0, c_firmware);
		if(payload== 0 && payloadT[0]!=0x44) sprintf(line2, "%s %.2f [SC-36 | PSGroove]", line0, c_firmware);
		if(dex_mode)
		{
			if(c_firmware==4.00f)
				sprintf(line2, "%s 03.60-04.30 DEX", line0);
			else
			{
				if(payload == 0 && payloadT[0]==0x44)
					sprintf(line2, "%s %.2f DEX [SC-36 | Standard]", line0, c_firmware);
				else
					sprintf(line2, "%s %.2f DEX", line0, c_firmware);
			}
		}
		else
		if(payload== 0 && payloadT[0]==0x44)
		{
			if(!dex_mode)
				sprintf(line2, "%s %.2f [SC-36 | Standard]", line0, c_firmware);
			else
				sprintf(line2, "%s %.2f DEX [SC-36 | Standard]", line0, c_firmware);
		}
		if(payload== 1) sprintf(line2, "%s %.2f [SC-36 | Hermes]", line0, c_firmware);
		if(payload== 2) sprintf(line2, "%s %.2f [SC-35 | PL3]", line0, c_firmware);

		net_avail=cellNetCtlGetInfo(16, &net_info);
		if(net_avail<0)
		{
			sprintf(line3, "%s: [%s]", STR_IP, STR_NOTAV);
		}
		else
			sprintf(line3, "%s: %s", STR_IP, net_info.ip_address);

		//sprintf(line3, "%08X | %08X", idps0, idps1);
		sprintf(sys_info, "%s %s: %s\n\n%s\n%s\n%s\n%s\n%s", (char*) STR_APP_NAME"-MULTI", (char*) "\nVersion", line4, line2, line3, line1, line5, line7);
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_back, sys_info, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog();
		dialog_ret=0;
}

void exit_app()
{
	if(auto_reboot) {system_call_4(379,0x1200,0,0,0);}// lv2_sm_shutdown(0x8201, NULL, 0);
	syscall_838("/dev_rebug");
	exit(0);

}
void quit_app()
{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_yes_no, STR_QUITXMB, dialog_fun1, (void*)0x0000aaaa, NULL );
		wait_dialog();

		if(dialog_ret==1)
		{
			unload_modules();
			exit_app();
		}
}

void shutdown_system(u8 mode) // X0-off X1-reboot, X=0 no prompt, X=1 prompt
{
	if( !(mode&0x10) || (mode&0x10))
	{
		unload_modules();
		if(!(mode&1))
		{
			system_call_4(379,0x1100,0,0,0);
		}
		if(mode&1)
		{
			lv2_sm_shutdown(0x8201, NULL, 0);
		} // 0x1100/0x100 = turn off,0x1200/0x200=reboot
		exit(0);
	}
}

void read_flash(u64 device, u32 _flags, char *iso_path)
{
	u32 buf_size=MB(2);
	u8* read_buffer = (unsigned char *) memalign(128, buf_size);
	u32 readlen=0;
	u64 disc_size=0;
	device_info_t disc_info;
	int rr;
	int dev_id;
	char filename[256];
	char _iso_path[512];
	char tmp_path[512];
	time_t time_start_l=time(NULL);

	rr=sys_storage_open(device, &dev_id);

	if(!rr) rr=sys_storage_get_device_info(device, &disc_info);

	disc_size = disc_info.sector_size * disc_info.total_sectors;

	if(disc_size && !rr)// && disc_in_tray!=PSX_DISC // && (exist((char*)"/dev_bdvd"))
	{
		initConsole();
		u32 sector=0;
		u64 sectors_written=0;
		u32 sec_step=(buf_size/disc_info.sector_size);
		u32 sec_step2=0;
		u64 split_limit=0xFFFF0000ULL;

		u8   split_part=0;
		bool split_mode=(strstr(iso_path, "/dev_hdd")==NULL);
		u64  split_segment=0;

		float read_speed=0.f;
		int prog=0;
		int prog_old=0;
		int eta=0;
		int f_iso=-1;
		u64 written=0;
		u8 save_ok=0;

		if(split_mode && disc_size>=split_limit)
			sprintf(_iso_path, "%s.%i", iso_path, split_part);
		else
			sprintf(_iso_path, "%s", iso_path);

		remove(_iso_path);
		#define DUMP_OFFSET_SYSROM				0x2401fc00000ull
		#define DUMP_SIZE_SYSROM						0x40000ull

		if(cellFsOpen(_iso_path, CELL_FS_O_CREAT|CELL_FS_O_RDWR|CELL_FS_O_TRUNC, &f_iso, NULL, 0)==CELL_FS_SUCCEEDED) save_ok=1;

		if( save_ok )
		{
		if(!is_nor())
		{
			uint64_t position;
			cellFsLseek(f_iso, 0x40000, CELL_FS_SEEK_SET, &position);
		}
			DPrintf("IMAGE SIZE: %i sectors (%.0f MB) (%u bytes per sector)\nSAVE PATH : %s\n\nPress [TRIANGLE] to abort\n\n", disc_info.total_sectors, (double) (disc_size/1048576), disc_info.sector_size, _iso_path);
			 flip();
			dialog_ret=0;
		}

		if ( save_ok )
		{
			time_start_l=time(NULL);
			dialog_ret=0;

			int seconds=time_start_l+1;
			int seconds2=seconds;
			while(1)
			{
				sec_step2=sec_step;
				if( (sector+sec_step)>disc_info.total_sectors ) sec_step2=disc_info.total_sectors-sector;
				if(!sec_step2) break;
				for(int retry=1;retry<30;retry++)
				{
					rr=sys_storage_read2(dev_id, sector, sec_step2, read_buffer, &readlen, _flags);

					if(!rr) break;

					DPrintf("!! | Sector: %06X/%06X | READ ERROR | Retrying... (%i/30)\n", sector, disc_info.total_sectors-sector, retry);
					flip();

					rr=sys_storage_read(dev_id, 0, 1, read_buffer, &readlen); //seek to start
					sys_timer_usleep(1000000);
					rr=sys_storage_read(dev_id, disc_info.total_sectors-2, 1, read_buffer, &readlen); //seek to end
					sys_timer_usleep(1000000);

					sec_step2/=2; if(sec_step2<2) sec_step2=1;
					readlen=0;
					pad_read();	if (new_pad&BUTTON_TRIANGLE) break;
				}

				seconds= (int) (time(NULL)-time_start_l)+1;
				if(!readlen || rr) break;
				pad_read();	if(new_pad&BUTTON_TRIANGLE) break;

				if(cellFsWrite(f_iso, (const void *)read_buffer, readlen*disc_info.sector_size, &written)!=CELL_FS_SUCCEEDED) break;
				if(written != (readlen*disc_info.sector_size)) break;

				sectors_written+=readlen;
				if(sectors_written==disc_info.total_sectors) break;

				prog=(sectors_written*100/disc_info.total_sectors);
				read_speed=(float)(((sectors_written*disc_info.sector_size)/seconds)/1048576.f);
				if(prog!=prog_old || ((seconds-seconds2)) )
				{
					if(sectors_written>0 && seconds>0) eta=(int) ((disc_info.total_sectors-sectors_written)/(sectors_written/seconds));
					 seconds2=seconds;
					DPrintf("%2i | Sector: %06X/%06X | Read: %5.f MB (%4.2f MB/s) | ETA %02i:%02i min\n", prog, sector, disc_info.total_sectors-sector, (double) (((u64)sector*disc_info.sector_size)/1048576),
						read_speed, (eta/60), eta % 60
						);
					prog_old=prog;
					flip();
				}

				if(readlen!=sec_step2) break;
				sector+=sec_step2;
				split_segment+=readlen*disc_info.sector_size;

				if(split_mode && ( (split_segment+sec_step2*disc_info.sector_size)>=split_limit))
				{
					//fclose(f_iso);
					cellFsClose(f_iso);
					cellFsChmod(_iso_path, 0666);
					split_part++;
					split_segment=0;
					sprintf(_iso_path, "%s.%i", iso_path, split_part);
					remove(_iso_path);
					DPrintf("** | Sector: %06X/%06X | Split segment: [%i]\n", sector, disc_info.total_sectors-sector, split_part);

					//f_iso = fopen(_iso_path, "wb");
					if(cellFsOpen(_iso_path, CELL_FS_O_CREAT|CELL_FS_O_RDWR|CELL_FS_O_TRUNC, &f_iso, NULL, 0)!=CELL_FS_SUCCEEDED) break;

				}
			}

			cellFsClose(f_iso);
			cellFsChmod(_iso_path, 0666);

			if(sectors_written!=disc_info.total_sectors) remove(_iso_path);

			if(progress_bar) cellMsgDialogAbort();
			if(!split_part)
			{
				sprintf(filename, "%s.0", iso_path);
				sprintf(tmp_path, "%s", iso_path);
				rename(filename, tmp_path);
				sprintf(_iso_path, "%s", tmp_path);
				cellFsChmod(tmp_path, 0666);
			}
			if(sectors_written!=disc_info.total_sectors || sectors_written==disc_info.total_sectors)
			{

				if(!is_nor())
				{
				uint64_t val_sysrom;
//				uint64_t nread_sysrom;
				uint64_t off;
				FILE *fp=fopen(_iso_path, "a+");
				u64 byte={0xffffffffffffffffULL};
				for (off = DUMP_OFFSET_SYSROM; off < DUMP_OFFSET_SYSROM + DUMP_SIZE_SYSROM; off += sizeof(uint64_t))
				{
				val_sysrom = peek_lv1_cobra(off);
				fwrite(&val_sysrom, 8, 1, fp);
				}
				for(uint64_t i=0;i<0xFC0000; i+=8)
				{
				fwrite(&byte, 8, 1, fp);
				}
				fclose(fp);
				}

				if(!is_nor())
				{
				uint64_t val_sysrom;
//				uint64_t nread_sysrom;
				uint64_t off;
				FILE *fp=fopen(_iso_path, "r+");
				fseek(fp, 0x0, SEEK_SET);
				for (off = DUMP_OFFSET_SYSROM; off < DUMP_OFFSET_SYSROM + DUMP_SIZE_SYSROM; off += sizeof(uint64_t))
				{
				val_sysrom = peek_lv1_cobra(off);
				fwrite(&val_sysrom, 8, 1, fp);
				}
				fclose(fp);
				}

				sprintf(_iso_path, "%s", iso_path);
				sprintf(filename, (const char*) "%s\n\n%s\n\n%s %.0f %s %i:%02i %s (%.2f %s)", STR_1, _iso_path, STR_2, (double) (disc_size/1048576), STR_3, (seconds/60), seconds % 60, STR_4, (double)(((sectors_written*disc_info.sector_size)/seconds)/1048576.f), STR_5);
			}
			else
				sprintf(filename, (const char*)"%s%lu%s%lu", STR_READFLASHERROR1, sectors_written, STR_READFLASHERROR2, disc_info.total_sectors);

			cellMsgDialogAbort();
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, filename, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
		}
		cellMsgDialogAbort();
		termConsole();
		new_pad=0;
	}
	else
	{
		cellMsgDialogAbort();
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, (const char*) "Data cannot be created!", dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	}

//cancel_iso_raw:
	rr=sys_storage_close(dev_id);
	free(read_buffer);

	ss_timer=0;
	ss_timer_last=time(NULL);
}

u8 get_idps(u8 _eid) //0=EID0, 5=EID5
{
	idps0=0;
	idps1=0;
	u8 retID=0;

	u16 o=0x70;
	u64 start_flash_sector=376; //2f070-> | 30200->303D0 | 70/1D0
	u64 device=FLASH_DEVICE_NOR;

	if(!is_nor())
	{
		start_flash_sector=516;
		device=FLASH_DEVICE_NAND;
	}

	if(_eid==5)
	{
		start_flash_sector+=9;
		o=0x1D0;
	}

	u32 readlen=0;
	u64 disc_size=0;
	device_info_t disc_info;
	int rr;
	int dev_id;

	rr=sys_storage_open(device, &dev_id);
	if(!rr) rr=sys_storage_get_device_info(device, &disc_info);
	disc_size = disc_info.sector_size * disc_info.total_sectors;
	u32 buf_size=disc_info.sector_size*1;
	u8* rb = (unsigned char *) memalign(128, buf_size);
	memset(rb, 0, buf_size);

	if(disc_size && !rr)
	{
		rr=sys_storage_read2(dev_id, start_flash_sector, 1, rb, &readlen, FLASH_FLAGS);
		idps0=
			(u64)rb[o + 0] << 56 |
			(u64)rb[o + 1] << 48 |
			(u64)rb[o + 2] << 40 |
			(u64)rb[o + 3] << 32 |
			(u64)rb[o + 4] << 24 |
			(u64)rb[o + 5] << 16 |
			(u64)rb[o + 6] <<  8 |
			(u64)rb[o + 7] <<  0;

		idps1=
			(u64)rb[o + 8] << 56 |
			(u64)rb[o + 9] << 48 |
			(u64)rb[o +10] << 40 |
			(u64)rb[o +11] << 32 |
			(u64)rb[o +12] << 24 |
			(u64)rb[o +13] << 16 |
			(u64)rb[o +14] <<  8 |
			(u64)rb[o +15] <<  0;

		retID=rb[o + 5];
	}
	rr=sys_storage_close(dev_id);
	free(rb);
	return retID;
}

void set_idps(u8 _val) //0=EID5, 1=EID0, 2=EID5, 3..13=0x82..0x8E
{

	u8 newID=0;
	u8 found=0;

	if(_val==0 || _val==2) newID=get_idps(5);
	else if(_val==1) newID=get_idps(0);
	else
	{
		get_idps(5);
		newID=_val-3+0x82;
	}

	if(!newID || newID<0x80 || newID>0xAA) return;
	if(!idps0 || !idps1) return;

	u64 cval1, cval2, newIDPS=0;

	open_please_wait();

	u64 cIDPS= (idps0 & 0xFFFFFFFFFF00FFFFULL);

    for(uint64_t n = 0; n < 0x7ffff0ULL; n ++)
	{
		cval1 = peekq(0x8000000000000000ULL + n    ) & 0xFFFFFFFFFF00FFFFULL;
		cval2 = peekq(0x8000000000000000ULL + n + 8);
        if( cval1 == cIDPS && cval2 == idps1)
		{
			newIDPS= cIDPS | (newID << 16);
			pokeq(0x8000000000000000ULL + n, newIDPS);
			n+=8;
			found++;
		}
	}

	char msg[512];
	if(found)
		sprintf(msg, "%s %i %s 0x%02X.", STR_CHANGEIDPSOK1, STR_CHANGEIDPSOK2, found, newID);
	else
		sprintf(msg, "%s: %08X-%08X-%08X-%08X", idps0>>32, idps0, idps1>>32, idps1);

	cellMsgDialogAbort();
	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, msg, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	cellMsgDialogAbort();
}

u8 get_eid() //0=CEX, 1=DEX
{

	dex_flash=0;
	u32 start_flash_address=0x2f000;
	u64 start_flash_sector=376;
	u64 device=FLASH_DEVICE_NOR;
	if(!is_nor())
	{
		start_flash_address=0x40800;
		start_flash_sector=516;
		device=FLASH_DEVICE_NAND;
	}

	u32 readlen=0;
	u64 disc_size=0;
	device_info_t disc_info;
	int rr;
	int dev_id;

	rr=sys_storage_open(device, &dev_id);

	if(!rr) rr=sys_storage_get_device_info(device, &disc_info);

	disc_size = disc_info.sector_size * disc_info.total_sectors;

	u32 buf_size=disc_info.sector_size*1;
	u8* read_buffer = (unsigned char *) memalign(128, buf_size);

	read_buffer[0x75]=0x00;
	if(disc_size && !rr)
	{
		rr=sys_storage_read2(dev_id, start_flash_sector, 1, read_buffer, &readlen, FLASH_FLAGS);
		if(read_buffer[0x75]==0x82) dex_flash=1;
	}
	rr=sys_storage_close(dev_id);
	free(read_buffer);
	return dex_flash;
}

void search_flash(u8 _mode, u8 _progr) // 0=CEX->DEX, 1=DEX->CEX
{
	u64 start_flash_sector;//=1536; // 0xC0000
	start_flash_sector=15872;

	u32 readlen=0;
	u64 disc_size=0;
	int rr;
	int dev_id;

	device_info_t disc_info;
	u64 device=FLASH_DEVICE_NAND;
	if(is_nor()) device=FLASH_DEVICE_NOR;

	rr=sys_storage_open(device, &dev_id);

	if(!rr) rr=sys_storage_get_device_info(device, &disc_info);

	disc_size = disc_info.sector_size * disc_info.total_sectors;

	u32 buf_size=disc_info.sector_size*3;
	u8* read_buffer = (unsigned char *) memalign(128, buf_size);
	u8 found=0, found_dex=0, found_cex=0;
	u8 ros=0;
	char msg[512];

	if(disc_size && !rr)
	{
		for(u8 m=0;m<2;m++)
		{
			found=0;
			found_dex=0;
			found_cex=0;
			if(m==0)
			{
				start_flash_sector=1536;
				if(!is_nor()) start_flash_sector=1024;
			}
			else
			if(m==1)
			{
				start_flash_sector=15872;
				if(!is_nor()) start_flash_sector=15360;
			}
			else break;
			rr=sys_storage_read2(dev_id, start_flash_sector, 3, read_buffer, &readlen, FLASH_FLAGS);

			if(readlen==3 && !rr)
			{
				for(u32 n=0; n<(readlen*disc_info.sector_size)-8; n+=8)
				{
					if(
							read_buffer[n+0]=='l' &&
							read_buffer[n+1]=='v' &&
							read_buffer[n+2]=='2' &&
							read_buffer[n+7]=='n'
						)
					{
						if(read_buffer[n+3]=='D') found_dex=1;
						if(read_buffer[n+3]=='C') found_cex=1;
					}
				}

				if(found_cex && found_dex || (!found_dex && !found_cex))
				{
					found=0;
					continue;
				}

				for(u32 n=0; n<(readlen*disc_info.sector_size)-8; n+=8)
				{
					if(!_mode && found_dex) //CEX->DEX
					{
						//lv2_kernel.self => lv2Ckernel.self
						//lv2Dkernel.self => lv2_kernel.self
						if(
							read_buffer[n+0]=='l' &&
							read_buffer[n+2]=='2' &&
							read_buffer[n+3]=='_' &&
							read_buffer[n+7]=='n'
							)
						{
							read_buffer[n+3] = 'C';
							found++;
							n+=8;
						}

						if(
							read_buffer[n+0]=='l' &&
							read_buffer[n+2]=='2' &&
							read_buffer[n+3]=='D' &&
							read_buffer[n+7]=='n'
							)
						{
							read_buffer[n+3] = '_';
							found++;
							n+=8;
						}
					}
					else
					if(_mode && found_cex)
					{
						//lv2_kernel.self => lv2Dkernel.self
						//lv2Ckernel.self => lv2_kernel.self
						if(
							read_buffer[n+0]=='l' &&
							read_buffer[n+2]=='2' &&
							read_buffer[n+3]=='_' &&
							read_buffer[n+7]=='n'
							)
						{
							read_buffer[n+3] = 'D';
							found++;
							n+=8;
						}

						if(
							read_buffer[n+0]=='l' &&
							read_buffer[n+2]=='2' &&
							read_buffer[n+3]=='C' &&
							read_buffer[n+7]=='n'
							)
						{
							read_buffer[n+3] = '_';
							found++;
							n+=8;
						}
					}
					if(found==2) break;
				}
			}



		/*FILE *fpA;
		sprintf(msg, "/dev_usb000/FD%i.BIN", _mode);
		remove(msg);
		fpA = fopen ( msg, "wb" );
		fwrite(read_buffer, 1536, 1, fpA);
		fclose(fpA);*/

			if(found==2)
			{
				rr=sys_storage_write(dev_id, start_flash_sector, 3, read_buffer, &readlen, FLASH_FLAGS);

				if(readlen==3 && !rr)
				{
					ros++;
				}
			}
		}

		if(ros)
		{
			if(!_mode)
				sprintf(msg, STR_SWP2DEXOK);
			else
				sprintf(msg, STR_SWP2CEXOK);
		}
		else
			sprintf(msg, STR_SWPERROR);

		if(ros || _progr)
		{
			cellMsgDialogAbort();
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, msg, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
			cellMsgDialogAbort();
		}

		if(ros && !rr)
		{
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_NEEDRESTSYS, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			//lv2_sm_shutdown(0x8201, NULL, 0);
			system_call_4(379,0x1200,0,0,0);
		}
		new_pad=0;
	}
	else
	{
		cellMsgDialogAbort();
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_NEEDRESTSYS, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	}

	cellMsgDialogAbort();
	rr=sys_storage_close(dev_id);
	free(read_buffer);

	ss_timer=0;
	ss_timer_last=time(NULL);
}

void write_eid(u64 device, u32 _flags, char *iso_path)
{

	get_eid();
	if(!strstr(iso_path, ".EID0.NORBIN") && !strstr(iso_path, ".EID0.NANDBIN")) return;

	u32 start_flash_address=0x2f000;
	u64 start_flash_sector=376;

	if(strstr(iso_path, ".EID0.NORBIN") && (!is_nor() || is_size(iso_path)!=16777216) ) return;
	if(strstr(iso_path, ".EID0.NANDBIN") && (is_nor() || (is_size(iso_path)!=251396096 && is_size(iso_path)!=268435456))) return;

	if(strstr(iso_path, ".EID0.NANDBIN"))
	{
		start_flash_address=0x40800;
		start_flash_sector=516;
	}

	u32 readlen=0;
	u64 disc_size=0;
	device_info_t disc_info;
	int rr;
	int dev_id;
	char msg[256];
	char _iso_path[512];
	time_t time_start_l=time(NULL);
	u8 check_kernel=0;
	u8 read_buffer2x=0;

	rr=sys_storage_open(device, &dev_id);

	if(!rr) rr=sys_storage_get_device_info(device, &disc_info);

	disc_size = disc_info.sector_size * disc_info.total_sectors;

	u32 buf_size=disc_info.sector_size*1;
	u8* read_buffer = (unsigned char *) memalign(128, buf_size);
	u8* read_buffer2 = (unsigned char *) memalign(128, buf_size);

	if(disc_size && !rr)// && disc_in_tray!=PSX_DISC // && (exist((char*)"/dev_bdvd"))
	{
		initConsole();
		u64 sectors_written=0;
		int f_iso=-1;

		sprintf(_iso_path, "%s", iso_path);

		//DPrintf("IMAGE SIZE: %i sectors (%.0f MB) (%u bytes per sector)\nSAVE PATH : %s\n\nPress [TRIANGLE] to abort\n\n", disc_info.total_sectors, (double) (disc_size/1048576), disc_info.sector_size, _iso_path);
		//flip();
		dialog_ret=0;
		//if(progress_bar) {cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL	|CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF	|CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NONE	|CELL_MSGDIALOG_TYPE_PROGRESSBAR_SINGLE, (const char*)"Overwriting FLASH region, please wait...",	NULL,	NULL,	NULL); flipc(90);}

		time_start_l=time(NULL);
		dialog_ret=0;

		u64 pos;
		rr=1;

		struct CellFsStat stat;
		cellFsStat(_iso_path, &stat);
		if(strstr(iso_path, ".EID0.NANDBIN") && stat.st_size==268435456)
		{
		start_flash_address=0x80800;
		}

		if(cellFsOpen(_iso_path, CELL_FS_O_RDONLY, &f_iso, NULL, 0)==CELL_FS_SUCCEEDED
			&& cellFsLseek(f_iso, start_flash_address, CELL_FS_SEEK_SET, &pos)==CELL_FS_SUCCEEDED)
		{
			if(pos==start_flash_address && cellFsRead(f_iso, read_buffer, 0x200, &pos)==CELL_FS_SUCCEEDED)
			{
				if(pos==0x200)
				{
					//read NOR IDPS and compare to input file
					//warn user in case of IDPS conflict
					bool idps_bad=true;

					rr=sys_storage_read2(dev_id, start_flash_sector, 1, read_buffer2, &readlen, _flags);

					char eid_info[512];
					cellMsgDialogAbort();
					dialog_ret=0;

					if( rr || !readlen ||
						read_buffer[0x70]!=read_buffer2[0x70] || read_buffer[0x71]!=read_buffer2[0x71] ||
						read_buffer[0x72]!=read_buffer2[0x72] || read_buffer[0x73]!=read_buffer2[0x73] ||
						read_buffer[0x74]!=read_buffer2[0x74] || read_buffer[0x76]!=read_buffer2[0x76] ||
						read_buffer[0x77]!=read_buffer2[0x77] || read_buffer[0x78]!=read_buffer2[0x78] ||
						read_buffer[0x79]!=read_buffer2[0x79] || read_buffer[0x7A]!=read_buffer2[0x7A] ||
						read_buffer[0x7B]!=read_buffer2[0x7B] || read_buffer[0x7C]!=read_buffer2[0x7C] ||
						read_buffer[0x7D]!=read_buffer2[0x7D] || read_buffer[0x7E]!=read_buffer2[0x7E] ||
						read_buffer[0x7F]!=read_buffer2[0x7F]
						)
					{
						if( rr || !readlen)
							sprintf(eid_info, "%s : sector#%i!", STR_WEIDERROR1, start_flash_sector);
						else
							sprintf(eid_info, STR_WEIDERROR2);
						cellMsgDialogOpen2( type_dialog_ok, eid_info, dialog_fun2, (void*)0x0000aaab, NULL );
						wait_dialog_simple();
						cellMsgDialogAbort();
						cellFsClose(f_iso);
						goto leave_flash;
					}
					else
					{
						if(read_buffer[0x75]==read_buffer2[0x75])
							sprintf(eid_info, "%s 0x%02X\n\n%s", STR_WEIDRUSURE1, read_buffer2[0x75], STR_WEIDRUSURE2);
						else
							sprintf(eid_info, "%s 0x%02X\n%s 0x%02X\n\n%s", STR_WEIDRUSURE3, read_buffer2[0x75], STR_WEIDRUSURE4, read_buffer[0x75], STR_WEIDRUSURE2);
						cellMsgDialogOpen2( type_dialog_yes_no, (const char*) eid_info, dialog_fun1, (void*)0x0000aaaa, NULL );
						wait_dialog_simple();
						cellMsgDialogAbort();

						if(dialog_ret==1)
						{
							dialog_ret=0;
							sprintf(eid_info, STR_WEIDRRUUSURE);
							cellMsgDialogOpen2( type_dialog_yes_no, (const char*) eid_info, dialog_fun1, (void*)0x0000aaaa, NULL );
							wait_dialog_simple();
							cellMsgDialogAbort();
							if(dialog_ret==1) idps_bad=0;
						}
						else
						{
							idps_bad=true;
							sectors_written=0;
							cellFsClose(f_iso);
							goto leave_flash;
						}
					}

					if(!idps_bad)
					{
						//readlen=128;
						read_buffer2x=read_buffer2[0x75];
						read_buffer2[0x75]=read_buffer[0x75];

						memcpy(read_buffer2+0x90, read_buffer+0x90, 0xC0);
						rr=sys_storage_write(dev_id, start_flash_sector, 1, read_buffer2, &readlen, _flags);
						sectors_written=readlen;
					}
					else
					{
						cellFsClose(f_iso);
						goto leave_flash;
					}

					//if(readlen==128) DPrintf("OK!\n"); else DPrintf("ERROR!\n");
				}
			}
			cellFsClose(f_iso);
		}

		if(progress_bar) cellMsgDialogAbort();
		if(sectors_written==1 && !rr)
		{
			check_kernel=1;

			if(read_buffer[0x75]==0x82)
				sprintf(msg, STR_WEIDOK1);
			else
			if(read_buffer2x==0x82)
				sprintf(msg, STR_WEIDOK1);
			else
				sprintf(msg, STR_WEIDOK2);
		}
		else
			sprintf(msg, STR_WEIDERROR3);

		cellMsgDialogAbort();
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, msg, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
		cellMsgDialogAbort();
		termConsole();
		new_pad=0;
	}
	else
	{
		cellMsgDialogAbort();
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_WEIDERROR4, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
	}

leave_flash:
	cellMsgDialogAbort();
	rr=sys_storage_close(dev_id);
	free(read_buffer);
	free(read_buffer2);

	if(check_kernel)
	{
		get_eid();
		if(!dex_flash)
		{
			rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx",
				(char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.dex");
			rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.cex",
				(char*)"/dev_rebug/vsh/module/software_update_plugin.sprx");
			// Host IP display for DEX mode
			rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
				(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex");
			rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex",
				(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
		}
		else
		{
			rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx",
				(char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.cex");
			rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.dex",
				(char*)"/dev_rebug/vsh/module/software_update_plugin.sprx");
            // Host IP display for DEX mode
			rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
				(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex");
			rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex",
				(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
		}

		if(!dex_flash)
		{
			search_flash(1, 0);
		}
	}

	ss_timer=0;
	ss_timer_last=time(NULL);
}

void write_to_device()
{
    if(!exist((char *)"/dev_hdd0/game/RBGTLBOX2/USRDIR/eid_root_key"))
    {
        if((c_firmware==4.21f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && dex_mode)
        {
            char message[512];
            sprintf(message, "%s (%2.2f DEX kernel) %s", STR_WTDMSG1, c_firmware, STR_WTDMSG2);
            cellMsgDialogOpen2( type_dialog_ok, message, dialog_fun2, (void*)0x0000aaab, NULL );
        }
        else if((c_firmware==3.55f || c_firmware==4.21f || c_firmware==4.46f || c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) && !dex_mode)
        {
            char message[512];
            sprintf(message, "%s (%2.2f CEX kernel) %s", STR_WTDMSG1, c_firmware, STR_WTDMSG2);
            cellMsgDialogOpen2( type_dialog_ok, message, dialog_fun2, (void*)0x0000aaab, NULL );
        }
        else
        {
            cellMsgDialogOpen2( type_dialog_ok, STR_WTDMSG3, dialog_fun2, (void*)0x0000aaab, NULL );
        }
        dialog_ret=0; wait_dialog_simple();
    }
    if(exist((char *)"/dev_hdd0/game/RBGTLBOX2/USRDIR/eid_root_key"))
    {
        dialog_ret=0;
        u8 cid=get_idps(0);//EID0 TargetID
		char str[128];
		
        if(cid!=0x82) {
			sprintf(str, "%s CEX to DEX ?", STR_WTDMSG4);
			cellMsgDialogOpen2( type_dialog_yes_no, str, dialog_fun1, (void*)0x0000aaaa, NULL );
		}   
        else {
			sprintf(str, "%s DEX to CEX ?", STR_WTDMSG4);
			cellMsgDialogOpen2( type_dialog_yes_no, str, dialog_fun1, (void*)0x0000aaaa, NULL );
		}
            
        wait_dialog_simple();

        if(dialog_ret==1)
        {
            dialog_ret=0;
            u8 eid5_idps=get_idps(5);
            if(eid5_idps==0x82)
            {
                dialog_ret=0;
				
                cellMsgDialogOpen2( type_dialog_yes_no, STR_WTDMSG5, dialog_fun1, (void*)0x0000aaab, NULL );
                wait_dialog_simple();
                if(dialog_ret!=1) return;

                dialog_ret=0;
            }
            cex_dex();

            get_eid();
            if(!dex_flash)
            {
                rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx",
                       (char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.dex");
                rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.cex",
                       (char*)"/dev_rebug/vsh/module/software_update_plugin.sprx");
				// Host IP addr display for DEX MODE
                rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
                       (char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex");
                rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex",
                       (char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
            }
            else
            {
                rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx",
                       (char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.cex");
                rename((char*)"/dev_rebug/vsh/module/software_update_plugin.sprx.dex",
                       (char*)"/dev_rebug/vsh/module/software_update_plugin.sprx");
				// Host IP addr display for DEX MODE
                rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
                       (char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex");
                rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex",
                       (char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
            }

            if(!dex_flash)
            {
                search_flash(1, 0);
            }

            ss_timer=0;
            ss_timer_last=time(NULL);
            if(exist((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/error_cex_dex"))
            {
                dialog_ret=0;
                cellMsgDialogOpen2( type_dialog_ok, STR_WTDMSG6, dialog_fun2, (void*)0x0000aaab, NULL );
                wait_dialog_simple();
                remove("/dev_hdd0/game/RBGTLBOX2/USRDIR/error_cex_dex");
            }
            return;
            cellMsgDialogAbort();
        }
        dialog_ret=0;
    }
    u64 device;
    char iso_path[512];

    get_eid();
    if(dex_flash)
    {
        if(is_nor()) sprintf(iso_path, "/dev_usb000/CEX-FLASH.EID0.NORBIN");
        else         sprintf(iso_path, "/dev_usb000/CEX-FLASH.EID0.NANDBIN");
    }
    else
    {
        if(is_nor()) sprintf(iso_path, "/dev_usb000/DEX-FLASH.EID0.NORBIN");
        else         sprintf(iso_path, "/dev_usb000/DEX-FLASH.EID0.NANDBIN");
    }

    if(is_nor()) device=FLASH_DEVICE_NOR; else device=FLASH_DEVICE_NAND;

    if(exist(iso_path) && strstr(iso_path, ".EID0.NORBIN") || strstr(iso_path, ".EID0.NANDBIN"))
        {write_eid(device, FLASH_FLAGS, iso_path); return;}

    cellMsgDialogAbort();
    dialog_ret=0;
    char msg[512];
    sprintf(msg, STR_WTDMSG7, ":\n\n%s", iso_path);
    cellMsgDialogOpen2( type_dialog_ok, (const char*) msg, dialog_fun2, (void*)0x0000aaab, NULL );
    wait_dialog_simple();

    dialog_ret=0;
    if(is_nor())
        cellMsgDialogOpen2( type_dialog_ok, STR_WTDMSG8, dialog_fun2, (void*)0x0000aaab, NULL );
    else
        cellMsgDialogOpen2( type_dialog_ok, STR_WTDMSG9, dialog_fun2, (void*)0x0000aaab, NULL );
    wait_dialog_simple();
    return;
}

int main(int argc, char **argv)
{
	is_cobra = is_cobra_based();

	cellSysutilRegisterCallback( 0, sysutil_callback, NULL );

	(void)argc;

	int ret;

	ret = cellSysmoduleLoadModule( CELL_SYSMODULE_FS );
	ret = cellSysmoduleLoadModule( CELL_SYSMODULE_GCM_SYS );
	if (ret != CELL_OK) exit(0);

	host_addr = memalign(MB(1), MB(2));

	if(cellGcmInit(KB(256), MB(2), host_addr) != CELL_OK) exit(0);
	if(initDisplay()!=0) exit(0);
	initShader();
	setRenderTarget(0);
	setRenderTarget(1);
	setDrawEnv();
	if(setRenderObject()) exit(0);

	cellPadInit(8);
	initFont();
	parse_ini();

	sprintf(app_path, STR_APP_ID);

	if(!strncmp( argv[0], "/dev_hdd0/game/", 15))
	{
		char *s;
		int n=0;
		s= ((char *) argv[0])+15;
		while(*s!=0 && *s!='/' && n<10) {app_path[n]=*s++; n++;} app_path[n]=0;
	}

	sprintf(app_homedir, "/dev_hdd0/game/%s",app_path);
	sprintf(app_usrdir, "%s/USRDIR", app_homedir);

	mkdir(app_homedir, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
	mkdir(app_usrdir, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);

	sprintf(app_img, "%s/img", app_usrdir);
	mkdir(app_img, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);

	char string1[1024], filename[1024];

	sprintf(current_version_NULL, "%s", current_version);
	sys_spu_initialize(6, 0);
	cellFsAioInit((char*)"/dev_bdvd");
	cellFsAioInit((char*)"/dev_hdd0");

	ret = load_modules();
	if(ret!=CELL_OK && ret!=0) exit(0);

	initAIO();
	cellSysutilEnableBgmPlayback();
	pad_reset();

//	dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, "This is a dummy Blu-ray Disc application to aid support\nfor broken BD-ROM drives in REBUG CFW environment.", dialog_fun2, (void*)0x0000aaab, NULL );	wait_dialog_simple();
//	exit(0);

	//if(!exist((char*)"/dev_flash/vsh/etc/index.dat.nrm") && !exist((char*)"/dev_flash/vsh/etc/index.dat.swp"))
	if(!exist((char*)"/dev_flash/rebug/packages"))
	{
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_MAINNORBG, dialog_fun2, (void*)0x0000aaab, NULL );	wait_dialog_simple();
		rebug_compatible=0;
	}
	if(!exist((char*)"/dev_flash/vsh/etc/index.dat.nrm") && !exist((char*)"/dev_flash/vsh/etc/index.dat.swp"))
	{
		rex_compatible=0;
	}

	if(!exist((char*)"/dev_flash/rebug/cobra/stage2.cex") /*&& !exist((char*)"/dev_flash/rebug/cobra/stage2.dex")*/)
	{
		if(!exist((char*)"/dev_flash/rebug/cobra/stage2.cex.bak") /*&& !exist((char*)"/dev_flash/rebug/cobra/stage2.dex.bak")*/)
		{
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_MAINNOCOB, dialog_fun2, (void*)0x0000aaab, NULL );	wait_dialog_simple();
			cobra_compatible=0;
		}
	}

/*	for(int n=0; n<7; n++)
	{
		pad_read();
		reset_mode = reset_mode || (( (new_pad | old_pad) & (BUTTON_R3 | BUTTON_L3 | BUTTON_R2 | BUTTON_L2) ) == (BUTTON_R3 | BUTTON_L3 | BUTTON_R2 | BUTTON_L2));
		new_pad=0;
	}

	if(reset_mode)
	{

	}
*/

	/* LANGUAGE */
	get_all_language();
	load_activ();
	update_language();

	// allocate buffers for images (one big buffer = 79MB)

	u32 buf_align= FB(1);
	u32 frame_buf_size = (buf_align * 7);//+6963200;// for text_bmpS 320x320x15 -> 1920*648 * 4
	frame_buf_size = ( frame_buf_size + 0xfffff ) & ( ~0xfffff );

	text_bmp = (u8 *) memalign(0x100000, frame_buf_size);
	if(map_rsx_memory( text_bmp, frame_buf_size)) exit(0);

	text_bmpUBG	= text_bmp + buf_align * 1; //2! x 1920x1080 XMB ICONS
	text_bmpUPSR= text_bmp + buf_align * 2; //2  x 1920x1080 XMB TEXTS
	text_FONT	= text_bmp + buf_align * 4;

	text_FMS	= text_bmp + buf_align * 5;
	text_bmpS	= text_bmp + buf_align * 6;// + (DASH_W*DASH_H*4*(MAX_XMB_ICONS+1)); //13.75MB 14417920 640x512*4 * 11

//====================================================================
	xmb_icon_refresh=	text_FMS+( 0*65536);
	xmb_icon_home	=	text_FMS+( 1*65536);

	xmb_icon_util	=	text_FMS+( 6*65536);
	xmb_icon_off	=	text_FMS+( 7*65536);
	xmb_icon_info	=	text_FMS+( 8*65536);

	xmb_icon_globe	=	text_FMS+( 9*65536);
	xmb_icon_help	=	text_FMS+(10*65536);
	xmb_icon_quit	=	text_FMS+(11*65536);
	xmb_icon_star	=	text_FMS+(12*65536);
	xmb_icon_desk	=	text_FMS+(13*65536);
	xmb_icon_hdd	=	text_FMS+(14*65536);
	xmb_icon_blu	=	text_FMS+(15*65536);
	xmb_icon_tool	=	text_FMS+(16*65536);
	xmb_icon_note	=	text_FMS+(17*65536);
	xmb_icon_film	=	text_FMS+(18*65536);
	xmb_icon_photo	=	text_FMS+(19*65536);
	xmb_icon_update	=	text_FMS+(20*65536);
	xmb_icon_ss		=	text_FMS+(21*65536);
	xmb_icon_showtime=	text_FMS+(22*65536);
	xmb_icon_theme	=	text_FMS+(23*65536); //XMB.PNG end (24 icons 128x128 = 128x3072)
//====================================================================
	xmb_icon_retro	=	text_FMS+(24*65536); //XMB2.PNG start
	xmb_icon_ftp	=	text_FMS+(25*65536);
	xmb_icon_folder	=	text_FMS+(26*65536);
	xmb_icon_usb	=	text_FMS+(27*65536);
	xmb_icon_psx	=	text_FMS+(28*65536);
	xmb_icon_ps2	=	text_FMS+(29*65536);
	xmb_icon_psp	=	text_FMS+(30*65536);
	xmb_icon_dvd	=	text_FMS+(31*65536);
	xmb_icon_bdv	=	text_FMS+(32*65536);
	xmb_icon_psp2	=	text_FMS+(33*65536);
	xmb_icon_usb_update=text_FMS+(34*65536);

	xmb_icon_psx_n	=	text_FMS+(35*65536);
	xmb_icon_psp_n	=	text_FMS+(36*65536);
	xmb_icon_dvd_n	=	text_FMS+(37*65536);
	xmb_icon_bdv_n	=	text_FMS+(38*65536);
	xmb_icon_blu_n	=	text_FMS+(39*65536);
	xmb_icon_ps2_n	=	text_FMS+(40*65536);

//	text_???		=	text_FMS+(35*65536); //
	xmb_icon_blend	=	text_FMS+(46*65536);
//	text_???LAST	=	text_FMS+(46*65536); //XMB2.PNG end (24 icons 128x128 = 128x3072)
//====================================================================
	xmb_icon_star_small	=	text_FMS+(48*65536)+(16384*0);//64*64*4
	xmb_icon_blu_small	=	text_FMS+(48*65536)+(16384*1);//64*64*4
	xmb_icon_net_small	=	text_FMS+(48*65536)+(16384*2);//64*64*4
//	xmb_icon_????_small	=	text_FMS+(48*65536)+(16384*3);

//	xmb_icon_????_small	=	text_FMS+(49*65536)+(16384*0);
	xmb_col				=	text_FMS+(50*65536); // Current XMMB Column name (300x30)
//====================================================================
//	xmb_icon_dev	=	text_FMS+(51*65536); // XMB64.PNG start (32 icons 64x64 = 64x2048) (51->58)
//====================================================================
	text_DOX		=	text_FMS+(59*65536); // DOX.PNG (256x256) (59->62)
//====================================================================
	text_MSG		=	text_FMS+(63*65536); // Legend pop-up in XMMB mode (300x70) (63->64)
	text_INFO		=	text_FMS+(65*65536); // Info pop-up in XMMB mode (300x70) (65->66)

	u32 buf_align2= (96 * 96 * 4);
	text_HDD   = text_INFO + 65536*2;
	text_USB   = text_HDD + buf_align2 * 1;
	text_BLU_1 = text_HDD + buf_align2 * 2;
	text_OFF_2 = text_HDD + buf_align2 * 3;
	text_CFC_3 = text_HDD + buf_align2 * 4;
	text_SDC_4 = text_HDD + buf_align2 * 5;
	text_MSC_5 = text_HDD + buf_align2 * 6;
	text_NET_6 = text_HDD + buf_align2 * 7;

	buf_align2= (320 * 320 * 4);
	text_PBOX  = text_NET_6 + 36864; //96*96*4
	text_PBOX1 = text_PBOX + buf_align2  * 1;
	text_CBOX  = text_PBOX + buf_align2  * 2;
	text_GBOX  = text_PBOX + (buf_align2 * 2) + 496976; //17
	text_TEXTS = text_PBOX + buf_align2  * 4;
	text_DEVS  = text_PBOX + (buf_align2 * 4) + 269952; //491520
	text_TEMP  = text_DEVS + 761472;

//	text_???		=	text_FMS+(67*65536); //
//	text_???		=	text_FMS+(120*65536);//
	xmb_clock		=	text_FMS+(122*65536);// Clock (300x30) (and info-popups)
	xmb_icon_arrow	=	text_FMS+(124*65536);//
	xmb_icon_logo	=	text_FMS+(125*65536);// 125
//	text_???LAST	=	text_FMS+(125*65536);// end of text_FMS frame buffer
//====================================================================

	buf_align2= (320 * 320 * 4);
	text_bmpIC = text_bmpS + buf_align2 * 1;
	text_DROPS = text_bmpS + buf_align2 * 2;
	text_SLIDER= text_DROPS + 262144;
	text_legend= text_bmpS + buf_align2 * 3; //+14

	for(int n=0; n<MAX_XMB_THUMBS; n++)
	{
		xmb_icon_buf[n].used=-1;
		xmb_icon_buf[n].column=0;
		xmb_icon_buf[n].data=text_bmpUBG+(n*XMB_THUMB_WIDTH*XMB_THUMB_HEIGHT*4);
	}


	sprintf(xmb_columns[0], STR_APP_NAME);
	sprintf(xmb_columns[1], STR_SYSTEM);
	sprintf(xmb_columns[2], STR_SELECTOR);
	sprintf(xmb_columns[3], STR_LV1PATCH);
	sprintf(xmb_columns[4], STR_DTC);
	sprintf(xmb_columns[5], STR_UTLS);
	sprintf(xmb_columns[6], "Empty");
	sprintf(xmb_columns[7], "Empty");
	sprintf(xmb_columns[8], "Empty");
	sprintf(xmb_columns[9], "Empty");

	for(int n=0; n<MAX_XMB_ICONS ; n++) sprintf(xmb[n].name, "%s", xmb_columns[n]);

	c_firmware = (float) get_system_version();

	u64 CEX=0x4345580000000000ULL;
	u64 DEX=0x4445580000000000ULL;

	if(peekq(0x80000000002E79C8ULL)==DEX) {dex_mode=2; c_firmware=3.41f;}
	else
	if(peekq(0x80000000002CFF98ULL)==CEX) {dex_mode=0; c_firmware=3.41f;}
	else
	if(peekq(0x80000000002EFE20ULL)==DEX) {dex_mode=2; c_firmware=3.55f;}
	else
	if(peekq(0x80000000002D83D0ULL)==CEX) {dex_mode=0; c_firmware=3.55f;}
	else
	if(peekq(0x8000000000302D88ULL)==DEX) {dex_mode=2; c_firmware=4.21f;}
	else
	if(peekq(0x80000000002E8610ULL)==CEX) {dex_mode=0; c_firmware=4.21f;}
	else
	if(peekq(0x80000000002E9F08ULL)==CEX) {dex_mode=0; c_firmware=4.30f;}
	else
	if(peekq(0x8000000000304630ULL)==DEX) {dex_mode=2; c_firmware=4.30f;}
	else
	if(peekq(0x80000000002E9F18ULL)==CEX) {dex_mode=0; c_firmware=4.31f;}
	else
	if(peekq(0x80000000002EA488ULL)==CEX) {dex_mode=0; c_firmware=4.40f;}
	else
	if(peekq(0x80000000002EA498ULL)==CEX) {dex_mode=0; c_firmware=4.41f;}
	else
	if(peekq(0x8000000000304EF0ULL)==DEX) {dex_mode=2; c_firmware=4.41f;}
	else
	if(peekq(0x80000000002EA9B8ULL)==CEX) {dex_mode=0; c_firmware=4.46f;}
	else
	if(peekq(0x8000000000305410ULL)==DEX) {dex_mode=2; c_firmware=4.46f;}
	else
	if(peekq(0x80000000002E9BE0ULL)==CEX) {dex_mode=0; c_firmware=4.50f;}
	else
	if(peekq(0x8000000000309698ULL)==DEX) {dex_mode=2; c_firmware=4.50f;}
	else
	if(peekq(0x80000000002E9D70ULL)==CEX) {dex_mode=0; c_firmware=4.53f;}
	else
	if(peekq(0x800000000030AEA8ULL)==DEX) {dex_mode=2; c_firmware=4.53f;}
	else
	if(peekq(0x80000000002EC5E0ULL)==CEX) {dex_mode=0; c_firmware=4.55f;}
	else
	if(peekq(0x800000000030D6A8ULL)==DEX) {dex_mode=2; c_firmware=4.55f;}
	else
	if(peekq(0x80000000002ED850ULL)==CEX) {dex_mode=0; c_firmware=4.60f;}
	else
	if(peekq(0x80000000002ED860ULL)==CEX && peekq(0x80000000002FC938ULL)!=0x323031342F31312FULL) {c_firmware=4.65f;} //timestamp: 2014/08
	else
	if(peekq(0x80000000002ED860ULL)==CEX && peekq(0x80000000002FC938ULL)==0x323031342F31312FULL) {c_firmware=4.66f;} //timestamp: 2014/11
	else
 	if(peekq(0x800000000030F1A8ULL)==DEX && peekq(0x800000000031EBA8ULL)!=0x323031342F31312FULL) {dex_mode=2; c_firmware=4.65f;} //timestamp: 2014/08
	else
	if(peekq(0x800000000030F1A8ULL)==DEX && peekq(0x800000000031EBA8ULL)==0x323031342F31312FULL) {dex_mode=2; c_firmware=4.66f;} //timestamp: 2014/11
	else
	if(peekq(0x80000000002ED778ULL)==CEX) {dex_mode=0; c_firmware=4.70f;}
	else
	if(peekq(0x800000000030F240ULL)==DEX) {dex_mode=2; c_firmware=4.70f;}
	else
	if(peekq(0x80000000002ED818ULL)==CEX && peekq(0x80000000002FCB68ULL)==0x323031352F30342FULL) {dex_mode=0; c_firmware=4.75f;} //timestamp: 2015/04
	else
	if(peekq(0x80000000002ED818ULL)==CEX && peekq(0x80000000002FCB68ULL)==0x323031352F30382FULL) {dex_mode=0; c_firmware=4.76f;} //timestamp: 2015/08
    else
	if(peekq(0x80000000002ED818ULL)==CEX && peekq(0x80000000002FCB68ULL)==0x323031352F31322FULL) {dex_mode=0; c_firmware=4.78f;} //timestamp: 2015/12
    else
	if(peekq(0x80000000002ED818ULL)==CEX && peekq(0x80000000002FCB68ULL)==0x323031362F31302FULL) {dex_mode=0; c_firmware=4.81f;} //timestamp: 2016/10
    else
	if(peekq(0x800000000030F2D0ULL)==DEX && peekq(0x800000000031EF48ULL)==0x323031352F30342FULL) {dex_mode=2; c_firmware=4.75f;} //timestamp: 2015/04
    else
	if(peekq(0x800000000030F2D0ULL)==DEX && peekq(0x800000000031EF48ULL)==0x323031352F30382FULL) {dex_mode=2; c_firmware=4.76f;} //timestamp: 2015/08
	else
	if(peekq(0x800000000030F2D0ULL)==DEX && peekq(0x800000000031EF48ULL)==0x323031352F31322FULL) {dex_mode=2; c_firmware=4.78f;} //timestamp: 2015/12
	else
	if(peekq(0x80000000002ED808ULL)==CEX) {dex_mode=0; c_firmware=4.80f;}
	else
	if(peekq(0x800000000030F3A0ULL)==DEX) {dex_mode=2; c_firmware=4.80f;}
	else
	if(peekq(0x800000000030F3B0ULL)==DEX) {dex_mode=2; c_firmware=4.81f;}
	else
	if(c_firmware == 0)
    {
    dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_MAINNOFW, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog_simple();
    exit(0);
    }
	/*else
	{
		if(c_firmware<3.55f)
			c_firmware=3.41f;
	}*/

	//if(c_firmware>=4.21f && peekq(0x8000000000001790ULL) == 0x800000000000173CULL) bdisk_mode=0; else bdisk_mode=1;

	if(c_firmware==3.55f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_355;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_355;
		SYSCALL_TABLE			= SYSCALL_TABLE_355;
	}
	else
	if(c_firmware==3.55f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_355D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_355D;
		SYSCALL_TABLE			= SYSCALL_TABLE_355D;
	}
	else
	if(c_firmware==4.21f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_421;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_421;
		SYSCALL_TABLE			= SYSCALL_TABLE_421;
	}
	else
	if(c_firmware==4.21f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_421D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_421D;
		SYSCALL_TABLE			= SYSCALL_TABLE_421D;
	}
	else
	if(c_firmware==4.30f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_430;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_430;
		SYSCALL_TABLE			= SYSCALL_TABLE_430;
	}
	else
	if(c_firmware==4.31f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_431;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_431;
		SYSCALL_TABLE			= SYSCALL_TABLE_431;
	}
	else
	if(c_firmware==4.30f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_430D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_430D;
		SYSCALL_TABLE			= SYSCALL_TABLE_430D;
	}
	else
	if(c_firmware==4.40f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_440;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_440;
		SYSCALL_TABLE			= SYSCALL_TABLE_440;
	}
	else
	if(c_firmware==4.41f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_441;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_441;
		SYSCALL_TABLE			= SYSCALL_TABLE_441;
	}
	else
	if(c_firmware==4.41f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_441D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_441D;
		SYSCALL_TABLE			= SYSCALL_TABLE_441D;
	}
	else
	if(c_firmware==4.46f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_446;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_446;
		SYSCALL_TABLE			= SYSCALL_TABLE_446;
	}
	else
	if(c_firmware==4.46f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_446D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_446D;
		SYSCALL_TABLE			= SYSCALL_TABLE_446D;
	}
	else
	if(c_firmware==4.50f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_450;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_450;
		SYSCALL_TABLE			= SYSCALL_TABLE_450;
	}
	else
	if(c_firmware==4.53f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_453;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_453;
		SYSCALL_TABLE			= SYSCALL_TABLE_453;
	}
	else
	if(c_firmware==4.53f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_453D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_453D;
		SYSCALL_TABLE			= SYSCALL_TABLE_453D;
	}
	else
	if(c_firmware==4.55f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_455;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_455;
		SYSCALL_TABLE			= SYSCALL_TABLE_455;
	}
	else
	if(c_firmware==4.55f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_455D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_455D;
		SYSCALL_TABLE			= SYSCALL_TABLE_455D;
	}
	else
	if(c_firmware==4.60f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_460;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_460;
		SYSCALL_TABLE			= SYSCALL_TABLE_460;
	}
	else
	if(c_firmware==4.65f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_465;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_465;
		SYSCALL_TABLE			= SYSCALL_TABLE_465;
	}
	else
	if(c_firmware==4.66f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_466;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_466;
		SYSCALL_TABLE			= SYSCALL_TABLE_466;
	}
	else
	if(c_firmware==4.65f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_465D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_465D;
		SYSCALL_TABLE			= SYSCALL_TABLE_465D;
	}
	else
	if(c_firmware==4.66f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_466D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_466D;
		SYSCALL_TABLE			= SYSCALL_TABLE_466D;
	}
	else
	if(c_firmware==4.70f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_470;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_470;
		SYSCALL_TABLE			= SYSCALL_TABLE_470;
	}
	else
	if(c_firmware==4.70f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_470D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_470D;
		SYSCALL_TABLE			= SYSCALL_TABLE_470D;
	}
	else
	if(c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.81f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_475;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_475;
		SYSCALL_TABLE			= SYSCALL_TABLE_475;
	}
	else
	if(c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_475D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_475D;
		SYSCALL_TABLE			= SYSCALL_TABLE_475D;
	}
	else
	if(c_firmware==4.80f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_480;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_480;
		SYSCALL_TABLE			= SYSCALL_TABLE_475; // The same for 4.75-4.80
	}
	else
	if(c_firmware==4.80f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_480D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_480D;
		SYSCALL_TABLE			= SYSCALL_TABLE_480D;
	}
	else
	if(c_firmware==4.81f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_481D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_481D;
		SYSCALL_TABLE			= SYSCALL_TABLE_480D;
	}
	else
	/*if(c_firmware==4.76f && !dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_476;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_476;
		SYSCALL_TABLE			= SYSCALL_TABLE_476;
	}
	else
	if(c_firmware==4.76f && dex_mode)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_476D;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_476D;
		SYSCALL_TABLE			= SYSCALL_TABLE_476D;
	}
	else*/
	if(c_firmware==3.41f)
	{
		HVSC_SYSCALL_ADDR		= HVSC_SYSCALL_ADDR_341;
		NEW_POKE_SYSCALL_ADDR	= NEW_POKE_SYSCALL_ADDR_341;
		SYSCALL_TABLE			= SYSCALL_TABLE_341;
	}


	generic_patches();

	free_all_buffers();
	free_text_buffers();

	sprintf(options_bin,  "%s/options.bin", app_usrdir);

	unmount_dev_flash();
	if(exist((char*)"/dev_rebug"))
		{pad_motor(1,16); mount_dev_blind=1;}
	else
		mount_dev_flash();

	if(!exist((char*)"/dev_rebug"))
	{
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, STR_MAINERRFLASH, dialog_fun2, (void*)0x0000aaab, NULL );	wait_dialog_simple();
		exit(0);
	}

	rnd=time(NULL)&0x03;

	sprintf(xmbicons,	"%s/XMB.PNG", app_img);
//	sprintf(xmbicons2,	"%s/XMB2.PNG", app_img);

	sprintf(xmbbg,		"%s/XMBBG.PNG", app_img);

	sprintf(blankBG, "%s/ICON0.PNG", app_homedir);

    payload = -1;  // 0->psgroove  1->hermes  2->PL3
	payloadT[0]=0x20;

	load_texture(text_bmpIC, blankBG, 320);

	memset(text_PBOX, 0x10, 409600);
	memset(text_PBOX1, 0x10, 409600);
	memset(text_CBOX, 0x10, 496976); //349x356 x4
	memset(text_GBOX, 0x00, 312000); //260x300 x4

	sprintf(filename, "%s/DOX.PNG", app_img);
	load_texture(text_DOX, filename, dox_width);

	clean_up();
	remove((char*)"/dev_hdd0/tmp/turnoff");

	for(int n=0; n<MAX_STARS;n++)
	{
		stars[n].x=rndv(1920);
		stars[n].y=rndv(1080);
		stars[n].bri=rndv(128);
		stars[n].size=rndv(XMB_SPARK_SIZE)+1;
	}

	init_finished=1;

	net_avail=cellNetCtlGetInfo(16, &net_info);

	ss_timer=0;
	ss_timer_last=time(NULL);

	pad_reset();
	cellGcmSetFlipMode(CELL_GCM_DISPLAY_VSYNC);

	init_xmb_icons();
	load_texture(text_FMS, xmbicons, 128);
//	mip_texture( xmb_icon_star_small, xmb_icon_star, 128, 128, -4);

	/* main GUI loop */
	main_ftp(0); //start the FTP server

	while(1)
	{

//start_of_loop:
	pad_read();
	dimc=0; dim=0; c_opacity_delta=0;


force_reload:

	// make screenshot in RAW RGB
	if ( ((old_pad & BUTTON_START) && (new_pad & BUTTON_R2)))
	{
		new_pad=0;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		char video_mem[64];
		sprintf(video_mem, "/dev_hdd0/%04d%02d%02d-%02d%02d%02d-SCREENSHOT.RAW", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		FILE *fpA;
		remove(video_mem);
		fpA = fopen ( video_mem, "wb" );
		uint64_t c_pos=0;
		for(c_pos=0;c_pos<video_buffer;c_pos+=4){
			fwrite((uint8_t*)(color_base_addr)+c_pos+1, 3, 1, fpA);
		}
		fclose(fpA);
		if(exist((char*)"/dev_usb000")){
			sprintf(string1, "/dev_usb000/%s", video_mem+10);
			file_copy(video_mem, string1, 0);
			remove(video_mem);
			sprintf(video_mem, "%s", string1);
		}
		else
		if(exist((char*)"/dev_usb001")){
			sprintf(string1, "/dev_usb001/%s", video_mem+10);
			file_copy(video_mem, string1, 0);
			remove(video_mem);
			sprintf(video_mem, "%s", string1);
		}
		sprintf(string1, "%s:\n\n[%s]", STR_SCRSHTSAVED, video_mem);
		dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, string1, dialog_fun2, (void*)0x0000aaab, NULL );	wait_dialog();
	}

	// screensaver
	if (( (old_pad & BUTTON_L2) && (old_pad & BUTTON_R2)) || (ss_timer>=(ss_timeout*60) && ss_timeout)) screen_saver();
	if(ss_timer>=(sao_timeout*3600) && sao_timeout) shutdown_system(0);

	if ( (new_pad & BUTTON_R3))
	{
		new_pad=0;//new_pad=0;
		user_font++; if (user_font>5) user_font=0;
		//redraw_column_texts(xmb0_icon);
		goto force_reload;
	}

	if ( (new_pad & BUTTON_L3))
	{
		new_pad=0;//new_pad=0;
		show_temp++; if (show_temp>3) show_temp=0;
		seconds_clock=0;
		goto force_reload;
	}

	if(read_pad_info()) {cellSysutilCheckCallback(); sys_timer_usleep(3336); goto force_reload;}
	xmb0_icon=xmb_icon;

	int xRegID = 0;

	if ( (new_pad & BUTTON_CROSS) &&
		(xmb[xmb_icon].member[xmb[xmb_icon].first].type==6 || xmb[xmb_icon].member[xmb[xmb_icon].first].type==7))
	{
		while(xmb_slide || xmb_slide_y){draw_whole_xmb(1);}

		int ret_f=-1;

		if(xmb_icon==1) // Home/System column functions
		{
			if(xmb[xmb_icon].first==0) // system information
			{
				is_any_xmb_column=xmb_icon;
				show_sysinfo();
				is_any_xmb_column=0;
				goto xmb_cancel_option;
			}

			if(xmb[xmb_icon].first==1) {unload_modules(); exit_app();}	// quit to XMB
			//if(xmb[xmb_icon].first==2) {shutdown_system(0x11); }		// restart PS3
			//if(xmb[xmb_icon].first==3) {boot_otherOS(); }				// boot OtherOS
			if(xmb[xmb_icon].first==5) {shutdown_system(0x10);}			// shutdown PS3
		}
		if(xmb_icon==4) // DEX/CEX
		{
			if(rex_compatible==1)
			{
			if(xmb[xmb_icon].first==0) swap_kernel();
			if(xmb[xmb_icon].first==1) write_to_device();
			}
			//if(xmb[xmb_icon].first==2) dump_flash();
		}

		if(xmb_icon==5) // Utilities
		{
			if(xmb[xmb_icon].first==3) {add_utilities();}

			int n = (c_firmware==3.55f) ? 5 : 4;
			xRegID = n + 0;
			if(xmb[xmb_icon].first==n+1) {resize_vflash();}
			if(xmb[xmb_icon].first==n+2) {install_petiboot();} //lv2
			if(xmb[xmb_icon].first==n+3) {set_gameos_flag();} //lv2
			if(xmb[xmb_icon].first==n+4) {create_packages();}
			if(xmb[xmb_icon].first==n+5) {export_lv(1);} //lv1
			if(xmb[xmb_icon].first==n+6) {export_lv(0);} //lv2
			if(xmb[xmb_icon].first==n+7) {dump_flash();}
            if((!dex_mode && (c_firmware==3.55f || c_firmware==4.46f || c_firmware==4.65f || c_firmware==4.66f) ) || c_firmware==4.21f  || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f)
			{
			  if(xmb[xmb_icon].first==n+8) {dump_root_key();}
			}
		}

		if((xmb[xmb_icon].member[xmb[xmb_icon].first].option_size)) // || xmb[2].first<3 //settings
		{
/*			if(xmb[2].first==0)
			{
				goto xmb_cancel_option;
			}
*/

			opt_list_max=0;
			for(int n=0; n<xmb[xmb_icon].member[xmb[xmb_icon].first].option_size; n++)
			{
				sprintf(opt_list[opt_list_max].label, "-");	sprintf(opt_list[opt_list_max].value, "-"); opt_list_max++;
				opt_list[n].color=0;
			}


			for(int n=0; n<xmb[xmb_icon].member[xmb[xmb_icon].first].option_size; n++)
			{
				sprintf(opt_list[n].label, " %s", xmb[xmb_icon].member[xmb[xmb_icon].first].option[n].label);
				sprintf(opt_list[n].value, "%s", "---");
			}

			ret_f=open_side_menu( (opt_list_max>12?300:500), xmb[xmb_icon].member[xmb[xmb_icon].first].option_selected);
			//MM_LOG("Side menu result: %i\n", ret_f);
			//MM_LOG("Side menu result: %s\n", xmb[xmb_icon].member[xmb[xmb_icon].first].optionini);
			if(ret_f<0 || xmb[xmb_icon].member[xmb[xmb_icon].first].option_selected==ret_f) goto xmb_cancel_option;

			xmb[xmb_icon].member[xmb[xmb_icon].first].option_selected=ret_f;
			parse_settings();

			apply_settings(xmb[xmb_icon].member[xmb[xmb_icon].first].optionini, ret_f, 0);
			if(xRegID) xmb[xmb_icon].member[xRegID].option_selected=0;

			/*if(!strcmp(xmb[2].member[xmb[2].first].optionini, "mount_dev_blind"))
			{
				if(mount_dev_blind) mount_dev_flash(); else unmount_dev_flash();
			}*/

			redraw_column_texts(xmb_icon);

            xmb_cancel_option:
			new_pad=0;
			ss_timer=0;
			ss_timer_last=time(NULL);
			xmb_bg_counter=XMB_BG_COUNTER;
			dialog_ret=0;
		}
	}

	if ( ( ( (new_pad & BUTTON_CIRCLE) && key_repeat) || (new_pad & BUTTON_RED) ))
	{
		new_pad=0;
		c_opacity_delta=16;	dimc=0; dim=1;
		quit_app();
	}


		if(dim==1) c_opacity+=c_opacity_delta;
		if(c_opacity<0x20) c_opacity=0x20;
		if(c_opacity>0xff) c_opacity=0xff;

		c_opacity2=c_opacity;
		if(c_opacity2>0xc0) c_opacity2=0xc0;
		if(c_opacity2<0x21) c_opacity2=0x00;

		draw_whole_xmb(0);
		cellDbgFontDrawGcm();
		flip();
	} //end of main loop

	ret = unload_modules();
	exit_app();
	return 0;
} //end of main


void flip(void)
{
	if(frame_index)
	{
		cellSysutilCheckCallback();
		cellConsolePoll();
	}

	cellDbgFontPrintf( 0.99f, 0.98f, 0.5f,0x10101010, payloadT);
	cellDbgFontDrawGcm();

	static int first=1;
	if (first!=1) {
		while (cellGcmGetFlipStatus()!=CELL_GCM_DISPLAY_FLIP_STATUS_DONE){
			sys_timer_usleep(300);
		}
	}
	cellGcmResetFlipStatus();
	first=0;

	cellGcmSetFlip(gCellGcmCurrentContext, frame_index);
	cellGcmFlush(gCellGcmCurrentContext);

	cellGcmSetWaitFlip(gCellGcmCurrentContext);
	cellGcmFlush(gCellGcmCurrentContext);

	frame_index^=1;

	setDrawEnv();
	setRenderColor();

	vert_indx=0; // reset the vertex index
	vert_texture_indx=0;

	angle+=3.6f;
	if(angle>=360.f) angle=0.f;
}

/*
void change_bri(u8 *buffer, int delta, u32 size)
{
	u32 pixel;
	u32 deltaR;
	u32 deltaG;
	u32 deltaB;
	float delta2=1.0f;		//positive increases brightness
	if(delta<0) delta2=0.f; //negative decreases brightness
	delta=abs(delta);		//size=+/-%
	for(u32 fsr=0; fsr<size; fsr+=4)
	{
		pixel=*(uint32_t*) ((uint8_t*)(buffer)+fsr);
		deltaR = ((u32)((float)((pixel>>24)&0xff)*(delta2+(float)delta/100.f)));if(deltaR>0xff) deltaR=0xff;
		deltaG = ((u32)((float)((pixel>>16)&0xff)*(delta2+(float)delta/100.f)));if(deltaG>0xff) deltaG=0xff;
		deltaB = ((u32)((float)((pixel>> 8)&0xff)*(delta2+(float)delta/100.f)));if(deltaB>0xff) deltaB=0xff;
		*(uint32_t*) ((uint8_t*)(buffer)+fsr)= deltaR<<24 | deltaG<<16 | deltaB<<8 | pixel&0xff;
	}
}


static void png_thread_entry( uint64_t arg )
{
	(void)arg;
	is_decoding_png=1;
	if(init_finished)
	{
		int _xmb_icon=arg&0xf;
		int cn=arg>>8;
		if(xmb[_xmb_icon].member[cn].status==1)
		{
			load_png_texture_th( xmb[_xmb_icon].member[cn].icon, xmb[_xmb_icon].member[cn].icon_path);//, _xmb_icon==8?408:xmb[_xmb_icon].member[cn].iconw);
			if(png_w_th && png_h_th && ((png_w_th+png_h_th)<=(XMB_THUMB_WIDTH+XMB_THUMB_HEIGHT)) && (xmb[_xmb_icon].member[cn].status==1))
			{
				xmb[_xmb_icon].member[cn].iconw=png_w_th;
				xmb[_xmb_icon].member[cn].iconh=png_h_th;
				xmb[_xmb_icon].member[cn].status=2;
				if(_xmb_icon==7) put_texture_with_alpha_gen( xmb[_xmb_icon].member[cn].icon, xmb_icon_star_small, 32, 32, 32, 320, 283, 5);
				else if((_xmb_icon==6 || _xmb_icon==5 || _xmb_icon==8) && png_w_th>64 && png_h_th>64 && (strstr((xmb[_xmb_icon].member[cn].file_path), "/app_home"))) put_texture_with_alpha_gen( xmb[_xmb_icon].member[cn].icon, xmb_icon_net_small, 64, 64, 64, png_w_th, png_w_th-64, 0);
			}
			else
			{
				xmb[_xmb_icon].member[cn].status=2;
				if(in_xmb_mode())
				{
					//if(_xmb_icon==6) xmb[_xmb_icon].member[cn].icon=xmb[6].data;
					//else if(_xmb_icon==7) xmb[_xmb_icon].member[cn].icon=xmb[7].data;
					//else
						if(_xmb_icon==5) xmb[_xmb_icon].member[cn].icon=xmb_icon_film;
					else if(_xmb_icon==3) xmb[_xmb_icon].member[cn].icon=xmb_icon_photo;
					else if(_xmb_icon==8) xmb[_xmb_icon].member[cn].icon=xmb_icon_retro;
					else if(_xmb_icon==0) xmb[_xmb_icon].member[cn].icon=xmb_icon_star;
					else xmb[_xmb_icon].member[cn].icon=xmb_icon_help;
				}
				else
				{
					if(xmb[_xmb_icon].member[cn].type==2) xmb[_xmb_icon].member[cn].icon=xmb_icon_film;
					//else xmb[_xmb_icon].member[cn].icon=xmb[6].data;
					else xmb[_xmb_icon].member[cn].icon=xmb_icon_help;
				}
			}
		}
	}
	is_decoding_png=0;
	sys_ppu_thread_exit(0);
}
*/

/*
static void jpg_thread_entry( uint64_t arg )
{
	(void)arg;
	is_decoding_jpg=1;
	if(init_finished)
	{
		int _xmb_icon=arg&0xf;
		int cn=arg>>8;
		if(cn<xmb[_xmb_icon].size)
		if(xmb[_xmb_icon].member[cn].status==1)
		{
			scale_icon_h=176;
			load_jpg_texture_th( xmb[_xmb_icon].member[cn].icon, xmb[_xmb_icon].member[cn].icon_path, xmb[_xmb_icon].member[cn].iconw);
			if(jpg_w && jpg_h && ((jpg_w+jpg_h)<=(XMB_THUMB_WIDTH+XMB_THUMB_HEIGHT)) && (xmb[_xmb_icon].member[cn].status==1))
			{
				xmb[_xmb_icon].member[cn].iconw=jpg_w;
				xmb[_xmb_icon].member[cn].iconh=jpg_h;
				xmb[_xmb_icon].member[cn].status=2;
			}
			else
			{
				xmb[_xmb_icon].member[cn].status=2;
				if(_xmb_icon==5) xmb[_xmb_icon].member[cn].icon=xmb_icon_film;
				else if(_xmb_icon==3) xmb[_xmb_icon].member[cn].icon=xmb_icon_photo;
				else if(_xmb_icon==4) xmb[_xmb_icon].member[cn].icon=xmb_icon_note;
				else if(_xmb_icon==5) xmb[_xmb_icon].member[cn].icon=xmb_icon_film;
				else if(_xmb_icon==8) xmb[_xmb_icon].member[cn].icon=xmb_icon_retro;
				else if(_xmb_icon==0) xmb[_xmb_icon].member[cn].icon=xmb_icon_star;
				else xmb[_xmb_icon].member[cn].icon=xmb_icon_help;
			}
		}
	}
	scale_icon_h=0;
	is_decoding_jpg=0;
	sys_ppu_thread_exit(0);
}

void load_jpg_threaded(int _xmb_icon, int cn)
{
	if(is_decoding_jpg) return;
	is_decoding_jpg=1;
	sys_ppu_thread_create( &jpgdec_thr_id, jpg_thread_entry,
						   (_xmb_icon | (cn<<8) ),
						   misc_thr_prio-100, app_stack_size,
						   0, "jpeg" );
}


void load_png_threaded(int _xmb_icon, int cn)
{
	if(is_decoding_png) return;
	is_decoding_png=1;
	sys_ppu_thread_create( &pngdec_thr_id, png_thread_entry,
						   (_xmb_icon | (cn<<8) ),
						   misc_thr_prio-100, app_stack_size,//misc_thr_prio
						   0, "png" );
}
*/

#ifdef USE_DEBUG

#define PC_DEVELOPMENT_IP_ADDRESS "10.20.2.209"

int sock=-1;
struct sockaddr_in target;

void logger_init (void)
{
	sock=socket(AF_INET, SOCK_DGRAM, 0);
	target.sin_family = AF_INET;
	target.sin_port = htons(3490);
	inet_pton(AF_INET, PC_DEVELOPMENT_IP_ADDRESS, &target.sin_addr);
}

void net_send(const char *__format,...)
{
	if(sock<0)
	{
		logger_init();
		if(sock<0) return;
	}

	char sendbuf[4096];

	va_list args;

	va_start(args, __format);
	vsnprintf(sendbuf, 4000, __format, args);
	va_end(args);

	int len = strlen(sendbuf);
	sendto(sock, sendbuf, len, MSG_DONTWAIT, (const struct sockaddr*)&target, sizeof(target));
}

#endif

// ===========================================
void set_xReg()
{
	if(util_xReg==0)
		return;

	if(util_xReg==1)
       {
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_yes_no, STR_SAVXREG, dialog_fun1, (void*)0x0000aaaa, NULL );
		wait_dialog_simple();
		if(dialog_ret!=1) return;

		if(exist((char*)"/dev_flash2/etc/xRegistry.sys"))
		{
			dialog_ret=0;
			if(exist((char*)"/dev_usb000"))
			{
				cellFsUnlink("/dev_usb000/xRegistry.sys");
				file_copy((char*)"/dev_flash2/etc/xRegistry.sys",(char*)"/dev_usb000/xRegistry.sys",0);

				if(exist((char*)"/dev_usb000/xRegistry.sys"))
					cellMsgDialogOpen2( type_dialog_ok, STR_SAVXREGOK, dialog_fun1, (void*)0x0000aaaa, NULL );
			    	else
					cellMsgDialogOpen2( type_dialog_ok, STR_SAVXREGERR1, dialog_fun1, (void*)0x0000aaaa, NULL );
		 	}
		 	else
	     	cellMsgDialogOpen2( type_dialog_ok, STR_SAVXREGERR2, dialog_fun1, (void*)0x0000aaaa, NULL );
		 	wait_dialog_simple();
     	}
	        else
 	        {
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_SAVXREGERR3, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
	        }
      }


		if (util_xReg==2)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_yes_no, STR_RESXREG, dialog_fun1, (void*)0x0000aaaa, NULL );
		wait_dialog_simple();
		if(dialog_ret!=1) return;

		if(exist((char*)"/dev_usb000/xRegistry.sys"))
		{
			cellFsUnlink("/dev_flash2/etc/xRegistry.sys.bak");
			rename("/dev_flash2/etc/xRegistry.sys", "/dev_flash2/etc/xRegistry.sys.bak");
			cellFsUnlink("/dev_flash2/etc/xRegistry.sys");
			file_copy((char*)"/dev_usb000/xRegistry.sys",(char*)"/dev_flash2/etc/xRegistry.sys",0);

			dialog_ret=0;
			if(exist((char*)"/dev_flash2/etc/xRegistry.sys"))
				cellMsgDialogOpen2( type_dialog_ok, STR_RESXREGOK, dialog_fun1, (void*)0x0000aaaa, NULL );
			else
			{
				rename("/dev_flash2/etc/xRegistry.sys.bak", "/dev_flash2/etc/xRegistry.sys");
				cellMsgDialogOpen2( type_dialog_ok, STR_RESXREGERR1, dialog_fun1, (void*)0x0000aaaa, NULL );
			}

			wait_dialog_simple();
   	 	}
		else
		{
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_RESXREGERR2, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
		}

	}
		else
		return;
}


void create_packages()
{
	mkdir((char*)"/dev_hdd0/packages", S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
	dialog_ret=0;
	if(exist((char*)"/dev_hdd0/packages"))
		cellMsgDialogOpen2( type_dialog_ok, STR_MAKPKGOK, dialog_fun2, (void*)0x0000aaab, NULL );
	else
		cellMsgDialogOpen2( type_dialog_ok, STR_MAKPKGERR , dialog_fun2, (void*)0x0000aaab, NULL );
	wait_dialog_simple();
}

void load_lv2_kernel(u8 _sel)
{
	initAIO();
	if(lv2_kernels==0 || _sel==0 || _sel>=lv2_kernels)
	{
		lv2_kernel=0;
		return;
	}

	char path[512];
	char msg[512];
	sprintf(path, lv2_kernel_files[_sel].path);
	if(is_size(path)<1024 | !exist(path))
	{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, STR_LOADLV2KERR1, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();
		lv2_kernel=0;
		return;
	}
	sprintf(msg, "%s\n\nKernel selected: %s\nKernel path: %s", STR_LOADLV2K ,lv2_kernel_files[_sel].name, path);
	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_yes_no, msg, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog_simple();
	if(dialog_ret!=1) {lv2_kernel=0; return;}

	if(strcmp(path, "/dev_flash/lv2_kernel.self"))
	{
		remove((char*)"/dev_rebug/lv2_kernel.self");

		if(get_free_drive_space((char*)"/dev_flash")<(is_size(path)+2048))
		{
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_LOADLV2KNOSPACE, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			lv2_kernel=0;
			return;
		}

		// copy [path] to /dev_flash/lv2_kernel.self and patch LV1

		open_please_wait();

		initAIO();
		file_copy(path, (char*)"/dev_rebug/lv2_kernel.self", 0);
		cellMsgDialogAbort();
	}

	if(exist((char*)"/dev_flash/lv2_kernel.self"))
	{
		u64 lv2_offset=0x9D0C8; //4.21
		if(c_firmware==3.55f)
			lv2_offset=0x16BE60;

		if(peek_lv1_cobra(lv2_offset)!=0x2F666C682F6F732FULL) //do some search
		{
			cellMsgDialogAbort();
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_no, STR_LOADLV2KCHKLV1, dialog_fun2, (void*)0x0000aaab, NULL );
			flipc(120);
			dialog_ret=0;

			lv2_offset=0;
			for(u64 addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
			{
				lv2_offset=addr;
				if(peek_lv1_cobra(addr) == 0x2F6F732F6C76325FULL) // /os/lv2_
				{
					lv2_offset=addr-4;
					break;
				}
			}

			cellMsgDialogAbort();

		}

		if(lv2_offset && peek_lv1_cobra(lv2_offset)==0x2F666C682F6F732FULL)
		{
			poke_lv1(lv2_offset +  0, 0x2F6C6F63616C5F73ULL);
			poke_lv1(lv2_offset +  8, 0x7973302F6C76325FULL);
			poke_lv1(lv2_offset + 16, 0x6B65726E656C2E73ULL);
			poke_lv1(lv2_offset + 24, 0x656C660000000000ULL);
		}
		lv2_sm_shutdown(0x8201, NULL, 0);
	}

	cellMsgDialogAbort();
	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_ok, STR_LOADLV2KERR2, dialog_fun2, (void*)0x0000aaab, NULL );
	wait_dialog_simple();
	lv2_kernel=0;
}

void reboot_ps3(u8 _mode)
{
	if(_mode==1)
		lv2_sm_shutdown(0x8201, NULL, 0);
	else
	if(_mode==2)
		{system_call_4(379,0x1200,0,0,0);}
}

void boot_otherOS(u8 _mode)
{
	if(!_mode) return;

	dialog_ret=0;
	if(_mode==1)
		cellMsgDialogOpen2( type_dialog_yes_no, STR_BOOTOOSMSG1, dialog_fun1, (void*)0x0000aaaa, NULL );
	else
		cellMsgDialogOpen2( type_dialog_yes_no, STR_BOOTOOSMSG2, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog_simple();
	if(dialog_ret!=1) {otheros=0; return;}

	uint32_t dev_handle;
	int start_sector, sector_count;
	struct storage_device_info info;
	uint8_t buf[VFLASH5_SECTOR_SIZE * 16];
	struct os_area_header *hdr;
	struct os_area_params *params;
	uint32_t unknown2;
	int result;

	dev_handle = 0;

	result = lv2_storage_get_device_info(VFLASH5_DEV_ID, &info);
	if (result) {
		goto done;
	}

	if (info.capacity < VFLASH5_HEADER_SECTORS) {
		goto done;
	}

	result = lv2_storage_open(VFLASH5_DEV_ID, &dev_handle);
	if (result) {
		goto done;
	}

	/* write os header and params */

	start_sector = 0;
	sector_count = VFLASH5_HEADER_SECTORS;

	memset(buf, 0, sizeof(buf));
	hdr = (struct os_area_header *) buf;
	params = (struct os_area_params *) (buf + OS_AREA_SEGMENT_SIZE);

	result = lv2_storage_read(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
	if (result) {
		goto done;
	}

	if (strncmp((const char *) hdr->magic, HEADER_MAGIC, sizeof(hdr->magic))) {
		goto done;
	}

	if (hdr->version != HEADER_VERSION) {
		goto done;
	}

	if (params->boot_flag == PARAM_BOOT_FLAG_GAME_OS) {
		params->boot_flag = PARAM_BOOT_FLAG_OTHER_OS;

		result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
		if (result) {
			goto done;
		}
	}
	result = lv2_storage_close(dev_handle);

	if(_mode==1)
		apply_settings((char*)"none", 1, 1); // apply all LV1 patches

	result = lv2_sm_shutdown(0x8201, NULL, 0);
	if (result) {
		goto done_reboot;
	}

done:

	result = lv2_storage_close(dev_handle);

done_reboot:

	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_ok, STR_BOOTOOSERR1, dialog_fun2, (void*)0x0000aaab, NULL );
	wait_dialog_simple();
}

void swap_kernel()
{
	get_eid();
	if(dex_flash)
	{
		if(!dex_mode) search_flash(0, 1);
		else search_flash(1, 1);
	}
	else
	{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, STR_SWPKERR, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();
	}
}

void dump_flash()
{
	dialog_ret=0;
	cellMsgDialogOpen2( type_dialog_yes_no, STR_DMPFLSH, dialog_fun1, (void*)0x0000aaaa, NULL );
	wait_dialog();
	if(dialog_ret==1)
	{
//	Export flash

		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		char lv2file[512];
		char string1[512];

		if(is_nor())
			sprintf(lv2file, "/dev_usb000/%04d%02d%02d-%02d%02d%02d-FLASH-NOR-FW%1.2f.NORBIN", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
		else
			sprintf(lv2file, "/dev_usb000/%04d%02d%02d-%02d%02d%02d-FLASH-NAND-FW%1.2f.NANDBIN", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
		remove(lv2file);

		if(get_free_drive_space((char*)"/dev_usb000")<0x10000000ULL)//256MB
		{
			cellMsgDialogAbort();
			sprintf(string1, STR_DMPFLSHERR);
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, string1, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog();
		}
		else
		{
			if(is_nor())
				read_flash(FLASH_DEVICE_NOR, FLASH_FLAGS, lv2file);
			else
			{
/*			uint64_t tmp1;
			uint64_t tmp2;
			uint64_t poke_restore;
			for(uint64_t i=0x270000ULL;i<0x300000ULL;i+=4)
			{
			tmp1=peek_lv1_cobra(i);
			tmp2=peek_lv1_cobra(i+8);
			if((tmp1==0x39840200F8010090ULL) && (tmp2==0xFBC100707D3E4B78ULL))
			{
			poke_restore=tmp1;
			poke_lv1(tmp1, 0x39840000f8010090ull);
			break;
			}
			}*/
				read_flash(FLASH_DEVICE_NAND, FLASH_FLAGS, lv2file);
//				poke_lv1(poke_restore, 0x39840200F8010090ULL);
			}

			cellMsgDialogAbort();
			cellFsChmod(lv2file, 0666);
		}
	}
}

int readmem(unsigned char *_x, uint64_t _fsiz, uint64_t _chunk, u8 mode) //read lv1/lv2 memory chunk
{

	uint64_t n, m;
	uint64_t val;

    for(n = 0; n < _chunk; n += 8) {
		 if((_fsiz + n)>0x7ffff8ULL && mode==2) return (int)(n-8);
		 if(mode==2)
	        val = peekq(0x8000000000000000ULL + _fsiz + n);

		else
		{
			val = peek_lv1_cobra(0x0000000000000000ULL + _fsiz + n);
		}
		 for(m = 0; m<8; m++) {
			 _x[n+7-m] = (unsigned char) ((val >> (m*8)) & 0x00000000000000ffULL);
		 }
	}

	return _chunk;

}

void export_lv(u8 _mode)
{
	dialog_ret=0;
	if(!_mode)
		cellMsgDialogOpen2( type_dialog_yes_no, STR_EXPLVMSG1, dialog_fun1, (void*)0x0000aaaa, NULL );
	else
		cellMsgDialogOpen2( type_dialog_yes_no, STR_EXPLVMSG2, dialog_fun1, (void*)0x0000aaaa, NULL );

	wait_dialog();
	if(dialog_ret==1)
	{
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		char lv2file[512];
		char string1[512];
		char _dev[64];
		u64 readb=0;
		FILE *fpA;
		sprintf(_dev, "/dev_hdd0");
		if(exist((char*)"/dev_usb000")) sprintf(_dev, "/dev_usb000");
		else if(exist((char*)"/dev_usb001")) sprintf(_dev, "/dev_usb001");
		else if(exist((char*)"/dev_usb006")) sprintf(_dev, "/dev_usb006");

		if(!_mode)
		{
			if(dex_mode==1)
				sprintf(lv2file, "%s/%04d%02d%02d-%02d%02d%02d-LV2-FW%1.2f.BIN", app_usrdir, timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
			else
				sprintf(lv2file, "%s/%04d%02d%02d-%02d%02d%02d-LV2-FW%1.2f.BIN", _dev, timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
			sprintf(string1, "%s:\n\n%s\n\n%s", STR_EXPLVMSG5, lv2file, STR_EXPLVWAIT );
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_no, string1, dialog_fun2, (void*)0x0000aaab, NULL );
			flipc(60);

			remove(lv2file);
			fpA = fopen ( lv2file, "wb" );
			if(fpA)
			{
				readb=readmem((unsigned char *) text_FONT, 0, 0x800000, 2);
				fwrite(text_FONT, readb, 1, fpA);
				fclose(fpA);
				cellFsChmod(lv2file, 0666);
				sprintf(string1, "%s:\n\n%s", STR_EXPLVOK, lv2file);
			}
			else
				sprintf(string1, "%s: %s", STR_EXPLVERR, lv2file);
			cellMsgDialogAbort();
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, string1, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog();
		}
		else
		{
			if(dex_mode==1)
				sprintf(lv2file, "%s/%04d%02d%02d-%02d%02d%02d-LV1-FW%1.2f.BIN", app_usrdir, timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
			else
				sprintf(lv2file, "%s/%04d%02d%02d-%02d%02d%02d-LV1-FW%1.2f.BIN", _dev, timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, c_firmware);
			remove(lv2file);
			sprintf(string1, "%s:\n\n%s\n\n%s", STR_EXPLVMSG3, lv2file, STR_EXPLVMSG4);
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_no, string1, dialog_fun2, (void*)0x0000aaab, NULL );
			flipc(60);

			fpA = fopen ( lv2file, "wb" );
			if(fpA)
			{
				readb=readmem((unsigned char *) text_FONT, 0, 0x800000, 1);
				fwrite(text_FONT, readb, 1, fpA);
				readb=readmem((unsigned char *) text_FONT, 0x800000, 0x800000, 1);
				fwrite(text_FONT, readb, 1, fpA);
				fclose(fpA);
				cellFsChmod(lv2file, 0666);
				sprintf(string1, "%s:\n\n%s", STR_EXPLVOK2, lv2file);
			}
			else
				sprintf(string1, "%s: %s", STR_EXPLVERR2, lv2file);
			cellMsgDialogAbort();
			dialog_ret=0; cellMsgDialogOpen2( type_dialog_ok, string1, dialog_fun2, (void*)0x0000aaab, NULL ); wait_dialog();
		}
		load_texture(text_FMS, xmbicons, 128);
	}
}

void check_settings()
{
	u8 cid=get_idps(0);// 0 = EID0, 5 = EID5
	if(c_firmware==3.55f)
	{
		if(  peek_lv1_cobra(0x3025EC       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0; // yes
		if( (peek_lv1_cobra(0x2AE200) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0; // yes
		if( (peek_lv1_cobra(0x2D5EB0) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0; // yes
		if( (peek_lv1_cobra(0x0AC574) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0; // Yes
		if( (peek_lv1_cobra(0x0FDBB4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0; //yes
		if( (peek_lv1_cobra(0x16F460) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0; //yes

		if( (peek_lv1_cobra(0x273494) >> 32) == 0x3920005FULL)			lv1_enc=1;	else lv1_enc=0; //yes
		if( (peek_lv1_cobra(0x0FB1CC) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0; // yes
		if( (peek_lv1_cobra(0x2DDCF4) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0; //yes
		if( (peek_lv1_cobra(0x2F2678) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0; //yes
		if( (peek_lv1_cobra(0x2E3CF4) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0; //yes

		if( (peek_lv1_cobra(0x214990) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0; //yes
		if( (peek_lv1_cobra(0x7125AC) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0; //yes
		if( (peek_lv1_cobra(0x25B340) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0; //yes

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		/* NOR */
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x11B4D8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x1524D8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x1264DC) >> 32) == 0x38600001ULL) ) lv1_go=1;

		/* NAND */
		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x0A34D8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(!is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x76A4D8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x11E4DC) >> 32) == 0x38600001ULL) ) lv1_go=1;
		/* NOR , NAND */
		if((cid==0x82) &&((peek_lv1_cobra(0x11D4DC) >> 32) == 0x38600001ULL) ) lv1_go=1;

	}
	else if(c_firmware==4.21f)
	{
		if(  peek_lv1_cobra(0x309984  +  24) == 0xE8830018F8A40000ULL)	lv1_pp=1;	else lv1_pp=0;
		if( (peek_lv1_cobra(0x2B3F6C) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;
		if( (peek_lv1_cobra(0x2DD244) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0;
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0;
		if( (peek_lv1_cobra(0x0FEB88) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;
		if( (peek_lv1_cobra(0x16F800) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)			lv1_enc=1;	else lv1_enc=0;
		if( (peek_lv1_cobra(0x0FBE00) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;
		if( (peek_lv1_cobra(0x2E5088) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0;
		if( (peek_lv1_cobra(0x2F99F0) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;
		if( (peek_lv1_cobra(0x2EB088) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0;

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0;
		if( (peek_lv1_cobra(0x71422C) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0;
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		/* NOR, CEX , DEX */
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x118370) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1; //4.21 CEX
		if(is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x11F370) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1; //4.21 DEX

		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x122F34) >> 32) == 0x38600001ULL) ) lv1_go=1; //4.21 CEX
		if(is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x119F34) >> 32) == 0x38600001ULL) ) lv1_go=1; //4.21 DEX
		/* NAND, CEX , DEX */
		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x0A4370) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1; //4.21 CEX
		if(!is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x0A3370) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1; //4.21 DEX

		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x11EF34) >> 32) == 0x38600001ULL) ) lv1_go=1; //4.21 CEX
		if(!is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0x11DF34) >> 32) == 0x38600001ULL) ) lv1_go=1; //4.21 DEX
	}
	else if(c_firmware==4.30f) // Fixed
	{
		if(  peek_lv1_cobra(0x3099C4       ) == 0xE8830018E8840000ULL)    lv1_pp=1; else lv1_pp=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2B3FAC) >> 32) == 0x60000000ULL)            lv1_lv2=1; else lv1_lv2=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD284) >> 32) == 0x60000000ULL)            lv1_htab=1; else lv1_htab=0; // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)            lv1_indi=1; else lv1_indi=0; // Fixed
		if( (peek_lv1_cobra(0x0FEB98) >> 32) == 0x38000000ULL)            lv1_um=1; else lv1_um=0;   // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)            lv1_dm=1; else lv1_dm=0;   // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)            lv1_enc=1; else lv1_enc=0;  // Fixed
		if( (peek_lv1_cobra(0x0FBE10) >> 32) == 0x60000000ULL)            lv1_pkg=1; else lv1_pkg=0;  // Fixed
		if( (peek_lv1_cobra(0x2E50C8) >> 32) == 0xE81E0020ULL)            lv1_lpar=1; else lv1_lpar=0; // Fixed
		if( (peek_lv1_cobra(0x2F9A30) >> 32) == 0x3920FFFFULL)            lv1_spe=1; else lv1_spe=0;  // Fixed
		if( (peek_lv1_cobra(0x2EB0C8) >> 32) == 0x3800000FULL)            lv1_dabr=1; else lv1_dabr=0; // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)            lv1_gart=1; else lv1_gart=0; // Fixed
		if( (peek_lv1_cobra(0x714ACC) >> 32) == 0x60000000ULL)            lv1_keys=1; else lv1_keys=0; // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)            lv1_acl=1; else lv1_acl=0;  //

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		/* NOR , CEX*/
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x1253B0) >> 32) == 0x6400FFFFULL) )            lv1_smgo=1;
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x0A3F74) >> 32) == 0x38600001ULL) )           lv1_go=1;

		/* NAND , CEX */
		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x1613B0) >> 32) == 0x6400FFFFULL) )            lv1_smgo=1;
		if(!is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x76FF74) >> 32) == 0x38600001ULL) )           lv1_go=1;

		/* NAND , NOR , DEX */
		if( (cid==0x82) &&((peek_lv1_cobra(0x11C3B0) >> 32) == 0x6400FFFFULL) )            lv1_smgo=1;	 // 4.30 DEX
		if( (cid==0x82) &&((peek_lv1_cobra(0x0F2F74) >> 32) == 0x38600001ULL) )           lv1_go=1; // 4.30 DEX
	}
	else if(c_firmware==4.31f) // MLT 4.31 CFW supported
	{
		if(  peek_lv1_cobra(0x3099C4       ) == 0xE8830018E8840000ULL)    lv1_pp=1; else lv1_pp=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2B3FAC) >> 32) == 0x60000000ULL)            lv1_lv2=1; else lv1_lv2=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD284) >> 32) == 0x60000000ULL)            lv1_htab=1; else lv1_htab=0; // Fixed   IDA
		if( (peek_lv1_cobra(0x0AD594) >> 32) == 0x38600000ULL)            lv1_indi=1; else lv1_indi=0; // Fixed
		if( (peek_lv1_cobra(0x0FFB98) >> 32) == 0x38000000ULL)            lv1_um=1; else lv1_um=0;   // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)            lv1_dm=1; else lv1_dm=0;   // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)            lv1_enc=1; else lv1_enc=0;  // Fixed
		if( is_nor() && (peek_lv1_cobra(0x11C3B0) >> 32) == 0x6400FFFFULL)            lv1_smgo=1; else lv1_smgo=0; // Fixed
		if( (peek_lv1_cobra(0x0FCE10) >> 32) == 0x60000000ULL)            lv1_pkg=1; else lv1_pkg=0;  // Fixed
		if( (peek_lv1_cobra(0x2E50C8) >> 32) == 0xE81E0020ULL)            lv1_lpar=1; else lv1_lpar=0; // Fixed
		if( (peek_lv1_cobra(0x2F9A30) >> 32) == 0x3920FFFFULL)            lv1_spe=1; else lv1_spe=0;  // Fixed
		if( (peek_lv1_cobra(0x2EB0C8) >> 32) == 0x3800000FULL)            lv1_dabr=1; else lv1_dabr=0; // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)            lv1_gart=1; else lv1_gart=0; // Fixed
		if( (peek_lv1_cobra(0x715ACC) >> 32) == 0x60000000ULL)            lv1_keys=1; else lv1_keys=0; // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)            lv1_acl=1; else lv1_acl=0;  //
		if( is_nor() && (peek_lv1_cobra(0x126F74) >> 32) == 0x38600001ULL)            lv1_go=1; else lv1_go=0;   //
	}
	else if(c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f) // Fixed
	{
		if(  peek_lv1_cobra(0x3099C4       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2B3FAC) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD284) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0; // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0; // Fixed
		if( (peek_lv1_cobra(0x0FEB8C) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;   // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;   // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)			lv1_enc=1;	else lv1_enc=0;  // Fixed
		if( (peek_lv1_cobra(0x0FBE04) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;  // Fixed
		if( (peek_lv1_cobra(0x2E50C8) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0; // Fixed
		if( (peek_lv1_cobra(0x2F9A30) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;  // Fixed
		if( (peek_lv1_cobra(0x2EB0C8) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0; // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0; // Fixed
		if( (peek_lv1_cobra(0x714AB0) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0; // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;  // Fixed

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		/* NAND , NOR */
		if((cid!=0x82) && ((peek_lv1_cobra(0x11D3B0) >> 32) == 0x6400FFFFULL) )			lv1_smgo=1;
		if((cid==0x82) && ((peek_lv1_cobra(0x11EF74) >> 32) == 0x38600001ULL) )			lv1_go=1;
		/* NOR PS3 */
		if((cid==0x82)&& is_nor() && ((peek_lv1_cobra(0x1543B0) >> 32) == 0x6400FFFFULL) )			lv1_smgo=1;
		if((cid!=0x82) && is_nor() && ((peek_lv1_cobra(0x127F74) >> 32) == 0x38600001ULL) )			lv1_go=1;
		/* NAND PS3 */
		if((cid==0x82) && !is_nor() && ((peek_lv1_cobra(0x37C3B0) >> 32) == 0x6400FFFFULL) )			lv1_smgo=1;
		if((cid!=0x82) && !is_nor() && ((peek_lv1_cobra(0x787F74) >> 32) == 0x38600001ULL) )			lv1_go=1;
	}
	else if(c_firmware==4.50f) // Fixed
	{
		if(  peek_lv1_cobra(0x3099C4       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2B3FAC) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD284) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0; // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0; // Fixed
		if( (peek_lv1_cobra(0x0FEBD4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;   // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;   // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)			lv1_enc=1;	else lv1_enc=0;  // Fixed
		if( is_nor() && (peek_lv1_cobra(0x11D4B8) >> 32) == 0x6400FFFFULL)			lv1_smgo=1;	else lv1_smgo=0; // Fixed
		if( (peek_lv1_cobra(0x0FBE24) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;  // Fixed
		if( (peek_lv1_cobra(0x2E50C8) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0; // Fixed
		if( (peek_lv1_cobra(0x2F9A30) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;  // Fixed
		if( (peek_lv1_cobra(0x2EB0C8) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0; // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0; // Fixed
		if( (peek_lv1_cobra(0x714CEC) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0; // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;  // Fixed
		if( is_nor() && (peek_lv1_cobra(0x11807C) >> 32) == 0x38600001ULL)			lv1_go=1;	else lv1_go=0;   // Fixed
	}
	else if(c_firmware==4.53f) // Fixed
	{
		if(  peek_lv1_cobra(0x3099C4       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2B3FAC) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD284) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0; // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0; // Fixed
		if( (peek_lv1_cobra(0x0FEBD4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;   // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;   // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392000DFULL)			lv1_enc=1;	else lv1_enc=0;  // Fixed
		if( (peek_lv1_cobra(0x0FBE24) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;  // Fixed
		if( (peek_lv1_cobra(0x2E50C8) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0; // Fixed
		if( (peek_lv1_cobra(0x2F9A30) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;  // Fixed
		if( (peek_lv1_cobra(0x2EB0C8) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0; // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0; // Fixed
		if( (peek_lv1_cobra(0x714CEC) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0; // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;  // Fixed

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		/* NOR, DEX */
		if(is_nor() && (cid==0x82) &&((peek_lv1_cobra(0x1544B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;

		/* NAND, CEX */
		if(!is_nor() && (cid==0x82) &&((peek_lv1_cobra(0x37C4B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;

        /* NAND , NOR , CEX */
		if((cid!=0x82) &&((peek_lv1_cobra(0x11D4B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if((cid!=0x82) &&((peek_lv1_cobra(0x11807C) >> 32) == 0x38600001ULL) ) lv1_go=1;
		if((cid==0x82) &&((peek_lv1_cobra(0x11F07C) >> 32) == 0x38600001ULL) ) lv1_go=1;
	}
	else if(c_firmware==4.55f) // Fixed
	{
		if(  peek_lv1_cobra(0x309E4C       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;    // Fixed   IDA
		if( (peek_lv1_cobra(0x2b4434) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD70C) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0;  // Fixed
		if( (peek_lv1_cobra(0x0FEBD4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;    // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;    // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392001DFULL)			lv1_enc=1;	else lv1_enc=0;   // Fixed
		if( (peek_lv1_cobra(0x0FBE24) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;   // Fixed
		if( (peek_lv1_cobra(0x2E5550) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0;  // Fixed
		if( (peek_lv1_cobra(0x2F9EB8) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;   // Fixed
		if( (peek_lv1_cobra(0x2EB550) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0;  // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0;  // Fixed
		if( (peek_lv1_cobra(0x714D50) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0;  // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;   // Fixed

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x11D4B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(is_nor() && (cid==0x82) &&((peek_lv1_cobra(0x11C4B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;

		if(is_nor() &&(cid!=0x82) &&((peek_lv1_cobra(0x11807C) >> 32) == 0x38600001ULL) ) lv1_go=1;
		if(is_nor() &&(cid==0x82) &&((peek_lv1_cobra(0xF307C) >> 32) == 0x38600001ULL) ) lv1_go=1;

	}
	else if(c_firmware==4.60f) // Fixed
	{
		if(  peek_lv1_cobra(0x309E4C       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;    // Fixed   IDA
		if( (peek_lv1_cobra(0x2b4434) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD70C) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0;  // Fixed
		if( (peek_lv1_cobra(0x0FEBD4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;    // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;    // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392001DFULL)			lv1_enc=1;	else lv1_enc=0;   // Fixed
		if( (peek_lv1_cobra(0x0FBE24) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;   // Fixed
		if( (peek_lv1_cobra(0x2E50AC) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0;  // Fixed
		if( (peek_lv1_cobra(0x2F9EB8) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;   // Fixed
		if( (peek_lv1_cobra(0x2EB550) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0;  // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0;  // Fixed
		if( (peek_lv1_cobra(0x714D50) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0;  // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;   // Fixed

		/* Value changes depending on kernel type*/
        lv1_smgo = lv1_go = 0;
		if(is_nor() && (cid!=0x82) &&((peek_lv1_cobra(0x11D4CC) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;
		if(is_nor() && (cid!=0x82) &&((peek_lv1_cobra(0x11C4B8) >> 32) == 0x6400FFFFULL) ) lv1_smgo=1;

		if(is_nor() && (cid!=0x82) &&((peek_lv1_cobra(0x118090) >> 32) == 0x38600001ULL) ) lv1_go=1;
		if(is_nor() && (cid!=0x82) &&((peek_lv1_cobra(0xF307C) >> 32) == 0x38600001ULL) ) lv1_go=1;

	}
	else if(c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) // Fixed
	{
		if(  peek_lv1_cobra(0x309E4C       ) == 0xE8830018E8840000ULL)	lv1_pp=1;	else lv1_pp=0;    // Fixed   IDA
		if( (peek_lv1_cobra(0x2b4434) >> 32) == 0x60000000ULL)			lv1_lv2=1;	else lv1_lv2=0;   // Fixed   IDA
		if( (peek_lv1_cobra(0x2DD70C) >> 32) == 0x60000000ULL)			lv1_htab=1; else lv1_htab=0;  // Fixed   IDA
		if( (peek_lv1_cobra(0x0AC594) >> 32) == 0x38600000ULL)			lv1_indi=1; else lv1_indi=0;  // Fixed
		if( (peek_lv1_cobra(0x0FEBD4) >> 32) == 0x38000000ULL)			lv1_um=1;	else lv1_um=0;    // Fixed
		if( (peek_lv1_cobra(0x16FB08) >> 32) == 0x38600000ULL)			lv1_dm=1;	else lv1_dm=0;    // Fixed

		if( (peek_lv1_cobra(0x274FEC) >> 32) == 0x392001DFULL)			lv1_enc=1;	else lv1_enc=0;   // Fixed

		if( (peek_lv1_cobra(0x0FBE24) >> 32) == 0x60000000ULL)			lv1_pkg=1;	else lv1_pkg=0;   // Fixed
		if( (peek_lv1_cobra(0x2E5550) >> 32) == 0xE81E0020ULL)			lv1_lpar=1;	else lv1_lpar=0;  // Fixed
		if( (peek_lv1_cobra(0x2F9EB8) >> 32) == 0x3920FFFFULL)			lv1_spe=1;	else lv1_spe=0;   // Fixed
		if( (peek_lv1_cobra(0x2EB550) >> 32) == 0x3800000FULL)			lv1_dabr=1;	else lv1_dabr=0;  // Fixed

		if( (peek_lv1_cobra(0x214F1C) >> 32) == 0x38001000ULL)			lv1_gart=1;	else lv1_gart=0;  // Fixed
		if( (peek_lv1_cobra(0x714D50) >> 32) == 0x60000000ULL)			lv1_keys=1;	else lv1_keys=0;  // Fixed
		if( (peek_lv1_cobra(0x25C504) >> 32) == 0x38600001ULL)			lv1_acl=1;	else lv1_acl=0;   // Fixed

		/* Value changes depending on kernel type*/
		lv1_smgo = lv1_go = 0;
		if((cid!=0x82) &&is_nor() && ((peek_lv1_cobra(0x1194CC) >> 32) == 0x6400FFFFULL) ) 	lv1_smgo=1; //CEX , NOR
		if((cid==0x82) &&is_nor() && ((peek_lv1_cobra(0x1504CC) >> 32) == 0x6400FFFFULL) ) 	lv1_smgo=1; //DEX , NOR
		if((cid!=0x82) &&!is_nor() && ((peek_lv1_cobra(0x7814CC) >> 32) == 0x6400FFFFULL) ) 	lv1_smgo=1; //CEX , NAND
		if((cid==0x82) &&!is_nor() && ((peek_lv1_cobra(0x3784CC) >> 32) == 0x6400FFFFULL) ) 	lv1_smgo=1; //DEX , NAND

		if((cid!=0x82) &&is_nor() && ((peek_lv1_cobra(0x168090) >> 32) == 0x38600001ULL) ) 	lv1_go=1; //CEX ,NOR
		if((cid!=0x82) &&((peek_lv1_cobra(0x11C090) >> 32) == 0x38600001ULL) ) 	lv1_go=1; //CEX ,NAND
		if((cid==0x82) &&((peek_lv1_cobra(0x11B090) >> 32) == 0x38600001ULL) ) 	lv1_go=1; //DEX NAND, NOR

	}


	if(xmb_icon!=2 /*&& rebug_checked  */&& cobra_checked) return; // don't access flash each time we check current settings

	//rebug_checked=1;
	cobra_checked=1;
	if( exist((char*)"/dev_flash/rebug/cobra/stage2.cex") /*&&
		exist((char*)"/dev_flash/rebug/cobra/stage2.dex")*/)
	{
		cobra_mode=1;	//enabled
	}
	else
	{
		if( exist((char*)"/dev_flash/rebug/cobra/stage2.cex.bak") /*&&
			exist((char*)"/dev_flash/rebug/cobra/stage2.dex.bak")*/)
		{
			cobra_mode=0;	//disabled
		}
	}

struct CellFsStat stat;
cellFsStat("/dev_rebug/ps2emu/ps2_netemu.self", &stat);
if(stat.st_size==2605184)
	swap_emu=1;

else if(stat.st_size==2605232)
	swap_emu=1;

else if(stat.st_size==2605216)
	swap_emu=1;

else if(stat.st_size==2605280)
	swap_emu=1;

else if(stat.st_size==2605232)
	swap_emu=1;

else if(stat.st_size==2605104)
	swap_emu=1;

else
	swap_emu=0;

	if( exist((char*)"/dev_flash/vsh/module/webftp_server.sprx") )
		webman_mode=1;	//enabled
	else if( exist((char*)"/dev_flash/vsh/module/webftp_server.sprx.bak") )
		webman_mode=0;	//disabled
	if(  (c_firmware==4.78f || c_firmware==4.80f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.org") )
		cfw_settings=1;	//enabled
	else if(  (c_firmware==4.78f || c_firmware==4.80f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.cfw") )
		cfw_settings=0;	//disabled
	if( (c_firmware==4.81f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.off") )
		cfw_settings=1;	//enabled
	else if( (c_firmware==4.81f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.on") )
		cfw_settings=0;	//disabled
	/*if( (c_firmware==4.81f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.org") )
		wmlp=1;	//enabled
	else if( (c_firmware==4.81f) && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.wmlp") )
		wmlp=0;	//disabled*/	
	if( exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex") )
		xmb_plugin=1;	//enabled
	else if( exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex") )
		xmb_plugin=0;	//disabled

	if( exist((char*)"/dev_flash/vsh/module/vsh.self.swp") )
	{
		if( exist((char*)"/dev_flash/vsh/module/sysconf_plugin.sprx.cex") )
		{
/*			strcpy(status1, "NORMAL");
			strcpy(status2, "2");
			strcpy(status3, "DEBUG");
			strcpy(select1, "   L1+CROSS TO SET: [REBUG MODE]");
			strcpy(select2, "         OPTION UNAVAILABLE");
			strcpy(select3, "     R2+CROSS TO USE: [MENU 1]");
			mode_select = 1;
			xmb_select = 0;
			menu_select = 1;
*/
			rebug_mode=0;	//normal
			menu_mode=1;	//dex
			xmb_mode=1;		//debug

		}
		else
		{
/*			strcpy(status1, "NORMAL");
			strcpy(status2, "1");
			strcpy(status3, "DEBUG");
			strcpy(select1, "   L1+CROSS TO SET: [REBUG MODE]");
			strcpy(select2, "         OPTION UNAVAILABLE");
			strcpy(select3, "     R2+CROSS TO USE: [MENU 2]");
			mode_select = 1;
			xmb_select = 0;
			menu_select = 2;
*/
			rebug_mode=0;	//normal
			menu_mode=0;	//cex
			xmb_mode=1;		//debug
		}
	}
	else
	{
		if( exist((char*)"/dev_flash/vsh/module/vsh.self.dexsp") )
		{
			if( exist((char*)"/dev_flash/vsh/module/sysconf_plugin.sprx.cex") )
			{
/*				strcpy(status1, "REBUG");
				strcpy(status2, "2");
				strcpy(status3, "RETAIL");
				strcpy(select1, "   L1+CROSS TO SET: [NORMAL MODE]");
				strcpy(select2, "    R1+CROSS TO USE: [DEBUG XMB]");
				strcpy(select3, "         OPTION UNAVAILABLE");
				mode_select = 0;
				xmb_select = 1;
				menu_select = 0;
*/
				rebug_mode=1;	//rebug
				menu_mode=1;	//dex (NA)
				xmb_mode=0;		//retail
			}
			else
			{
/*				strcpy(status1, "REBUG");
				strcpy(status2, "1");
				strcpy(status3, "RETAIL");
				strcpy(select1, "   L1+CROSS TO SET: [NORMAL MODE]");
				strcpy(select2, "    R1+CROSS TO USE: [DEBUG XMB]");
				strcpy(select3, "         OPTION UNAVAILABLE");
				mode_select = 0;
				xmb_select = 1;
				menu_select = 0;
*/
				rebug_mode=1;	//rebug
				menu_mode=0;	//cex (NA)
				xmb_mode=0;		//retail
			}
		}
		else
		{
			if( exist((char*)"/dev_flash/vsh/module/sysconf_plugin.sprx.cex") )
			{
/*				strcpy(status1, "REBUG");
				strcpy(status2, "2");
				strcpy(status3, "DEBUG");
				strcpy(select1, "   L1+CROSS TO SET: [NORMAL MODE]");
				strcpy(select2, "   R1+CROSS TO USE: [RETAIL XMB]");
				strcpy(select3, "     R2+CROSS TO USE: [MENU 1]");
				mode_select = 0;
				xmb_select = 2;
				menu_select = 1;
*/
				rebug_mode=1;	//rebug
				menu_mode=1;	//dex
				xmb_mode=1;		//debug
			}
			else
			{
/*				strcpy(status1, "REBUG");
				strcpy(status2, "1");
				strcpy(status3, "DEBUG");
				strcpy(select1, "   L1+CROSS TO SET: [NORMAL MODE]");
				strcpy(select2, "   R1+CROSS TO USE: [RETAIL XMB]");
				strcpy(select3, "     R2+CROSS TO USE: [MENU 2]");
				mode_select = 0;
				xmb_select = 2;
				menu_select = 2;
*/
				rebug_mode=1;	//rebug
				menu_mode=0;	//cex
				xmb_mode=1;		//debug
			}
		}
	}


}

void change_lv1_um(u8 val)
{
	if(c_firmware==3.55f)
	{
		u64 org= peek_lv1_cobra(0x0FDBB4) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x0FDBB4, 0x3800000000000000ULL | org);
		else	poke_lv1(0x0FDBB4, 0xE818000800000000ULL | org);
	}

	if(c_firmware==4.21f) // Fixed
	{
		u64 org=peek_lv1_cobra(0x0FEB88) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x0FEB88, 0x3800000000000000ULL | org);
		else	poke_lv1(0x0FEB88, 0xE818000800000000ULL | org);
	}

	if(c_firmware==4.30f || c_firmware==4.31f) // Fixed
	{
		u64 org=peek_lv1_cobra(0x0FEB98) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x0FEB98, 0x3800000000000000ULL | org);
		else	poke_lv1(0x0FEB98, 0xE818000800000000ULL | org);
	}
	if(c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f) // Fixed
	{
		u64 org=peek_lv1_cobra(0x0FEB8C) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x0FEB8C, 0x3800000000000000ULL | org);
		else	poke_lv1(0x0FEB8C, 0xE818000800000000ULL | org);
	}
	if(c_firmware==4.50f ||  c_firmware==4.53f ||  c_firmware==4.55f ||  c_firmware==4.60f ||  c_firmware==4.65f ||  c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) // Fixed
	{
		u64 org=peek_lv1_cobra(0x0FEBD4) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x0FEBD4, 0x3800000000000000ULL | org);
		else	poke_lv1(0x0FEBD4, 0xE818000800000000ULL | org);
	}
}

void change_lv1_dm(u8 val)
{
	if(c_firmware==3.55f)
	{
		u64 org=peek_lv1_cobra(0x16F3BC) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F3BC, 0x6000000000000000ULL | org); //enable patch
		else	poke_lv1(0x16F3BC, 0xF801009800000000ULL | org); //restore original

		org=	peek_lv1_cobra(0x16F3E0) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F3E0, 0x3860000100000000ULL | org);
		else	poke_lv1(0x16F3E0, 0x4BFFF0E500000000ULL | org);

		org=	peek_lv1_cobra(0x16F458) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F458, 0x3BE0000100000000ULL | org);
		else	poke_lv1(0x16F458, 0x38A1007000000000ULL | org);

		org=	peek_lv1_cobra(0x16F460) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F460, 0x3860000000000000ULL | org);
		else	poke_lv1(0x16F460, 0x48005FA500000000ULL | org);
	}

	if(c_firmware==4.21f)
	{
		u64 org=peek_lv1_cobra(0x16F75C) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F75C, 0x6000000000000000ULL | org); //enable patch
		else	poke_lv1(0x16F75C, 0xF801009800000000ULL | org); //restore original

		org=	peek_lv1_cobra(0x16F780) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F780, 0x3860000100000000ULL | org);
		else	poke_lv1(0x16F780, 0x4BFFF0E500000000ULL | org);

		org=	peek_lv1_cobra(0x16F7F8) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F7F8, 0x3BE0000100000000ULL | org);
		else	poke_lv1(0x16F7F8, 0x38A1007000000000ULL | org);

		org=	peek_lv1_cobra(0x16F800) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16F800, 0x3860000000000000ULL | org);
		else	poke_lv1(0x16F800, 0x4800606500000000ULL | org);
	}

	if(c_firmware==4.30f || c_firmware==4.31f || c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f || c_firmware==4.50f || c_firmware==4.53f || c_firmware==4.55f || c_firmware==4.60f || c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) // Fixed
	{
		u64 org=peek_lv1_cobra(0x16FA64) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16FA64, 0x6000000000000000ULL | org); //enable patch
		else	poke_lv1(0x16FA64, 0xF801009800000000ULL | org); //restore original

		org=	peek_lv1_cobra(0x16FA88) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16FA88, 0x3860000100000000ULL | org);
		else	poke_lv1(0x16FA88, 0x4BFFF0E500000000ULL | org);

		org=	peek_lv1_cobra(0x16FB00) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16FB00, 0x3BE0000100000000ULL | org);
		else	poke_lv1(0x16FB00, 0x38A1007000000000ULL | org);

		org=	peek_lv1_cobra(0x16FB08) & 0x00000000FFFFFFFFULL;
		if(val)	poke_lv1(0x16FB08, 0x3860000000000000ULL | org);
		else	poke_lv1(0x16FB08, 0x4800606500000000ULL | org);
	}
}

void apply_settings(char *option, int val, u8 _forced)
{
	u8 cid=get_idps(0);
	u64 org=0;

	if(c_firmware==3.55f)
	{
		if(!strcmp(option, "lv1_pp"))// || _forced)
		{
			if(val)
			{
				poke_lv1(0x3025EC +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3025EC +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3025EC + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3025EC + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3025EC +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3025EC +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3025EC + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3025EC + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced)
		{
			org= peek_lv1_cobra(0x2AE200) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2AE200, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2AE200, 0x419E00E800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced)
		{
			org= peek_lv1_cobra(0x2D5EB0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2D5EB0, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2D5EB0, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced)
		{
			org= peek_lv1_cobra(0x0AC574) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC574, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC574, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced)
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced)
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced)
		{
			org=	peek_lv1_cobra(0x273494) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x273494, 0x3920005F00000000ULL | org);
			else	poke_lv1(0x273494, 0x3920004F00000000ULL | org);
		}


		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 3.55 , NOR
		{
			org=	peek_lv1_cobra(0x11B4D8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11B4D8, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x11B4D8, 0x6400003B00000000ULL | org);

			if(val)	poke_lv1(0x11B4DC, 0xF93F01C86000FFFEULL);
			else	poke_lv1(0x11B4DC, 0xF93F01C86000F7EEULL);
		}

		if((cid==0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 3.55 , NOR
		{
			org=	peek_lv1_cobra(0x1524D8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x1524D8, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x1524D8, 0x6400003B00000000ULL | org);

			if(val)	poke_lv1(0x1524DC, 0xF93F01C86000FFFEULL);
			else	poke_lv1(0x1524DC, 0xF93F01C86000F7EEULL);
		}

		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 3.55 , NAND
		{
			org=	peek_lv1_cobra(0x0A34D8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0A34D8, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x0A34D8, 0x6400003B00000000ULL | org);

			if(val)	poke_lv1(0x0A34DC, 0xF93F01C86000FFFEULL);
			else	poke_lv1(0x0A34DC, 0xF93F01C86000F7EEULL);
		}

		if((cid==0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 3.55 , NAND
		{
			org=	peek_lv1_cobra(0x76A4D8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x76A4D8, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x76A4D8, 0x6400003B00000000ULL | org);

			if(val)	poke_lv1(0x76A4DC, 0xF93F01C86000FFFEULL);
			else	poke_lv1(0x76A4DC, 0xF93F01C86000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced)
		{
			org=	peek_lv1_cobra(0x0FB1CC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FB1CC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FB1CC, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced)
		{
			if(val)
			{
				poke_lv1(0x2DD5CC +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2DD5CC +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2DD5CC + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2DD850 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2DD850 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2DD850 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2DD850 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2DDCF4 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2DDCF4 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2DDCF4 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2DDCF4 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2DD5CC +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2DD5CC +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2DD5CC + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2DD850 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2DD850 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2DD850 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2DD850 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2DDCF4 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2DDCF4 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2DDCF4 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2DDCF4 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced)
		{
			org=	peek_lv1_cobra(0x2F2678) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F2678, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F2678, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced)
		{
			org=	peek_lv1_cobra(0x2E3CF4) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2E3CF4, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2E3CF4, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced)
		{
			org=	peek_lv1_cobra(0x214990) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214990, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214990, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced)
		{
			org=	peek_lv1_cobra(0x7125AC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x7125AC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x7125AC, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced)
		{
			if(val)
			{
				poke_lv1(0x25B340 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25B340 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25B340 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25B340 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // CEX 3.55 NOR
		{
			org=	peek_lv1_cobra(0x1264DC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x1264DC, 0x3860000100000000ULL | org);
			else	poke_lv1(0x1264DC, 0x3860000000000000ULL | org);
		}

		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // CEX 3.55 NAND
		{
			org=	peek_lv1_cobra(0x11E4DC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11E4DC, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11E4DC, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&  (_forced || !strcmp(option, "lv1_go")) ) // DEX 3.55 NOR , NAND
		{
			org=	peek_lv1_cobra(0x11D4DC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11D4DC, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11D4DC, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_prodmode") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_prodmode"))
			{
				toggle_product_mode_flag();
				util_prodmode=read_product_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 3.55 Firmware

	else if(c_firmware==4.21f)
	{
		if(!strcmp(option, "lv1_pp") || _forced)
		{
			if(val)
			{
				poke_lv1(0x309984 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x309984 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x309984 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x309984 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x309984 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x309984 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x309984 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x309984 + 24, 0x6000FFECF80300C0ULL);
			}
		}

		if(!strcmp(option, "lv1_lv2") || _forced)
		{
			org= peek_lv1_cobra(0x2B3F6C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3F6C, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3F6C, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced)
		{
			org= peek_lv1_cobra(0x2DD244) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD244, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD244, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced)
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced)
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced)
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced)
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.21, NOR
		{
			org=	peek_lv1_cobra(0x118378) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x118378, 0x6000FFFE00000000ULL | org);
			else	poke_lv1(0x118378, 0x6000F7EE00000000ULL | org);

			if(val)	poke_lv1(0x118370, 0x6400FFFFF93F01D0ULL);
			else	poke_lv1(0x118370, 0x640003FBF93F01D0ULL);
		}

		if((cid==0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.21, NOR
		{
			org=	peek_lv1_cobra(0x11F378) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11F378, 0x6000FFFE00000000ULL | org);
			else	poke_lv1(0x11F378, 0x6000F7EE00000000ULL | org);

			if(val)	poke_lv1(0x11F370, 0x6400FFFFF93F01D0ULL);
			else	poke_lv1(0x11F370, 0x640003FBF93F01D0ULL);
		}

		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.21, NAND
		{
			org=	peek_lv1_cobra(0x0A4378) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0A4378, 0x6000FFFE00000000ULL | org);
			else	poke_lv1(0x0A4378, 0x6000F7EE00000000ULL | org);

			if(val)	poke_lv1(0x0A4370, 0x6400FFFFF93F01D0ULL);
			else	poke_lv1(0x0A4370, 0x640003FBF93F01D0ULL);
		}

		if((cid==0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.21, NAND
		{
			org=	peek_lv1_cobra(0x0A3378) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0A3378, 0x6000FFFE00000000ULL | org);
			else	poke_lv1(0x0A3378, 0x6000F7EE00000000ULL | org);

			if(val)	poke_lv1(0x0A3370, 0x6400FFFFF93F01D0ULL);
			else	poke_lv1(0x0A3370, 0x640003FBF93F01D0ULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced)
		{
			org=	peek_lv1_cobra(0x0FBE00) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE00, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE00, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced)
		{
			if(val)
			{
				poke_lv1(0x2E4960 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E4960 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E4960 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4BE4 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4BE4 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4BE4 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4BE4 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E5088 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E5088 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E5088 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E5088 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{

				poke_lv1(0x2E4960 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E4960 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E4960 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4BE4 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4BE4 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4BE4 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4BE4 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E5088 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E5088 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E5088 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E5088 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced)
		{
			org=	peek_lv1_cobra(0x2F99F0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F99F0, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F99F0, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced)
		{
			org=	peek_lv1_cobra(0x2EB088) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB088, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB088, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced)
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced)
		{
			org=	peek_lv1_cobra(0x71422C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x71422C, 0x6000000000000000ULL | org);
			else	poke_lv1(0x71422C, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced)
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // CEX 4.21, NOR
		{
			org=	peek_lv1_cobra(0x122F34) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x122F34, 0x3860000100000000ULL | org);
			else	poke_lv1(0x122F34, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // DEX 4.21, NOR
		{
			org=	peek_lv1_cobra(0x119F34) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x119F34, 0x3860000100000000ULL | org);
			else	poke_lv1(0x119F34, 0x3860000000000000ULL | org);
		}

		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // CEX 4.21, NAND
		{
			org=	peek_lv1_cobra(0x11EF34) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11EF34, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11EF34, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_go")) ) // DEX 4.21, NAND
		{
			org=	peek_lv1_cobra(0x11DF34) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11DF34, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11DF34, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}
	} // 4.21 Firmware

	if(c_firmware==4.30f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x3099C4 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3099C4 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3099C4 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3099C4 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3099C4 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3099C4 + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2B3FAC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3FAC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3FAC, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD284) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD284, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD284, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.30 , NOR
		{
			org=	peek_lv1_cobra(0x1253B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x1253B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x1253B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x1253B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x1253B4, 0xF93F01D06000F7EEULL);
		}

		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.30 , NAND
		{
			org=	peek_lv1_cobra(0x1613B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x1613B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x1613B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x1613B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x1613B4, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.30 , NAND , NOR
		{
			org=	peek_lv1_cobra(0x11C3B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11C3B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x11C3B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x11C3B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x11C3B4, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE10) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE10, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE10, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9A30) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9A30, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9A30, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB0C8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB0C8, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB0C8, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714AB0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714AB0, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714AB0, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.30 NOR
		{
			org=	peek_lv1_cobra(0x0A3F74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0A3F74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x0A3F74, 0x3860000000000000ULL | org);
		}

		if((cid!=0x82) &&!is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.30 NAND
		{
			org=	peek_lv1_cobra(0x76FF74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x76FF74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x76FF74, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&  (_forced || !strcmp(option, "lv1_go")) ) //DEX 4.30 NAND , NOR
		{
			org=	peek_lv1_cobra(0x0F2F74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0F2F74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x0F2F74, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.30 Firmware

	if(c_firmware==4.31f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x3099C4 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3099C4 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3099C4 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3099C4 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3099C4 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3099C4 + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2B3FAC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3FAC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3FAC, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD284) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD284, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD284, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}


		if(!strcmp(option, "lv1_smgo") || _forced && is_nor()) // Fixed
		{
			org=	peek_lv1_cobra(0x11C3B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11C3B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x11C3B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x1253B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x1253B4, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FCE10) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FCE10, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FCE10, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9A30) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9A30, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9A30, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB0C8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB0C8, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB0C8, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x715ACC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x715ACC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x715ACC, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if(!strcmp(option, "lv1_go") || _forced && is_nor())  // breaks OtherOS on NAND PS3s // Fixed [MAYBE NEED REVISION]
		{
			org=	peek_lv1_cobra(0x126F74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x126F74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x126F74, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.31 Firmware


	if(c_firmware==4.40f || c_firmware==4.41f || c_firmware==4.46f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x3099C4 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3099C4 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3099C4 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3099C4 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3099C4 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3099C4 + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2B3FAC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3FAC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3FAC, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD284) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD284, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD284, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}


		if((cid!=0x82) && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.40,4.41, 4.46 , NOR , NAND
		{
			org=	peek_lv1_cobra(0x11D3B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11D3B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x11D3B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x11D3B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x11D3B4, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) && !is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.40,4.41, 4.46 ,  NAND
		{
			org=	peek_lv1_cobra(0x37C3B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x37C3B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x37C3B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x37C3B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x37C3B4, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) && is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.40,4.41, 4.46 NOR
		{
			org=	peek_lv1_cobra(0x1543B0) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x1543B0, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x1543B0, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x1543B4, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x1543B4, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE04) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE04, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE04, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9A30) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9A30, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9A30, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB0C8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB0C8, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB0C8, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714ACC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714ACC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714ACC, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) && is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.40,4.41, 4.46 NOR
		{
			org=	peek_lv1_cobra(0x127F74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x127F74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x127F74, 0x3860000000000000ULL | org);
		}

		if((cid!=0x82) && !is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.40,4.41, 4.46 NAND
		{
			org=	peek_lv1_cobra(0x787F74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x787F74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x787F74, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&  (_forced || !strcmp(option, "lv1_go")) ) //DEX 4.40,4.41, 4.46 NOR , NAND
		{
			org=	peek_lv1_cobra(0x11EF74) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11EF74, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11EF74, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.40, 4.41, 4.46 Firmware

	if(c_firmware==4.50f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x3099C4 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3099C4 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3099C4 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3099C4 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3099C4 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3099C4 + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2B3FAC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3FAC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3FAC, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD284) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD284, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD284, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}


		if(!strcmp(option, "lv1_smgo") || _forced && is_nor()) // Fixed
		{
			org=	peek_lv1_cobra(0x11D4B8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11D4B8, 0x6400FFFF00000000ULL | org);
			else	poke_lv1(0x11D4B8, 0x640003FB00000000ULL | org);

			if(val)	poke_lv1(0x11D4BC, 0xF93F01D06000FFFEULL);
			else	poke_lv1(0x11D4BC, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE24) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE24, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE24, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9A30) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9A30, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9A30, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB0C8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB0C8, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB0C8, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714CEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714CEC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714CEC, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if(!strcmp(option, "lv1_go") || _forced && is_nor())  // breaks OtherOS on NAND PS3s // Fixed [MAYBE NEED REVISION]
		{
			org=	peek_lv1_cobra(0x11807C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11807C, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11807C, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.50 Firmware

	if(c_firmware==4.53f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x3099C4 +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x3099C4 +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x3099C4 + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x3099C4 +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x3099C4 +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x3099C4 + 16, 0x380000006400FFFFULL);
				poke_lv1(0x3099C4 + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2B3FAC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2B3FAC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2B3FAC, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD284) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD284, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD284, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392000DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392000CF00000000ULL | org);
		}


		if((cid!=0x82) && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.53 NAND , NOR
		{
			org = peek_lv1_cobra(0x11D4B8) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x11D4B8, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x11D4B8, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x11D4BC, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x11D4BC, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) && is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.53 NOR
		{
			org = peek_lv1_cobra(0x1544B8) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x1544B8, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x1544B8, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x1544BC, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x1544BC, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) && !is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.53 NAND
		{
			org = peek_lv1_cobra(0x37C4B8) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x37C4B8, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x37C4B8, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x37C4BC, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x37C4BC, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE24) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE24, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE24, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E49A0 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E49A0 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E49A0 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E4C24 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E4C24 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E4C24 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E4C24 + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E50C8 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50C8 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50C8 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50C8 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9A30) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9A30, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9A30, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB0C8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB0C8, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB0C8, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714CEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714CEC, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714CEC, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.53 NAND , NOR
		{
			org = peek_lv1_cobra(0x11807C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11807C, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11807C, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&  (_forced || !strcmp(option, "lv1_go")) ) //DEX 4.53 , NOR , NAND
		{
			org = peek_lv1_cobra(0x11F07C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11F07C, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11F07C, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.53 Firmware

	if(c_firmware==4.55f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x309E4C +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x309E4C +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x309E4C + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x309E4C + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x309E4C +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x309E4C +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x309E4C + 16, 0x380000006400FFFFULL);
				poke_lv1(0x309E4C + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2b4434) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2b4434, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2b4434, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD70C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD70C, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD70C, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392001DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392001CF00000000ULL | org);
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.55
		{
			org = peek_lv1_cobra(0x11D4B8) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x11D4B8, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x11D4B8, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x11D4BC, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x11D4BC, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.55
		{
			org = peek_lv1_cobra(0x11C4B8) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x11C4B8, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x11C4B8, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x11C4BC, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x11C4BC, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE24) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE24, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE24, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9EB8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9EB8, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9EB8, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB550) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB550, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB550, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714D50) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714D50, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714D50, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.55
		{
			org = peek_lv1_cobra(0x11807C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11807C, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11807C, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) &&is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //DEX 4.55
		{
			org = peek_lv1_cobra(0xF307C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0xF307C, 0x3860000100000000ULL | org);
			else	poke_lv1(0xF307C, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.55 Firmware

	if(c_firmware==4.60f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x309E4C +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x309E4C +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x309E4C + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x309E4C + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x309E4C +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x309E4C +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x309E4C + 16, 0x380000006400FFFFULL);
				poke_lv1(0x309E4C + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2b4434) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2b4434, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2b4434, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD70C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD70C, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD70C, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392001DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392001CF00000000ULL | org);
		}

		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.60 , NOR
		{
			org = peek_lv1_cobra(0x11D4CC) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x11D4CC, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x11D4CC, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x11D4D0, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x11D4D0, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE24) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE24, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE24, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2F9EB8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9EB8, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9EB8, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x2EB550) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB550, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB550, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714D50) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714D50, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714D50, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.60, NOR
		{
			org = peek_lv1_cobra(0x118090) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x118090, 0x3860000100000000ULL | org);
			else	poke_lv1(0x118090, 0x3860000000000000ULL | org);
		}


		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.60 Firmware

	if(c_firmware==4.65f || c_firmware==4.66f || c_firmware==4.70f || c_firmware==4.75f || c_firmware==4.76f || c_firmware==4.78f || c_firmware==4.80f || c_firmware==4.81f) // Fixed
	{
		if(!strcmp(option, "lv1_pp"))// || _forced) // Fixed
		{
			if(val)
			{
				poke_lv1(0x309E4C +  0, 0xE8830018E8840000ULL);
				poke_lv1(0x309E4C +  8, 0xF88300C84E800020ULL);
				poke_lv1(0x309E4C + 16, 0x38000000E8A30020ULL);
				poke_lv1(0x309E4C + 24, 0xE8830018F8A40000ULL);
			}
			else
			{
				poke_lv1(0x309E4C +  0, 0x6400FFFF6000FFECULL);
				poke_lv1(0x309E4C +  8, 0xF80300C04E800020ULL);
				poke_lv1(0x309E4C + 16, 0x380000006400FFFFULL);
				poke_lv1(0x309E4C + 24, 0x6000FFECF80300C0ULL);
			}
		}
		if(!strcmp(option, "lv1_lv2") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2b4434) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2b4434, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2b4434, 0x419E011800000000ULL | org);
		}

		if(!strcmp(option, "lv1_htab") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x2DD70C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2DD70C, 0x6000000000000000ULL | org);
			else	poke_lv1(0x2DD70C, 0x41DA005400000000ULL | org);
		}

		if(!strcmp(option, "lv1_indi") || _forced) // Fixed
		{
			org= peek_lv1_cobra(0x0AC594) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0AC594, 0x3860000000000000ULL | org);
			else	poke_lv1(0x0AC594, 0x7C63003800000000ULL | org);
		}

		if(!strcmp(option, "lv1_um") || _forced) // Fixed
		{
			change_lv1_um(_forced|val);
		}

		if(!strcmp(option, "lv1_dm") || _forced) // Fixed
		{
			change_lv1_dm(_forced|val);
		}

		if(!strcmp(option, "lv1_enc") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x274FEC) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x274FEC, 0x392001DF00000000ULL | org);
			else	poke_lv1(0x274FEC, 0x392001CF00000000ULL | org);
		}


		if((cid!=0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.65-4.75 , NOR
		{
			org = peek_lv1_cobra(0x1194CC) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x1194CC, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x1194CC, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x1194D0, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x1194D0, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) &&is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.65-4.75 , NOR
		{
			org = peek_lv1_cobra(0x1504CC) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x1504CC, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x1504CC, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x1504D0, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x1504D0, 0xF93F01D06000F7EEULL);
		}
		if((cid!=0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // CEX 4.65-4.75 , NAND
		{
			org = peek_lv1_cobra(0x7814CC) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x7814CC, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x7814CC, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x7814D0, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x7814D0, 0xF93F01D06000F7EEULL);
		}

		if((cid==0x82) &&!is_nor() && (_forced || !strcmp(option, "lv1_smgo")) ) // DEX 4.65-4.75 , NAND
		{
			org = peek_lv1_cobra(0x3784CC) & 0x00000000FFFFFFFFULL;
			if(val) poke_lv1(0x3784CC, 0x6400FFFF00000000ULL | org);
			else    poke_lv1(0x3784CC, 0x640003FB00000000ULL | org);

			if(val) poke_lv1(0x3784D0, 0xF93F01D06000FFFEULL);
			else    poke_lv1(0x3784D0, 0xF93F01D06000F7EEULL);
		}

		if(!strcmp(option, "lv1_pkg") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x0FBE24) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x0FBE24, 0x6000000000000000ULL | org);
			else	poke_lv1(0x0FBE24, 0x419D00A800000000ULL | org);
		}

		if(!strcmp(option, "lv1_lpar") || _forced) // Fixed  IDA
		{
			if(val)
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0020E95E0028ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0030E8FE0038ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0038EBFE0018ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0048EBFE0018ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0020E93E0028ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0030E91E0038ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0040E8DE0048ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0048EBFE0018ULL);

			}
			else
			{
				poke_lv1(0x2E4E28 +  0, 0xE81E0018E95E0020ULL);
				poke_lv1(0x2E4E28 +  8, 0xE91E0028E8FE0030ULL);
				poke_lv1(0x2E4E28 + 12, 0xE8FE0030EBEB0050ULL);

				poke_lv1(0x2E50AC +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E50AC +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E50AC + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E50AC + 20, 0xE8DE0040EBEB0050ULL);

				poke_lv1(0x2E5550 +  0, 0xE81E0018E93E0020ULL);
				poke_lv1(0x2E5550 +  8, 0xE95E0028E91E0030ULL);
				poke_lv1(0x2E5550 + 16, 0xE8FE0038E8DE0040ULL);
				poke_lv1(0x2E5550 + 20, 0xE8DE0040EBEB0050ULL);
			}
		}

		if(!strcmp(option, "lv1_spe") || _forced) // Fixed IDA
		{
			org=	peek_lv1_cobra(0x2F9EB8) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2F9EB8, 0x3920FFFF00000000ULL | org);
			else	poke_lv1(0x2F9EB8, 0x3920000900000000ULL | org);
		}

		if(!strcmp(option, "lv1_dabr") || _forced) // Fixed IDA
		{
			org=	peek_lv1_cobra(0x2EB550) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x2EB550, 0x3800000F00000000ULL | org);
			else	poke_lv1(0x2EB550, 0x3800000B00000000ULL | org);
		}

		if(!strcmp(option, "lv1_gart") || _forced) // Fixed IDA
		{
			org=	peek_lv1_cobra(0x214F1C) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x214F1C, 0x3800100000000000ULL | org);
			else	poke_lv1(0x214F1C, 0x3C00000100000000ULL | org);
		}

		if(!strcmp(option, "lv1_keys") || _forced) // Fixed
		{
			org=	peek_lv1_cobra(0x714D50) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x714D50, 0x6000000000000000ULL | org);
			else	poke_lv1(0x714D50, 0x419D004C00000000ULL | org);
		}

		if(!strcmp(option, "lv1_acl") || _forced) // Fixed IDA
		{
			if(val)
			{
				poke_lv1(0x25C504 + 0, 0x386000012F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E001438000001ULL);
			}
			else
			{
				poke_lv1(0x25C504 + 0, 0x5463063E2F830000ULL);
				poke_lv1(0x25C504 + 8, 0x419E0014E8010070ULL);
			}
		}

		if((cid!=0x82) &&is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.65-4.75 , NOR
		{
			org = peek_lv1_cobra(0x168090) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x168090, 0x3860000100000000ULL | org);
			else	poke_lv1(0x168090, 0x3860000000000000ULL | org);
		}

		if((cid!=0x82) &&!is_nor() &&  (_forced || !strcmp(option, "lv1_go")) ) //CEX 4.65-4.75 , NAND
		{
			org = peek_lv1_cobra(0x11C090) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11C090, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11C090, 0x3860000000000000ULL | org);
		}

		if((cid==0x82) && (_forced || !strcmp(option, "lv1_go")) ) //DEX 4.65-4.75 , NOR , NAND
		{
			org = peek_lv1_cobra(0x11B090) & 0x00000000FFFFFFFFULL;
			if(val)	poke_lv1(0x11B090, 0x3860000100000000ULL | org);
			else	poke_lv1(0x11B090, 0x3860000000000000ULL | org);
		}

		if(!strcmp(option, "util_recovery") || !strcmp(option, "util_qa"))
		{
			u8 old_lv1_um=lv1_um;
			u8 old_lv1_dm=lv1_dm;
			u8 to_restore=0;
			if(!lv1_um || !lv1_dm)
			{
				change_lv1_um(1);
				change_lv1_dm(1);
				to_restore=1;
			}

			if(!strcmp(option, "util_recovery"))
			{
				toggle_recover_mode_flag();
				util_recovery=read_recover_mode_flag();
			}

			if(!strcmp(option, "util_qa"))
			{
				toggle_qa_flag();
				util_qa=read_qa_flag();
			}

			if(to_restore)
			{
				change_lv1_um(old_lv1_um);
				change_lv1_dm(old_lv1_dm);
			}
		}

	} // 4.60, 4.65, 4.66, 4.70 , 4.75, 4.76 Firmware

	if(!strcmp(option, "util_idps"))
	{
		set_idps(val);
	}

	if(!strcmp(option, "util_xReg")) set_xReg();


	if(!strcmp(option, "otheros")) boot_otherOS(val);
	if(!strcmp(option, "reboot")) reboot_ps3(val);

	if(!strcmp(option, "lv2_kernel")) {load_lv2_kernel(val);add_utilities();}
	if(!strcmp(option, "util_qa") || !strcmp(option, "util_prodmode") || !strcmp(option, "util_recovery"))
		add_utilities();


	struct stat statinfo;
	char status[512];
	int result;
	sprintf(status, "***");
	if(!strcmp(option, "rebug_mode"))
	{
		//  [NORMAL MODE]
		if(!rebug_mode) //switch to normal //mode_select == 0)
		{
		  result = stat("/dev_rebug/vsh/module/vsh.self.nrm", &statinfo);
		  if(result == 0 )
		  {
			if(rename ("/dev_rebug/vsh/module/vsh.self","/dev_rebug/vsh/module/vsh.self.swp" )== 0 &&
				rename ("/dev_rebug/vsh/module/vsh.self.nrm","/dev_rebug/vsh/module/vsh.self" )== 0){};

			if(rename ("/dev_rebug/vsh/etc/index.dat","/dev_rebug/vsh/etc/index.dat.swp" )== 0 &&
				rename ("/dev_rebug/vsh/etc/index.dat.nrm","/dev_rebug/vsh/etc/index.dat" )== 0){};

			if(rename ("/dev_rebug/vsh/etc/version.txt","/dev_rebug/vsh/etc/version.txt.swp" )== 0 &&
				rename ("/dev_rebug/vsh/etc/version.txt.nrm", "/dev_rebug/vsh/etc/version.txt") == 0)
			{
				strcpy(status, STR_MODNORM);
				auto_reboot = 1;
			}
			else
			{
				strcpy(status, STR_TRYAGAIN);
			}
		  }
		  else
		  {
				strcpy(status, STR_MODNORMALRD);
		  }
		}

		// [REBUG MODE]
		if(rebug_mode) //switch to REBUG //mode_select == 1)
		{
		  result = stat("/dev_rebug/vsh/module/vsh.self.swp", &statinfo);
		  if(result == 0 )
		  {
			result = stat("/dev_rebug/vsh/module/vsh.self.dexsp", &statinfo);
			if(result == 0 )
			{
				result = stat("/dev_rebug/vsh/module/sysconf_plugin.sprx.cex", &statinfo);
				if(result == 0)
				{
					/*result = stat("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", &statinfo);
					if(result == 0)
					{
						if(rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco","/dev_rebug/vsh/resource/sysconf_plugin.rco.dex" )== 0 &&
						rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", "/dev_rebug/vsh/resource/sysconf_plugin.rco") == 0){};
					}*/
					if(rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx","/dev_rebug/vsh/module/sysconf_plugin.sprx.dex" )== 0 &&
						rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx.cex", "/dev_rebug/vsh/module/sysconf_plugin.sprx") == 0){};
				}
			}

			if(rename ("/dev_rebug/vsh/module/vsh.self","/dev_rebug/vsh/module/vsh.self.nrm" )== 0 &&
				rename ("/dev_rebug/vsh/module/vsh.self.swp","/dev_rebug/vsh/module/vsh.self" )== 0){};

			if(rename ("/dev_rebug/vsh/etc/index.dat","/dev_rebug/vsh/etc/index.dat.nrm" )== 0 &&
				rename ("/dev_rebug/vsh/etc/index.dat.swp","/dev_rebug/vsh/etc/index.dat" )== 0){};

			if(rename ("/dev_rebug/vsh/etc/version.txt","/dev_rebug/vsh/etc/version.txt.nrm" )== 0 &&
				rename ("/dev_rebug/vsh/etc/version.txt.swp", "/dev_rebug/vsh/etc/version.txt") == 0)
			{
				strcpy(status, STR_MODREB);
				auto_reboot = 1;
			}
			else
			{
				strcpy(status, STR_TRYAGAIN);
			}
		  }
		  else
		  {
			strcpy(status, STR_MODREBALRD);
		  }
		}

		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();
	}

    //bool is_cobra_toggle = (!strcmp(option, "cobra_mode"));
#define ORG_DIR              "/dev_hdd0/game/RBGTLBOX2/USRDIR/wm_lang"	
#define LANG_DIR             "/dev_hdd0/tmp/wm_lang"

	if(!strcmp(option, "webman_mode"))
	{
		if((webman_mode==0) && exist((char*)"/dev_rebug/vsh/module/webftp_server.sprx"))
		{
			rename ("/dev_rebug/vsh/module/webftp_server.sprx","/dev_rebug/vsh/module/webftp_server.sprx.bak" );
			cellFsUnlink("/dev_hdd0/xmlhost/game_plugin/fb.xml");

			strcpy(status, STR_WMDIS);
			auto_reboot = 1;
		}
		else if((webman_mode==1) && exist((char*)"/dev_rebug/vsh/module/webftp_server.sprx.bak"))
		{
			rename ("/dev_rebug/vsh/module/webftp_server.sprx.bak","/dev_rebug/vsh/module/webftp_server.sprx" );
				mkdir((char*)"/dev_hdd0/tmp/wm_lang", S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
				file_copy((char*)ORG_DIR "/LANG_EN.TXT", (char*)LANG_DIR "/LANG_EN.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_AR.TXT", (char*)LANG_DIR "/LANG_AR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_CN.TXT", (char*)LANG_DIR "/LANG_CN.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_DE.TXT", (char*)LANG_DIR "/LANG_DE.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_ES.TXT", (char*)LANG_DIR "/LANG_ES.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_FR.TXT", (char*)LANG_DIR "/LANG_FR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_GR.TXT", (char*)LANG_DIR "/LANG_GR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_DK.TXT", (char*)LANG_DIR "/LANG_DK.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_HU.TXT", (char*)LANG_DIR "/LANG_HU.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_HR.TXT", (char*)LANG_DIR "/LANG_HR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_BG.TXT", (char*)LANG_DIR "/LANG_BG.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_CZ.TXT", (char*)LANG_DIR "/LANG_CZ.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_SK.TXT", (char*)LANG_DIR "/LANG_SK.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_IN.TXT", (char*)LANG_DIR "/LANG_IN.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_IT.TXT", (char*)LANG_DIR "/LANG_IT.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_JP.TXT", (char*)LANG_DIR "/LANG_JP.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_KR.TXT", (char*)LANG_DIR "/LANG_KR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_NL.TXT", (char*)LANG_DIR "/LANG_NL.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_PL.TXT", (char*)LANG_DIR "/LANG_PL.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_PT.TXT", (char*)LANG_DIR "/LANG_PT.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_RU.TXT", (char*)LANG_DIR "/LANG_RU.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_TR.TXT", (char*)LANG_DIR "/LANG_TR.TXT", 0);
				file_copy((char*)ORG_DIR "/LANG_ZH.TXT", (char*)LANG_DIR "/LANG_ZH.TXT", 0);
			strcpy(status, STR_WMENA);	
			auto_reboot = 1;
		}
		if(auto_reboot)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();


		//system_call_4(379,0x1200,0,0,0);
		}
	}

	if(!strcmp(option, "xmb_mode") && rebug_mode)
	{
		// [RETAIL XMB]: REBUG MODE ONLY
		if(xmb_mode==0) //switch to retail xmb //xmb_select == 2)
		{
		  result = stat("/dev_rebug/vsh/module/vsh.self.nrm", &statinfo);
		  if(result == 0 )
		  {
			result = stat("/dev_rebug/vsh/module/vsh.self.cexsp", &statinfo);
			if(result == 0 )
			{
				if(rename ("/dev_rebug/vsh/module/vsh.self","/dev_rebug/vsh/module/vsh.self.dexsp" )== 0 &&
					rename ("/dev_rebug/vsh/module/vsh.self.cexsp","/dev_rebug/vsh/module/vsh.self" )== 0)
				{
					if(menu_mode==0) //switch to CEX QA menu //menu_select == 1)
					{
						if(rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx","/dev_rebug/vsh/module/sysconf_plugin.sprx.dex" )== 0 &&
							rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx.cex", "/dev_rebug/vsh/module/sysconf_plugin.sprx") == 0){};
						/*result = stat("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", &statinfo);
						if(result == 0)
						{
							if(rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco","/dev_rebug/vsh/resource/sysconf_plugin.rco.dex" )== 0 &&
								rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", "/dev_rebug/vsh/resource/sysconf_plugin.rco") == 0){};
						}*/
					}
					strcpy(status, STR_MODRET);
					auto_reboot = 1;
				}
				else
				{
					strcpy(status, STR_TRYAGAIN);
				}
			}
			else
			{
				strcpy(status, STR_MODRETERR1);
			}
		  }
		  else
		  {
			strcpy(status, STR_MODRETERR2);
		  }
		}

		// [DEBUG XMB]: REBUG MODE ONLY
		if(xmb_mode==1) // switch to debug xmb //xmb_select == 1)
		{
		  result = stat("/dev_rebug/vsh/module/vsh.self.nrm", &statinfo);
		  if(result == 0 )
		  {
			result = stat("/dev_rebug/vsh/module/vsh.self.dexsp", &statinfo);
			if(result == 0 )
			{
				if(rename ("/dev_rebug/vsh/module/vsh.self","/dev_rebug/vsh/module/vsh.self.cexsp" )== 0 &&
					rename ("/dev_rebug/vsh/module/vsh.self.dexsp","/dev_rebug/vsh/module/vsh.self" )== 0)
				{
					strcpy(status, STR_MODDEB);
					auto_reboot = 1;
				}
				else
				{
					strcpy(status, STR_TRYAGAIN);
				}
			}
			else
			{
				strcpy(status, STR_MODDEBERR1);
			}
		  }
		  else
		  {
			strcpy(status, STR_MODDEBERR2);
		  }
		}

		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();
	}

	if(!strcmp(option, "menu_mode"))
	{
		// [DEBUG SETTINGS: MENU 1]: CEX
		if(menu_mode==0) //menu_select == 1)
		{
			result = stat("/dev_rebug/vsh/module/sysconf_plugin.sprx.cex", &statinfo);
			if(result == 0)
			{
				/*result = stat("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", &statinfo);
				if(result == 0)
				{
					if(rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco","/dev_rebug/vsh/resource/sysconf_plugin.rco.dex" )== 0 &&
						rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco.cex", "/dev_rebug/vsh/resource/sysconf_plugin.rco") == 0){};
				}*/

				if(rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx","/dev_rebug/vsh/module/sysconf_plugin.sprx.dex" )== 0 &&
					rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx.cex", "/dev_rebug/vsh/module/sysconf_plugin.sprx") == 0)
				{
					if(auto_reboot)
					{
						strcpy(status, STR_MODDEBSQA);
					}
					else
					{
						strcpy(status, STR_MODDEBSQAOK);
					}
				}
				else
				{
					strcpy(status, STR_MODDEBSQAERR1);
				}
			}
			else
			{
				strcpy(status, STR_MODDEBSQAERR2);
			}
		}
		// [DEBUG SETTINGS: MENU 2]: DEX
		if(menu_mode==1) //menu_select == 2)
		{
			result = stat("/dev_rebug/vsh/module/sysconf_plugin.sprx.dex", &statinfo);
			if(result == 0)
			{
				/*result = stat("/dev_rebug/vsh/resource/sysconf_plugin.rco.dex", &statinfo);
				if(result == 0)
				{
					if(rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco","/dev_rebug/vsh/resource/sysconf_plugin.rco.cex" )== 0 &&
						rename ("/dev_rebug/vsh/resource/sysconf_plugin.rco.dex", "/dev_rebug/vsh/resource/sysconf_plugin.rco") == 0){};
				}*/
				if(rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx","/dev_rebug/vsh/module/sysconf_plugin.sprx.cex" )== 0 &&
					rename ("/dev_rebug/vsh/module/sysconf_plugin.sprx.dex", "/dev_rebug/vsh/module/sysconf_plugin.sprx") == 0)

				{
					if(auto_reboot)
					{
						strcpy(status, STR_MODDEBSDEB);
					}
					else
					{
						strcpy(status, STR_MODDEBSDEBOK);
					}
				}
				else
				{
					strcpy(status, STR_MODDEBSDEBERR1);
				}
			}
			else
			{
				strcpy(status, STR_MODDEBSDEBERR2);
			}
		}

		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();
	}

	if(!strcmp(option, "cobra_mode"))
	{
		// [DEBUG SETTINGS: MENU 1]: CEX
		if((cobra_mode==1) /*&& (!is_cobra)*/) //menu_select == 1)
		{
			result = stat("/dev_rebug/rebug/cobra/stage2.cex.bak", &statinfo);
			if(result == 0)
			{
				rename ("/dev_rebug/rebug/cobra/stage2.cex.bak","/dev_rebug/rebug/cobra/stage2.cex" );

				result = stat("/dev_rebug/rebug/cobra/stage2.dex.bak", &statinfo);
				if(result == 0)
				{
						rename ("/dev_rebug/rebug/cobra/stage2.dex.bak", "/dev_rebug/rebug/cobra/stage2.dex");
				}

				{
					strcpy(status, STR_MODCOBENA /*"System will reboot now."*/);
					auto_reboot = 1;
				}
			}
		}
		// [DEBUG SETTINGS: MENU 2]: DEX
		else if((cobra_mode==0) /*&& (is_cobra)*/) //menu_select == 2)
		{
			result = stat("/dev_rebug/rebug/cobra/stage2.cex", &statinfo);
			if(result == 0)
			{
				rename ("/dev_rebug/rebug/cobra/stage2.cex","/dev_rebug/rebug/cobra/stage2.cex.bak" );

				result = stat("/dev_rebug/rebug/cobra/stage2.dex", &statinfo);
				if(result == 0)
				{
						rename ("/dev_rebug/rebug/cobra/stage2.dex", "/dev_rebug/rebug/cobra/stage2.dex.bak");
				}

				{
					strcpy(status, STR_MODCOBDIS /*"System will reboot now."*/);
					auto_reboot = 1;
				}
			}
		}
		else
		{
		strcpy(status, STR_ERRFLASH);
		}

		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();

		/*
		if(auto_reboot)
		{
			system_call_4(379,0x1200,0,0,0);
		}
        */
	}

	if(!strcmp(option, "swap_emu"))
	{
		if(!exist((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self.cobra"))
		{
			file_copy((char*)"/dev_rebug/ps2emu/ps2_netemu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self.cobra", 0);
			file_copy((char*)"/dev_rebug/ps2emu/ps2_emu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_emu.self.cobra", 0);
			file_copy((char*)"/dev_rebug/ps2emu/ps2_gxemu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_gxemu.self.cobra", 0);
		}
		if(!exist((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self") && !exist((char*)"/dev_usb000/rebug/ps2_netemu.self"))
		{
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_PS2ERR1, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			return;
		}
		//if(!exist((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self"))
		if(exist((char*)"/dev_usb000/rebug/ps2_netemu.self"))
		{
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_yes_no, STR_PS2CONT, dialog_fun1, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			if(dialog_ret==1)
			{
			unlink("/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self");
			unlink("/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_gxemu.self");
			unlink("/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_emu.self");
			file_copy((char*)"/dev_usb000/rebug/ps2_netemu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self", 0);
			file_copy((char*)"/dev_usb000/rebug/ps2_emu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_emu.self", 0);
			file_copy((char*)"/dev_usb000/rebug/ps2_gxemu.self", (char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_gxemu.self", 0);
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_PS2COP, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			}
		}
		if(exist((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self"))
		{
			unlink("/dev_rebug/ps2emu/ps2_netemu.self");
			unlink("/dev_rebug/ps2emu/ps2_gxemu.self");
			unlink("/dev_rebug/ps2emu/ps2_emu.self");
			if(swap_emu==0)
			{
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self", (char*)"/dev_rebug/ps2emu/ps2_netemu.self", 0);
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_emu.self", (char*)"/dev_rebug/ps2emu/ps2_emu.self", 0);
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_gxemu.self", (char*)"/dev_rebug/ps2emu/ps2_gxemu.self", 0);
			}
			else
			{
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_netemu.self.cobra", (char*)"/dev_rebug/ps2emu/ps2_netemu.self", 0);
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_emu.self.cobra", (char*)"/dev_rebug/ps2emu/ps2_emu.self", 0);
				file_copy((char*)"/dev_hdd0/game/RBGTLBOX2/USRDIR/ps2_gxemu.self.cobra", (char*)"/dev_rebug/ps2emu/ps2_gxemu.self", 0);
			}
			dialog_ret=0;
			cellMsgDialogOpen2( type_dialog_ok, STR_PS2SWPOK, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
		}
	}

	if((c_firmware==4.78f || c_firmware==4.80f) && !strcmp(option, "cfw_settings"))
	{

	if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.org") )
		cfw_settings=1;	//enabled
	else if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.cfw") )
		cfw_settings=0;	//disabled

			if(cfw_settings==1 && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.org") )
			{
				rename((char*)"/dev_rebug/vsh/module/xai_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak");
				rename((char*)"/dev_rebug/vsh/resource/xai_plugin.rco",
					(char*)"/dev_rebug/vsh/resource/xai_plugin.rco.bak");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.cfw");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.org",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.cfw");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.org",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml");
			strcpy(status, STR_XMBCFWDIS);
			auto_reboot = 1;
			}
			else
			if(cfw_settings==0  && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.cfw")
								&& exist((char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak")
								&& exist((char*)"/dev_rebug/vsh/resource/xai_plugin.rco.bak")
				)
			{
				rename((char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak",
					(char*)"/dev_rebug/vsh/module/xai_plugin.sprx");
				rename((char*)"/dev_rebug/vsh/resource/xai_plugin.rco.bak",
					(char*)"/dev_rebug/vsh/resource/xai_plugin.rco");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.org");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml.cfw",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network.xml");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.org");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml.cfw",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_network_tool2.xml");
			sprintf(status, "XMB CFW settings MOD is Enabled. The feature will be available via Network Column on XMB.");
			auto_reboot = 1;
			}
		if(auto_reboot)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();


		//system_call_4(379,0x1200,0,0,0);
		}
	}
	else if((c_firmware==4.81f) && !strcmp(option, "cfw_settings"))
	{

	if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.off") )
		cfw_settings=1;	//enabled
	else if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.on") )
		cfw_settings=0;	//disabled

			if(cfw_settings==1 && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.off") )
			{
				rename((char*)"/dev_rebug/vsh/module/xai_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.on");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.off",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml");
			sprintf(status, "XMB CFW settings MOD is Disabled. The plugin will be unloaded on next boot.");
			auto_reboot = 1;
			}
			else
			if(cfw_settings==0  && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.on")
								&& exist((char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak")
				)
			{
				rename((char*)"/dev_rebug/vsh/module/xai_plugin.sprx.bak",
					(char*)"/dev_rebug/vsh/module/xai_plugin.sprx");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.off");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml.on",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/cfw_settings.xml");
			sprintf(status, "XMB CFW settings MOD is Enabled. The feature will be available via Network Column on XMB.");
			auto_reboot = 1;
			}
		if(auto_reboot)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();


		//system_call_4(379,0x1200,0,0,0);
		}
	}/*
	if(!strcmp(option, "wmlp"))
	{

	if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.org") )
		wmlp=1;	//enabled
	else if( exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.wmlp") )
		wmlp=0;	//disabled

			if(wmlp==1 && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.org") )
			{
				rename((char*)"/dev_rebug/vsh/module/wboard_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/wboard_plugin.sprx.wmlp");
				rename((char*)"/dev_rebug/vsh/module/wboard_plugin.sprx.org",
					(char*)"/dev_rebug/vsh/module/wboard_plugin.sprx");					
				rename((char*)"/dev_rebug/vsh/module/explore_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/explore_plugin.sprx.wmlp");
				rename((char*)"/dev_rebug/vsh/module/explore_plugin.sprx.org",
					(char*)"/dev_rebug/vsh/module/explore_plugin.sprx");					
				rename((char*)"/dev_rebug/vsh/module/explore_category_game.sprx",
					(char*)"/dev_rebug/vsh/module/explore_category_game.sprx.wmlp");
				rename((char*)"/dev_rebug/vsh/module/explore_category_game.sprx.org",
					(char*)"/dev_rebug/vsh/module/explore_category_game.sprx");					
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml.wmlp");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml.org",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml");					
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml.wmlp");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml.org",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.wmlp");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.org",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml");				
			sprintf(status, "webMAN MOD LaunchPAD is Disabled. The plugin will be unloaded on next boot.");				
			auto_reboot = 1;
			}
			else
			if(wmlp==0  && exist((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.wmlp"))
			{									
				rename((char*)"/dev_rebug/vsh/module/wboard_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/wboard_plugin.sprx.org");
				rename((char*)"/dev_rebug/vsh/module/wboard_plugin.sprx.wmlp",
					(char*)"/dev_rebug/vsh/module/wboard_plugin.sprx");					
				rename((char*)"/dev_rebug/vsh/module/explore_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/explore_plugin.sprx.org");
				rename((char*)"/dev_rebug/vsh/module/explore_plugin.sprx.wmlp",
					(char*)"/dev_rebug/vsh/module/explore_plugin.sprx");					
				rename((char*)"/dev_rebug/vsh/module/explore_category_game.sprx",
					(char*)"/dev_rebug/vsh/module/explore_category_game.sprx.org");
				rename((char*)"/dev_rebug/vsh/module/explore_category_game.sprx.wmlp",
					(char*)"/dev_rebug/vsh/module/explore_category_game.sprx");					
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml.org");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml.wmlp",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game.xml");					
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml.org");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml.wmlp",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_game_tool2.xml");	
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.org");
				rename((char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml.wmlp",
					(char*)"/dev_rebug/vsh/resource/explore/xmb/category_psn.xml");
			sprintf(status, "webMAN MOD LaunchPAD is Enabled. The feature will be available on What's NEW on XMB.\n\n webMAN must be enabled to use this feature as well as PSN account registration on your user profile.");			
			auto_reboot = 1;
			}
		if(auto_reboot)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();


		//system_call_4(379,0x1200,0,0,0);
		}
	}	*/

	if(!strcmp(option, "xmb_plugin"))
	{

	if( exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex") )
		xmb_plugin=1;	//enabled
	else if( exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex") )
		xmb_plugin=0;	//disabled

			if(xmb_plugin==1 && exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex") )
			{
				rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex");
				rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex",
					(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
			sprintf(status, "HOST information will no longer be displayed on XMB.");
			auto_reboot = 1;
			}
			else
			if(xmb_plugin==0  && exist((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex")	)
			{
				rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx",
					(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.cex");
				rename((char*)"/dev_rebug/vsh/module/xmb_plugin.sprx.dex",
					(char*)"/dev_rebug/vsh/module/xmb_plugin.sprx");
			sprintf(status, "HOST information will be displayed on XMB.");
			auto_reboot = 1;
			}
		if(auto_reboot)
		{
		dialog_ret=0;
		cellMsgDialogOpen2( type_dialog_ok, (const char*) status, dialog_fun2, (void*)0x0000aaab, NULL );
		wait_dialog_simple();


		//system_call_4(379,0x1200,0,0,0);
		}
	}

	if(!strcmp(option, "gameos_flag"))
	{
		uint32_t dev_handle;
		int start_sector, sector_count;
		struct storage_device_info info;
		uint8_t buf[VFLASH5_SECTOR_SIZE * 16];
		struct os_area_header *hdr;
		struct os_area_params *params;
		uint32_t unknown2;

		dev_handle = 0;

		result = lv2_storage_get_device_info(VFLASH5_DEV_ID, &info);
		if (result) {
			goto done;
		}

		if (info.capacity < VFLASH5_HEADER_SECTORS) {
			goto done;
		}

		result = lv2_storage_open(VFLASH5_DEV_ID, &dev_handle);
		if (result) {
			goto done;
		}

		/* write os header and params */

		start_sector = 0;
		sector_count = VFLASH5_HEADER_SECTORS;

		memset(buf, 0, sizeof(buf));
		hdr = (struct os_area_header *) buf;
		params = (struct os_area_params *) (buf + OS_AREA_SEGMENT_SIZE);

		result = lv2_storage_read(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
		if (result) {
			goto done;
		}

		if (strncmp((const char *) hdr->magic, HEADER_MAGIC, sizeof(hdr->magic))) {
			goto done;
		}

		if (hdr->version != HEADER_VERSION) {
			goto done;
		}

		if (params->boot_flag == PARAM_BOOT_FLAG_OTHER_OS) {
			params->boot_flag = PARAM_BOOT_FLAG_GAME_OS;

			result = lv2_storage_write(dev_handle, 0, start_sector, sector_count, buf, &unknown2, 0);
			if (result) {
				goto done;
			}
		}
		else
		{
			cellMsgDialogOpen2( type_dialog_ok, STR_FLASHSET, dialog_fun2, (void*)0x0000aaab, NULL );
			wait_dialog_simple();
			lv2_sm_shutdown(0x8201, NULL, 0);
		}

		result = lv2_storage_close(dev_handle);

		lv2_sm_shutdown(0x8201, NULL, 0);

done:
		lv2_storage_close(dev_handle);
	}

	 if(!strcmp(option, "lang"))
	{
		save_activ();
		//update_language();
		char reload[128];
		sprintf(reload, "/dev_hdd0/game/%s/USRDIR/RELOAD.SELF", STR_APP_ID);
		launch_self2(reload);
	}
	
	check_settings();
}