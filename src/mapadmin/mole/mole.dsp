# Microsoft Developer Studio Project File - Name="mole" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=mole - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mole.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mole.mak" CFG="mole - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mole - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mole - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mole - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\inc3.5" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_CLIENT_APP" /D "_AFXDLL" /D "_WIDE_SERVER_FORMAT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "mole - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /vd0 /GR /GX /ZI /Od /I "..\inc3.5" /I "ñ:\sdk2002\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_WIDE_SERVER_FORMAT" /D "_CLIENT_APP" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdiplus.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mole - Win32 Release"
# Name "mole - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\SRC\ABSTRDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\APPMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\ASSOC.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\BARDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\BLDMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\BMPDEF.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\CBOX.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\CNCTMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\CONDUCT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\CONVDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\CSPOT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\Diagitem.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\DLG0.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DRAWITEM.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DRAWOBJ.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DRAWPRIM.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVEIWIDS.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVIEW0.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVIEW1.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVIEW2.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVIEW3.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DVPRINT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\DWIN.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\fulink.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\GRAPHDOC.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\gSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\IDNTREEB.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\INSTRDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\KSIKEY.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\KSIUTIL.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\LAWDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\LINETMPL.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\LOMEDIT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MAPMOLE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MAPSTORE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MAPUPDTR.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MathConst.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MATHUTIL.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MDOC.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MEASMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\merkator.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\MISCMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MOVEMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MSUTIL.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MVIEW.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\MYBAR.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\OBJDEF.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\OBJTREE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\PFMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\Polar.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\POSLIST.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\PRNDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\PTRECT.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\RECTMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\rgnspace.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\RSTRMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SELMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SHABDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SLTRMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SPCTREE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SPINMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SPLMODE.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\STDAFX.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\SUBLDLG.CPP
# End Source File
# Begin Source File

SOURCE=..\SRC\Undo.cpp
# End Source File
# Begin Source File

SOURCE=..\SRC\UTIL.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\SRC\dvprint.h
# End Source File
# Begin Source File

SOURCE=..\SRC\MathConst.h
# End Source File
# Begin Source File

SOURCE=..\SRC\mercator.h
# End Source File
# Begin Source File

SOURCE=..\SRC\polar.h
# End Source File
# Begin Source File

SOURCE=..\SRC\rgnspace.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\SRC\RES\ADDSEL.CUR
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\blue.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\change_c.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\changepa.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\cr_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\cw_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\d_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\decscale.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\delete.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\draw.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\editobj.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\externpa.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\full_siz.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\green.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\hand.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\horz_siz.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\icon2.ico
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\icon3.ico
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\icon4.ico
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\incscale.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\l_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\load_arr.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\ltd_dele.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\ltd_inse.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\ltd_move.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\map_mod1.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\map_mode.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\map_prin.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\map_user.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\MOLE.ICO
# End Source File
# Begin Source File

SOURCE=..\SRC\mole.RC
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\MOVE.CUR
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\printfra.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\r_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\range_do.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\range_up.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\restore.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\SELECT.CUR
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\selecttr.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\seljump.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\SUBSEL.CUR
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\turn.cur
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\u_arrow.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\unload_a.bmp
# End Source File
# Begin Source File

SOURCE=..\SRC\RES\vert_siz.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\gc.lib
# End Source File
# Begin Source File

SOURCE="..\lib\joke-3.5.1.lib"
# End Source File
# Begin Source File

SOURCE="..\lib\ksi-3.5.1.lib"
# End Source File
# Begin Source File

SOURCE=C:\SDK2002\Lib\ComDlg32.Lib
# End Source File
# End Target
# End Project
