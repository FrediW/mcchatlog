#ifndef __TOOLS__
#define __TOOLS__

#include <wx/wx.h>

unsigned long long GetNow();
wxString itos(long int Value, int Dec = 0, bool Space = false);
wxString itospad(long int Value, int Length);
wxColour GetColourFromString(const wxString& Colour);

#endif
