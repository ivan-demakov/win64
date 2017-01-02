#include "stdafx.h"

#include "prndlg.h"
#include "mathutil.h"
#include "msutil.h"
#include "ksikey.h"
#include "util.h"
#include "dwin.h"
#include "mview.h"
#include "prndlg.h"
#include "mdoc.h"
#include "splmode.h"
#include "drawobj.h"
#include "ksi.h"
#include "ksi_type.h"
#include "ksiutil.h"
#include "dlg0.h"
#include "linetmpl.h"
#ifndef _FULL_PROJECT
  #include "mapmole.h"
#else
  #include "tgmapmole.h"
  #include "prjlib.h"
#endif
#include <gdiplus.h>
#include "mercator.h"
//=====================================================================
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//=====================================================================
static void
KsiAppend( ksi_obj& ksi_lst, ksi_obj ksi_elm )
{
  ksi_elm = KSI_LIST1( ksi_elm );

  if( KSI_NULL_P( ksi_lst ))
    ksi_lst = ksi_elm;
  else
    ksi_set_cdr_x( ksi_last_pair( ksi_lst ), ksi_elm );
}
//=====================================================================
static ksi_obj
ksi_rel( ksi_obj lst )
{
  ksi_obj pll = ksi_nil;
  ksi_obj cp = KSI_CAR( lst );
  lst = KSI_CDR( lst );
  while( !KSI_NULL_P( lst ))
  {
    ksi_obj np = KSI_CAR( lst );
    pll = ksi_cons ( ksi_cons( ksi_sub( KSI_CAR( cp ), KSI_CAR( np )),
                               ksi_sub( KSI_CDR( cp ), KSI_CDR( np ))), pll );
    cp = np;
    lst = KSI_CDR( lst );
  }
  return ksi_cons( cp, pll );
}
//=====================================================================
static ksi_obj
MakeFilled( CPoint* pPnt, int nPnt, CPoint* pRgn, int nRgn, ksi_obj fb_color, ksi_obj fill )
{
  ksi_obj ksi_cont = ksi_nil;
  LItemHash2 lh( nPnt + nRgn );
  SmartClipPlg( pPnt, nPnt, pRgn, nRgn, &lh, 1, 1 );
  SmartClipPlg( pRgn, nRgn, pPnt, nPnt, &lh, 1, 1 );

  ksi_obj ksi_pls = ksi_nil;
  for( CPoint p0, p1 ; lh.GetHead( p0, p1 ) ; )
  {
    for( ksi_obj ksi_pll = KSI_LIST2( KsiPoint( p0 ), KsiPoint( p1 ));;)
    {

      CPoint np;
      if( lh.GetNext( p0, np ))
        ksi_pll = ksi_cons( KsiPoint( p0 = np ), ksi_pll );
      else
      if( lh.GetNext( p1, np ))
        ksi_set_cdr_x( ksi_last_pair( ksi_pll ), KSI_LIST1( KsiPoint( p1 = np )));
      else
        break;
    }
    if( !KSI_NULL_P( ksi_pll ))
      ksi_pls = ksi_cons( ksi_rel( ksi_pll ), ksi_pls );
  }

  for( ; KSI_PLIST_P( ksi_pls ) ; ksi_pls = KSI_CDR( ksi_pls ))
    ksi_cont = ksi_cons(
      KSI_LIST5( keyFplg,
                 ksi_cons( keyPoly, KSI_CAR( ksi_pls )),
                 ksi_cons( keyRule, keyWndRule ),
                 fb_color,
                 fill ),
      ksi_cont );
  return ksi_cont;
}
//=====================================================================
ksi_obj
CStreetPlanMode::MakeKsiCont( CDrawContainer* pObj, int bHR, int bGrayScale, CPoint* pRgn, int nRgn, CRect& box )
{
  static char const* kt[][2] =
  {
    "bar",    "outlined-bar",
    "pie",    "outlined-pie",
    "chord",  "outlined-chord",
    "oval",   "outlined-oval",
    "circle", "outlined-circle"
  };
  
  CMapDoc* pDoc = pView->GetDocument();
  ksi_obj cont = ksi_nil;

  int tp = CDrawObject::GetDrawnClass();
  int bSAS = pObj->m_bShowAsSelected && ( pObj->m_bSASMap[tp>>3] & ( 1 << ( tp & 7 )));
  CDrawObject* pDrObj = CDrawObject::GetDrawnObject();
  int bSpc = pDrObj->IsSpecSelected();
  int bSel = pObj->IsSelected() && !bSpc;

  for( CVisiblePrim* pPrim = pObj->GetHead() ; pPrim ; pPrim = pPrim->GetNext())
  {
    CBox pbx;
    pPrim->CalcBoundBox( pbx );
    if( !Intersect( &box, &pbx ) ||
        !( bHR ? pPrim->IsHigVisible() : pPrim->IsLowVisible()) ||
        pPrim->IsSelVisible() && !pPrim->IsSelected() && !bSAS )
      continue;

    ksi_obj prim = *pPrim;
    ksi_obj type = KSI_CAR( prim );
    ksi_obj item = KSI_CDR( prim );
    ksi_obj val;

    if( bSel )
    {
		int i;
      for(i = CELEM( kt ) ;
           --i >= 0 && _stricmp( kt[i][0], KSI_KEY_PTR( type )) ; );
      if( i >= 0 )
      {
        ksi_set_car_x( prim, type = ksi_str02key( kt[i][1] ));
        item = ksi_assv_set_x( item, keyColor, translate_color( CItem::GetSpecColor( 0 , pObj->SelectFlag())));
      }
    }

    item = ksi_assv_remove_x( item, keySVis );
    item = ksi_assv_remove_x( item, keyLVis );
    item = ksi_assv_remove_x( item, keyHVis );

    if( KSI_EQ_P( type, keyCont ))
      item = KSI_CDR( MakeKsiCont(( CDrawContainer*)pPrim, bHR, bGrayScale, pRgn, nRgn, box ));

    if( KSI_EQ_P( type, keyIcon ))
    {
      int iconRef = ksi_num2int( ksi_assv_ref( item, keyReference ));
      item = ksi_assv_remove_x( item, keyReference );
      CBmpDef* pBD = pDoc->GetBmpDef( iconRef );
      CSize sz( pBD->GetBmpSize( CSplDialog::GetCurScaleEnb() ? pView->GetAltScale() : CSplDialog::GetScale()));
      CSizeItem si( sz.cx / 2, sz.cy / 2 );
      ksi_obj ref = ksi_cons( keyReference, ksi_str02string( pBD->GetBmpPath()));
      item = ksi_cons( ref, ksi_cons( si, item ));
    }

    //TextByLine 
    if( KSI_EQ_P( type, keyTextByLine ))
    {
      type = ksi_str02key( "multitext" );
      val = ksi_assv( keyTxtItem, item );
      ksi_set_car_x( val, ksi_str02key( "strings" ));
      for( ksi_obj itl = KSI_CDR( val ) ; KSI_PLIST_P( itl ) ; itl = KSI_CDR( itl ))
      {
        ksi_obj itm = KSI_CAR( itl );
        ksi_obj vec = KSI_CDR( itm );
        CPoint p( ksi_num2int( KSI_VEC_REF( vec, 0 )),
                  ksi_num2int( KSI_VEC_REF( vec, 1 )));
        int a = ksi_num2int( KSI_VEC_REF( vec, 2 ));
        p = TurnPoint( p - box.TopLeft(), -a );
        KSI_VEC_REF( vec, 0 ) = ksi_int2num( p.x );
        KSI_VEC_REF( vec, 1 ) = ksi_int2num( p.y );
      }

      ksi_obj crd = ksi_make_vector( ksi_two, ksi_zero );
      KSI_VEC_REF( crd, 0 ) = ksi_int2num( box.left );
      KSI_VEC_REF( crd, 1 ) = ksi_int2num( box.top );
      item = ksi_assq_set_x( item, keyCoord, crd );
      item = ksi_assq_set_x( item, keyAngle, ksi_zero );
    }  

    //color
    val = ksi_assv_ref( item, keyColor );
    int bGs = bGrayScale || KSI_EQ_P( type, keyText ) && bSpc;
    if( KSI_TRUE_P( val ))
      item = ksi_assq_set_x( item, keyColor, translate_color( bGs ? 0 : CItem::GetSpecColor( bSel ? 0 : ksi_num2int( val ), pObj->SelectFlag())));

    //fill
    ksi_obj hval = ksi_assv_ref( item, keyFill );
    ksi_obj ksi_fill;
    
    if( KSI_TRUE_P( hval ))
    {
      ksi_obj vec = ksi_make_vector( ksi_two, ksi_false );
      int bHatched = strcmp( KSI_STR_PTR( ksi_keyword2string( hval )), "solid" );
      KSI_VEC_REF( vec, 0 ) = bHatched ? keyFHatched : keyFSolid;
      KSI_VEC_REF( vec, 1 ) = bHatched ? hval : ksi_zero;
      ksi_set_cdr_x( prim, ksi_assv_set_x( item, keyFill, vec ));
      ksi_fill = ksi_cons( keyFill, vec );
      item = ksi_assq_set_x( item, keyFill, vec );
    }

    //fb_color
    val = ksi_assv_ref( item, keyFBColor );
    ksi_obj ksi_fbcolor;
    if( KSI_TRUE_P( val ))
    {
      int fcn = ksi_num2int( KSI_VEC_REF( val, 0 ));
      int bcn = ksi_num2int( KSI_VEC_REF( val, 1 ));
      int fcc = CItem::GetSpecColor( fcn, pObj->SelectFlag());
      int bcc = CItem::GetSpecColor( bcn, pObj->SelectFlag());
      ksi_obj fc = bSel ? KSI_MK_SINT( -1 ) : translate_color( bGrayScale ? -1 : bSpc ? pDrObj->GetSpecSelectColor() : fcc );
      ksi_obj bc = bSel ? KSI_MK_SINT( -1 ) : bcn >= 0 ? translate_color( bGrayScale ? -1 : bSpc ? pDrObj->GetSpecSelectColor() : bcc ) : KSI_VEC_REF( val, 1 );
      KSI_VEC_REF( val, 1 ) = bc;
      KSI_VEC_REF( val, 0 ) = fc;
      ksi_fbcolor = ksi_cons( keyFBColor, val );
      item = ksi_assq_set_x( item, keyFBColor, val );
    }

    //strok
    val = ksi_assv_ref( item, keyStrok );
    if( KSI_TRUE_P( hval ) && KSI_FALSE_P( val ) && bSel )
    {
      val = CStrokItem::translate_style( PS_SOLID, 0 );
      item = ksi_assq_set_x( item, keyStrok, val );
    }

    int nTmpl = KSI_VEC_P( val ) && KSI_NUM_P( KSI_VEC_REF( val, 1 )) ? 
      ksi_num2int( KSI_VEC_REF( val, 1 )) : 0;

    CLineTmpl* pTmpl = CLineTmpl::GetTemplate( nTmpl );
    if( pTmpl )
      KSI_VEC_REF( val, 1 ) = ksi_zero;

    //font
    val = ksi_assv_ref( item, keyFont );
    if( KSI_TRUE_P( val ))
      KSI_VEC_REF( val, 0 ) = ksi_str02string( pDoc->num2fontface( ksi_num2int( KSI_VEC_REF( val, 0 ))));

    //coord
    if( KSI_TRUE_P( ksi_assv_ref( item, keyCoord )))
    {
      if( !IsPointInPolygon( CPoint( pbx.left,  pbx.top    ), pRgn, nRgn, 0 ) ||
          !IsPointInPolygon( CPoint( pbx.left,  pbx.bottom ), pRgn, nRgn, 0 ) ||
          !IsPointInPolygon( CPoint( pbx.right, pbx.top    ), pRgn, nRgn, 0 ) ||
          !IsPointInPolygon( CPoint( pbx.right, pbx.bottom ), pRgn, nRgn, 0 ))
      continue;
    }

    //poly
    ksi_obj pll = ksi_assv_ref( item, keyPoly );

    if( KSI_TRUE_P( pll ))
    {
      int nPnt;
      CPoint* pPnt;
      ( pPnt = pPrim->GetPolygon( &nPnt )) ||
      ( pPnt = pPrim->GetPolyline( &nPnt ));

      // polygon
      if(( !pObj->IsSelected() || bSpc ) &&
          KSI_TRUE_P( ksi_memv( type, KSI_LIST3( keyOplg, keySplg, keyFplg ))))
      {
        ksi_obj fl = MakeFilled( pPnt, nPnt, pRgn, nRgn, ksi_fbcolor, ksi_fill );
        while( KSI_PLIST_P( fl ))
        {
          KsiAppend( cont, KSI_CAR( fl ));
          fl = KSI_CDR( fl );
        }
      }

      if( KSI_EQV_P( type, keyFplg ))
      {
        if( !pObj->IsSelected())
          continue;
        item = ksi_assv_remove_x( item, keyFill );
        item = ksi_assv_set_x( item, keyColor, translate_color( bGrayScale ? 0 : CItem::GetSpecColor( 0, pObj->SelectFlag())));
        ksi_obj strok = ksi_make_vector( KSI_MK_SINT( 4 ), ksi_zero );
        item = ksi_assv_set_x( item, keyStrok, strok );
        type = keyBpll;
      }

      int bSmart = KSI_EQV_P( type, keySplg );
      int bBound = KSI_TRUE_P( ksi_memv( type, KSI_LIST3( keyOplg, keySplg, keyBpll )));

      if( bBound )
      {
        item = ksi_assv_remove_x( item, keyFill );
        item = ksi_assv_remove_x( item, keyRule );
        item = ksi_assv_remove_x( item, keyFBColor );
        type = keyPolyline;
        item = ksi_assv_set_x( item, keyPoly, pll );
      }

      ksi_obj lns = ksi_nil;

      // polyline
      if( KSI_TRUE_P( ksi_memv( type, KSI_LIST2( keyPolyline, keyFootnote ))))
      {
        CViewDC dc( pView );  

        if( pTmpl )
        {
          int mi, ma;
          pTmpl->GetRange( &mi, &ma );
          int D = ma - mi;
          CSize d( D, 0 );
          dc.LPtoDP( &d );
          if( d.cx <= 2 )
            pTmpl = 0;
        }
        if( pTmpl )
        {
          for( int i = nPnt ; --i > 0 ; ++pPnt )
          {
            LItemHash lh( 1 );
            SmartClipLine( pPnt[0], pPnt[1], pRgn, nRgn, &lh, 1, 1 );
            CPoint p[2];
            while( lh.GetHead( p[0], p[1] ))
            {
              TGstream buf;
              pTmpl->SetImgBuf( &buf );
              pTmpl->Draw( &dc, p, 2 );
              pTmpl->SetImgBuf( 0 );
              int np = buf.pcount() / sizeof( int );
              if( !np )
                continue;

              int* pi = (int*)buf.str();

              for( int i = np ; --i >= 0 ; pi += 5 )
              {
                ksi_obj w = ksi_int2num( pi[0] );
                CPoint c0( pi[1], pi[2] ), c1( pi[3], pi[4] );
                LItemHash lh( 1 );
                SmartClipLine( c0, c1, pRgn, nRgn, &lh, 1, 1 );
                CPoint t0, t1;
                while( lh.GetHead( t0, t1 ))
                {
                  ksi_obj p0 = KsiPoint( t0 );
                  ksi_obj p1 = KsiPoint( t1 - t0 );
				  ksi_obj l;
                  for( l = lns ; KSI_PLIST_P( l ) && !KSI_EQV_P( w, KSI_CAR( KSI_CAR( l ))) ; l = KSI_CDR( l ));
                  if( KSI_NULL_P( l ))
                    lns = ksi_cons( KSI_LIST3( w, p0, p1 ), lns );
                  else
                    ksi_set_cdr_x( KSI_CAR( l ), ksi_cons( p0, 
                                   ksi_cons( p1, KSI_CDR( KSI_CAR( l )))));
                }
              }
            }
          }
          
          ksi_obj kst = ksi_vector2list( ksi_assv_ref( item, keyStrok ));
          for( ksi_obj l = lns ; KSI_PLIST_P( l ) ; l = KSI_CDR( l ))
          {
            ksi_obj itm = KSI_CAR( l );
            ksi_obj new_item = ksi_copy_tree( item );
            ksi_obj nst = ksi_list2vector( kst );
            KSI_VEC_REF( nst, 0 ) = KSI_CAR( itm );
            new_item = ksi_assv_set_x( new_item, keyStrok, nst );
            new_item = ksi_assv_set_x( new_item, keyPoly,  KSI_CDR( itm ));
            new_item = ksi_assv_set_x( new_item, keyAngle, ksi_zero );
            new_item = ksi_assv_set_x( new_item, keyCoord, ksi_make_vector( ksi_three, ksi_zero ));
            new_item = ksi_cons( keyLines, new_item );
            KsiAppend( cont, new_item );
          }
          continue;
        }
        else
        {
          LItemHash lh( 1 );
          if( bBound )
            SmartClipLine( pPnt[0], pPnt[nPnt-1], pRgn, nRgn, &lh, 1, 1 );
          for( int i = nPnt ; --i > 0 ; ++pPnt )
            SmartClipLine( pPnt[0], pPnt[1], pRgn, nRgn, &lh, 1, 1 );

          if( bSmart )
            lh.RemDup();

          lns = lh;

          if( KSI_NULL_P( lns ))
            continue;

          item = ksi_assv_set_x( item, keyPoly, lns );
          item = ksi_assv_set_x( item, keyAngle, ksi_zero );
          item = ksi_assv_set_x( item, keyCoord, ksi_make_vector( ksi_three, ksi_zero ));
          type = keyLines;
        }
      }

      // lines
      else
      if( KSI_EQV_P( type, keyLines ))
      {
        LItemHash lh( nPnt );
        for( int i = nPnt ; ( i -= 2 ) >= 0 ; pPnt += 2 )
          SmartClipLine( pPnt[0], pPnt[1], pRgn, nRgn, &lh, 1, 1 );

        lns = lh;
        if( KSI_NULL_P( lns ))
          continue;
        
        item = ksi_assv_set_x( item, keyPoly, lns );
        item = ksi_assv_set_x( item, keyAngle, ksi_zero );
        item = ksi_assv_set_x( item, keyCoord, ksi_make_vector( ksi_three, ksi_zero ));
      }
    }
    KsiAppend( cont, ksi_cons( type, item ));
  }

  return ksi_cons( keyCont, KSI_LIST1( cont ));
}
//=====================================================================
void
CStreetPlanMode::OutObj( CDrawObject* pObj, int sc, ksi_obj ksi_port, 
                         CPoint* pRgn, int nRgn, CRect& box )
{
  if( Intersect( pObj->GetBoundBox(), &box ))
  {
    CDrawObject::SetDrawnObject( pObj ); 
    int bHR = ( pObj->Status() & 0x80 ) && sc <= pView->m_HiResLimit;
    ksi_obj cont = MakeKsiCont( pObj->GetActualCont(), bHR, !CSplDialog::GetColorEnb(), pRgn, nRgn, box );
    CDrawObject::SetDrawnObject( 0 );
    if( KSI_PLIST_P( KSI_CAR( KSI_CDR( cont ))))
    {
      OutputKsiObject( KSI_LIST1( cont ), ksi_port );
      ksi_newline( ksi_port );
      ksi_newline( ksi_port );
    }
   }
}
//=====================================================================
int
CStreetPlanMode::SavePlan( CPoint* pRgn, int nRgn )
{
  CIntAssocMap* pct = &pView->GetDocument()->m_ViewColorTab;
  int const c = 176;
  static int k = -1;
  if( pct->TranslateKey( k ) < 0 )
  {
    k = pct->FindFreeKey();
    pct->Add( k, RGB( c, c, c ));
  }
  pView->GetLayer( 0 )->SetVisible( 1 );
  CIdent id( 0, 0, 0 );
  pView->HideObject( id, 1 );
  CDrawContainer* pc = new CDrawContainer;
  COutlinedPolygon* pl = new COutlinedPolygon(
    CFillItem( CForeBackItem( k, -1 ), BS_HATCHED, HS_DIAGCROSS ),
    CStrokItem( 0, PS_SOLID, 0 ), nRgn, pRgn );
  pl->SetFlags( SF_LOW_VISIBLE | SF_HIG_VISIBLE, SF_STATUS_MASK );
  pc->AddTail( pl );
  CDrawObject* pObj = new CDrawObject( id, 0, 0, 0, pc );
  pObj->CalcBoundBox();
  delete pView->m_ClipData.AddObject( pObj );
  pView->ShowObjectBox( pObj );

  if( AskBox( "Сохранить фрагмент?" ) != IDYES )
  {
    pView->HideObject( id, 1 );   
    return AM_RREMOVE | AM_RDONE;
  }

  CWaitCursor wc;
  CBox box( pRgn, nRgn );
  CPoint C( box.CenterPoint());
  int an = pView->GetModeAngle();
  int cs = pView->GetAltScale();
  int sc = CSplDialog::GetCurScaleEnb() ? cs : CSplDialog::GetScale();
  CSize size( ::MulDiv( box.Width(), cs, sc ), ::MulDiv( box.Height(), cs, sc ));

  CViewDC dc( pView );
  dc.LPtoDP( &size );

  CGraphData data;
  pView->GetDocument()->CreateLayers( &data );

  CRect r0( 0, 0, 0, 0 );
	CPoint tc( box.CenterPoint());
  sc /= MerkatorData.ScaleFactor( Point( tc.x, tc.y ));
  pView->LoadFromStore( &data, box, r0, sc, r0, sc );
	pView->AddLoadStaticSublayer( &data, sc, box ); 

  for( int t = FULL_RANGE ; --t >= 0 ; )
  {
    CLayer* psl = pView->m_ClipData.GetLayerByClass( t );
    CLayer* pdl = data.GetLayerByClass( t );
    pdl->SetVisible( psl->LayerOn());
  }

  if( an )
  {
    CDrawObject* pObj;
    box.Reset();
    for( int i = nRgn ; --i >= 0 ; box |= ( pRgn[i] = TurnPoint( pRgn[i], C, -an )));

    for( int n = 2 ; --n >= 0 ; )
      for( CLayer* pLayer = data.GetFirstLayer( n ) ; pLayer = data.GetNextLayer() ; )
        if( pLayer->LayerOn())
          for( pLayer->SetFirst() ; pObj = pLayer->FindNext() ; pObj->SpinOn( C, -an ));
  }

  int bDoSel = CSplDialog::GetSelectEnb();
  CObjList localSelect;  

  if( bDoSel )
    for( POSITION pos = pView->m_SelectList.GetHeadPosition() ; pos ; )
    {
      CDrawObject* pSel = pView->m_SelectList.GetNext( pos );
      CDrawObject* pObj = data.GetObjectPtr( pSel->TabIdent());
      if( pObj )
        data.RemoveObject( pObj );
			else
        pObj = new CDrawObject( pSel );
			if( pSel->IsSpecSelected())
        pObj->SetSpecSelection( pSel );
			else
        pObj->SetSelectFlag( pSel->SelectFlag());
      localSelect.AddTail( pObj );
    }  

  if( CSplDialog::IsToClipboard())
  {
    ReportHint( "Сохранение в буфере обмена..." );
    CMemDC memDC( &dc );

    CBitmap bmp;
    if( !bmp.CreateCompatibleBitmap( &dc, size.cx, size.cy ))
      ErrBox( "Выделен слишком большой фрагмент" );
    else
    {
      memDC.SelectObject( &bmp );
      memDC.SetViewportOrg( 0, 0);
      memDC.SetViewportExt( size );
      memDC.SetWindowOrg( box.TopLeft());
      memDC.SetWindowExt( box.Size());
      memDC.FillSolidRect( box, RGB( -1, -1, -1 ));

      CRgn rgn;
      memDC.LPtoDP( pRgn, nRgn );
      rgn.CreatePolygonRgn( pRgn, nRgn, WINDING );
      memDC.SelectClipRgn( &rgn );
      memDC.DPtoLP( pRgn, nRgn );
      memDC.m_bPrinting = 1;

      pView->GetDocument()->OnBeginPrinting( CSplDialog::GetColorEnb());
      CDrawContainer::SetHR( sc <= pView->m_HiResLimit );

      using namespace Gdiplus;
      GdiplusStartupInput gdiplusStartupInput;
      ULONG_PTR GdiplusToken;
      GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );
      data.Draw( &memDC, SUBLAYER|OBJLAYER );
      for( POSITION pos = localSelect.GetHeadPosition() ; pos ; )
           localSelect.GetNext( pos )->Draw( &memDC );
      GdiplusShutdown( GdiplusToken );
      pView->GetDocument()->OnEndPrinting();

      if( pView->OpenClipboard())
      {
        EmptyClipboard();
        ::SetClipboardData( CF_BITMAP, HBITMAP( bmp ));
        CloseClipboard();
      }
      else
        ErrBox( "Невозможно открыть буфер обмена" );
    }
  }

  if( CSplDialog::IsToFile())
  {
    ReportHint( "Сохранение в файле..." );
#ifndef _FULL_PROJECT
    CString splPath( App.GetProjectFilePath( "plan.spl" ));
#else
    CString splPath( GetProjectWriteDir() + "plan.spl" );
#endif;
	std::ofstream splFile( splPath, std::ios::out | std::ios::trunc, std::filebuf::sh_read | std::filebuf::sh_write );
    if( !splFile.good())
    {
      ErrBox( "Невозможно открыть файл\n"
              "для сохранения фрагмента уличного чертежа" );
      pView->HideObject( id, 1 );   
      return AM_RREMOVE | AM_RDONE;
    }

    splFile << "#(" << box.left << " " << box.top << " "
            << box.right << " " << box.bottom << ")\n\n";

    splFile.close();

    ksi_obj ksi_port = ksi_open_file( ksi_str02string( splPath ), ksi_str02string( "at" ));

    for( CLayer* pLayer = data.GetFirstLayer( 0 ) ; pLayer = data.GetNextLayer() ; )
      if( pLayer->LayerOn())
        for( pLayer->SetFirst() ; pObj = pLayer->FindNext() ;
             OutObj( pObj, sc, ksi_port, pRgn, nRgn, box ));

    for( POSITION pos = localSelect.GetHeadPosition() ; pos ;
         OutObj( localSelect.GetNext( pos ), sc, ksi_port, pRgn, nRgn, box ));

    ksi_close_port( ksi_port );
  }

  if( CSplDialog::IsToMetafile())
  {
    ReportHint( "Сохранение в метафйле..." );
    static char szFilter[] = "Meta Files (*.emf)|*.emf|All Files (*.*)|*.*||";

    CFileDialog* fdlg = new CFileDialog( 0, ".emf", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter );

    if( DoModalDlg( fdlg, CE_PAR_DLG ) == IDOK )
    {
      int hr = dc.GetDeviceCaps( LOGPIXELSX );
      int vr = dc.GetDeviceCaps( LOGPIXELSY );
      int hs = 2540;
      int vs = 2540;
     
      CRect S( 0, 0, 
               ::MulDiv( box.Width(),  SmInUnit * 1000, sc ),
               ::MulDiv( box.Height(), SmInUnit * 1000, sc ));

      CRect V( 0, 0, 
               ::MulDiv( S.Width(),  hr, hs ),
               ::MulDiv( S.Height(), vr, vs ));

      CMetaFileDC mdc;
      if( !mdc.CreateEnhanced( &dc, fdlg->GetPathName(), 0, 0 ))
        ErrBox( "Ошибка создания метафайла" );
      else
      {
        dc.SetMapMode( MM_ANISOTROPIC );
        dc.SetWindowOrg( box.TopLeft());
        dc.SetWindowExt( box.Size());
        dc.SetViewportOrg( 0, 0 );
        dc.SetViewportExt( V.Size());

        mdc.SetMapMode( MM_ANISOTROPIC );
        mdc.SetWindowOrg( box.TopLeft());
        mdc.SetWindowExt( box.Size());
        mdc.SetViewportOrg( 0, 0 );
        mdc.SetViewportExt( V.Size());
        mdc.SetAttribDC( dc.m_hAttribDC );
        mdc.m_bPrinting = 1;

        mdc.LPtoDP( pRgn, nRgn );
        CRgn rgn;
        rgn.CreatePolygonRgn( pRgn, nRgn, WINDING );
        mdc.SelectClipRgn( &rgn );

        pView->GetDocument()->OnBeginPrinting( CSplDialog::GetColorEnb());
        CDrawContainer::SetHR( sc <= pView->m_HiResLimit );
        using namespace Gdiplus;
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR GdiplusToken;
        GdiplusStartup( &GdiplusToken, &gdiplusStartupInput, 0 );
        mdc.FillSolidRect( &box, RGB( -1, -1, -1 ));
        data.Draw( &mdc, SUBLAYER|OBJLAYER );
        for( POSITION pos = localSelect.GetHeadPosition() ; pos ; )
             localSelect.GetNext( pos )->Draw( &mdc );
        GdiplusShutdown( GdiplusToken );
        pView->GetDocument()->OnEndPrinting();
        HENHMETAFILE hmf = CloseEnhMetaFile( mdc.GetSafeHdc());
        if( !hmf )
          ErrBox( "Ошибка закрытия метафайла" );
        else
          DeleteEnhMetaFile( hmf );
      }
      delete fdlg;
    }
  }

  pView->HideObject( id, 1 );   
  while( !localSelect.IsEmpty())
    delete localSelect.RemoveHead();;

  return AM_RREMOVE | AM_RDONE;
}
//=====================================================================
int
CStreetPlanMode::SavePlan( int R )
{
  CWaitCursor wc;
  ReportHint( "Расчет региона..." );

  if( CSplDialog::IsRectRgn())
  {
    CBox SB( m_pOwner->CalcSelectBox());
    int a = m_pOwner->GetModeAngle();
    if( a )
    {
      CSize cntr( SB.TopLeft());
      SB.Reset();
      for( POSITION pos = m_pOwner->m_SelectList.GetHeadPosition() ; pos ; )
      {
        CDrawObject* pCopy = m_pOwner->m_SelectList.GetNext( pos )->CopyObject();
        pCopy->SpinOn( cntr, -a );
        SB |= pCopy->GetBoundBox();
        delete pCopy;
      }
      SB.InflateRect( R, R );
      SetLT( TurnPoint( SB.TopLeft(), cntr, a ));
      SetRB( TopLeft() + SB.Size());
    }
    else
    {
      SB.InflateRect( R, R );
      SetLT( SB.TopLeft());
      SetRB( SB.BottomRight());
    }
    CPoint rgn[4];
    memcpy( rgn, Rect2Poly( *this, a ), sizeof rgn );
    return SavePlan( rgn, 4 );
  }

  CList< CPoint*, CPoint*& > rgl;

  CObjList ptl, pll;
  POSITION pos;
  for(pos = pView->m_SelectList.GetHeadPosition() ; pos ; )
  {
    CDrawObject* pObj = pView->m_SelectList.GetNext( pos );
    int ntp = SH_NOTHING;
    if( pObj->GetAnyPoints( &ntp, 0 ))
      if( ntp == SH_POINT )
        ptl.AddHead( pObj );
      else
        pll.AddHead( pObj );
  }

  POSITION pos0;
  for(pos0 = ptl.GetTailPosition() ; pos0 ; )
  {
    pos = pos0;
    CDrawObject* pOb0 = ptl.GetPrev( pos0 );
    CPoint* pPt0 = pOb0->GetAnyPoints();
    int bf = 0;
    for( POSITION pos1 = pll.GetHeadPosition() ; !bf && pos1 ; )
    {
      CDrawObject* pOb1 = pll.GetNext( pos1 );
      int nPnt;
      CPoint* pPnt = pOb1->GetAnyPoints( 0, &nPnt );
      while( --nPnt >= 0 && *pPnt++ != *pPt0 );
      bf = nPnt >= 0;
    }
    if( bf )
      ptl.RemoveAt( pos );
  }

  pll.AddHead( &ptl );

  while( !pll.IsEmpty())
  {
    CDrawObject* pObj = pll.RemoveHead();
    int nPnt;
    int nTyp;
    CPoint* pPnt = pObj->GetAnyPoints( &nTyp, &nPnt );

    TGstream spc;
    int nsp = MakeSpace( spc, pPnt, nPnt, R, nTyp == SH_POLYLINE ); 
    CPoint* spg = (CPoint*)spc.str();
    CPoint* pRgn = new CPoint[nsp+1];
    pRgn->x = nsp;
    memcpy( pRgn + 1, spg, nsp * sizeof( CPoint ));
    rgl.AddHead( pRgn );
  }

  POSITION pos1;
  for( pos0 = rgl.GetHeadPosition(); pos0 ; )
  {
    CPoint* p0 = rgl.GetAt( pos0 );
    pos1 = pos0;
    for( rgl.GetNext( pos1 ) ; pos1 ; rgl.GetNext( pos1 ))
    {
      CPoint* p1 = rgl.GetAt( pos1 );
      if( IsPolygonCrossPolygon( p0+1, p0->x, p1 + 1, p1->x ))
      {
        CPoint* p2 = SmartConnectPolygons( p0 + 1, p0->x, p1 + 1, p1->x );
        delete p0;
        delete p1;
        rgl.RemoveAt( pos1 );
        rgl.SetAt( pos0, p2 );
        break;
      }
    }
    if( !pos1 )
      rgl.GetNext( pos0 );
  }

  while( rgl.GetCount() > 1 )
  {
    CPoint* p0 = rgl.RemoveHead();
    CPoint* p1;
    POSITION s = 0;
    int n[2];
    int d = 0x7fffffff;
    for( POSITION ps1 = rgl.GetHeadPosition() ; ps1 ; rgl.GetNext( ps1 ))
    {
      CPoint* t1 = rgl.GetAt( ps1 );
      int ni[2];
      if( FindPointToPoint( p0 + 1, p0->x, t1 + 1, t1->x, &ni ))
      {
        int t = dist( p0[ni[0]+1], t1[ni[1]+1] );
        if( t < d )
        {
          d = t;
          p1 = t1;
          s = ps1;
          n[0] = ni[0];
          n[1] = ni[1];
        }
      }
    }
    if( s )
    {
      CPoint* p2 = SubConPolygons( p0 + 1, p0->x, p1 + 1, p1->x, n[0], n[1] );
      rgl.RemoveAt( s );
      delete p0;
      delete p1;
      rgl.AddHead( p2 );
    }
    else
      rgl.AddTail( p0 );
  }

  CPoint* pRgn = rgl.RemoveHead();
  int r = SavePlan( pRgn+1, pRgn->x );
  delete pRgn;
  return r;
}
//=====================================================================
int
CStreetPlanMode::Open()
{
  if( CSplDialog::IsAutoRgn())
  {
    if( m_pOwner->m_SelectList.IsEmpty())
      ErrBox( "Для вычисления региона необходимо\n"
              "выделить один или несколько объектов" );
    else
    {
      SavePlan( max( CSplDialog::GetExtraSize(), 10 ) * 100 / SmInUnit );
      m_pOwner->ShowSelectedNum();
    }
    m_pOwner->RemoveMode( this );
    return 0;
  }
  m_nMode = 2; 
  return 1; 
}
//=====================================================================
int
CStreetPlanMode::Term( int bDestroy )
{
  if( !bDestroy )
    m_pOwner->ShowSelectedNum();
  return 0;
}
//=====================================================================
int
CStreetPlanMode::OnLButtonDown( UINT& flags, CPoint& loc )
{
  if( !(( m_nMode ^= 1 ) & 1 ))
  {
    CPoint rgn[4];
    memcpy( rgn, Rect2Poly( *this, m_pOwner->GetModeAngle()), sizeof rgn );
    return SavePlan( rgn, 4 );
  }

  SetLT( loc );
  return AM_RDONE;
}
//=====================================================================
int
CStreetPlanMode::OnMouseMove( UINT& flags, CPoint& loc )
{
  if( !( m_nMode & 1 ))
    return AM_RPROPAGATE;

  CSize s( m_pOwner->m_WindowSize ); 
  SetRB( TurnPoint( loc, TopLeft(), -m_pOwner->GetModeAngle()));
  int w = Width(), h = Height();
  if( abs( w ) > s.cx )
    right = left + ( w > 0 ? s.cx : -s.cx );
  if( abs( h ) > s.cy )
    bottom = top + ( h > 0 ? s.cy : -s.cy );
  return AM_RPROPAGATE;
}
//=====================================================================
int
CStreetPlanMode::OnRButtonDown( UINT& flags, CPoint& loc )
{
  if( m_nMode == 2 )
    return AM_RREMOVE | AM_RDONE;

  m_nMode = 2;
  return AM_RDONE;
}
//=====================================================================
void
CStreetPlanMode::Draw( CDC* pDC )
{
  CRectMode::DrawA( pDC, m_pOwner->GetModeAngle());
}
//=====================================================================
