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
  \brief Interface for class DialogFavourite.
*/

#ifndef _dialog_favourite_h_
#define _dialog_favourite_h_

#include "common.h"

class EvolvotronMain;

//! Provides a dialog for controlling which functions are available.
class DialogFavourite : public QDialog
{
 private:
  Q_OBJECT

 protected:
  //! Owner.
  EvolvotronMain* _parent;

  //! Function name to be used as the root node of new image function tres (no favourite if empty)
  std::string _favourite_function;

  //! Flag specifying whether favourite function should be exposed
  bool _favourite_function_unwrapped;

  //! Map function names to combo box entries.
  std::map<std::string,unsigned int> _favourite_fn_to_index;

  //! Look up function names from combo box.
  std::map<unsigned int,std::string> _index_to_favourite_fn;

  //! Select favourite function, if any.
  QComboBox* _favourite;

  //! Controls unwrapped state.
  QCheckBox* _unwrapped;

  //! Make GUI match _favourite_function and _favourite_function_unwrapped state
  void update_gui_from_state();

 protected slots:

  //! Invoked on combo-box selection.
  void changed_favourite(int i);

  //! Invoked on checkbox toggle.
  void changed_unwrapped(bool b);

 public:
  //! Constructor.
  DialogFavourite(EvolvotronMain* parent);

  //! Destructor.
  ~DialogFavourite();

  //! Accessor
  const std::string& favourite_function() const
    {
      return _favourite_function;
    }
  
  //! Accessor. Returns true if function name recognised.
  bool favourite_function(const std::string& f);
  
  //! Accessor.
  bool favourite_function_unwrapped() const
    {
      return _favourite_function_unwrapped;
    }

  //! Accessor.
  void favourite_function_unwrapped(bool v);
};

#endif
