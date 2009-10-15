#!/usr/bin/env python

# Convert Tim-style text to html or qml
#
# The Rules:
# - Lines with all upper case words to h2 or h3 capwords depending on next line underlining (first to h1/title though)
#   (must be 3 chars or more) 
#   (todo: relax to not all upper case... no need to capwords if not)
# - Other text to p, blank lines break a p
# - Lines beginning with "- " (NB space) to ul/li (bulleted)
# - Lines beginning with "-?" (no space) to ul/li (?) with <br> at end of first line
# - Words delim <xxx> to <i>xxx</i>
# "$ " at start of line indicates one line of code (add <br> too)

import sys
import string
import re

def line_of_dashes(n):
    r=""
    for i in xrange(n):
        r+="-"
    return r

def line_of_equals(n):
    r=""
    for i in xrange(n):
        r+="="
    return r

class TextToMarkup:

    def __init__(self,m,s):        
        self.startup=1       # True
        self.scope_p=0       # False
        self.scope_ul=0      # False
        self.scope_li=0      # False
        self.done_title=0    # False
        self.skipnextline=0  # False
        self.mode=m
        self.stringify=s

    def dispose(self,l):
        if self.stringify:
            self.output.write("\"")       # Actually, they should all have been &quot;-ed anyway
            for c in l:
                if c=="\"":
                    self.output.write("\\\"")
                else:
                    self.output.write(c)
            self.output.write("\\n\"\n")
        else:
            self.output.write(l+"\n")

    def process_word(self,w):
        r=""
        if len(w)<3:                      # Special case allows "<" or "<>" without turning italic
            for i in xrange(len(w)):
                if w[i]=="<":
                    r+="&lt;"
                elif w[i]==">":
                    r+="&gt;"
                else:
                    r+=w[i]
        else:
            for i in xrange(len(w)):
                if w[i]=="<":
                    r+="<i>"
                elif w[i]==">":
                    r+="</i>"
                elif w[i]=='"':
                    r+="&quot;"
                elif w[i]=="&":
                    r+="&amp;"
                else:
                    r+=w[i]
        return r

    def process_paragraph_text(self,txt):

        is_code=0 # False
        specialbreak=0 # False 
        r="  "

        if txt[0]=="-":
            if txt[1]==" ":
                txt=txt[2:]
            else:
                specialbreak=1 # True
            if self.scope_ul and self.scope_li:
                r+="</li>"
                self.scope_li=0 # False
            if not self.scope_ul:
                r+="<ul>"
                self.scope_ul=1 # True
            if not self.scope_li:
                r+="<li>"
                self.scope_li=1 # True

        elif txt[0]=="$":
            is_code=1           # True
            r+="<code>"
            txt=txt[2:]

        for w in txt.split():
            r+=self.process_word(w)
            r+=" "
        if is_code:
            r+="</code>"
        if specialbreak:
            r+="<br>"
        return r

    def process(self,in_stream,out_stream):
        self.output=out_stream
        self.input=in_stream

        if self.mode=="html":
            self.dispose("<html>")

        while 1:    # True
 
            if self.startup:
                self.currline_raw=in_stream.readline()
                self.nextline_raw=in_stream.readline()
                self.startup=0   # False
            else:
                self.currline_raw=self.nextline_raw            
                self.nextline_raw=in_stream.readline()

            if not self.currline_raw:
                break

            if self.skipnextline:
                self.skipnextline=0 # False
                continue

            # Should track last line too
            self.currline=self.currline_raw.strip()
            self.nextline=self.nextline_raw.strip()

            if len(self.currline)>2 and self.nextline==line_of_equals(len(self.currline)):
                if self.done_title:
                    self.dispose("<h2>"+string.capwords(self.currline)+"</h2>")
                    self.skipnextline=1 # True
                    continue
                else:
                    if (self.mode=="html"):
                        self.dispose("<head>")
                        self.dispose("<!--- AUTOMATICALLY GENERATED FILE : DO NOT EDIT --->")
                        self.dispose("<title>"+string.capwords(self.currline)+"</title>")
                        self.dispose("</head>")
                        self.dispose("<body>")
                    elif (self.mode=="qml"):
                        self.dispose("<qt title='"+string.capwords(self.currline)+"'>")
                    self.dispose("<h1>"+string.capwords(self.currline)+"</h1>")
                    self.done_title=1 # True
                    self.skipnextline=1 # True
                    continue
            elif len(self.currline)>2 and self.nextline==line_of_dashes(len(self.currline)):
                self.dispose("<h3>"+string.capwords(self.currline)+"</h3>")
                self.skipnextline=1 # True
                continue
            elif self.scope_p:
                if (len(self.currline)):
                    self.dispose(self.process_paragraph_text(self.currline))
                else:
                    if self.scope_li:
                        self.dispose("</li>")
                        self.scope_li=0 # False
                    if self.scope_ul:
                        self.dispose("</ul>")
                        self.scope_ul=0 # False                        
                    self.dispose("</p>")
                    self.scope_p=0 # False
            elif len(self.currline):
                self.dispose("<p>")
                self.dispose(self.process_paragraph_text(self.currline))
                self.scope_p=1 # True
            else:
                self.dispose("")

        if self.mode=="html":
            self.dispose("</body>")
            self.dispose("</html>")

#########################################
        
if __name__=='__main__':

    mode=None
    stringify=0 # False
    for i in xrange(1,len(sys.argv)):
        if sys.argv[i]=="-qml":
            mode="qml"
        if sys.argv[i]=="-html":
            mode="html"
        elif sys.argv[i]=="-s":
            stringify=1 # True
            
    t2m=TextToMarkup(mode,stringify)    # "html" and "qml" are alternatives.  Should be stringify option.
    t2m.process(sys.stdin,sys.stdout)

