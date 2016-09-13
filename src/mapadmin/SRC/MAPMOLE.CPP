#include "stdafx.h"

#include "linetmpl.h"
#include "bldmode.h"
#include "mapmole.h"
#include "dwin.h"
#include "mdoc.h"
#include "mview.h"
#include "drawitem.h"
#include "util.h"
#include "ksiutil.h"
#include "ksikey.h"
#include "msutil.h"
#include "ksi_type.h"
#include <direct.h>
#include  <io.h>
//=========================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=========================================================================
//=========================================================================
CMapmoleApp App;
//=========================================================================
BEGIN_MESSAGE_MAP( CMapmoleApp, CWinApp )
  ON_COMMAND( ID_FILE_NEW,         OnFileNew )
  ON_COMMAND( IDM_HELP,            CWinApp::OnHelp )
  ON_COMMAND( ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup )
END_MESSAGE_MAP()
//=========================================================================
CMapmoleApp::CMapmoleApp()
{}
//=========================================================================
LPCSTR
CMapmoleApp::GetProjectFilePath( LPCSTR name )
{
  strcpy( m_FilePath, m_pszProfileName );
  strcpy( strrchr( m_FilePath, '\\' ), Compose( "\\", name, 0 ));
  return m_FilePath;
}
//=========================================================================
CMapDoc*
CMapmoleApp::GetDocument()
{
  CMainWin* pMainWnd = (CMainWin*)AfxGetMainWnd();
  CFrameWnd* pFrameWnd = pMainWnd ? pMainWnd->GetActiveFrame() : 0;
  return pFrameWnd ? (CMapDoc*)pFrameWnd->GetActiveDocument() : 0;
}
//=========================================================================
CDocument*
CMapmoleApp::OpenDocumentFile( LPCSTR DocFileName )
{
  ASSERT( DocFileName );
  if( !DocFileName )
    return 0;

  if( !FileExist( DocFileName ))
    if( AskBox( Compose( "Файл проекта \"", DocFileName, "\" не найден!\n"
                         "Создать пустой проект?", 0 )) == IDYES )
      MapStore().CreateEmpty( m_ModulePath );
    else
      return 0;

  fstream DataStream;
  DataStream.open( DocFileName, ios::in|ios::out|ios::binary, filebuf::sh_read );
  if( DataStream.good())
    DataStream.close();
  else
  {
    ErrBox( "Проект занят или недоступен!" );
    return 0;
  }

  CMapDoc* pDoc = (CMapDoc*)CWinApp::OpenDocumentFile( DocFileName );

	ksi_obj kt;
	
	kt = ksi_nil;
  if( FileExist( GetProjectFilePath( "abstract.scm" )))
	{
    ksi_load( ksi_str02string( m_FilePath ));
  	kt = ksi_eval_str_with_catch( "abstract-param-list" );
		if( !KSI_LIST_P( kt ))
			ErrBox( "Некорректный код в \"abstract.scm\"" );
	}
	if( !KSI_PLIST_P( kt ))
  	ksi_defsym( "abstract-param-list", ksi_nil, ksi_current_env());

	kt = ksi_nil;
  if( FileExist( GetProjectFilePath( "concret.scm" )))
	{
    ksi_load( ksi_str02string( m_FilePath ));
  	kt = ksi_eval_str_with_catch( "concret-param-list" );
		if( !KSI_LIST_P( kt ))
			ErrBox( "Некорректный код в \"concret.scm\"" );
	}
	if( !KSI_PLIST_P( kt ))
	  ksi_defsym( "concret-param-list", ksi_nil, ksi_current_env());

  ksi_load( ksi_str02string( "ObjMap" ));

  LPTSTR m = pDoc->SetDocType();
  if( m )
  {
    ErrBox( m );
    pDoc->OnCloseDocument();
    return 0;
  }

  if( !pView->StartInit())
  {
    pDoc->OnCloseDocument();
    return 0;
  }

  m_pMainWnd->SetForegroundWindow();
  return pDoc;
}
//=========================================================================
void
CMapmoleApp::OnFileNew()
{
  CDocument* pDoc = OpenDocumentFile( GetProfileString( "General", "Project" ));
  if( !pDoc )
    ExitInstance();
}
//=========================================================================
static Ksi_Prim_Def ksi_func[] =
{
  { "put-hint",                 (ksi_proc_t)&ksi_put_hint,                 KSI_CALL_ARG1, 1 },
  { "get-reisshina-angle",      (ksi_proc_t)&ksi_get_reisshina_angle,      KSI_CALL_ARG0, 0 },
  { "set-reisshina-active",     (ksi_proc_t)&ksi_set_reisshina_active,     KSI_CALL_ARG1, 1 },
  { "set-reisshina-prev-point", (ksi_proc_t)&ksi_set_reisshina_prev_point, KSI_CALL_ARG1, 1 },
  { "get-all-of-class",         (ksi_proc_t)&ksi_get_all_of_class,         KSI_CALL_ARG2, 2 },
  { "num->font-face",           (ksi_proc_t)&ksi_num2fontface,             KSI_CALL_ARG1, 1 },
  { "get-bottom-class-list",    (ksi_proc_t)&ksi_get_bottom_list,          KSI_CALL_ARG1, 1 },
  { "is-derived-from-ex",       (ksi_proc_t)&ksi_is_derived_from_ex,       KSI_CALL_ARG2, 2 },
  { "get-shape-num",            (ksi_proc_t)&ksi_get_shape_num,            KSI_CALL_ARG0, 0 },
  { "draw-diff",                (ksi_proc_t)&ksi_draw_diff,                KSI_CALL_ARG1, 1 },
  { "draw-icon",                (ksi_proc_t)&ksi_draw_icon,                KSI_CALL_ARG3, 3 },
  { "len->string",              (ksi_proc_t)&ksi_len2string,               KSI_CALL_ARG1, 1 },
  { "set-cursor!",              (ksi_proc_t)&ksi_set_cursor,               KSI_CALL_ARG1, 1 },
  { "set-text-cursor",          (ksi_proc_t)&ksi_set_text_cursor,          KSI_CALL_ARG3, 3 },
  { "kill-text-cursor",         (ksi_proc_t)&ksi_kill_text_cursor,         KSI_CALL_ARG0, 0 },
  { "remove-selfcross",         (ksi_proc_t)&ksi_remove_selfcross,         KSI_CALL_ARG1, 1 },
  { "calc-string-width",        (ksi_proc_t)&ksi_calc_string_width,        KSI_CALL_ARG4, 3 },
  { "make-trace",               (ksi_proc_t)&ksi_mk_trace,                 KSI_CALL_ARG2, 2 },
  { "make-ring",                (ksi_proc_t)&ksi_mk_ring,                  KSI_CALL_ARG2, 2 },
  { "make-ellipsed",            (ksi_proc_t)&ksi_mk_ellipse,               KSI_CALL_ARG1, 1 },
  { "make-ovaled",              (ksi_proc_t)&ksi_mk_oval,                  KSI_CALL_ARG1, 1 },
  { "make-splined",             (ksi_proc_t)&ksi_mk_spline,                KSI_CALL_ARG2, 2 },
  { "calc-arc-param",           (ksi_proc_t)&calc_arc_param,               KSI_CALL_ARG1, 1 },
  { "make-arced",               (ksi_proc_t)&ksi_mk_arc,                   KSI_CALL_ARG1, 1 },
  { "make-sota",                (ksi_proc_t)&ksi_mk_sota,                  KSI_CALL_ARG1, 1 },
  { "true-dist",                (ksi_proc_t)&ksi_true_dist,                KSI_CALL_ARG2, 2 },
  { "find-inner-point",         (ksi_proc_t)&ksi_find_inner_point,         KSI_CALL_ARG1, 1 },
  { "test-point-object",        (ksi_proc_t)&ksi_test_point_object,        KSI_CALL_ARG2, 2 },
  { 0 }
};
//=========================================================================
static void
init_keys()
{
  static struct
  {
    ksi_obj* kp;
    char* kn;
  } kd[] =
  {
    { &keyOplg,      "outlined-polygon" }, { &keyFplg,      "polygon"  },
    { &keyFHatched,  "fill-hatched"     }, { &keyColor,     "color"    },
    { &keyBpll,      "bound-polyline"   }, { &keyDelta,     "delta"    },
    { &keyRoundrect, "roundrect"        }, { &keyPoint,     "point"    },
    { &keyReference, "reference"        }, { &keyStrok,     "strok"    },
    { &keySplg,      "smart-polygon"    }, { &keyCoord,     "coord"    },
    { &keyFootnote,  "footnote"         }, { &keyLines,     "lines"    },
    { &keyContents,  "contents"         }, { &keyAngle,     "angle"    },
    { &keyFSolid,    "fill-solid"       }, { &keyFill,      "fill"     },
    { &keyPolyline,  "polyline"         }, { &keyPoly,      "poly"     },
    { &keyRefpoint,  "refpoint"         }, { &keySize,      "size"     },
    { &keyIconref,   "icon-ref"         }, { &keyHint,      "hint"     },
    { &keyFBColor,   "fbcolor"          }, { &keyIcon,      "icon"     },
    { &keyEllipse,   "ellipse"          }, { &keyText,      "text"     },
    { &keyCont,      "container"        }, { &keyRect,      "rect"     },
    { &keyHVis,      "h-visible"        }, { &keyArc,       "arc"      },
    { &keyLVis,      "l-visible"        }, { &keyRule,      "rule"     },
    { &keySVis,      "s-visible"        }, { &keyTxtItem,   "txt-item" },
    { &keyString,    "string"           }, { &keyFont,      "font"     },
    { &keyWndRule,   "winding-rule"     }, { &keyTextByLine,"text-by-line" },
    { &keyTable,     "table"            }, { &keyEntry,     "entry"        },
		{	&keyObject,    "object"           }, { &keyClass,     "class"        },
		{ &keyStatus,    "status"           }, { &keyNode,      "node"         },
		{ &keyShape,     "shape"            }
  };

  for( int i = CELEM( kd ) ; --i >= 0 ; *kd[i].kp = ksi_ptr02key( kd[i].kn ));
}
//=========================================================================
int
CMapmoleApp::InitInstance()
{
  char const* cl = GetCommandLine();
  char tmp[_MAX_PATH], name[_MAX_FNAME];
  _splitpath( cl, tmp, tmp, name, tmp );

  if( cl[0] == '"' )
    ++cl;
  if( strchr( cl, '\\' ))
    strcpy( m_ModulePath, cl );
  else
  {
    int dl = GetCurrentDirectory( sizeof m_ModulePath, m_ModulePath );
    strcpy( m_ModulePath + dl, Compose( "\\", cl, 0 ));
  }
  char* pp = strrchr( m_ModulePath, '.' );

  char buf[256];
  char const pifn[] = "project.ini";

  strcpy( pp ? pp : m_ModulePath + strlen( m_ModulePath ), ".ini" );
  if( !FileExist( m_ModulePath ))
  {
    ErrBox( Compose( "Не найден файл", m_ModulePath, 0 ));
    return 0;
  }

  if( !GetPrivateProfileSection( "project", buf, sizeof buf, m_ModulePath ))
  {
    ErrBox( "Не указан проект!" );
    return 0;
  }

  pp = strrchr( m_ModulePath, '\\' );

  strcpy( pp, Compose( "\\", buf, "\\", pifn, 0 ));

  if( !FileExist( m_ModulePath ))
  {
    ErrBox( "Не найден файл project.ini!" );
    return 0;
  }

  delete (char*)m_pszProfileName;
  m_pszProfileName = new char[strlen( m_ModulePath ) + 1];
  strcpy( (char*)m_pszProfileName, m_ModulePath );

  pp = strrchr( strcpy( buf, m_ModulePath ), '\\' );
  pp[1] = 0;
  SetCurrentDirectory( buf );

  ksi_init( name, 0 );
  ksi_joke_win_init( m_hInstance, m_lpCmdLine );
  int lfwm = GetProfileInt( "KSI_ERRORS", "WriteLogFile", 2 );
  if( lfwm )
  {
    CString lfn( GetProfileString( "KSI_ERRORS", "KsiLogFileName", "KsiLog" ));
    lfn = App.GetProjectFilePath( lfn );
    TRY
    {
      CFile::Remove( lfn );
    }
    CATCH( CFileException, e )
    {
      #ifdef _DEBUG
      afxDump << "File " << lfn << " cannot be removed\n";
      #endif
    }
    END_CATCH
    ksi_open_errlog( errlog_info, ksi_str02string( lfn ));
  }
  else
  {
    ksi_obj err_handler = (ksi_obj)ksi_new_prim( "#<err-handler>", (ksi_proc_t)ksi_error_handler, KSI_CALL_REST0, 0 );
    ksi_add_error_handler( err_handler );
  }

  ksi_load_init_file( KSI_BOOT_FILE );
  ksi_load_init_file( KSI_INIT_FILE );
  ksi_reg_unit( ksi_func, 0 );
  init_keys();

#ifdef _AFXDLL
  Enable3dControls();
#else
  Enable3dControlsStatic();
#endif

  AddDocTemplate(
    new CSingleDocTemplate( IDR_MAP_DOC,
                            RUNTIME_CLASS( CMapDoc ),
                            RUNTIME_CLASS( CMainWin ),
                            RUNTIME_CLASS( CMapView )));

  CCommandLineInfo cmdInfo;
  ParseCommandLine( cmdInfo );

  if( !ProcessShellCommand( cmdInfo ))
    return 0;

  if( !m_pMainWnd )
  {
    ErrBox( "Ошибка создания главного окна!" );
    return 0;
  }

  OnIdle( 0 );
  return 1;
}
//=========================================================================
void
CMapmoleApp::WinHelp( DWORD dwData, UINT nCmd )
{
  CWinApp::WinHelp( dwData, HELP_CONTENTS );
}
//=========================================================================
LRESULT
CMapmoleApp::ProcessWndProcException( CException* e, const MSG* pMsg )
{
  e->ReportError();
  return pMsg->hwnd ? 0L : 1L;
}
//======================================================================
int
CMapmoleApp::GetProfileKeys( char const* pSection, char* buf, int size )
{
  GetPrivateProfileString( pSection, 0, "", buf, size, m_ModulePath );
  for( int n = 0 ; *buf ; buf += strlen( buf ) + 1, ++n );
  return n;
}
//======================================================================
CConductor*
CMapmoleApp::GetConductor()
{
  return CMapView::GetConductor();
}
//=========================================================================
CString
CMapmoleApp::GetSubbmpPath( int bLocal )
{
  if( bLocal < 2 )
  {
    CString t( bLocal ? "" : CMapView::GetConductor()->GetCurrentSubbmpPath());
    CString r( t.IsEmpty() ? GetProjectFilePath( bLocal ? "subbmplocal\\" : "subbmp\\" ) : t );

    if( t.IsEmpty() && _access( r, 04 ) == 0 )
      _mkdir( r );

    return r;
  }


#ifdef _FULL_PROJECT
  CString t( CMapView::GetConductor()->GetCurrentStaticbmpPath());
#else
  CString t;
#endif

  return t.IsEmpty() ? GetProfileString( "General", "subbmpstatic", "" ) : t;
}
//=========================================================================
