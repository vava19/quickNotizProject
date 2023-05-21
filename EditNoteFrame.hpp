#ifndef EDIT_NOTE_FRAME_HPP
#define EDIT_NOTE_FRAME_HPP


#include <wx/frame.h>
#include <wx/textctrl.h>
#include <sqlite3.h>

#include "Note.hpp"

class EditNoteFrame : public wxFrame
{
public:
    EditNoteFrame(wxFrame* parent, const Note& note, sqlite3* db);

    void OnSaveClicked(wxCommandEvent& event);
    void OnDeleteClicked(wxCommandEvent& event);

    // help function
    void DeleteNoteFromDatabase();
    void SaveNoteToDatabase();

private:
    Note noteData;
    wxTextCtrl* titleTextCtrl;
    wxTextCtrl* contentTextCtrl;
    sqlite3* database;
    wxString originalTitle;
};


#endif