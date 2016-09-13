//=====================================================================
// структуры записи графических примитивов
//=====================================================================
#ifndef _GPL_H_
#define _GPL_H_
//=====================================================================
#include "t.h"
//=====================================================================
#pragma pack( push, 1 )
//=====================================================================
// флаги и маски типа примитива
enum PrimFlag
{
  PF_LVISIBLE   = 0x8000,  // видимый при низком разрешении
  PF_HVISIBLE   = 0x4000,  // видимый при высоком разрешении
  PF_SVISIBLE   = 0x2000,  // видимый при выделении
  PF_STROKED    = 0x1000,  // штриховой
  PF_FILLED     = 0x0800,  // закрашенный
  PF_BOUND      = 0x0400,  // замкнутый
  PF_ROUND      = 0x0200,  // круглый
  PF_CHORD      = 0x0100,  // хорда
  PF_SMART      = 0x0080,  // изысканный
  PF_SHAPE      = 0x0040,  // формобразующий
  PF_FLAG_MASK  = 0xfff0,  // маска флагов
  PF_VISIBLE    = PF_LVISIBLE | PF_HVISIBLE,
  PF_SHAPE_MASK = ~( PF_VISIBLE | PF_SVISIBLE ),

  PF_NOTHING    = 0x0000,  // пусто
  PF_CONTAINER  = 0x0001,  // контейнер
  PF_POINT      = 0x0002,  // точка
  PF_TEXT       = 0x0003,  // текст
  PF_MULTILINE  = 0x0004,  // мультилинейный
  PF_POLYPOINT  = 0x0005,  // полилинейный
  PF_RECTANGLE  = 0x0006,  // прямоугольник
  PF_ELLIPSE    = 0x0007,  // эллипс/окружность
  PF_ARC        = 0x0008,  // дуга
  PF_ICON       = 0x0009,  // ссылка
  PF_FOOTNOTE   = 0x000a,  // выноска
  PF_TEXTBYLINE = 0x000b,  // текст по линии
  PF_TYPE_MASK  = 0x000f   // маска типа
};
//=====================================================================
struct point
{
  int x, y;
};
//=====================================================================
struct rect
{
  int left, top, right, bottom;
};
//=====================================================================
// параметры прямоугольника
struct RectParam : public rect
{
  short angle;  // угол поворота относительно левого верхнего угла
};
//=====================================================================
// параметры дуги
struct ArcParam : public RectParam
{
  short delta;  // угловой размер
};
//=====================================================================
// параметры ссылки
struct Icon : public point
{
  dword id; // идентификатор
};

struct ExIcon : Icon
{
  int xSize;
  int ySize;
};
//=====================================================================
//=====================================================================
struct PenParam
{
  dword style;
  word  width;
  word  color;
};

struct ExPenParam
{
  dword style;
  word  width;
  dword color;
};
//=====================================================================
struct BrushParam
{
  word  style;
  word  fill;
  dword hatch;
  word  fore;
  word  back;
};

struct ExBrushParam
{
  word  style;
  word  fill;
  dword hatch;
  dword fore;
  dword back;
};
//=====================================================================
// параметры строки
struct StringParam
{
  point offset;    // смещение
  short angle;     // угол поворота
  word  length;    // длина
  char  string[0]; // содержимое
};
//=====================================================================
// параметры шрифта
struct FontParam
{
  short number;    // номер
  short height;    // высота
  short weight;    // толщина
  byte  style;     // стиль
  dword color;     // цвет
};

struct FontParam1  // для версий >= 0x107
{
  short number;    // номер
  dword height;    // высота
  short weight;    // толщина
  byte  style;     // стиль
  short color;     // цвет
};
//=====================================================================
// параметры текста
struct Text
{
  point org;
  union StrPar
  {
    struct FulPar
    {
      union FntPar
      {
        FontParam   font;
        FontParam1  font1;
      } FontParams;
      StringParam string;
    } full;
    StringParam string;
  } param;
};
//=====================================================================
// параметры текста вдоль линии
struct TxtItem
{
  point org;
  short angle;
  char  letter[2];
};

struct TextByLine
{
  union FntPar
  {
    FontParam   font;
    FontParam1  font1;
  } FontParams;
  dword   length;
  TxtItem items[0];
};
//=====================================================================
// параметры полилинейного
struct PolyParam
{
  word  length;    // количество точек
  point points[0]; // массив координат
};
//=====================================================================
// параметры контейнера
struct Container
{
  word length;    // количество примитивов
  byte prims[0];  // массив примитивов
};
//=====================================================================
// параметры произвольного геометрического примитива
union Drawable
{
  PolyParam  polypoint;
  RectParam  rectangle;
  rect       ellipse;
  ArcParam   arc;
};
//=====================================================================
// параметры штрихового
struct Stroked
{
  PenParam pen;
  Drawable primParam;
};
//=====================================================================
// параметры закрашенного
struct Filled
{
  BrushParam brush;
  Drawable primParam;
};
//=====================================================================
// параметры окаймленного
struct Outlined
{
  PenParam pen;
  BrushParam brush;
  Drawable primParam;
};
//=====================================================================
// параметры объекта
struct PrimRecord
{
  word type;
  union
  {
    Container container;
    point      pnt;
    Icon       icon;
    Text       text;
    TextByLine textbyline;
    Stroked    stroked;
    Filled     filled;
    Outlined   outlined;
    Drawable   drawable;
  } param;
};
//=====================================================================
#pragma pack( pop )
//=====================================================================
#endif
