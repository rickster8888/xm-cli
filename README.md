# xm-cli

A CLI using XM netsdk toolkit for IP Cameras based on XM (such as Lutec)

# Build

## pre-requisites

* Linux XM netsdk libraries (https://download.xm030.cn/d/MDAwMDA3MzM=)

## notes

In my builds, the include path for netsdk.h is in ../include and for libxmnetsdk.so is in ../x64.

If you are using 32 bit linux OS, then use the x86 version of the library.  I haven't test that but there
should be no reason why it shouldn't work.

# Tools

## ipcamera-download

A simple tool to download videos stored on the camera.  The files can be watched on XM player or your device app.
Using standard media players will not have any audio, to use a player like vlc, you should convert with the XM convertor
(available on the XiongMai Tech website).

## ipcamera-diskinfo

A simple tool to show diskspace on the camera

# Supported hardware

Should work on any Xiongmai tech hardware, you will need the netsdk linux libaries (again on XiongMai tech website) but
only tested on Lutec (as that's all I have).

The vendors known to have XiongMai hardware are:

9Trading, Abowone, AHWVSE, ANRAN, ASECAM, Autoeye, AZISHN, A-ZONE, BESDER/BESDERSEC, BESSKY, Bestmo, BFMore, BOAVISION, BULWARK,
CANAVIS, CWH, DAGRO, datocctv, DEFEWAY, digoo, DiySecurityCameraWorld, DONPHIA, ENKLOV, ESAMACT, ESCAM, EVTEVISION, Fayele, 
FLOUREON , Funi, GADINAN, GARUNK, HAMROL, HAMROLTE, Highfly, Hiseeu, HISVISION, HMQC, IHOMEGUARD, ISSEUSEE, iTooner, JENNOV, 
Jooan, Jshida, JUESENWDM, JUFENG, JZTEK, KERUI, KKMOON, KONLEN, Kopda, Lenyes, LESHP, LEVCOECAM, LINGSEE, LOOSAFE, Lutec, 
MIEBUL, MISECU, Nextrend, OEM, OLOEY, OUERTECH, QNTSQ, SACAM, SANNCE, SANSCO, SecTec, Shell film, Sifvision / sifsecurityvision, 
smar, SMTSEC, SSICON, SUNBA, Sunivision, Susikum, TECBOX, Techage, Techege, TianAnXun, TMEZON, TVPSii, Unique Vision, 
unitoptek, USAFEQLO, VOLDRELI, Westmile, Westshine, Wistino, Witrue, WNK Security Technology, WOFEA, WOSHIJIA, WUSONLUSAN, 
XIAO MA, XinAnX, xloongx, YiiSPO, YUCHENG, YUNSYE, zclever, zilnk, ZJUXIN, zmodo, and ZRHUNTER.

I am unable to verify any of the above (except LUTEC).
