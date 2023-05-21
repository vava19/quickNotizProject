#ifndef NOTE_HPP
#define NOTE_HPP

#include <wx/string.h>

class Note
{
public:
    wxString title;
    wxString content;

    Note(const wxString& _title = "", const wxString& _content = "");
    
    wxString getTitle() const;
    wxString getContent() const;
};



#endif // NOTE_HPP
