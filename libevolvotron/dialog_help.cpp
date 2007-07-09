// Source file for evolvotron
// Copyright (C) 2007 Tim Day
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*! \file
  \brief Implementation of class DialogHelp.
*/

#include "libevolvotron_precompiled.h"

#include "dialog_help.h"

#include "license.h"

//! The text to be displayed
/*! \todo Should obtain this from processing external (X)HTML docs, if there were any.
 */
static const char*const helptext_short=
"<qt title='Evolvotron Quick Reference'>"
"<h1>Evolvotron Quick Reference</h1>"
"<h2>Keyboard</h2>"
"<ul>"
"  <li>"
"    Ctrl-F - Toggle fullscreen mode"
"  </li>"
"  <li>"
"    Ctrl-M - Toggle menubar hiding"
"  </li>"
"  <li>"
"    Esc - Returns to normal mode from full-screen/menu-hidden mode."
"  </li>"
"  <li>"
"    Ctrl-R - Restart"
"  </li>"
"  <li>"
"    Ctrl-Z - Undo"
"  </li>"
"</ul>"
"<h2>Mouse</h2>"
"<h3>Left-click</h3>"
"<p>Spawns mutant offspring.</p>"
"<h3>Middle-drag</h2>"
"<ul>"
"  <li>"
"    Unmodified - pan"
"  </li>"
"  <li>"
"    Shift - isotropic zoom"
"  </li>"
"  <li>"
"    Alt-Shift - anisotropic zoom"
"  </li>"
"  <li>"
"    Ctrl - rotate about centre"
"  </li>"
"  <li>"
"    Ctrl-Alt - shear"
"  </li>"
"</ul>"
"<h3>Right-click</h3>"
"<p>Brings up context menu.</p>"
"<h2>Command Line Options</h2>"
"<h3>General</h3>"
"<ul>"
"  <li>"
"    <b>-g</b> <i>cols</i> <i>rows</i> : Set display cells in grid."
"  </li>"
"  <li>"
"    <b>-t</b> <i>threads</i> : Set number of compute threads."
"  </li>"
"  <li>"
"    <b>-n</b> <i>niceness</i> : Set priority of compute threads relative to main (GUI) thread."
"  </li>"
"  <li>"
"    <b>-F</b> : Start in fullscreen mode."
"  </li>"
"  <li>"
"    <b>-M</b> : Start with menu and status bars hidden."
"  </li>"
"</ul>"
"<h3>Animation</h3>"
"<ul>"
"  <li>"
"    <b>-f</b> <i>frames</i> : Generate animations with specified number of frames."
"  </li>"
"  <li>"
"    <b>-r</b> <i>frames-per-second</i> : Display animations at specified framerate."
"  </li>"
"  <li>"
"    <b>-linz</b> : Vary z linearly with time instead of sinusoidally."
"  </li>"
"</ul>"
"<h3>Power User</h3>"
"<ul>"
"  <li>"
"    <b>-v</b> : Spew debug info to stderr.  Includes list of supported function names."
"  </li>"
"  <li>"
"    <b>-x</b> <i>function</i> : Set specified function name as \"favourite\" at top-level, with random wrapper functions."
"  </li>"
"  <li>"
"    <b>-X</b> <i>function</i> : Set specified function name as \"favourite\" at top-level, with no wrapper."
"  </li>"
"  <li>"
"    <b>-spheremap</b> : Work with spheremaps instead of planar textures.  (NB Middle-mouse drags do not behave sensibly in this mode)."
"  </li>"
"</ul>"
"</qt>"
;

static const char*const helptext_long=
#include "usage_text.h"
;

DialogHelp::DialogHelp(QWidget* parent,bool full)
  :QDialog(parent)
{
  setCaption(full ? "Evolvotron User Manual" : "Evolvotron Quick Reference");
  setMinimumSize(300,200);

  _vbox=new QVBox(this);
  
  _browser=new QTextBrowser(_vbox);
  _browser->setText(full ? helptext_long : helptext_short);

  _ok=new QPushButton("OK",_vbox);

  //! \todo: These button settings don't seem to do anything.  Find out what's up.
  _ok->setAutoDefault(true);
  _ok->setDefault(true);

  connect(
	  _ok,SIGNAL(clicked()),
	  this,SLOT(hide())
	  );
}

void DialogHelp::resizeEvent(QResizeEvent*)
{
  _vbox->resize(size());
}
