// Source file for evolvotron
// Copyright (C) 2005 Tim Day
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
  \brief Interface for class DialogFavourite.
*/

#ifndef _dialog_favourite_h_
#define _dialog_favourite_h_

class EvolvotronMain;

//! Provides a dialog for controlling which functions are available.
class DialogFavourite : public QDialog
{
 private:
  Q_OBJECT

  typedef QDialog Superclass;

 protected:
  //! Owner.
  EvolvotronMain* _parent;

  //! Function name to be used as the root node of new image function tres (no favourite if empty)
  std::string _favourite_function;

  //! Flag specifying whether favourite function should be exposed
  bool _favourite_function_unwrapped;

  //! Top level holder of all the dialog content.
  QVBox* _dialog_content;

  //! Select favourite function, if any.
  QComboBox* _favourite;

  //! Map function names to combo box entries.
  std::map<std::string,unsigned int> _favourite_fn_to_index;

  //! Look up function names from combo box.
  std::map<unsigned int,std::string> _index_to_favourite_fn;

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
  virtual ~DialogFavourite();

  //! Accessor
  const std::string& favourite_function() const
    {
      return _favourite_function;
    }
  
  //! Accessor. Returns true if function name recognised.
  const bool favourite_function(const std::string& f);
  
  //! Accessor.
  const bool favourite_function_unwrapped() const
    {
      return _favourite_function_unwrapped;
    }

  //! Accessor.
  void favourite_function_unwrapped(bool v);

  //! Handle resizes.
  virtual void resizeEvent(QResizeEvent* e);

};

#endif
