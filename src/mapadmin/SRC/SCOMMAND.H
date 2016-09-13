#ifndef _SCOMMAND_H_
#define _SCOMMAND_H_

enum CONDUCT_EVENT
{
  CE_EVENT_INIT      = 0,
  CE_OBJ_BUILD       = 1,
  CE_OBJ_EDIT        = 2,
  CE_OBJ_JOIN        = 3,
  CE_OBJ_PARAM       = 4,
  CE_OBJ_DELETE      = 5,
  CE_OBJ_RESTORE     = 6,
  CE_SCHEEM_START    = 7,
  CE_SCHEEM_LEN      = 8,
  CE_SCHEEM_END      = 9,
  CE_OBJ_QUERY       = 10,
  CE_OBJ_COPY        = 14,
  CE_TRACE_ADD       = 15,
  CE_TRACE_SUB       = 16,
  CE_SCHEEM_LEN_ADD  = 17,
  CE_SCHEEM_LEN_SUB  = 18,
  CE_SPACE_QUERY     = 19,
  CE_OBJ_CLASS       = 20,
  CE_OBJ_BUILD_END   = 21,
  CE_OBJ_CUT_IN      = 22,
  CE_OBJ_CUT_OUT     = 23,

  CE_SHORTTRACE_END  = 24,
  CE_SHORTTRACE_CNC  = 25,
  CE_SHORTTRACE_ADD  = 26,
  CE_SHORTTRACE_SAB  = 27,

  CE_CHECK_RESULT    = 29,
  CE_GET_USER_RIGHTS = 30,

  CE_SPECIAL_COMMAND = 100,
  CE_DOC_COMMAND     = 200,
  CE_FIND_COMMAND    = 300,

  CE_MODAL_DLG       = 400,
  CE_INF_DLG         = CE_MODAL_DLG + 1,
  CE_ERR_DLG         = CE_MODAL_DLG + 2,
  CE_ASK_DLG         = CE_MODAL_DLG + 3,
  CE_PAR_DLG         = CE_MODAL_DLG + 4,

  CE_CRT_WND_MSG     = CE_MODAL_DLG + 5,
  CE_NOT_IN_TRACE    = CE_MODAL_DLG + 6,
  CE_MODAL_DLG_END   = CE_MODAL_DLG + 9
};

enum RTS_LEVEL
{
  UR_NONE   = 0,
  UR_L_VIEW = 1,
  UR_H_VIEW = 2,
  UR_L_EDIT = 3,
  UR_H_EDIT = 4,
  UR_ADMN   = 100
};

enum FORM_TYPE
{
  FT_INTERNAL_DLG  = 0,
  FT_EXTHTA_DLG    = 1,
  FT_EXTHTTP_DLG   = 2,
  FT_EXTSLIGHT_DLG = 3
};

#endif