/**************************************************************************/
/*  Copyright 2012 Tim Day                                                */
/*                                                                        */
/*  This file is part of Evolvotron                                       */
/*                                                                        */
/*  Evolvotron is free software: you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by  */
/*  the Free Software Foundation, either version 3 of the License, or     */
/*  (at your option) any later version.                                   */
/*                                                                        */
/*  Evolvotron is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details.                          */
/*                                                                        */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Evolvotron.  If not, see <http://www.gnu.org/licenses/>.   */
/**************************************************************************/

/*! \file
  \brief Implementation of class DialogHelp.
*/



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
"    F - Toggle fullscreen mode"
"  </li>"
"  <li>"
"    M - Hide menu and status bars"
"  </li>"
"  <li>"
"    Esc - Returns to normal mode from full-screen/menu-hidden mode."
"  </li>"
"  <li>"
"    R - Reset (reset mutation parameters and locks)"
"  </li>"
"  <li>"
"    T - Restart (preserve mutation parameters and locks)"
"  </li>"
"  <li>"
"    X - Remix (randomize function weights and restart)"
"  </li>"
"  <li>"
"    Ctrl-Z or U - Undo"
"  </li>"
"</ul>"
"<h2>Mouse</h2>"
"<h3>Left-click</h3>"
"<p>Spawns mutant offspring.</p>"
"<h3>Middle-drag</h3>"
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
"<p>"
"Command line options are described in the user manual."
"A short summary can be obtained by invoking evolvotron with the <code>-h</code> or <code>--help</code> option"
"</p>"
"</qt>"
;

static const char*const helptext_long=
#include "usage_text.h"
;

DialogHelp::DialogHelp(QWidget* parent,bool full)
  :QDialog(parent)
{
  setWindowTitle(full ? "Evolvotron User Manual" : "Evolvotron Quick Reference");
  setMinimumSize(480,360);
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);

  QTextBrowser*const browser=new QTextBrowser;
  layout()->addWidget(browser);
  browser->setText(full ? helptext_long : helptext_short);

  QPushButton*const ok=new QPushButton("OK");
  layout()->addWidget(ok);
  ok->setDefault(true);

  connect
    (
     ok,SIGNAL(clicked()),
     this,SLOT(hide())
     );
}

DialogHelp::~DialogHelp()
{}
