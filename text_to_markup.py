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
# - Lines ending with ":" or with different amount of indent on next line add <br> 
# - Words delim <xxx> to <i>xxx</i>
# "$ " at start of line indicates one line of code

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

    def __init__(self,m):        
        self.startup=True
        self.scope_p=False
        self.scope_ul=False
        self.scope_li=False
        self.done_title=False
        self.skipnextline=False
        self.mode=m

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

        is_code=False
        specialbreak=txt[len(txt)-1]==":" or self.currline_indent!=self.nextline_indent
        r="  "

        if txt[0]=="-":
            if txt[1]==" ":
                txt=txt[2:]
            else:
                specialbreak=True
            if self.scope_ul and self.scope_li:
                r+="</li>"
                self.scope_li=False
            if not self.scope_ul:
                r+="<ul>"
                self.scope_ul=True
            if not self.scope_li:
                r+="<li>"
                self.scope_li=True

        elif txt[0]=="$":
            is_code=True
            r+="<code>"
            txt=txt[2:]
            specialbreak=True

        for w in txt.split():
            r+=self.process_word(w)
            r+=" "
        if is_code:
            r+="</code>"
        if specialbreak:
            r+="<br>"
        return r

    def process(self,in_stream):
        if self.mode=="html":
            print "<html>"

        while True:

            if self.startup:
                self.currline_raw=in_stream.readline()
                self.nextline_raw=in_stream.readline()
                self.startup=False
            else:
                self.currline_raw=self.nextline_raw            
                self.nextline_raw=in_stream.readline()

            if not self.currline_raw:
                break

            if self.skipnextline:
                self.skipnextline=False
                continue

            # Should track last line too
            self.currline=self.currline_raw.strip()
            self.nextline=self.nextline_raw.strip()

            self.currline_indent=len(self.currline)-len(self.currline.lstrip())
            self.nextline_indent=len(self.nextline)-len(self.nextline.lstrip())

            if len(self.currline)>2 and self.nextline==line_of_equals(len(self.currline)):
                if self.done_title:
                    print "<h2>"+string.capwords(self.currline)+"</h2>"
                    self.skipnextline=True
                    continue
                else:
                    if (self.mode=="html"):
                        print "<head>"
                        print "<title>"+string.capwords(self.currline)+"</title>"
                        print "</head>"
                        print "<body>"
                    elif (self.mode=="qml"):
                        print "<qt title='"+string.capwords(self.currline)+"'>"
                    print "<h1>"+string.capwords(self.currline)+"</h1>"
                    self.done_title=True
                    self.skipnextline=True
                    continue
            elif len(self.currline)>2 and self.nextline==line_of_dashes(len(self.currline)):
                print "<h3>"+string.capwords(self.currline)+"</h3>"
                self.skipnextline=True
                continue
            elif self.scope_p:
                if (len(self.currline)):
                    print self.process_paragraph_text(self.currline)
                else:
                    if self.scope_li:
                        print "</li>"
                        self.scope_li=False
                    if self.scope_ul:
                        print "</ul>"
                        self.scope_ul=False                        
                    print "</p>"
                    self.scope_p=False
            elif len(self.currline):
                print "<p>"
                print self.process_paragraph_text(self.currline)
                self.scope_p=True
            else:
                print

        if self.mode=="html":
            print "</body>"
            print "</html>"

#########################################
        
if __name__=='__main__':

    t2m=TextToMarkup("html")    # "html" and "qml" are alternatives.  Should be stringify option.
    t2m.process(sys.stdin)

