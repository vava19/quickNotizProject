#include "Note.hpp"

Note::Note(const wxString& _title, const wxString& _content)
    : title(_title), content(_content)
{
}

wxString Note::getTitle() const {
    return title;
}

wxString Note::getContent() const {
    return content;
}
