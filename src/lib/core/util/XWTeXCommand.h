/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ** 这是秀文小书店管理程序的一部分。本软件为商业软件，未经秀文公司许可，任何
 ** 单位和个人不得复制、传播源代码以及二进制文件。
 **
 ****************************************************************************/
#ifndef XWTEXCOMMAND_H
#define XWTEXCOMMAND_H

#include "XWConst.h"
#include "XWEquiRegion.h"

#define CMD_DEFAULT_CODE 1073741824

#define CMD_NULL_CODE 0
#define CMD_INVALID_CODE 127
#define CMD_VAR_CODE 117440512

#define CMD_ESCAPE  0
#define CMD_RELAX   0
#define CMD_LEFT_BRACE  1
#define CMD_RIGHT_BRACE 2
#define CMD_MATH_SHIFT  3
#define CMD_TAB_MARK    4
#define CMD_CAR_RET     5
#define CMD_OUT_PARAM   5
#define CMD_MACRO_PARAM 6
#define CMD_SUP_MARK    7
#define CMD_SUB_MARK    8
#define CMD_IGNORE      9
#define CMD_END_V       9
#define CMD_SPACER      10
#define CMD_LETTER      11
#define CMD_OTHER_CHAR  12
#define CMD_ACTIVE_CHAR 13
#define CMD_PAR_END     13
#define CMD_MATCH       13
#define CMD_COMMENT     14
#define CMD_END_MATCH   14
#define CMD_STOP        14
#define CMD_INVALID_CHAR  15
#define CMD_DELIM_NUM     15
#define MAX_CHAR_CODE     15

#define CMD_CHAR_NUM      16
#define CMD_MATH_CHAR_NUM 17
#define CMD_MARK  18
#define CMD_XRAY  19
#define CMD_MAKE_BOX  20
#define CMD_HMOVE     21
#define CMD_VMOVE     22
#define CMD_UN_HBOX   23
#define CMD_UN_VBOX   24
#define CMD_REMOVE_ITEM  25
#define CMD_HSKIP  26
#define CMD_VSKIP  27
#define CMD_MSKIP  28
#define CMD_KERN   29
#define CMD_MKERN  30
#define CMD_LEADER_SHIP  31
#define CMD_HALIGN  32
#define CMD_VALIGN  33
#define CMD_NO_ALIGN 34
#define CMD_VRULE    35
#define CMD_HRULE    36
#define CMD_INSERT   37
#define CMD_VADJUST  38
#define CMD_IGNORE_SPACES  39
#define CMD_AFTER_ASSIGNMENT 40
#define CMD_AFTER_GROUP  41
#define CMD_BREAK_PENALTY  42
#define CMD_START_PAR 43
#define CMD_ITAL_CORR 44
#define CMD_ACCENT 45
#define CMD_MATH_ACCENT  46
#define CMD_DISCRETIONARY 47
#define CMD_EQ_NO 48
#define CMD_LEFT_RIGHT 49
#define CMD_MATH_COMP  50
#define CMD_LIMIT_SWITCH  51
#define CMD_ABOVE  52
#define CMD_MATH_STYLE  53
#define CMD_MATH_CHOICE 54
#define CMD_NON_SCRIPT  55
#define CMD_VCENTER  56
#define CMD_CASE_SHIFT 57
#define CMD_MESSAGE  58
#define CMD_EXTENSION  59
#define CMD_IN_STREAM  60
#define CMD_BEGIN_GROUP  61
#define CMD_END_GROUP  62
#define CMD_OMIT  63
#define CMD_EX_SPACE  64
#define CMD_NO_BOUNDARY  65
#define CMD_RADICAL  66
#define CMD_END_CS_NAME  67
#define CMD_CHAR_GHOST  (CMD_END_CS_NAME + 1) //68
#define CMD_ASSIGN_LOCAL_BOX (CMD_CHAR_GHOST + 1) //69
#define CMD_MIN_INTERNAL (CMD_ASSIGN_LOCAL_BOX + 1) //70
#define CMD_CHAR_GIVEN  CMD_MIN_INTERNAL //70
#define CMD_MATH_GIVEN (CMD_CHAR_GIVEN + 1) //71
#define CMD_OMATH_GIVEN  (CMD_MATH_GIVEN + 1) //72
#define CMD_LAST_ITEM  (CMD_OMATH_GIVEN + 1) //73
#define MAX_NON_PREFIXED_COMMAND  CMD_LAST_ITEM //73

#define CMD_TOKS_REGISTER (MAX_NON_PREFIXED_COMMAND + 1) //74
#define CMD_ASSIGN_TOKS (CMD_TOKS_REGISTER + 1) //75
#define CMD_ASSIGN_INT (CMD_ASSIGN_TOKS + 1) //76
#define CMD_ASSIGN_DIMEN  (CMD_ASSIGN_INT + 1) //77
#define CMD_ASSIGN_GLUE  (CMD_ASSIGN_DIMEN + 1) //78
#define CMD_ASSIGN_MU_GLUE  (CMD_ASSIGN_GLUE + 1) //79
#define CMD_ASSIGN_FONT_DIMEN  (CMD_ASSIGN_MU_GLUE + 1) //80

#define CMD_FONT_DIMEN_CODE 0
#define CMD_CHAR_WIDTH_CODE  (CMD_FONT_DIMEN_CODE + 1) //1
#define CMD_CHAR_HEIGHT_CODE (CMD_CHAR_WIDTH_CODE + 1) //2
#define CMD_CHAR_DEPTH_CODE  (CMD_CHAR_HEIGHT_CODE + 1) //3
#define CMD_CHAR_ITALIC_CODE (CMD_CHAR_DEPTH_CODE + 1) //4
#define CMD_ASSIGN_FONT_INT  (CMD_ASSIGN_FONT_DIMEN + 1) //81
#define CMD_SET_AUX  (CMD_ASSIGN_FONT_INT + 1) //82
#define CMD_SET_PREV_GRAF  (CMD_SET_AUX + 1) //83
#define CMD_SET_PAGE_DIMEN  (CMD_SET_PREV_GRAF + 1) //84
#define CMD_SET_PAGE_INT    (CMD_SET_PAGE_DIMEN + 1) //85
#define CMD_SET_BOX_DIMEN   (CMD_SET_PAGE_INT + 1) //86
#define CMD_SET_SHAPE       (CMD_SET_BOX_DIMEN + 1) //87
#define CMD_DEF_CODE  (CMD_SET_SHAPE + 1) //88
#define CMD_DEF_FAMILY  (CMD_DEF_CODE + 1) //89
#define CMD_SET_FONT  (CMD_DEF_FAMILY + 1) //90
#define CMD_DEF_FONT  (CMD_SET_FONT + 1) //91
#define CMD_REGISTER  (CMD_DEF_FONT + 1) //92
#define CMD_ASSIGN_NEXT_FAKE_MATH  (CMD_REGISTER + 1) //93
#define CMD_ASSIGN_BOX_DIR  (CMD_REGISTER + 2) //94
#define CMD_ASSIGN_DIR  (CMD_REGISTER + 3) //95

#define MAX_INTERNAL  CMD_ASSIGN_DIR //95

#define CMD_ADVANCE  (MAX_INTERNAL + 1) //96
#define CMD_MULTIPLY  (CMD_ADVANCE + 1) //97
#define CMD_DIVIDE  (CMD_MULTIPLY + 1) //98
#define CMD_PREFIX  (CMD_DIVIDE + 1) //99
#define CMD_LET  (CMD_PREFIX + 1) //100
#define CMD_SHORTHAND_DEF  (CMD_LET + 1) //101
#define CMD_READ_TO_CS  (CMD_SHORTHAND_DEF + 1) //102
#define CMD_DEF  (CMD_READ_TO_CS + 1) //103
#define CMD_SET_BOX  (CMD_DEF + 1) //104
#define CMD_HYPH_DATA  (CMD_SET_BOX + 1) //105
#define CMD_SET_INTERACTION  (CMD_HYPH_DATA + 1) //106
#define CMD_SET_OCP  (CMD_SET_INTERACTION + 1) //107
#define CMD_DEF_OCP  (CMD_SET_OCP + 1) //108
#define CMD_SET_OCP_LIST  (CMD_DEF_OCP + 1) //109
#define CMD_DEF_OCP_LIST  (CMD_SET_OCP_LIST + 1) //110
#define CMD_CLEAR_OCP_LISTS  (CMD_DEF_OCP_LIST + 1) //111
#define CMD_PUSH_OCP_LIST  (CMD_CLEAR_OCP_LISTS + 1) //112
#define CMD_POP_OCP_LIST  (CMD_PUSH_OCP_LIST + 1) //113
#define CMD_OCP_LIST_OP  (CMD_POP_OCP_LIST + 1) //114
#define CMD_OCP_TRACE_LEVEL  (CMD_OCP_LIST_OP + 1) //115
#define CMD_CHAR_TRANS  (CMD_OCP_TRACE_LEVEL + 1) //116
#define CMD_CHAR_MODE  (CMD_CHAR_TRANS + 1) //117
#define MAX_COMMAND  CMD_CHAR_MODE //117

#define CMD_UNDEFINED_CS  (MAX_COMMAND + 1) //118
#define CMD_EXPAND_AFTER  (MAX_COMMAND + 2) //119
#define CMD_NO_EXPAND  (MAX_COMMAND + 3) //120
#define CMD_INPUT  (MAX_COMMAND + 4) //121
#define CMD_IF_TEST  (MAX_COMMAND + 5) //122
#define CMD_FI_OR_ELSE  (MAX_COMMAND + 6) //123
#define CMD_CS_NAME  (MAX_COMMAND + 7) //124
#define CMD_CONVERT  (MAX_COMMAND + 8) //125
#define CMD_THE  (MAX_COMMAND + 9) //126
#define CMD_TOP_BOT_MARK  (MAX_COMMAND + 10) //127
#define CMD_CALL  (MAX_COMMAND + 11) //128
#define CMD_LONG_CALL  (MAX_COMMAND + 12) //129
#define CMD_OUTER_CALL  (MAX_COMMAND + 13) //130
#define CMD_LONG_OUTER_CALL  (MAX_COMMAND + 14) //131
#define CMD_END_TEMPLATE  (MAX_COMMAND + 15) //132
#define CMD_DONT_EXPAND  (MAX_COMMAND + 16) //133
#define CMD_GLUE_REF  (MAX_COMMAND + 17) //134
#define CMD_SHAPE_REF  (MAX_COMMAND + 18) //135
#define CMD_BOX_REF  (MAX_COMMAND + 19) //136
#define CMD_DATA  (MAX_COMMAND + 20) //137

#define CMD_VMODE 1
#define CMD_HMODE (CMD_VMODE + MAX_COMMAND + 1) //119
#define CMD_MMODE (CMD_HMODE + MAX_COMMAND + 1) //137

#define CMD_LINE_SKIP_CODE 0
#define CMD_BASELINE_SKIP_CODE 1
#define CMD_PAR_SKIP_CODE 2
#define CMD_ABOVE_DISPLAY_SKIP_CODE 3
#define CMD_BELOW_DISPLAY_SKIP_CODE 4
#define CMD_ABOVE_DISPLAY_SHORT_SKIP_CODE 5
#define CMD_BELOW_DISPLAY_SHORT_SKIP_CODE 6
#define CMD_LEFT_SKIP_CODE 7
#define CMD_RIGHT_SKIP_CODE 8
#define CMD_TOP_SKIP_CODE 9
#define CMD_SPLIT_TOP_SKIP_CODE 10
#define CMD_TAB_SKIP_CODE 11
#define CMD_SPACE_SKIP_CODE 12
#define CMD_XSPACE_SKIP_CODE 13
#define CMD_PAR_FILL_SKIP_CODE 14
#define CMD_THIN_MU_SKIP_CODE 15
#define CMD_MED_MU_SKIP_CODE 16
#define CMD_THICK_MU_SKIP_CODE 17
#define CMD_GLUE_PARS 18


#define CMD_PRETOLERANCE_CODE 0
#define CMD_TOLERANCE_CODE 1
#define CMD_LINE_PENALTY_CODE 2
#define CMD_HYPHEN_PENALTY_CODE 3
#define CMD_EX_HYPHEN_PENALTY_CODE 4
#define CMD_CLUB_PENALTY_CODE 5
#define CMD_WIDOW_PENALTY_CODE 6
#define CMD_DISPLAY_WIDOW_PENALTY_CODE 7
#define CMD_BROKEN_PENALTY_CODE 8
#define CMD_BIN_OP_PENALTY_CODE 9
#define CMD_REL_PENALTY_CODE 10
#define CMD_PRE_DISPLAY_PENALTY_CODE 11
#define CMD_POST_DISPLAY_PENALTY_CODE 12
#define CMD_INTER_LINE_PENALTY_CODE 13
#define CMD_DOUBLE_HYPHEN_DEMERITS_CODE 14
#define CMD_FINAL_HYPHEN_DEMERITS_CODE 15
#define CMD_ADJ_DEMERITS_CODE 16
#define CMD_MAG_CODE 17
#define CMD_DELIMITER_FACTOR_CODE 18
#define CMD_LOOSENESS_CODE 19
#define CMD_TIME_CODE 20
#define CMD_DAY_CODE 21
#define CMD_MONTH_CODE 22
#define CMD_YEAR_CODE 23
#define CMD_SHOW_BOX_BREADTH_CODE 24
#define CMD_SHOW_BOX_DEPTH_CODE 25
#define CMD_HBADNESS_CODE 26
#define CMD_VBADNESS_CODE 27
#define CMD_PAUSING_CODE 28
#define CMD_TRACING_ONLINE_CODE 29
#define CMD_TRACING_MACROS_CODE 30
#define CMD_TRACING_STATS_CODE 31
#define CMD_TRACING_PARAGRAPHS_CODE 32
#define CMD_TRACING_PAGES_CODE 33
#define CMD_TRACING_OUTPUT_CODE 34
#define CMD_TRACING_LOST_CHARS_CODE 35
#define CMD_TRACING_COMMANDS_CODE 36
#define CMD_TRACING_RESTORES_CODE 37
#define CMD_UC_HYPH_CODE 38
#define CMD_OUTPUT_PENALTY_CODE 39
#define CMD_MAX_DEAD_CYCLES_CODE 40
#define CMD_HANG_AFTER_CODE 41
#define CMD_FLOATING_PENALTY_CODE 42
#define CMD_GLOBAL_DEFS_CODE 43
#define CMD_CUR_FAM_CODE 44
#define CMD_ESCAPE_CHAR_CODE 45
#define CMD_DEFAULT_HYPHEN_CHAR_CODE 46
#define CMD_DEFAULT_SKEW_CHAR_CODE 47
#define CMD_END_LINE_CHAR_CODE 48
#define CMD_NEW_LINE_CHAR_CODE 49
#define CMD_LANGUAGE_CODE 50
#define CMD_LEFT_HYPHEN_MIN_CODE 51
#define CMD_RIGHT_HYPHEN_MIN_CODE 52
#define CMD_HOLDING_INSERTS_CODE 53
#define CMD_ERROR_CONTEXT_LINES_CODE 54
#define CMD_LOCAL_INTER_LINE_PENALTY_CODE 55
#define CMD_LOCAL_BROKEN_PENALTY_CODE 56
#define CMD_NO_LOCAL_WHATSITS_CODE 57
#define CMD_NO_LOCAL_DIRS_CODE 58
#define CMD_LEVEL_LOCAL_DIR_CODE 59
#define CMD_INT_PARS 61
#define CMD_TRACING_ASSIGNS_CODE CMD_INT_PARS  //61
#define CMD_TRACING_GROUPS_CODE (CMD_INT_PARS + 1) //62
#define CMD_TRACING_IFS_CODE (CMD_INT_PARS + 2) //63
#define CMD_TRACING_SCAN_TOKENS_CODE (CMD_INT_PARS + 3) //64
#define CMD_TRACING_NESTING_CODE (CMD_INT_PARS + 4) //65
#define CMD_PRE_DISPLAY_DIRECTION_CODE (CMD_INT_PARS + 5) //66
#define CMD_LAST_LINE_FIT_CODE (CMD_INT_PARS + 6) //67
#define CMD_SAVING_VDISCARDS_CODE (CMD_INT_PARS + 7) //68
#define CMD_SAVING_HYPH_CODES_CODE (CMD_INT_PARS + 8) //69
#define CMD_ETEX_STATE_CODE (CMD_INT_PARS + 9) //70

#define CMD_MARKS_CODE 5
#define CMD_TOP_MARK_CODE 0
#define CMD_FIRST_MARK_CODE 1
#define CMD_BOT_MARK_CODE 2
#define CMD_SPLIT_FIRST_MARK_CODE 3
#define CMD_SPLIT_BOT_MARK_CODE 4



#define CMD_PAGE_DIRECTION_CODE 0
#define CMD_BODY_DIRECTION_CODE 1
#define CMD_PAR_DIRECTION_CODE 2
#define CMD_TEXT_DIRECTION_CODE 3
#define CMD_MATH_DIRECTION_CODE 4
#define CMD_DIR_PARS 5

#define CMD_PAR_INDENT_CODE 0
#define CMD_MATH_SURROUND_CODE 1
#define CMD_LINE_SKIP_LIMIT_CODE 2
#define CMD_HSIZE_CODE 3
#define CMD_VSIZE_CODE 4
#define CMD_MAX_DEPTH_CODE 5
#define CMD_SPLIT_MAX_DEPTH_CODE 6
#define CMD_BOX_MAX_DEPTH_CODE 7
#define CMD_HFUZZ_CODE 8
#define CMD_VFUZZ_CODE 9
#define CMD_DELIMITER_SHORTFALL_CODE 10
#define CMD_NULL_DELIMITER_SPACE_CODE 11
#define CMD_SCRIPT_SPACE_CODE 12
#define CMD_PRE_DISPLAY_SIZE_CODE 13
#define CMD_DISPLAY_WIDTH_CODE 14
#define CMD_DISPLAY_INDENT_CODE 15
#define CMD_OVERFULL_RULE_CODE 16
#define CMD_HANG_INDENT_CODE 17
#define CMD_H_OFFSET_CODE 18
#define CMD_V_OFFSET_CODE 19
#define CMD_EMERGENCY_STRETCH_CODE 20
#define CMD_PAGE_WIDTH_CODE 21
#define CMD_PAGE_HEIGHT_CODE 22
#define CMD_PAGE_RIGHT_OFFSET_CODE 23
#define CMD_PAGE_BOTTOM_OFFSET_CODE 24
#define CMD_DIMEN_PARS 25

#define CMD_BOTTOM_LEVEL 0
#define CMD_SIMPLE_GROUP 1
#define CMD_HBOX_GROUP 2
#define CMD_ADJUSTED_HBOX_GROUP 3
#define CMD_VBOX_GROUP 4
#define CMD_VTOP_GROUP 5
#define CMD_ALIGN_GROUP 6
#define CMD_NO_ALIGN_GROUP 7
#define CMD_OUTPUT_GROUP 8
#define CMD_MATH_GROUP 9
#define CMD_DISC_GROUP 10
#define CMD_INSERT_GROUP 11
#define CMD_VCENTER_GROUP 12
#define CMD_MATH_CHOICE_GROUP 13
#define CMD_SEMI_SIMPLE_GROUP 14
#define CMD_MATH_SHIFT_GROUP 15
#define CMD_MATH_LEFT_GROUP 16
#define CMD_LOCAL_BOX_GROUP 17
#define MAX_GROUP_CODE 17

#define CMD_INT_VAL 0
#define CMD_DIMEN_VAL 1
#define CMD_GLUE_VAL 2
#define CMD_MU_VAL 3
#define CMD_DIR_VAL 4
#define CMD_IDENT_VAL 5
#define CMD_TOK_VAL 6
#define CMD_BOX_VAL 7
#define CMD_MARK_VAL 8



#define CMD_LAST_NODE_TYPE_CODE (CMD_GLUE_VAL + 1) //3
#define CMD_INPUT_LINE_NO_CODE (CMD_GLUE_VAL + 2) //4
#define CMD_BADNESS_CODE (CMD_GLUE_VAL + 3) //5
#define CMD_ALEPH_INT (CMD_GLUE_VAL + 4) //6
#define CMD_ALEPH_VERSION_CODE (CMD_ALEPH_INT) //6
#define CMD_OMEGA_VERSION_CODE (CMD_ALEPH_INT + 1) //7
#define CMD_ALEPH_MINOR_VERSION_CODE (CMD_ALEPH_INT + 2) //8
#define CMD_OMEGA_MINOR_VERSION_CODE (CMD_ALEPH_INT + 3) //9
#define CMD_ETEX_MINOR_VERSION_CODE (CMD_ALEPH_INT + 4) //10

#define CMD_ETEX_INT (CMD_ALEPH_INT + CNST_ALEPH_INT_NUM) //11
#define CMD_ETEX_VERSION_CODE CMD_ETEX_INT //11

#define CMD_NUMBER_CODE 0
#define CMD_ROMAN_NUMERAL_CODE 1
#define CMD_STRING_CODE 2
#define CMD_MEANING_CODE 3
#define CMD_FONT_NAME_CODE 4
#define CMD_ETEX_CODE 5
#define CMD_OMEGA_CODE 6
#define CMD_ALEPH_CODE 7
#define CMD_JOB_NAME_CODE 8

#define CMD_ALEPH_REVISION_CODE (CMD_JOB_NAME_CODE + 1) //9
#define CMD_OMEGA_REVISION_CODE (CMD_JOB_NAME_CODE + 2) //10
#define CMD_ETEX_REVISION_CODE (CMD_JOB_NAME_CODE + 3) //11
#define CMD_CURRENT_GROUP_LEVEL_CODE (CMD_ETEX_INT + 1) //12
#define CMD_CURRENT_GROUP_TYPE_CODE (CMD_ETEX_INT + 2) //13
#define CMD_CURRENT_IF_LEVEL_CODE (CMD_ETEX_INT + 3) //14
#define CMD_CURRENT_IF_TYPE_CODE (CMD_ETEX_INT + 4) //15
#define CMD_CURRENT_IF_BRANCH_CODE (CMD_ETEX_INT + 5) //16
#define CMD_GLUE_STRETCH_ORDER_CODE (CMD_ETEX_INT + 6) //17
#define CMD_GLUE_SHRINK_ORDER_CODE (CMD_ETEX_INT + 7) //18


#define CMD_ETEX_DIM (CMD_ETEX_INT + 8) //19
#define CMD_FONT_CHAR_WD_CODE CMD_ETEX_DIM //19
#define CMD_FONT_CHAR_HT_CODE (CMD_ETEX_DIM + 1) //20
#define CMD_FONT_CHAR_DP_CODE (CMD_ETEX_DIM + 2) //21
#define CMD_FONT_CHAR_IC_CODE (CMD_ETEX_DIM + 3) //22
#define CMD_PAR_SHAPE_LENGTH_CODE (CMD_ETEX_DIM + 4) //23
#define CMD_PAR_SHAPE_INDENT_CODE (CMD_ETEX_DIM + 5) //24
#define CMD_PAR_SHAPE_DIMEN_CODE (CMD_ETEX_DIM + 6) //25
#define CMD_GLUE_STRETCH_CODE (CMD_ETEX_DIM + 7) //26
#define CMD_GLUE_SHRINK_CODE (CMD_ETEX_DIM + 8) //27

#define CMD_ETEX_GLUE (CMD_ETEX_DIM + 9) //28
#define CMD_ETEX_MU (CMD_ETEX_GLUE + 1) //29
#define CMD_MU_TO_GLUE_CODE CMD_ETEX_GLUE //28
#define CMD_GLUE_TO_MU_CODE CMD_ETEX_MU //29
#define CMD_ETEX_EXPR (CMD_ETEX_MU + 1) //30

#define CMD_UNLESS_CODE 32
#define CMD_IF_CHAR_CODE 0
#define CMD_IF_CAT_CODE 1
#define CMD_IF_INT_CODE 2
#define CMD_IF_DIM_CODE 3
#define CMD_IF_ODD_CODE 4
#define CMD_IF_VMODE_CODE 5
#define CMD_IF_HMODE_CODE 6
#define CMD_IF_MMODE_CODE 7
#define CMD_IF_INNER_CODE 8
#define CMD_IF_VOID_CODE 9
#define CMD_IF_HBOX_CODE 10
#define CMD_IF_VBOX_CODE 11
#define CMD_IFX_CODE 12
#define CMD_IF_EOF_CODE 13
#define CMD_IF_TRUE_CODE 14
#define CMD_IF_FALSE_CODE 15
#define CMD_IF_CASE_CODE 16
#define CMD_IF_DEF_CODE  17
#define CMD_IF_CS_CODE   18
#define CMD_IF_FONT_CHAR_CODE 19

#define CMD_IF_CODE 1
#define CMD_FI_CODE 2
#define CMD_ELSE_CODE 3
#define CMD_OR_CODE 4


#define CMD_BOX_CODE 0
#define CMD_COPY_CODE 1
#define CMD_LAST_BOX_CODE 2
#define CMD_VSPLIT_CODE 3
#define CMD_VTOP_CODE 4

#define CMD_VMODE 1
#define CMD_HMODE (CMD_VMODE + MAX_COMMAND + 1)
#define CMD_MMODE (CMD_HMODE + MAX_COMMAND + 1)

#define CMD_SPAN_CODE  CNST_SPECIAL_CHAR
#define CMD_CR_CODE    (CMD_SPAN_CODE + 1)
#define CMD_CR_CR_CODE    (CMD_CR_CODE + 1)
#define CMD_IMMEDIATE_CODE 4
#define CMD_SET_LANGUAGE_CODE 5
        

#define CMD_FIL_CODE 0
#define CMD_FILL_CODE 1
#define CMD_SS_CODE 2
#define CMD_FIL_NEG_CODE 3
#define CMD_SKIP_CODE 4
#define CMD_MSKIP_CODE 

#define CMD_ABOVE_CODE 0
#define CMD_OVER_CODE 1
#define CMD_ATOP_CODE 2
#define CMD_DELIMITED_CODE 3

#define CMD_CHAR_DEF_CODE 0
#define CMD_MATH_CHAR_DEF_CODE 1
#define CMD_OMATH_CHAR_DEF_CODE 2
#define CMD_COUNT_DEF_CODE 3
#define CMD_DIMEN_DEF_CODE 4
#define CMD_SKIP_DEF_CODE 5
#define CMD_MU_SKIP_DEF_CODE 6
#define CMD_TOKS_DEF_CODE 7

#define CMD_SHOW_CODE 0
#define CMD_SHOW_BOX_CODE 1
#define CMD_SHOW_THE_CODE 2
#define CMD_SHOW_LISTS 3
#define CMD_SHOW_GROUPS 4
#define CMD_SHOW_TOKENS 5
#define CMD_SHOW_IFS 6

#define CMD_TRANS_INPUT 0
#define CMD_TRANS_OUTPUT 1
#define CMD_TRANS_DEFAULT_INPUT 2
#define CMD_TRANS_DEFAULT_OUTPUT 3
#define CMD_TRANS_NO_INPUT 4
#define CMD_TRANS_NO_OUTPUT 5
#define CMD_TRANS_NO_DEFAULT_INPUT 6
#define CMD_TRANS_NO_DEFAULT_OUTPUT 7

#define CMD_MODE_INPUT 0
#define CMD_MODE_OUTPUT 1
#define CMD_MODE_DEFAULT_INPUT 2
#define CMD_MODE_DEFAULT_OUTPUT 3
#define CMD_MODE_NO_INPUT 4
#define CMD_MODE_NO_OUTPUT 5
#define CMD_MODE_NO_DEFAULT_INPUT 6
#define CMD_MODE_NO_DEFAULT_OUTPUT 7

#define CMD_ADD_BEFORE_OP 1
#define CMD_ADD_AFTER_OP 2
#define CMD_REMOVE_BEFORE_OP 3
#define CMD_REMOVE_AFTER_OP 4


#define CMD_RESTORE_OLD_VALUE 0
#define CMD_RESTORE_ZERO 1
#define CMD_INSERT_TOKEN 2
#define CMD_LEVEL_BOUNDARY 3
#define CMD_RESTORE_SA 4

#define CMD_SLANT_CODE 1
#define CMD_SPACE_CODE 2
#define CMD_SPACE_STRETCH_CODE 3
#define CMD_SPACE_SHRINK_CODE 4
#define CMD_X_HEIGHT_CODE 5
#define CMD_QUAD_CODE 6
#define EXTRA_SPACE_CODE 7

#endif // XWTEXCOMMAND_H

