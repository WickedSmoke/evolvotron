#!/usr/bin/boron
/*
  Convert Tim-style text to html or qml

  The Rules:
  - Lines with all upper case words to h2 or h3 capwords depending on next
    line underlining (first to h1/title though)
    (must be 3 chars or more)
    (todo: relax to not all upper case... no need to capwords if not)
  - Other text to p, blank lines break a p
  - Lines beginning with "- " (NB space) to ul/li (bulleted)
  - Lines beginning with "-?" (no space) to ul/li (?) with <br> at end of
    first line
  - Words delim <xxx> to <i>xxx</i>
  "$ " at start of line indicates one line of code (add <br> too)
*/

line_of_dashes: func [n] [
    slice "----------------------------------------" n
]

line_of_equals: func [n] [
    slice "========================================" n
]

emit: :print
emit-stringify: func [text] [
    prin '"'        ; Actually, they should all have been quoted anyway.
    prin construct text ['"' {\"}]
    print {\n"}
]

process_word: func [w] [
    either lt? size? w 3 [
        ; Special case allows "<" or "<>" without turning italic
        construct w ['<' "&lt;"  '>' "&gt;"]
    ][
        construct w ['<' "<i>"  '>' "</i>"  '"' "&quot;"  '&' "&amp;"]
    ]
]

scope_p:
scope_ul:
scope_li: none

process_paragraph_text: func [txt /extern scope_ul scope_li] [
    is_code: specialbreak: false
    r: copy "  "

    switch first txt [
        '-' [
            either eq? ' ' second txt [
                txt: skip txt 2
            ][
                specialbreak: true
            ]
            if all [scope_ul scope_li] [
                append r "</li>"
                scope_li: false
            ]
            ifn scope_ul [
                append r "<ul>"
                scope_ul: true
            ]
            ifn scope_li [
                append r "<li>"
                scope_li: true
            ]
        ]
        '$' [
            is_code: true
            append r "<code>"
            txt: skip txt 2
        ]
    ]

    append r process_word txt
    ;foreach w txt.split() [appair r process_word w ' ']

    if is_code      [append r "</code>"]
    if specialbreak [append r "<br>"]
    r
]

capwords: func [text] [
    text: lowercase text
    poke text 1 uppercase first text
    parse text [some[
        thru ' ' tok: (poke tok 1 uppercase first tok)
    ]]
    text
]

;----------------------------------------

input: none
mode: 'html

forall args [
    switch first args [
        "-qml"  [mode: 'qml]
        "-html" [mode: 'html]
        "-s"    [emit: :emit-stringify]
        [input: first args]
   ]
]

ifn input [error "No document specified."]
input: read/text input

read-line: does [
    either eol: find input '^/' [
        ++ eol
        input0: slice input eol
        input: eol
        trim input0
    ] none
]

if eq? mode 'html [
    emit "<html>"
]

currline: read-line
nextline: read-line
done_title: none
skipnextline: false

while [currline] [
    line-len: size? currline

    case [
    all [gt? line-len 2  eq? nextline line_of_equals line-len] [
        either done_title [
            emit rejoin ["<h2>" capwords currline "</h2>"]
            skipnextline: true
        ][
            switch mode [
                'html [
                    emit "<head>"
                    emit "<!--- AUTOMATICALLY GENERATED FILE : DO NOT EDIT --->"
                    emit rejoin [
                        "<title>" capwords currline "</title>"
                    ]
                    emit "</head>"
                    emit "<body>"
                ]
                'qml [
                    emit rejoin ["<qt title='" capwords currline "'>"]
                ]
            ]
            emit rejoin ["<h1>" capwords currline "</h1>"]
            done_title: true
            skipnextline: true
        ]
    ]
    all [gt? line-len 2  eq? nextline line_of_dashes line-len] [
        emit rejoin ["<h3>" capwords currline "</h3>"]
        skipnextline: true
    ]
    scope_p [
        either not empty? currline [
            emit process_paragraph_text currline
        ][
            if scope_li [
                emit "</li>"
                scope_li: false
            ]
            if scope_ul [
                emit "</ul>"
                scope_ul: false
            ]
            emit "</p>"
            scope_p: false
        ]
    ]
    not empty? currline [
        emit "<p>"
        emit process_paragraph_text currline
        scope_p: true
    ]
    true [
        emit ""
    ]
    ]

    if skipnextline [
        skipnextline: false
        nextline: read-line
    ]
    currline: nextline
    nextline: read-line
]

if eq? mode 'html [
    emit "</body>"
    emit "</html>"
]
