#ifndef MY_APP_HPP
#define MY_APP_HPP

#include <wx/wx.h>
#include "DashboardFrame.hpp"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

#endif // MY_APP_HPP