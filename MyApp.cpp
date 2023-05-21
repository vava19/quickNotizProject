#include <wx/msgdlg.h>
#include "MyApp.hpp"
#include "DashboardFrame.hpp"
//
bool MyApp::OnInit()
{
    // Open the dbi
    sqlite3* database;
    int rc = sqlite3_open("notes.db", &database);

    if (rc)
    {
        wxMessageBox("Failed to open the database!");
        return false;
    }

    // Create the notes table if it doesn't exist
    wxString createTableQuery = "CREATE TABLE IF NOT EXISTS notes (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, content TEXT)";
    rc = sqlite3_exec(database, createTableQuery.c_str(), nullptr, nullptr, nullptr);

    if (rc != SQLITE_OK)
    {
        wxMessageBox("Failed to create the notes table!");
        sqlite3_close(database);
        return false;
    }

    // Close the database connection
    sqlite3_close(database);

    DashboardFrame* dashboardFrame = new DashboardFrame();
    dashboardFrame->Show();

    return true;
}

wxIMPLEMENT_APP(MyApp);
