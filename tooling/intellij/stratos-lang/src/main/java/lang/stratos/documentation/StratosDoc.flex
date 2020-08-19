package lang.stratos.documentation;

import com.intellij.lexer.FlexLexer;
import com.intellij.psi.tree.IElementType;
import static lang.stratos.util.StratosTokenSets.*;

%%

%{
  _StratosDocLexer() {
    this(null);
  }
%}

%class _StratosDocLexer
%implements FlexLexer
%unicode
%function advance
%type IElementType

%xstate COMMENT_BODY
%xstate AFTER_CRLF

WHITE_SPACE_NO_CRLF=[\ \t\f]

%%

<YYINITIAL>    "/**"                                                { yybegin(COMMENT_BODY); return MULTI_LINE_DOC_COMMENT_START;                         }
<YYINITIAL>    [^]                                                  { return BAD_CHARACTER;  /* can't happen */                                           }

<COMMENT_BODY> "*/"                                                 { return zzMarkedPos == zzEndRead ? MULTI_LINE_COMMENT_END : MULTI_LINE_COMMENT_BODY; }
<COMMENT_BODY> {WHITE_SPACE_NO_CRLF}* (\n+ {WHITE_SPACE_NO_CRLF}*)+ { yybegin(AFTER_CRLF); return WHITE_SPACE;                                            }
<COMMENT_BODY> .                                                    { return MULTI_LINE_COMMENT_BODY;                                                     }

<AFTER_CRLF>   "*/" | [^"*"]                                        { yypushback(yylength()); yybegin(COMMENT_BODY); break;                               }
<AFTER_CRLF>   "*"                                                  { yybegin(COMMENT_BODY); return DOC_COMMENT_LEADING_ASTERISK;                         }
