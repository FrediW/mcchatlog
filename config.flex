%option c++
%option noyywrap

%s quotedstring
%s comment

%%

<quotedstring>"\"" yy_pop_state();
<quotedstring>[^\"]* return CT_Word;
<comment>\n yy_pop_state();
<comment>[^\n]* return CT_Comment;
\( return CT_OpenParen;
\) return CT_CloseParen;
\, return CT_Comma;
\[ return CT_OpenBrack;
\] return CT_CloseBrack;
\: return CT_Colon;
[A-Za-z0-9_]+ return CT_Word;
\" yy_push_state(quotedstring);
" " return CT_Space;
\n return CT_EndOfLine;
\# yy_push_state(comment);
%%
