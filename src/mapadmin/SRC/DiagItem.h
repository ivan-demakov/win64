#ifndef DIAGITEM_H
#define DIAGITEM_H
//=====================================================================
int const DIAG_MAX_W = 40;
int const DIAG_MIN_W = 10;
//=====================================================================
// Параметр диаграммы для объекта
struct DiagItem
{
  float    m_Value;
  CString  m_Text;
  CPoint   m_Pg[3][4];
  // Конструктор
	DiagItem( 
		float Value = 0, // физическое значение параметра
		LPCSTR Text = 0  // текстовое представление параметра
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
// Элемент диаграммы
struct LegendItem
{
  COLORREF m_FillColor;
  CString  m_Prefix;
  CString  m_Postfix;
  // Конструктор элемента
	LegendItem( 
		COLORREF Color = 0, // цвет
		LPCSTR Prefix = 0,  // префикс
		LPCSTR Postfix = 0  // постфикс
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

// Легенда тематического выделения
class CLegend
{
private:
  COLORREF        m_FillColor;
  int             m_bRound;
  int             m_Width;
  float           m_PixelValue;
  LegenItemdArray m_Legend;
public:
  // Конструктор копии
	CLegend( CLegend* pLegend );
  // Конструктор 
  CLegend(
		COLORREF Color = 0,      // цвет заливки объекта  
		int bRound = 0,          // тип диаграммы: 0 - столбчатая, 1 - секторальная
		int Width = 0,           // щирина диаграммы в пикселах
		float PixelValue = 0,    // для столбчатых масштаб - пиксел на единицу параметра
    int nLegend = 0,         // количество элементов в диаграмме
		LegendItem* pLegend = 0  // указатель массива элементов
	);
  int GetSize() const { return m_Legend.GetSize(); }
	// Добавление нового элемента
	// Возвращает номер элемента >= 0, при ошибке -1
  int AddLegendItem( 
		COLORREF Color,    // цвет
		LPCSTR Prefix = 0, // префикс
		LPCSTR Postfix = 0 // постфикс
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

// Библиотека легенд
class CLegendCollection
{
private:
  LegenArray m_Content;

public:
	// добавление легенды в библиотеку
  int AddLegend( CLegend* pLegend );
  CLegend* GetLegend( int ndx );
};
//=====================================================================
#endif