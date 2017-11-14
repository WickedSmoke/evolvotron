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
  \brief Implementation of class DialogFunctions.
*/

#include "dialog_functions.h"

#include "evolvotron_main.h"
#include "function_registry.h"

/*! Dialog controls function weightings and related parameters.
 */
DialogFunctions::DialogFunctions(EvolvotronMain* parent,MutationParametersQObject* mp)
  :QDialog(parent)
  ,_parent(parent)
  ,_mutation_parameters(mp)
{
  assert(_parent!=0);

  setWindowTitle("Functions");
  setMinimumSize(480,360);     // Have to scroll through tabs if don't set this
  setSizeGripEnabled(true);

  setLayout(new QVBoxLayout);

  QTabWidget*const tabs=new QTabWidget;
  layout()->addWidget(tabs);

  QPushButton*const ok=new QPushButton("OK");
  layout()->addWidget(ok);
  ok->setDefault(true);

  for (int c=-1;c<FnClassifications;c++)
    {
      QWidget*const tab_content=new QWidget;
      QVBoxLayout*const tab_content_layout=new QVBoxLayout;
      tab_content->setLayout(tab_content_layout);
      tabs->addTab(tab_content,(c==-1 ? "All" : function_classification_name[c]));

      QWidget*const buttons=new QWidget;
      buttons->setLayout(new QHBoxLayout);
      tab_content->layout()->addWidget(buttons);
      QPushButton*const button_less=new QPushButton("Less");
      QPushButton*const button_rand=new QPushButton("Randomize");
      QPushButton*const button_more=new QPushButton("More");
      buttons->layout()->addWidget(button_less);
      buttons->layout()->addWidget(button_rand);
      buttons->layout()->addWidget(button_more);

      SignalExpanderClickedUint*const bx_rand=new SignalExpanderClickedUint(this,(c==-1 ? 0xffffffff : (1<<c)));

      connect(button_rand,SIGNAL(clicked()),bx_rand,SLOT(clicked()));
      connect(bx_rand,SIGNAL(clicked(uint)),this,SLOT(clicked_button_rand(uint)));

      QScrollArea*const scrollarea=new QScrollArea;
      scrollarea->setWidgetResizable(true);
      tab_content->layout()->addWidget(scrollarea);
      tab_content_layout->setStretchFactor(scrollarea,1);
      
      QWidget*const scrollcontent=new QWidget;
      scrollcontent->setLayout(new QVBoxLayout);
      scrollarea->setWidget(scrollcontent);

      for (FunctionRegistry::Registrations::const_iterator it=_parent->mutation_parameters().function_registry().registrations().begin();
	   it!=_parent->mutation_parameters().function_registry().registrations().end();
	   it++)
	{
	  const FunctionRegistration& fn=
#if BOOST_VERSION >= 103400
	    *(it->second);
#else
	    *it;
#endif
	  if (c==-1 || fn.classification()&(1<<c))
	    {
	      QGroupBox*const g=new QGroupBox(fn.name().c_str());
	      scrollarea->widget()->layout()->addWidget(g);
	      g->setLayout(new QHBoxLayout);
	      
	      QSizePolicy spx(QSizePolicy::Expanding,QSizePolicy::Preferred);
	      g->setSizePolicy(spx);
	      
	      g->layout()->addWidget(new QLabel("2^-10"));
	      QSlider*const s=new QSlider(Qt::Horizontal);
	      g->layout()->addWidget(s);
	      s->setMinimum(-10);
	      s->setMaximum(0);
	      s->setValue(0);
	      s->setTickInterval(1);
	      s->setTickPosition(QSlider::TicksBothSides);
	      s->setSizePolicy(spx);
	      g->layout()->addWidget(new QLabel("1"));
	      
	      _slider_to_function.insert(std::make_pair(s,&fn));

	      SignalExpanderValueChangedQSlider*const sx=new SignalExpanderValueChangedQSlider(this,s);
	      connect(
		      s,SIGNAL(valueChanged(int)),
		      sx,SLOT(valueChanged(int))
		      );
	      connect(
		      sx,SIGNAL(valueChanged(QSlider*,int)),
		      this,SLOT(changed_function_weighting(QSlider*,int))
		      );

	      // Wire up buttons
#if QT_VERSION >= 0x050000
	      // New Qt5 style supports lambdas!
	      connect(
                button_less,&QPushButton::clicked,
                s,[s](){s->triggerAction(QAbstractSlider::SliderSingleStepSub);}
              );
	      connect(
                button_more,&QPushButton::clicked,
                s,[s](){s->triggerAction(QAbstractSlider::SliderSingleStepAdd);}
              );
#else
	      // Otherwise fall back to old way
	      // Fortunately the deprecated subtractStep/addStep members are still available in Qt4
	      connect(
                button_less,SIGNAL(clicked()),
                s,SLOT(subtractStep())
              );
	      connect(
                button_more,SIGNAL(clicked()),
                s,SLOT(addStep())
              );
#endif
	    }
	}
    }

  // And add another tab for all the branching-ratio/dilution controls
  {
    QWidget*const vbox=new QWidget;
    vbox->setLayout(new QVBoxLayout);
    tabs->addTab(vbox,"Dilution");
    
    _branching_ratio=new QLabel;
    vbox->layout()->addWidget(_branching_ratio);
    
    QGroupBox*const c0=new QGroupBox("Required branching ratio after dilution");
    c0->setLayout(new QHBoxLayout);
    vbox->layout()->addWidget(c0);

    c0->layout()->addWidget(new QLabel("0.1"));
    _slider_target_branching_ratio=new QSlider(Qt::Horizontal);
    c0->layout()->addWidget(_slider_target_branching_ratio);
    _slider_target_branching_ratio->setMinimum(10);
    _slider_target_branching_ratio->setMaximum(90);
    _slider_target_branching_ratio->setValue(50);
    _slider_target_branching_ratio->setTickInterval(10);
    _slider_target_branching_ratio->setTickPosition(QSlider::TicksBothSides);
    _slider_target_branching_ratio->setToolTip("The branching ratio must be diluted to <1.0 to prevent formation of infinitely large function-trees.\nWarning: setting a high value results in complex function trees taking a long time to compute.\nSetting a low value results in very simple images.");
    c0->layout()->addWidget(new QLabel("0.9"));

    QGroupBox*const c1=new QGroupBox("Of diluting nodes, proportion constant:");
    c1->setLayout(new QHBoxLayout);
    vbox->layout()->addWidget(c1);
    c1->layout()->addWidget(new QLabel("0.0"));
    _slider_proportion_constant=new QSlider(Qt::Horizontal,c1);
    c1->layout()->addWidget(_slider_proportion_constant);
    _slider_proportion_constant->setMinimum(0);
    _slider_proportion_constant->setMaximum(100);
    _slider_proportion_constant->setValue(50);
    _slider_proportion_constant->setTickInterval(10);
    _slider_proportion_constant->setTickPosition(QSlider::TicksBothSides);
    _slider_proportion_constant->setToolTip("This specifies the proportion of diluting nodes which will be constant.");
    c1->layout()->addWidget(new QLabel("1.0"));
    
    QGroupBox*const c2=new QGroupBox("Of non-constant diluting nodes, proportion transforms");
    c2->setLayout(new QHBoxLayout);
    vbox->layout()->addWidget(c2);
    c2->layout()->addWidget(new QLabel("0.0"));
    _slider_identity_supression=new QSlider(Qt::Horizontal,c2);
    c2->layout()->addWidget(_slider_identity_supression);
    _slider_identity_supression->setMinimum(0);
    _slider_identity_supression->setMaximum(100);
    _slider_identity_supression->setValue(50);
    _slider_identity_supression->setTickInterval(10);
    _slider_identity_supression->setTickPosition(QSlider::TicksBothSides);
    _slider_identity_supression->setToolTip("This specifies the proportion of non-constant diluting nodes which will be transforms (c.f identity nodes).");
    c2->layout()->addWidget(new QLabel("1.0"));

  connect(
	  _slider_target_branching_ratio,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_target_branching_ratio(int))
	  );

  connect(
          ok,SIGNAL(clicked()),
          this,SLOT(hide())
          );

  connect(
	  _slider_proportion_constant,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_proportion_constant(int))
	  );

  connect(
	  _slider_identity_supression,SIGNAL(valueChanged(int)),
	  this,SLOT(changed_identity_supression(int))
	  );
  }

  setup_from_mutation_parameters();

  connect(
	  _mutation_parameters,SIGNAL(changed()),
	  this,SLOT(mutation_parameters_changed())
	  );
}

DialogFunctions::~DialogFunctions()
{}

void DialogFunctions::setup_from_mutation_parameters()
{
  const real b=_mutation_parameters->random_function_branching_ratio();
  const real s=_mutation_parameters->proportion_basic();
  const real br=(1.0-s)*b;

  std::stringstream msg;
  msg 
    << "Undiluted random function branching ratio is " << b << "\n"
    << "Diluting in proportion " << s << " to obtain required branching ratio.";
  _branching_ratio->setText(msg.str().c_str());

  _slider_target_branching_ratio->setValue(static_cast<int>(100.0*br+0.5));
  _slider_proportion_constant->setValue(static_cast<int>(0.5+100.0*_mutation_parameters->proportion_constant()));
  _slider_identity_supression->setValue(static_cast<int>(0.5+100.0*_mutation_parameters->identity_supression()));

  for (std::map<QSlider*,const FunctionRegistration*>::const_iterator it=_slider_to_function.begin();
       it!=_slider_to_function.end();
       it++
       )
    {
      const real w=_mutation_parameters->get_weighting((*it).second);
      const int iw=static_cast<int>(floor(0.5+(log(w)*(1.0/M_LN2))));
      if (iw!=(*it).first->value())
	{
	  (*it).first->setValue(iw);
	}
    }
}

void DialogFunctions::changed_target_branching_ratio(int v)
{
  const real target_branching_ratio=v/100.0;
  
  // Want to solve tgt=proportion_basic*0.0+(1.0-proportion_basic)*random_function_branching_ratio
  const real proportion_basic=1.0-target_branching_ratio/_mutation_parameters->random_function_branching_ratio();

  std::clog 
    << "Basic-node dilution proportion set to " 
    << proportion_basic 
    << " giving a branching ratio of "
    << (1.0-proportion_basic)*_mutation_parameters->random_function_branching_ratio()
    << "\n";

  _mutation_parameters->proportion_basic(proportion_basic);

  setup_from_mutation_parameters();
}

void DialogFunctions::changed_proportion_constant(int v)
{
  _mutation_parameters->proportion_constant(v/100.0);
}

void DialogFunctions::changed_identity_supression(int v)
{
  _mutation_parameters->identity_supression(v/100.0);
}

void DialogFunctions::changed_function_weighting(QSlider* s,int v)
{
  std::map<QSlider*,const FunctionRegistration*>::const_iterator it=_slider_to_function.find(s);
  if (it==_slider_to_function.end())
    {
      std::clog << "DialogFunctions::changed_function_weighting : unknown source slider, ignoring\n";
    }
  else
    {
      const FunctionRegistration* fn=(*it).second;
      
      const real w=pow(2,v);
      std::clog << fn->name() << " weighting changed to " << w << "\n";
      _mutation_parameters->change_function_weighting(fn,w);
    }
}

void DialogFunctions::clicked_button_rand(uint mask)
{
  std::clog << "Randomizing functions matching mask " << mask << "\n";
  _mutation_parameters->randomize_function_weightings_for_classifications(mask);
}

void DialogFunctions::mutation_parameters_changed()
{
  //std::clog << "[DialogFunctions::mutation_parameters_changed()]\n";
  setup_from_mutation_parameters();  
}
