#ifndef _MAPUPDTR_H_
#define _MAPUPDTR_H_
//=====================================================================
#include "mapstore.h"
#include "ksi.h"
//=====================================================================
enum
{
  CM_REMOVE_ALL        = 0x00000001,
  CM_CHANGE            = 0x00000002,
  CM_IGNORE            = 0x00000004,
  CM_CREATE_NEW        = 0x00000008,
  CM_REMOVE            = 0x00000010,
  CM_MOVE              = 0x00000020,
  CM_CLASS_CONVERT     = 0x00000040,
  CM_TABLE_CONVERT     = 0x00000080,
  CM_NODE_CONVERT      = 0x00000100,
  CM_SPACE_SELECT      = 0x00000200,
  CM_IMPROVE           = 0x00000400,
  CM_AUTOBUILD         = 0x00000800,
  CM_RESTORE           = 0x00001000,
  CM_COORD_CONVERT     = 0x00002000,
  CM_REMOVE_DUPLICATES = 0x00004000,
  CM_SEPARATE_POLY     = 0x00008000,
  CM_LORD_FORMAT       = 0x00010000,
  CM_NUMERATE_FROM     = 0x00020000,
  CM_ASSAMBLY_BY_CLASS = 0x00040000,
  CM_ASSAMBLY_BY_TABLE = 0x00080000,
  CM_ASSAMBLY_ALL      = 0x00100000,
  CM_SQUIZZE           = 0x00200000,
  CM_OBJECTS_CONVERT   = 0x00400000,
  CM_SET_NODE          = 0x00800000,
  CM_FLTER_ID_DIAP     = 0x01000000,
  CM_FLTER_ID_LIST     = 0x02000000,
  CM_TIME_SELECT       = 0x04000000,
  CM_KEY_SELECT        = 0x08000000,
  CM_OUTPUT_GEO_METR   = 0x10000000,
  CM_OUTPUT_GEO_GRAD   = 0x20000000,
  CM_UTM_CONVERT       = 0x40000000,
	CM_OUTPUT_GEO        = CM_OUTPUT_GEO_METR | CM_OUTPUT_GEO_GRAD

};
//=====================================================================
class CConvDlg;
class CBox;

class MapUpdater : public MapStore
{
public:
  void SetSize( CSize s );
  int SetModified( int m );
  int GetModified(){ return ModifiedFlag; }
	static CPoint UpdatePoint( double x, double y, double cf[2][3] );
	static CPoint UpdatePoint( CPoint pt, double cf0[2][3], double utm[5], double cf1[2][3], int bAbs );

  ResultCode GPL2GPL( CConvDlg* pDlg,
                      char const* txtPath, char const* logPath,
                      ksi_obj ksi_ParamList,
                      int nMode, 
                      int tableNum,
                      int minIdent, int maxIdent,
                      CTime time1, CTime time2,
                      int nNode,
                      char* pClassMap,
                      double pcf0[2][3],
											double utm[4],
                      double pcf1[2][3],
                      CPoint* Rgn, CRect const* pBox );

  ResultCode MIF2GPL( CConvDlg* pDlg,
                      char const* txtPath, char const* logPath,
                      int m_Mode, int nNode,
                      int classNumber, int statusNumber, 
                      int tableNumber, int entryNumber,
                      CString subsNumber,
                      int keyNumber, CString keyStr,
                      double pcf[2][3],
                      int decomposeSize );

  ResultCode GPL2TXT( CConvDlg* pDlg,
                      char const* txtPath,
                      ksi_obj ksi_ParamList,
                      int nMode,
                      int tableNum,
                      int minIdent, int maxIdent,
                      CTime time1, CTime time2,
                      char* pClassMap,
                      double pcf[2][3],
 										  CPoint* Rgn, CRect const* pBox );

  ResultCode GPL2MIF( CConvDlg* pDlg,
                      char const* txtPath, 
                      ksi_obj ksi_ParamList,
                      int nMode,
                      int tableNum,
                      int minIdent, int maxIdent,
                      CTime time1, CTime time2,
                      char* pClassMap,
                      double pcf[2][3],
 										  CPoint* Rgn, CRect const* pBox );

  ResultCode TXT2GPL( CConvDlg* pDlg,
                      char const* txtPath, char const* logPath,
                      ksi_obj ksi_ParamList,
                      int nMode, int nNode,
                      char* pClassMap,
                      double pcf[2][3]);
private:
  int TestInclude( char* pClassMap, ClassType t );
  int UpdateObjectCoord( void** ppSrc, double pcf0[2][3], double utm[4], double pcf1[2][3], CPoint* Rgn = 0 );
  int ksi2gpl( int nMode, int nNode, ksi_obj ksi_val, CIdent Id,
		           double pcf[2][3], ofstream& log, int oldEntry );
  int MoveObjectToProject( void* pSrc, CBox* pBox );
  int IsObjectInProject( CBox* pBox );
  int UpdateKsiObjDef( ksi_obj ksi_tmp, ksi_obj ksi_log,
		                   ksi_obj ksi_ParamList,
                       int nMode, int nNode, 
                       ksi_obj ksi_class_cnv_proc,
                       ksi_obj ksi_table_cnv_proc,
                       ksi_obj ksi_node_cnv_proc,
                       char* pClassMap,
											 double pcf[2][3],
                       ofstream& log );
  ResultCode OutputLog( ofstream& out, int oldEntry, void* pSrc );
  ResultCode CreateLog( char const* errPath, ofstream& out );
  void OutputObject( ofstream& out, void* pData );
  int CreatePosTab( IdentDirectHashTab* pIdTab, int bDelMode, int bIdentMode );
  static int BoxInRgn( CRect const& Box, CPoint* Rgn );
  static int PntInRgn( CPoint const& Pt, CPoint* Rgn );
};
//====================================================================
#endif