#ifndef __IDS_H__
#define __IDS_H__
//============================================
// Messages
//
//============================================
// Toolbars
#define IDB_MAP_VIEW            1000
#define IDB_MAP_OBJECTS         1001
#define IDB_MAP_MODE            1002
#define IDB_MAP_MODE_EXT        1003
#define IDB_MAP_MODE_EXT1       1004
#define IDB_MAP_FIND            1005
#define IDB_MAP_PRINT           1006
#define IDB_MAP_SEL_JMP         1007
//============================================
// Bitmaps
#define IDB_MAP_OBJECT          1040
#define IDB_L_ARROW             1041
#define IDB_R_ARROW             1042
#define IDB_U_ARROW             1043
#define IDB_D_ARROW             1044
#define IDB_LOAD_ARROW          1045
#define IDB_UNLOAD_ARROW        1046
#define IDB_UNDO                1047
#define IDB_REDO                1048
#define IDB_CW_ARROW            1049
#define IDB_CR_ARROW            1050
#define IDB_RANGE_UP_ARROW      1051
#define IDB_RANGE_DOWN_ARROW    1052
#define IDB_DELETE              1053
#define IDB_RUP_ARROW           1054
#define IDB_RDN_ARROW           1055
#define IDB_HORZ_SIZE_ALIGN     1056
#define IDB_VERT_SIZE_ALIGN     1057
#define IDB_FULL_SIZE_ALIGN     1058
#define IDB_GREEN               1059
#define IDB_BLUE                1060
//============================================
// Toolbars
#define IDW_TOOLBAR_BEG         1100
#define IDW_TOOLBAR_END         1116
//============================================
// Cursors
#define IDC_DRAW                1300
#define IDC_MOVE                1301
#define IDC_EDITOBJ             1302
#define IDC_INCSCALE            1303
#define IDC_DECSCALE            1304
#define IDC_GETSELECT           1305
#define IDC_ADDSELECT           1306
#define IDC_SUBSELECT           1307
#define IDC_PRINTFRAGMENT       1308
#define IDC_MYHAND              1309
#define IDC_TURN                1310
#define IDC_SELECTTRACE         1311
#define IDC_CHANGEPARAM         1312
#define IDC_QUERYPARAM          1313
#define IDC_MEGUREMENT          1314
#define IDC_RESTORE             1315
#define IDC_CHANGE_CLASS        1316

#define IDC_LTD_INSERT_CURSOR   1317
#define IDC_LTD_DELETE_CURSOR   1318
#define IDC_LTD_MOVE_CURSOR     1319
//============================================
// Multiple resource and Status panels
#define IDR_MAP_DOC             1400
#define IDM_STS0                1401
#define IDM_STS1                1402
#define IDM_STS2                1403
#define IDM_STS3                1404
//============================================
// Dialogs
#define IDD_PAGESETUP            1500
#define IDD_DOCSETUP             1501
#define IDD_OVERVIEW             1502
#define IDD_SETGRID              1503
#define IDD_ABSTRACT_DIALOG      1504
#define IDD_ICON_PARAM_DIALOG    1505
#define IDD_ICON_SELECT_DIALOG   1506
#define IDD_TEXT_PARAM_DIALOG    1507
#define IDD_LINE_PARAM_DIALOG    1508
#define IDD_POLYGON_PARAM_DIALOG 1509
#define IDD_EDIT_LAW_DIALOG      1510
#define IDD_LAYER_DIALOG         1511
#define IDD_INSTRUCT_DIALOG      1512
#define IDD_COORD_DIALOG         1513
#define IDD_GOTO_DIALOG          1514
#define IDD_SPACE_QUE_DIALOG     1515
#define IDD_LINE_TEMPL_DIALOG    1516
#define IDD_SUBLAYER_DEF_DIALOG  1517
#define IDD_SETDIST_DIALOG       1518
#define IDD_TOOLBAR_DIALOG       1519
#define IDD_SHAPE_DIALOG         1520
#define IDD_ANGLE_DIALOG         1521
#define IDD_COORD_GRID_DIALOG    1522
#define IDD_CONVERTOR_DIALOG     1523
#define IDD_CONVERT_COORD_DIALOG 1524
#define IDD_FIND_OBJECT_DIALOG   1525
#define IDD_SELEC_CLASSES_DIALOG 1526
#define IDD_GEO_COORD_DIALOG     1527
#define IDD_CHECKDLG             1528
#define IDD_SPL_DIALOG           1529
#define IDD_CALENDAR_DIALOG      1530
#define IDD_SELECT_OBJ_DIALOG    1531
#define IDD_COPY_PROJECT_DIALOG  1532
#define IDD_SELEC_CLAS_TAB_DLG   1533
#define IDD_REPER_DLG            1534
#define IDD_UTM_CONVERT_DIALOG   1535
#define IDD_CLONE_PARAM_DIALOG   1536
//=============================================================
// Timers
#define IDM_AUTOSCROLLTIMER 1700
#define IDM_TREESCROLLTIMER 1701
#define IDM_TEXTCURSORTIMER 1702
#define IDM_BLINKSELTIMER   1703
#define IDM_FULINKTIMER     1704
#define IDM_INSTRDLGTIMER   1705
//==============================================================
// AppCommands servised in common maner
#define IDM_APPCOMMAND_BEGIN     2000
#define IDM_APPCOMMAND_END       (IDM_APPCOMMAND_BEGIN+100)
#define IDM_GRID_MODE            2000
#define IDM_OBJEDIT_MODE         2001
#define IDM_CHANGEPARAM_MODE     2002
#define IDM_QUERYPARAM_MODE      2003
#define IDM_CROSS_MODE           2004
#define IDM_SUBSELECT_MODE       2005
#define IDM_ADDSELECT_MODE       2006
#define IDM_GETSELECT_MODE       2007
#define IDM_DECSCALE_MODE        2008
#define IDM_INCSCALE_MODE        2009
#define IDM_REISSHINA_MODE       2010
#define IDM_SETREISSHINA_MODE    2011
#define IDM_HAND_MODE            2012
#define IDM_STREETPLAN_MODE      2013
#define IDM_FIND_DIST_MODE       2014
#define IDM_LINE_MEASURE_MODE    2015
#define IDM_SQUARE_MEASURE_MODE  2016
#define IDM_SELECT_POLY_MODE     2017
#define IDM_CONNECT_MODE         2018
#define IDM_SET_COORD_GRID_MODE  2019
#define IDM_SET_GRID_MODE        2020
#define IDM_OVERVIEW_SELECT_MODE 2021
#define IDM_CHANGE_CLASS_MODE    2022
#define IDM_UNRECTPLAN_MODE      2023
#define IDM_RESTORE_MODE         2024
#define IDM_SHORTTRACE_MODE      2025 
#define IDM_BACKUP_PROJECT       2026 
#define IDM_MAGNIT_MODE          2027
#define IDM_MAGNIT_MODE1         2028
#define IDM_MAGNIT_MODE2         2029
#define IDM_MAGNIT_MODE2         2029
//============================================
// AppCommands servised in individual maner
#define IDM_OVERVIEW_MODE        2200
#define IDM_SELECT_OVERVIEW      2201
#define IDM_REFRESH_OVERVIEW     2202
#define IDM_PRINT_MAP_MODE       2203
#define IDM_PREVIEW_MAP_MODE     2204
#define IDM_COPY_MODE            2205
#define IDM_SELECT_MODE          2206
#define IDM_MOVE_MODE            2207
#define IDM_OBJCONTEXT_MODE      2208
#define IDM_DRAG_MODE            2209
#define IDM_CONTEXT_MODE         2210
#define IDM_SCALE_MODE           2211
#define IDM_PLACE_BMP_MODE       2212
#define IDM_GOTO                 2213
#define IDM_HELP                 2214
#define IDM_UNSELECT             2215
#define IDM_MAKE_STATISTICS      2216
#define IDM_MAKE_RGN_STATISTICS  2217
#define IDM_CONVERTOR            2218
#define IDM_SAVE_FULL_SCREEN     2219

#define IDM_OBJECT_ADMINISTATOR  2220
#define IDM_LINE_TEMPL_EDITOR    2221
#define IDM_SUBL_ADMINISTATOR    2222
#define IDM_BAR_ADMINISTATOR     2223
#define IDM_SPACE_QUERY          2224
#define IDM_EDITOR               2225
#define IDM_LOADER               2226
#define IDM_LAYERDLG             2227
#define IDM_SASDLG               2228
#define IDM_IMPROVE_POLYLINE     2229
#define IDM_DRAW_GRID            2230
#define IDM_GRIDSTEP             2231
#define IDM_ANGLESTEP            2232
#define IDM_DOCPRINTSETUP        2233
#define IDM_VIEWTRACE_MODE       2234
#define IDM_EDITTRACE_MODE       2235
#define IDM_CALCTRACE_MODE       2236
#define IDM_SUBLAYER             2237
#define IDM_SERIAL_BUILDING      2238
#define IDM_TRANSFOCATOR         2239
#define IDM_DISTANCE             2240
#define IDM_SENSIBILITY          2241
#define IDM_IMPROVE_PARAM        2242
#define IDM_SHOW_SCROLLBARS      2243
#define IDM_SELECT_LINKED        2244
#define IDM_SELECT_All_LINKED    2245

#define IDM_SHOW_AS_SELECTED     2246
#define IDM_TOOL_TIPS_CMD        2247
#define IDM_SHOW_BOX             2248
#define IDM_UNDO                 2249
#define IDM_REDO                 2250
#define IDM_LINE_ATTACHMENT      2251
#define IDM_TEST_REPER           2252
#define IDM_TEST_ALL_REPERS      2253

#define IDM_USE_GRID_CMD         2254
#define IDM_COORDGRID_MODE       2255
#define IDM_POLARCOORD_MODE      2256
#define IDM_SEL_JMP_HOME         2257
#define IDM_SEL_JMP_NEXT         2258
#define IDM_SEL_JMP_PREV         2259
#define IDM_SEL_JMP_END          2260

#define IDM_SET_NODE_QUERY       2261
#define IDM_AUTOSCROLL_MODE      2262

#define IDM_LOAD_TRACK_CMD       2263
#define IDM_LOAD_POINTS_CMD      2264

#define IDM_CUT_IN_CMD           2265
#define IDM_REM_CUT_IN_CMD       2266
#define IDM_TRACE_CLOSE          2267

#define IDM_SHORTTRACE_OPEN      2268
#define IDM_SHORTTRACE_CLOSE     2269
#define IDM_SHORTTRACE_CANCEL    2270
#define IDM_SHORTTRACE_BKSPACE   2271

#define IDM_USER_RIGHTS_OK       2272
#define IDM_USER_RIGHTS_OK_EXT   2273
#define IDM_USER_RIGHTS_CANCEL   2274

#define IDM_CLONE_LINES          2275

#define IDM_DOCCTL_BEGIN         2300
#define IDM_DOCCTL_END           (IDM_DOCCTL_BEGIN+100)
#define IDM_VIEWSCHEEM           (IDM_DOCCTL_BEGIN+0)
#define IDM_VIEWDOCUMENT         (IDM_DOCCTL_BEGIN+1)

#define IDM_FNDCTL_BEGIN         2400
#define IDM_FNDCTL_END           (IDM_FNDCTL_BEGIN+100)
#define IDM_CALCCANAL            (IDM_FNDCTL_BEGIN+0)
#define IDM_FINDOBJECT           (IDM_FNDCTL_BEGIN+1)
#define IDM_FINDBUILDING         (IDM_FNDCTL_BEGIN+2)
#define IDM_FIND_TAXOFON         (IDM_FNDCTL_BEGIN+3)
#define IDM_OBJHISTORY           (IDM_FNDCTL_BEGIN+4)
#define IDM_SPACE_QUERY_DEV      (IDM_FNDCTL_BEGIN+6)

#define IDM_SPECIAL_BEGIN        2500
#define IDM_SPECIAL_END          (IDM_SPECIAL_BEGIN+100)
#define IDM_SELECT_UZEL          (IDM_SPECIAL_BEGIN+0)
#define IDM_SPECIAL_CMD          (IDM_SPECIAL_BEGIN+1)
#define IDM_WELLCABEL            (IDM_SPECIAL_BEGIN+2)
#define IDM_SELECT_PROJECT       (IDM_SPECIAL_BEGIN+3)
#define IDM_UPDATE_STREET        (IDM_SPECIAL_BEGIN+4)
#define IDM_DEVORG               (IDM_SPECIAL_BEGIN+5)
#define IDM_APPABOUT             (IDM_SPECIAL_BEGIN+6)
#define IDM_CHANGE_UZEL          (IDM_SPECIAL_BEGIN+7)
//============================================
#define CLASS_RANGE              1024
#define SUBLAYER_CLASS           CLASS_RANGE
#define SUBLR_RANGE              4
#define SUBSTAT_CLASS            (CLASS_RANGE+SUBLR_RANGE+1)
#define SUBSTAT_RANGE            16
#define SUBFULL_RANGE            (SUBLR_RANGE+SUBSTAT_RANGE)
#define FULL_RANGE               (SUBSTAT_CLASS+SUBSTAT_RANGE+1)
#define BIT_FULL_RANGE           ((FULL_RANGE>>3)+1)
//============================================
#define IDM_NOTINMAPOBJECTS      3042
#define IDM_EXTFINDOBJECT        3043
#define IDM_EXTSETSCALE          3044
#define IDM_EXTSPACEQUE          3045
//============================================
#define IDM_CLASS_BEGIN         4000
#define IDM_CLASS_END           (IDM_CLASS_BEGIN+FULL_RANGE)
#define IDM_CONTEXT_BEGIN       IDM_CLASS_END
#define IDM_CONTEXT_END         (IDM_CONTEXT_BEGIN+FULL_RANGE)
//============================================
// Objects controls
#define IDM_OBJCTL_BEGIN        IDM_CONTEXT_END
#define IDM_OBJCTL_END          (IDM_OBJCTL_BEGIN+FULL_RANGE)
//============================================
// Debug
#define IDM_DEBUG1              IDM_OBJCTL_END
#define IDM_DEBUG2              (IDM_OBJCTL_END+1)
#define IDM_DEBUG3              (IDM_OBJCTL_END+2)
#define IDM_DEBUG4              (IDM_OBJCTL_END+3)
//============================================
// Messages
#define IDM_CONVERTOR_REPORT    (IDM_OBJCTL_END+4)
//============================================
#include "idg.h"
//============================================
#endif

