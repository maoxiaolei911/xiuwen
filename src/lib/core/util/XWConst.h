/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCONST_H
#define XWCONST_H

#define CNST_SCHAR_MIN -127
#define CNST_SCHAR_MAX 128
#define CNST_UCHAR_MIN 0
#define CNST_UCHAR_MAX 255
#define CNST_SHRT_MIN  -32767
#define CNST_SHRT_MAX  32767
#define CNST_USHRT_MAX 65535
#define CNST_INT_MIN   -4294967295
#define CNST_INT_MAX   4294967295
#define CNST_UINT_MIN  0
#define CNST_UINT_MAX  4294967295
#define CNST_MAX_INT  CNST_INT_MAX

#ifndef INT32_MIN
#define INT32_MIN   (-2147483647-1)
#endif
#ifndef INT32_MAX
#define INT32_MAX   (2147483647)
#endif


#define DVI_NUMERATOR   25400000
#define DVI_DENOMINATOR 473628672

#define DVI_PADDING    223

#define DVI_ID_BYTE 2
#define DVI_V_ID    3
#define XDVI_ID     5

#define DVI_SETCHAR0 0
#define DVI_SET1     128
#define DVI_SET2     129
#define DVI_SET3     130
#define DVI_SET4     131
#define DVI_SETRULE  132
#define DVI_PUT1     133
#define DVI_PUT2     134
#define DVI_PUT3     135
#define DVI_PUT4     136
#define DVI_PUTRULE  137
#define DVI_NOP      138
#define DVI_BOP      139
#define DVI_EOP      140
#define DVI_PUSH     141
#define DVI_POP      142
#define DVI_RIGHT1   143
#define DVI_RIGHT2   144
#define DVI_RIGHT3   145
#define DVI_RIGHT4   146
#define DVI_W0       147
#define DVI_W1       148
#define DVI_W2       149
#define DVI_W3       150
#define DVI_W4       151
#define DVI_X0       152
#define DVI_X1       153
#define DVI_X2       154
#define DVI_X3       155
#define DVI_X4       156
#define DVI_DOWN1    157
#define DVI_DOWN2    158
#define DVI_DOWN3    159
#define DVI_DOWN4    160
#define DVI_Y0       161
#define DVI_Y1       162
#define DVI_Y2       163
#define DVI_Y3       164
#define DVI_Y4       165
#define DVI_Z0       166
#define DVI_Z1       167
#define DVI_Z2       168
#define DVI_Z3       169
#define DVI_Z4       170
#define DVI_FNTNUM0  171
#define DVI_FNT1     235
#define DVI_FNT2     236
#define DVI_FNT3     237
#define DVI_FNT4     238
#define DVI_XXX1     239
#define DVI_XXX2     240
#define DVI_XXX3     241
#define DVI_XXX4     242
#define DVI_FNTDEF1  243
#define DVI_FNTDEF2  244
#define DVI_FNTDEF3  245
#define DVI_FNTDEF4  246
#define DVI_PRE      247
#define DVI_POST     248
#define DVI_POSTPOST 249
#define DVI_DIR      255

#define XDV_PIC_FILE        251
#define XDV_NATIVE_FONT_DEF 252
#define XDV_GLYPH_ARRAY     253
#define XDV_GLYPH_STRING    254

#endif // XWCONST_H

