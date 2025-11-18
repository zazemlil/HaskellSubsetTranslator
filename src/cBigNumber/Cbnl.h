/* --------------------------------------------------------------
    Целые числа с неограниченной разрядной сеткой (версия 2.2).
    Операции над машинными числами.

    http://www.imach.uran.ru/cbignum

    Copyright 1999-2025, Р.Н.Шакиров, ИМаш УрО PAH.
    All Rights Reserved.

    Код предоставляется на условиях лицензии "public domain",
    т.е. допускается свободное применение, модификация и
    распространение кода при условии сохранения копирайта
    и добавления сведений о всех внесенных изменениях.

    ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ "КАК ЕСТЬ"
    БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, ЯВНЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ. ВЫ
    ИСПОЛЬЗУЕТЕ ЕГО НА СВОЙ СОБСТВЕННЫЙ СТРАХ И РИСК. АВТОР
    НЕ БУДЕТ НЕСТИ ОТВЕТСТВЕННОСТЬ ЗА УТРАТУ ДАННЫХ, УЩЕРБ,
    УБЫТКИ ИЛИ ЛЮБЫЕ ДРУГИЕ ВИДЫ ПОТЕРЬ ПРИ ЛЮБЫХ СПОСОБАХ
    ИСПОЛЬЗОВАНИЯ ДАННОГО ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ.
-------------------------------------------------------------- */
#ifndef _CBNL_H
#define _CBNL_H

#include <assert.h>
#include <limits.h>

//================================================
//      Соглашение для вызова внешних функций.
//================================================

/*
    Visual C++ 6.0 и выше в 32-разрядном режиме.
*/
#ifdef  _MSC_VER
#if     _MSC_VER >= 1200
#ifdef  _M_IX86
#if 0
#define _CBNL_C     __fastcall
#endif
#endif//_M_IX86
#endif//_MSC_VER
#endif//_MSC_VER

/*
    Прочие трансляторы.
*/
#ifndef _CBNL_C
#define _CBNL_C
#endif//_CBNL_C

//================================================
//      Тип CBNC для подсчета битов.
//================================================

#ifndef CBNC
#define CBNC int
#endif//CBNC

//================================================
//      Тип CBNL.
//================================================
/*
    CBNL_LONG_LONG  Тип CBNL отличается от long
*/

/*
    Visual C++ в 64-разрядном режиме (x64/Itanium).
*/
#ifdef  _MSC_VER
#ifdef  _WIN64
#define CBNL __int64
#define CBNL_MIN    _I64_MIN
#define CBNL_MAX    _I64_MAX
#define UCBNL_MAX   _UI64_MAX
#define CBNL_LONG_LONG
#include "Cbnl64.inl"
#endif//_WIN64
#endif//_MSC_VER

/*
    Трансляторы с поддержкой C++11.
*/
#ifndef CBNL
#ifdef  __cplusplus
#if     __cplusplus >= 201103L
#if LLONG_MAX != LONG_MAX
#define CBNL long long
#define CBNL_MIN    LLONG_MIN
#define CBNL_MAX    LLONG_MAX
#define UCBNL_MAX   ULLONG_MAX
#define CBNL_LONG_LONG
#include "Cbnl.inl"
#endif
#endif//__cplusplus
#endif//__cplusplus
#endif//CBNL

/*
    Прочие трансляторы.
*/
#ifndef CBNL
#define CBNL long
#define CBNL_MIN    LONG_MIN
#define CBNL_MAX    LONG_MAX
#define UCBNL_MAX   ULONG_MAX
#include "Cbnl.inl"
#endif//CBNL

//================================================
//      Поддержка встроенных функций транслятора
//      и арифметики двойной разрядности.
//================================================

/*
    Visual C++ 6.0 и выше в 32-разрядном режиме:
    _CBNL_CBNL  Используется арифметика двойной разрядности
    _CBNL_MUL   Реализуется умножение в двойное слово
*/
#ifdef  _MSC_VER
#if     _MSC_VER >= 1200
#ifdef  _M_IX86
#define _CBNL_CBNL  __int64
#define _CBNL_MUL
#endif//_M_IX86
#endif//_MSC_VER
#endif//_MSC_VER

/*
    Visual C++ 2005 и выше в 32 или 64-разрядном режиме:
    _CBNL_MI    Используются встроенные функции Microsoft
    _CBNL_MUL   Реализуется умножение в двойное слово
*/
#ifdef  _MSC_VER
#if     _MSC_VER >= 1400
#ifdef  _M_IX86
#define _CBNL_MI    32
#endif//_M_IX86
#ifdef  _WIN64
#define _CBNL_MI    64
#define _CBNL_MUL
#endif//_WIN64
#endif//_MSC_VER
#endif//_MSC_VER

/*
    Visual C++ 2013 и выше:
    _CBNL_CBI   Используются встроенные функции переноса/займа
*/
#ifdef  _MSC_VER
#if     _MSC_VER >= 1800
#define _CBNL_CBI
#endif//_MSC_VER
#endif//_MSC_VER

/*
    Visual C++ 2019 и выше:
    _CBNL_DI    Используются встроенные функции деления двойного слова
    _CBNL_DIV   Реализуется деление двойного слова
*/
#ifdef  _MSC_VER
#if     _MSC_VER >= 1920
#define _CBNL_DI
#define _CBNL_DIV
#endif//_MSC_VER
#endif//_MSC_VER

/*
    Все трансляторы:
    _CBNL_MUL   Реализуется переносимое умножение в двойное слово
*/
#if 1
#define _CBNL_MUL
#endif

//================================================
//      Ограничения на использование операций
//      процессора для вычислений.
//================================================

/*
    Выполнять умножение в двойное слово через аддитивные операции,
    без применения аппаратного умножения. Класс не будет использовать
    аппаратное умножение для вычислений.
*/
#if 0
#undef  _CBNL_MUL
#undef  _CBNL_MI
#endif

/*
    Выполнять деление двойного слова через аддитивные операции вместо
    поддерживаемой транслятором команды. Однословное аппаратное деление
    применяется для вычислений в зависимости от настройки в Cbignumf.h
*/
#if 0
#undef  _CBNL_DIV
#undef  _CBNL_DI
#endif

/* --------------------------------------------------------------
    Макро для встроенных функций компилятора.
    _addCBNL    (l1,l2,*p)  сложение с выдачей переноса
    _adcCBNL  (c,l1,l2,*p)  сложение c переносом, выдается перенос
    _subCBNL    (l1,l2,*p)  вычитание с выдачей займа
    _sbbCBNL  (c,l1,l2,*p)  вычитание с займом, выдается заем
    _muldCBNL   (l1,l2,*p)  умножение чисел со знаком в двойное число *p,ret
    _umuldCBNL  (l1,l2,*p)  умножение чисел без знака двойное число *p,ret
    _divdCBNL (ll,lh,l2,*p) деление двойного слова со знаком, остаток *p
    _udivdCBNL(ll,lh,l2,*p) деление двойного слова без знака, остаток *p
    _moddCBNL   (ll,lh,l2)  остаток от деления двойного слова со знаком
    _umoddCBNL  (ll,lh,l2)  остаток от деления двойного слова без знака
    _ushldCBNL  (ll,lh,sh)  беззнаковый сдвиг двойного слова влево  (старшее)
    _ushrdCBNL  (ll,lh,sh)  беззнаковый сдвиг двойного слова вправо (младшее)
    _ushld1CBNL (ll,lh)     беззнаковый сдвиг двойного слова влево на бит
    _ushrd1CBNL (ll,lh)     беззнаковый сдвиг двойного слова вправо на бит
    _ushlCBNL   (l,sh)      беззнаковый сдвиг влево  (для читаемости кода)
    _ushrCBNL   (l,sh)      беззнаковый сдвиг вправо (для читаемости кода)
    _btCBNL     (num,sh)    извлечение бита по номеру разряда
    _ulzcntCBNL (num)       подсчет старших нулевых бит
    _ubsfCBNL   (*p,num)    поиск младшего значащего бита (Visual C++)
    _ubsrCBNL   (*p,num)    поиск старшего значащего бита (Visual C++)
-------------------------------------------------------------- */

#ifdef  _CBNL_MI

#include <intrin.h>

/*
    Операции с переносом/займом.
*/
#ifdef  _CBNL_CBI

#if     _CBNL_MI == 32
#pragma intrinsic(_addcarry_u32)
#pragma intrinsic(_subborrow_u32)
#define _addCBNL(l1,l2,p)   _addcarry_u32(0,l1,l2,(unsigned*)p)
#define _adcCBNL(c,l1,l2,p) _addcarry_u32(c,l1,l2,(unsigned*)p)
#define _subCBNL(l1,l2,p)   _subborrow_u32(0,l1,l2,(unsigned*)p)
#define _sbbCBNL(c,l1,l2,p) _subborrow_u32(c,l1,l2,(unsigned*)p)
#endif//_CBNL_MI

#if     _CBNL_MI == 64
#pragma intrinsic(_addcarry_u64)
#pragma intrinsic(_subborrow_u64)
#define _addCBNL(l1,l2,p)   _addcarry_u64(0,l1,l2,p)
#define _adcCBNL(c,l1,l2,p) _addcarry_u64(c,l1,l2,p)
#define _subCBNL(l1,l2,p)   _subborrow_u64(0,l1,l2,p)
#define _sbbCBNL(c,l1,l2,p) _subborrow_u64(c,l1,l2,p)
#endif//_CBNL_MI

#endif//_CBNL_CBI

/*
    Умножение в двойное слово доступно только в 64-разрядном режиме.
    _CBNL_MUL   Реализуется умножение в двойное слово
*/
#ifdef  _CBNL_MUL
#if     _CBNL_MI == 64
#pragma intrinsic(_mul128)
#pragma intrinsic(_umul128)
#define _muldCBNL   _mul128
#define _umuldCBNL  _umul128
#endif//_CBNL_MI
#endif//_CBNL_MUL

/*
    Деление двойного слова.
*/
#ifdef  _CBNL_DI

#undef  _divdCBNL
#undef  _udivdCBNL
#undef  _moddCBNL
#undef  _umoddCBNL

#if     _CBNL_MI == 32
#pragma intrinsic(_div64)
#pragma intrinsic(_udiv64)
#define _divdCBNL   _divdCBNL_c
#define _udivdCBNL  _udivdCBNL_c
#endif//_CBNL_MI

#if     _CBNL_MI == 64
#pragma intrinsic(_div128)
#pragma intrinsic(_udiv128)
#define _divdCBNL(ll,lh,l2,p)   _div128(lh,ll,l2,p)
#define _udivdCBNL(ll,lh,l2,p)  _udiv128(lh,ll,l2,p)
#endif//_CBNL_MI

#define _moddCBNL   _moddCBNL_c
#define _umoddCBNL  _umoddCBNL_c

#endif//_CBNL_DI

/*
    Сдвиг двойного слова доступен только в 64-разрядном режиме.
*/
#if     _CBNL_MI == 64
#pragma intrinsic(__shiftleft128)
#pragma intrinsic(__shiftright128)
#define _ushldCBNL(ll,lh,sh) __shiftleft128(ll,lh,(unsigned char)(sh))
#define _ushrdCBNL(ll,lh,sh) __shiftright128(ll,lh,(unsigned char)(sh))
#if 0 // Not effective
#define _ushld1CBNL(ll,lh)   __shiftleft128(ll,lh,1)
#define _ushrd1CBNL(ll,lh)   __shiftright128(ll,lh,1)
#endif
#endif//_CBNL_MI

/*
    Проверка и поиск битов.
*/
#if     _CBNL_MI == 32
#if 0 // Not effective
#pragma intrinsic(_bittest)
#define _btCBNL(num,sh)    _bittest(&(num),sh)
#endif
#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#define _ubsfCBNL _BitScanForward
#define _ubsrCBNL _BitScanReverse
#endif//_CBNL_MI

#if     _CBNL_MI == 64
#if 0 // Not effective
#pragma intrinsic(_bittest64)
#define _btCBNL(num,sh)    _bittest64(&(num),sh)
#endif
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#define _ubsfCBNL _BitScanForward64
#define _ubsrCBNL _BitScanReverse64
#endif//_CBNL_MI

/*
    Intel Haswell+ & AMD Excavator+
*/
#ifdef  __AVX2__

#if     _CBNL_MI == 32
#pragma intrinsic(__lzcnt)
#define _ulzcntCBNL __lzcnt
#endif//_CBNL_MI

#if     _CBNL_MI == 64
#pragma intrinsic(__lzcnt64)
#define _ulzcntCBNL __lzcnt64
#endif//_CBNL_MI

#endif//__AVX2__

#endif//_CBNL_MI

#if 0 // Для отладки
#undef  _muldCBNL
#undef  _umuldCBNL
#endif

#if 0 // Для отладки
#undef  _divdCBNL
#undef  _udivdCBNL
#undef  _moddCBNL
#undef  _umoddCBNL
#endif

//================================================
//      Внешние и встраиваемые функции.
//================================================

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

//      Медленное умножение чисел в двойное слово аддитивными операциями.
//      Реализовано в Cbignumf.inl для режима без аппаратного умножения,
//      во всем классе применяется при отключении макро _CBNL_MUL

#ifndef _CBNL_MUL

#undef  _muldCBNL
#define _muldCBNL   _muldCBNL_c
#undef  _umuldCBNL
#define _muldCBNL   _muldCBNL_c

CBNL   _CBNL_C  _muldCBNL_c  (CBNL, CBNL, CBNL*);
unsigned
CBNL   _CBNL_C  _umuldCBNL_c (unsigned CBNL, unsigned CBNL, unsigned CBNL*);

#endif//_CBNL_MUL

//      Деление двухсловного числа на однословное аддитивными операциями.
//      Реализовано в Cbignumf.inl для режима без аппаратного деления
//      что почти не влияет на скорость деления неограниченных чисел.
//      Во всем классе применяется при отключении макро _CBNL_DIV
//      или автоматически при недоступности аппаратной операции.

#ifndef _CBNL_DIV

#undef  _divdCBNL
#define _divdCBNL   _divdCBNL_c
#undef  _udivdCBNL
#define _udivdCBNL  _udivdCBNL_c
#undef  _moddCBNL
#define _moddCBNL   _moddCBNL_c
#undef  _umoddCBNL
#define _umoddCBNL  _umoddCBNL_c

CBNL   _CBNL_C  _divdCBNL_c  (CBNL, CBNL, CBNL, CBNL*);
unsigned
CBNL   _CBNL_C  _udivdCBNL_c (unsigned CBNL, unsigned CBNL, unsigned CBNL,
                                                            unsigned CBNL*);
CBNL   _CBNL_C  _moddCBNL_c  (CBNL, CBNL, CBNL);
unsigned
CBNL   _CBNL_C  _umoddCBNL_c (unsigned CBNL, unsigned CBNL, unsigned CBNL);

#endif//_CBNL_DIV

//      У некоторых функций имена с Long вместо CBNL, т.к. они
//      первоначально создавались для 32-разрядной версии класса.

//      Подсчет битов в числе.

int    _CBNL_C  cLongBits       (CBNL);         // Значащие биты.
int    _CBNL_C  cULongBits      (unsigned CBNL);
int    _CBNL_C  cLongExBits     (CBNL);         // Младшие 0-биты.
int    _CBNL_C  cLongExactLog2  (CBNL);         // Log2 для 2**n.

//      Генератор случайных чисел.

unsigned
CBNL   _CBNL_C  cLongRandom (unsigned long (*pfnRand)());

#ifdef  __cplusplus
}
#endif/*__cplusplus*/

#ifndef CBNL_LONG_LONG
inline unsigned
CBNL  _CBNL_C   cLongRandom (unsigned long (*pfnRand)())
{
  return (unsigned CBNL)(*pfnRand)();
}
#endif//CBNL_LONG_LONG

//      Умножение беззнаковых чисел CBNL в двухсловный массив.

inline unsigned
CBNL   _CBNL_C  cULongMul (                     // Умножение p = p1 * p2.
                unsigned CBNL   l1,             // Беззнаковое множимое.
                unsigned CBNL   l2,             // Беззнаковый множитель.
                unsigned CBNL   p [2]           // Беззнаковый результат.
        )                                       // Выдает младшее слово
{                                               // результата.
#ifdef  _umuldCBNL
  return (p [0] = _umuldCBNL (l1, l2, p + 1));  // Реализовано в _umuldCBNL.
#else //_umuldCBNL
#ifdef  _CBNL_CBNL
  *(unsigned _CBNL_CBNL *)p = (unsigned _CBNL_CBNL)l1 * l2;
#else //_CBNL_CBNL
  p [0] = (l1 & (((CBNL)1 << (CHAR_BIT * sizeof (CBNL) / 2)) - 1)) *
          (l2 & (((CBNL)1 << (CHAR_BIT * sizeof (CBNL) / 2)) - 1));
  p [1] = (l1 >> (CHAR_BIT * sizeof (CBNL) / 2)) *
          (l2 >> (CHAR_BIT * sizeof (CBNL) / 2));
  unsigned CBNL lh = p [1];
  unsigned CBNL lmac = *(CBNL*)((unsigned char *)p + sizeof (CBNL) / 2);
  unsigned CBNL lm = lmac;
  lmac += (l1 & (((CBNL)1 << (CHAR_BIT * sizeof (CBNL) / 2)) - 1)) *
          (l2 >> (CHAR_BIT * sizeof (CBNL) / 2));
  lh += (lmac < lm ? (CBNL)1 << ((CHAR_BIT * sizeof (CBNL) / 2)) : 0);
  lm = lmac;
  lmac += (l2 & (((CBNL)1 << (CHAR_BIT * sizeof (CBNL) / 2)) - 1)) *
          (l1 >> (CHAR_BIT * sizeof (CBNL) / 2));
  lh += (lmac < lm ? (CBNL)1 << ((CHAR_BIT * sizeof (CBNL) / 2)) : 0);
  p [1] = lh;
  *(CBNL*)((unsigned char *)p + sizeof (CBNL) / 2) = lmac;
#endif//_CBNL_CBNL
  return p [0];
#endif//_umuldCBNL
}

//      Умножение чисел CBNL в двухсловный массив.

inline
CBNL   _CBNL_C  cLongMul (                      // Умножение p = p1 * p2.
                        CBNL    l1,             // Множимое.
                        CBNL    l2,             // Множитель.
                        CBNL    p [2]           // Результат.
        )                                       // Выдает младшее слово
{                                               // результата.
#ifdef  _muldCBNL
  return (p [0] = _muldCBNL (l1, l2, p + 1));   // Реализовано в _muldCBNL.
#else //_muldCBNL
#ifdef  _CBNL_CBNL
  *(_CBNL_CBNL *)p = (_CBNL_CBNL)l1 * l2;
  return p [0];
#else //_CBNL_CBNL
  if ((l1 ^ l2) >= 0) {
    if (l1 < 0) { l1 = -l1; l2 = -l2; }         // Смена знака операндов.
    return cULongMul (l1, l2, (unsigned CBNL *)p);
  }
  else {
    if (l1 < 0) l1 = -l1; else l2 = -l2;        // Смена знака операнда.
    CBNL l = cULongMul (l1, l2, (unsigned CBNL *)p);
    p [1] = ~p [1] + ((p [0] = l = -l) == 0);    // Смена знака результата.
    return l;
  }
#endif//_CBNL_CBNL
#endif//_muldCBNL
}

//================================================
//      Совместимая реализация встроенных функций
//      если они не обеспечиваются компилятором
//      или ассемлерными пакетом.
//================================================

//      Функция умножения чисел CBNL в двойное слово.

#ifdef  _CBNL_MUL

#ifndef _muldCBNL
#define _muldCBNL   _muldCBNL_i
inline
CBNL   _CBNL_C  _muldCBNL_i (                   // Умножение l1 * l2.
                        CBNL    l1,             // Множимое.
                        CBNL    l2,             // Множитель.
                        CBNL    *p              // Старшее слово результата.
        )                                       // Выдает младшее слово.
{
  CBNL pp [2];
  CBNL l = cLongMul (l1, l2, pp); *p = pp [1];
  return l;
}
#endif

//      Функция умножения беззнаковых чисел CBNL в двойное слово.

#ifndef _umuldCBNL
#define _umuldCBNL  _umuldCBNL_i
inline unsigned
CBNL   _CBNL_C  _umuldCBNL_i (                  // Умножение l1 * l2.
                unsigned CBNL   l1,             // Беззнаковое множимое.
                unsigned CBNL   l2,             // Беззнаковый множитель.
                unsigned CBNL   *p              // Старшее слово результата.
        )                                       // Выдает младшее слово.
{
  unsigned CBNL pp [2];
  unsigned CBNL l = cULongMul (l1, l2, pp); *p = pp [1];
  return l;
}
#endif

#endif//_CBNL_MUL

//      Деление двухсловного числа на однословное с однословным частным
//      для Microsoft Visual C++.

#ifdef  _CBNL_DI

#if     _CBNL_MI == 32
inline
CBNL   _CBNL_C  _divdCBNL_c  (CBNL ll, CBNL lh, CBNL l2, CBNL *p)
{
  return _div64 (((__int64)lh<<32) + ll, l2, (int *)p);
}
inline
unsigned
CBNL   _CBNL_C  _udivdCBNL_c (unsigned CBNL ll, unsigned CBNL lh,
                              unsigned CBNL l2, unsigned CBNL *p)
{
  return _udiv64 (((__int64)lh<<32) + ll, l2, (unsigned int *)p);
}
inline
CBNL   _CBNL_C  _moddCBNL_c  (CBNL ll, CBNL lh, CBNL l2)
{
  int p; _div64 (((__int64)lh<<32) + ll, l2, &p); return p;
}
inline
unsigned
CBNL   _CBNL_C  _umoddCBNL_c (unsigned CBNL ll, unsigned CBNL lh,
                              unsigned CBNL l2)
{
  unsigned p; _udiv64 (((__int64)lh<<32) + ll, l2, &p); return p;
}
#endif//_CBNL_MI

#if     _CBNL_MI == 64
inline
CBNL   _CBNL_C  _moddCBNL_c  (CBNL ll, CBNL lh, CBNL l2)
{
  CBNL p; _div128 (lh, ll, l2, &p); return p;
}
inline
unsigned
CBNL   _CBNL_C  _umoddCBNL_c (unsigned CBNL ll, unsigned CBNL lh,
                              unsigned CBNL l2)
{
  unsigned CBNL p; _udiv128 (lh, ll, l2, &p); return p;
}
#endif//_CBNL_MI

#endif//_CBNL_DI

//      Переносимые функции с переносом/займом, сдвига и подчета битов.

#ifndef _addCBNL
inline
unsigned char _addCBNL  (unsigned CBNL l1, unsigned CBNL l2, unsigned CBNL *p)
{
  return ((*p = l1 + l2) < l1);
}
#endif

#ifndef _adcCBNL
inline
unsigned char _adcCBNL (unsigned char c,
                        unsigned CBNL l1, unsigned CBNL l2, unsigned CBNL *p)
{
  *p = l1 + l2 + c; return (*p < l1 || *p == l1 && c != 0);
}
#endif

#ifndef _subCBNL
inline
unsigned char _subCBNL (unsigned CBNL l1, unsigned CBNL l2, unsigned CBNL *p)
{
  return ((*p = l1 - l2) > l1);
}
#endif

#ifndef _sbbCBNL
inline
unsigned char _sbbCBNL (unsigned char c,
                        unsigned CBNL l1, unsigned CBNL l2, unsigned CBNL *p)
{
  *p = l1 - l2 - c; return (*p > l1 || *p == l1 && c != 0);
}
#endif

#ifndef _ushldCBNL
inline
unsigned CBNL _ushldCBNL (unsigned CBNL ll, unsigned CBNL lh, unsigned CBNC sh)
{
#if 0
  assert (~0U % (CHAR_BIT * sizeof (CBNL)) == CHAR_BIT * sizeof (CBNL) - 1);
  return ((lh << sh) + (ll >> ((~sh) & (CHAR_BIT * sizeof (CBNL) - 1)) >> 1));
#else
  assert (sh < CHAR_BIT * sizeof (CBNL));
  return ((lh << sh) + (ll >> (CHAR_BIT * sizeof (CBNL) - 1 - sh) >> 1));
#endif
}
#endif

#ifndef _ushrdCBNL
inline
unsigned CBNL _ushrdCBNL (unsigned CBNL ll, unsigned CBNL lh, unsigned CBNC sh)
{
#if 0
  assert (~0U % (CHAR_BIT * sizeof (CBNL)) == CHAR_BIT * sizeof (CBNL) - 1);
  return ((ll >> sh) + (lh << ((~sh) && (CHAR_BIT * sizeof (CBNL) - 1)) << 1));
#else
  assert (sh < CHAR_BIT * sizeof (CBNL));
  return ((ll >> sh) + (lh << (CHAR_BIT * sizeof (CBNL) - 1 - sh) << 1));
#endif
}
#endif

#ifndef _ushld1CBNL
inline
unsigned CBNL _ushld1CBNL (unsigned CBNL ll, unsigned CBNL lh)
{
  return ((lh << 1) + (ll >> (CHAR_BIT * sizeof (CBNL) - 1)));
}
#endif

#ifndef _ushrd1CBNL
inline
unsigned CBNL _ushrd1CBNL (unsigned CBNL ll, unsigned CBNL lh)
{
  return ((ll >> 1) + (lh << (CHAR_BIT * sizeof (CBNL) - 1)));
}
#endif

#ifndef _ushlCBNL
#define _ushlCBNL(l,sh)  ((unsigned CBNL)(l) << (unsigned CBNC)(sh))
#endif

#ifndef _ushrCBNL
#define _ushrCBNL(l,sh)  ((unsigned CBNL)(l) >> (unsigned CBNC)(sh))
#endif

#ifndef _btCBNL
inline
unsigned char _btCBNL (unsigned CBNL num, unsigned CBNC sh)
{
  return ((num & ((CBNL)1 << sh)) != 0);
}
#endif

#ifndef _ulzcntCBNL
inline
unsigned CBNL _ulzcntCBNL (unsigned CBNL num)
{
#ifdef  _ubsrCBNL
  unsigned long n;
  return (CHAR_BIT * sizeof (CBNL) - 1 - (_ubsrCBNL (&n, num)? (int)n: -1));
#else
  return (CHAR_BIT * sizeof (CBNL) - cULongBits(num));
#endif
}
#endif

#endif//_CBNL_H
