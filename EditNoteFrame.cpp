#include "EditNoteFrame.hpp"
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/wx.h>

EditNoteFrame::EditNoteFrame(wxFrame* parent, const Note& note, sqlite3* db)
    : wxFrame(parent, wxID_ANY, "Edit Note", wxDefaultPosition, wxSize(700, 700)), noteData(note), database(db)
{
    SetBackgroundColour(wxColour(178, 221, 230)); //styling
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // overtake the original title from vector
    originalTitle = note.title;

    // Title
    wxBoxSizer* titleSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* titleLabel = new wxStaticText(this, wxID_ANY, "Title: ");
    titleSizer->Add(titleLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    titleTextCtrl = new wxTextCtrl(this, wxID_ANY, noteData.title);
    titleSizer->Add(titleTextCtrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    mainSizer->Add(titleSizer, 0, wxEXPAND);

    // Conteent from input field
    wxStaticText* contentLabel = new wxStaticText(this, wxID_ANY, "Content: ");
    mainSizer->Add(contentLabel, 0, wxALIGN_LEFT | wxALL, 5);
    contentTextCtrl = new wxTextCtrl(this, wxID_ANY, noteData.content, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    mainSizer->Add(contentTextCtrl, 1, wxEXPAND | wxALL, 5);

    // Save Btn
    wxButton* saveButton = new wxButton(this, wxID_ANY, "Save");
    saveButton->Bind(wxEVT_BUTTON, &EditNoteFrame::OnSaveClicked, this);
    mainSizer->Add(saveButton, 0, wxALIGN_LEFT | wxALL, 5);

    // Delete Btn
    wxButton* deleteButton = new wxButton(this, wxID_ANY, "Delete");
    deleteButton->Bind(wxEVT_BUTTON, &EditNoteFrame::OnDeleteClicked, this);
    mainSizer->Add(deleteButton, 0, wxALIGN_LEFT | wxALL, 5);

    SetSizerAndFit(mainSizer);
}

void EditNoteFrame::OnSaveClicked(wxCommandEvent& event)
{
    originalTitle = noteData.title;

    noteData.title = titleTextCtrl->GetValue();
    noteData.content = contentTextCtrl->GetValue();

    // Save the note to the dbi
    SaveNoteToDatabase();

    wxMessageBox("Note saved!");

    // will NOT the edit frame
    Close();
}

void EditNoteFrame::OnDeleteClicked(wxCommandEvent& event)
{
    noteData.title = titleTextCtrl->GetValue();

    // Delete the note from the dbi
    DeleteNoteFromDatabase();

    wxMessageBox("Note deleted!");


    // Destroy the edit frame
    Close();
}

void EditNoteFrame::DeleteNoteFromDatabase()
{
    // Open the database connection
    sqlite3_stmt* stmt;
    int rc = sqlite3_open("notes.db", &database);
    //std::cout << "database  successfully opened" <<rc <<std::endl;

    if (rc)
    {
        wxMessageBox("Failed to open the database!");
        return;
    }

    wxString query;

    // Prepare the DELETE statement
    query = "DELETE FROM notes WHERE title = ?";
    rc = sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to prepare the DELETE statement!");
        sqlite3_close(database);
        return;
    }

    // Bind the value
    sqlite3_bind_text(stmt, 1, originalTitle.c_str(), originalTitle.length(), SQLITE_STATIC);
    //std::cout << "Note Title: " << noteData.title << std::endl;

    // Execute the DELETE statement
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        wxMessageBox("Failed to execute the DELETE statement!");
        sqlite3_finalize(stmt);
        sqlite3_close(database);
        return;
    }

    
    sqlite3_finalize(stmt);

     // Close the dbi verbindung
    sqlite3_close(database);

}

void EditNoteFrame::SaveNoteToDatabase()
{
    // Open the database connection
    sqlite3_stmt* stmt;
    int rc = sqlite3_open("notes.db", &database);

    if (rc)
    {
        wxMessageBox("Failed to open the database!");
        return;
    }

    wxString query;

    // Prepare the UPDATE statement
    query = "UPDATE Notes SET Title = ?, Content = ? WHERE Title = ?;";
    rc = sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to prepare the UPDATE statement!");
        sqlite3_close(database);
        return;
    }

    // Bind the values to the stmt parameters

    sqlite3_bind_text(stmt, 1, noteData.title.c_str(), noteData.title.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, noteData.content.c_str(), noteData.content.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, originalTitle.c_str(), originalTitle.length(), SQLITE_STATIC);

    // Execute the UPDATE statement
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        wxMessageBox("Failed to execute the UPDATE statement!");
        sqlite3_finalize(stmt);
        sqlite3_close(database);
        return;
    }

    // Finalize the stmt
    sqlite3_finalize(stmt);

    // Close the dbi connection
    sqlite3_close(database);
}
