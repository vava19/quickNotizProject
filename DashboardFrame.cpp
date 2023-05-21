#include "DashboardFrame.hpp"
#include "EditNoteFrame.hpp"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/settings.h>




DashboardFrame::DashboardFrame()
    : wxFrame(nullptr, wxID_ANY, "Note Dashboard", wxDefaultPosition, wxSize(800, 800))
{
    SetBackgroundColour(wxColour(173, 216, 230)); 
    // mainSizer just used for grid
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);


     // --    sizer for * dashboard elements hz
    wxBoxSizer* inputSizer = new wxBoxSizer(wxHORIZONTAL);

    // inputField for creating new note.
    titleTextCtrl = new wxTextCtrl(this, wxID_ANY);
     inputSizer->Add(titleTextCtrl, 1, wxALL | wxEXPAND, 5);


    //    per click add new note
    wxButton* addButton = new wxButton(this, wxID_ANY, "Add Note");
    addButton->SetFont(wxFont(wxFontInfo(12).Bold()));
    addButton->Bind(wxEVT_BUTTON, &DashboardFrame::OnAddNoteClicked, this);
    inputSizer->Add(addButton, 0, wxALL, 5);



    //   Button for refreshing dashboard frame content
    wxButton* refreshButton = new wxButton(this, wxID_ANY, "Refresh");
    refreshButton->SetFont(wxFont(wxFontInfo(12).Bold())); 
    refreshButton->Bind(wxEVT_BUTTON, &DashboardFrame::RefreshDashboard, this);
    inputSizer->Add(refreshButton, 0, wxALL, 5);

    mainSizer->Add(inputSizer, 0, wxEXPAND | wxALL, 10);

    // Create a grid sizer for the note boxes
    wxGridSizer* gridSizer = new wxGridSizer(5, 3, 10, 10);

    // Create    note boxes
    FetchNotesFromDatabase();

    for (size_t i = 0; i < notes.size(); ++i)
    {
        wxString title = notes[i].title;
        wxButton* button = new wxButton(this, wxID_HIGHEST + i, title, wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DashboardFrame::OnNoteSelected), nullptr, this);
        gridSizer->Add(button, 0, wxALL | wxEXPAND, 10);
    }

    mainSizer->Add(gridSizer, 1, wxALL | wxEXPAND, 10);

    SetSizerAndFit(mainSizer);


 // mouse movement on the surface
         wxStatusBar* statusBar = new wxStatusBar(this,wxID_ANY);
         SetStatusBar(statusBar);
        statusBar->SetStatusText("Mouse coordinates"); //
        statusBar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        this->Bind(wxEVT_MOTION, &DashboardFrame::OnMouseEvent, this); //panel


}

 //   mouse coordinates function
void DashboardFrame::OnMouseEvent(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();
    wxString message = wxString::Format("Mouse geht (x=%d y=%d)", mousePos.x, mousePos.y);  
    wxStatusBar* statusBar = GetStatusBar();
    if (statusBar)
        statusBar->SetStatusText(message);
    event.Skip();
}




void DashboardFrame::OnNoteSelected(wxCommandEvent& event)
{
    FetchNotesFromDatabase();
    int selectedNoteIndex = event.GetId() - wxID_HIGHEST; // based on ButtonID calc index of selected note

    // Open the EditNoteFrame for the selected note
    EditNoteFrame* editFrame = new EditNoteFrame(this, notes[selectedNoteIndex], nullptr);
    editFrame->Show();
}

void DashboardFrame::FetchNotesFromDatabase()
{
    // Open the database connection
    sqlite3* database;
    int rc = sqlite3_open("notes.db", &database);

    if (rc)
    {
        wxMessageBox("Failed to open the database!");
        return;
    }

    //  if it doesn't already exist    Create the notes table
    wxString createTableQuery = "CREATE TABLE IF NOT EXISTS notes (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, content TEXT)";
    rc = sqlite3_exec(database, createTableQuery.c_str(), nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to create the notes table!");
        sqlite3_close(database);
        return;
    }

    // Fetch * notes from the database
    wxString selectQuery = "SELECT title, content FROM notes";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(database, selectQuery.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to prepare the SELECT statement!");
        sqlite3_close(database);
        return;
    }
    // Clear the existing notes vector
    notes.clear();

    // Execute the SELECT statement and fetch the notes
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        wxString title(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        wxString content(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        notes.push_back(Note(title, content));
    }

      // Final SQL statement
    sqlite3_finalize(stmt);

      // Close the db connection
    sqlite3_close(database);
}

void DashboardFrame::OnAddNoteClicked(wxCommandEvent& event)
{
    wxString newTitle = titleTextCtrl->GetValue();
    if (!newTitle.IsEmpty())
    {
         // Create a new quickNOte + title
        Note newNote(newTitle);

        // Save the new note to the SQLite
        SaveNoteToDatabase(newNote);

          // A pushing new note into notes vector   ...
        notes.push_back(newNote);

        // Create a new button for this added note to the vector
        wxButton* button = new wxButton(this, wxID_HIGHEST + notes.size() - 1, newTitle, wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DashboardFrame::OnNoteSelected), nullptr, this);

         // Get tthe grid sizer and insert new btn
        wxSizer* gridSizer = GetSizer()->GetItem(1)->GetSizer();
        gridSizer->Add(button, 0, wxALL | wxEXPAND, 10);

        // Refresh + upt. the dasahboard
        gridSizer->Layout();
        GetSizer()->Fit(this);  // adjust size of the frame

        // Clear the textCtrl inputfield
        titleTextCtrl->Clear();
    }
}

void DashboardFrame::SaveNoteToDatabase(const Note& note)
{
    // Open the database connection
    sqlite3* database;
    int rc = sqlite3_open("notes.db", &database);

    if (rc)
    {
        wxMessageBox("Failed to open the database!");
        return;
    }

    wxString query;

    // Prepare the INSERT statement
    query = "INSERT INTO notes (title, content) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to  INSERT stmt");
        sqlite3_close(database);
        return;
    }

    // Bind the values to the statement parameters
    sqlite3_bind_text(stmt, 1, note.title.c_str(), note.title.length(), SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, note.content.c_str(), note.content.length(), SQLITE_STATIC);

     // Execute INSERT stmt
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        wxMessageBox("Failed to execute the INSERT stmt!");
        sqlite3_finalize(stmt);
        sqlite3_close(database);
        return;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

     // Close dbms conn.
    sqlite3_close(database);
}

void DashboardFrame::RefreshDashboard(wxCommandEvent& event)
{
    FetchNotesFromDatabase();
    // Clear the existing buttons
    wxSizer* gridSizer = GetSizer()->GetItem(1)->GetSizer();
    gridSizer->Clear(true);

    // Add new buttons for the updated notes vector
    for (size_t i = 0; i < notes.size(); ++i)
    {
        wxString title = notes[i].title;
        wxButton* button = new wxButton(this, wxID_HIGHEST + i, title, wxDefaultPosition, wxDefaultSize, wxBU_LEFT);
        button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DashboardFrame::OnNoteSelected), nullptr, this);
        gridSizer->Add(button, 0, wxALL | wxEXPAND, 10);
    }

    // Refresh + update dash
    gridSizer->Layout();
    GetSizer()->Fit(this);
}
