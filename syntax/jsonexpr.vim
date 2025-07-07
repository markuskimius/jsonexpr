" Vim syntax file
" Language:     JSONexpr
" Maintainer:   Mark Kim
" License:      Apache License 2.0
" URL:          https://github.com/markuskimius/jsonexpr
" Last change:  2025 Jul 6

" Do not load syntax file twice
if exists("b:current_syntax")
  finish
endif


"=============================================================================
" Configuration

syn case    match


"=============================================================================
" Comma-seprated value file syntax

" Identifier
syn match   jsonexprIdent       +\<[_A-Za-z][_A-Za-z0-9]*\([.][_A-Za-z][_A-Za-z0-9]*\)*\>+

" Types
syn match   jsonexprInt         +0+
syn match   jsonexprInt         +0[bB][01]\++
syn match   jsonexprInt         +0[0-7]\++
syn match   jsonexprInt         +[1-9][0-9]*+
syn match   jsonexprInt         +0[xX][0-9A-Fa-f]\++
syn match   jsonexprFloat       +\([1-9][0-9]*\|0\)[eE][-+]\?[0-9]\++
syn match   jsonexprFloat       +\([1-9][0-9]*\|0\)[.][0-9]\++
syn match   jsonexprFloat       +\([1-9][0-9]*\|0\)[.][0-9]\+[eE][-+]\?[0-9]\++

" String
syn region  jsonexprString      start=+"+ end=+"+ skip=+\(\\\\\|\\"\)+ contains=jsonexprBadChar,jsonexprChar
syn match   jsonexprBadChar     +\\.\@=+
syn match   jsonexprBadChar     +\\[xX][^"]\{0,2\}+
syn match   jsonexprBadChar     +\\u[^"]\{0,4\}+
syn match   jsonexprBadChar     +\\U[^"]\{0,8\}+
syn match   jsonexprChar        +\\[xX][0-9A-Fa-f]\{2\}+
syn match   jsonexprChar        +\\u[0-9A-Fa-f]\{4\}+
syn match   jsonexprChar        +\\U[0-9A-Fa-f]\{8\}+
syn match   jsonexprChar        +\\[\\"/bfnrt\n]+
syn match   jsonexprBadChar     +[^"]\%$+

" Keywords
syn keyword jsonexprNull        null
syn keyword jsonexprTrue        true
syn keyword jsonexprFalse       false

" Symbols
syn match   jsonexprConst       +\<\(VERSION\|VERSION_MAJOR\|VERSION_MINOR\|VERSION_PATCH\)\>+
syn match   jsonexprDynamic     +\<\(GLOBAL\|UPSCOPE\|LOCAL\|THIS\)\>+
syn match   jsonexprFunction    +\<\(PRINT\|BINARY\|CEIL\|FLOOR\|HEX\|IF\|LEN\|ROUND\|SQRT\|STRLEN\|FOR\|FOREACH\|FUNCTION\|WHILE\|EVAL\)\>+

" Operators
syn match   jsonexprOp          "\(!\|!=\|%\|&\|&&\|\*\|\*\*\|+\|++\|-\|--\|/\|<\|<<\|<=\|=\|==\|>\|>=\|>>\|>>>\|?\|\^\||\|||\|\~\)"
syn match   jsonexprOp          "\(%=\|&=\|*=\|**=\|+=\|-=\|/=\|<<=\|>>=\|>>>=\|^=\||=\)"
syn match   jsonexprDelim       "[:,]"
syn match   jsonexprGroup       "[\[({})\]]"
syn match   jsonexprTerm        ";"

" Comment
syn match   jsonexprComment     +#.*+
syn region  jsonexprComment     start=+/\*+ end=+\*/+


"=============================================================================
" Link definitions to syntax types

" Identifier
hi def link jsonexprIdent       Normal

" Types
hi def link jsonexprInt         Number
hi def link jsonexprFloat       Float

" String
hi def link jsonexprString      String
hi def link jsonexprBadChar     Error
hi def link jsonexprChar        Special

" Keywords
hi def link jsonexprNull        Constant
hi def link jsonexprTrue        Boolean
hi def link jsonexprFalse       Boolean

" Symbols
hi def link jsonexprConst       Identifier
hi def link jsonexprDynamic     Keyword
hi def link jsonexprFunction    Function

" Operators
hi def link jsonexprOp          Normal
hi def link jsonexprDelim       Normal
hi def link jsonexprGroup       Normal
hi def link jsonexprTerm        Normal

" Comments
hi def link jsonexprComment     Comment


"=============================================================================
" Cleanup

let b:current_syntax = "jsonexpr"
