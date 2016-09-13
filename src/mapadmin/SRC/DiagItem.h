#ifndef DIAGITEM_H
#define DIAGITEM_H
//=====================================================================
int const DIAG_MAX_W = 40;
int const DIAG_MIN_W = 10;
//=====================================================================
// �������� ��������� ��� �������
struct DiagItem
{
  float    m_Value;
  CString  m_Text;
  CPoint   m_Pg[3][4];
  // �����������
	DiagItem( 
		float Value = 0, // ���������� �������� ���������
		LPCSTR Text = 0  // ��������� ������������� ���������
	) :
    m_Value( Value ),
    m_Text( Text ? Text : "" )
  {}
  void Create( float Value = 0, LPCSTR Text = 0 )
  {
    m_Value = Value;
    m_Text  = Text ? Text : "";
  }
  void Create( DiagItem* pi )
  {
    m_Value = pi->m_Value;
    m_Text  = pi->m_Text ? pi->m_Text : "";
  }
};
//=====================================================================
// ������� ���������
struct LegendItem
{
  COLORREF m_FillColor;
  CString  m_Prefix;
  CString  m_Postfix;
  // ����������� ��������
	LegendItem( 
		COLORREF Color = 0, // ����
		LPCSTR Prefix = 0,  // �������
		LPCSTR Postfix = 0  // ��������
	):
    m_FillColor( Color ),
    m_Prefix( Prefix ? Prefix : "" ),
    m_Postfix( Postfix ? Postfix : "" )
  {}
  int operator==( LegendItem const& rItem ) const
  {
    return m_FillColor == rItem.m_FillColor &&
           m_Prefix    == rItem.m_Prefix    &&
           m_Postfix   == rItem.m_Postfix;
  }
  int operator!=( LegendItem const& rItem ) const { return !operator==( rItem ); }
};
//=====================================================================
typedef CArray< LegendItem, LegendItem& > LegenItemdArray;
#define MAX_LEGEND_SIZE 10

// ������� ������������� ���������
class CLegend
{
private:
  COLORREF        m_FillColor;
  int             m_bRound;
  int             m_Width;
  float           m_PixelValue;
  LegenItemdArray m_Legend;
public:
  // ����������� �����
	CLegend( CLegend* pLegend );
  // ����������� 
  CLegend(
		COLORREF Color = 0,      // ���� ������� �������  
		int bRound = 0,          // ��� ���������: 0 - ����������, 1 - ������������
		int Width = 0,           // ������ ��������� � ��������
		float PixelValue = 0,    // ��� ���������� ������� - ������ �� ������� ���������
    int nLegend = 0,         // ���������� ��������� � ���������
		LegendItem* pLegend = 0  // ��������� ������� ���������
	);
  int GetSize() const { return m_Legend.GetSize(); }
	// ���������� ������ ��������
	// ���������� ����� �������� >= 0, ��� ������ -1
  int AddLegendItem( 
		COLORREF Color,    // ����
		LPCSTR Prefix = 0, // �������
		LPCSTR Postfix = 0 // ��������
	);
  COLORREF GetFillColor() const { return m_FillColor; }
  void Draw( CDC* pDC, CPoint cp, DiagItem* pItem ) const;
  CString const* Detect( CSpot const& spot, DiagItem* pItem ) const;
  CBox CalcBox( CPoint cp, DiagItem* pItem ) const;
  int operator==( CLegend const& rLegend ) const;
  int operator!=( CLegend const& rLegend ) const { return !operator==( rLegend ); }
  CLegend& operator=( CLegend const& rLegend );
};
//=====================================================================
typedef CArray< CLegend, CLegend& > LegenArray;

// ���������� ������
class CLegendCollection
{
private:
  LegenArray m_Content;

public:
	// ���������� ������� � ����������
  int AddLegend( CLegend* pLegend );
  CLegend* GetLegend( int ndx );
};
//=====================================================================
#endif