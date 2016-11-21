#include "grid.h"

#include <iostream>

#include <wx/dcbuffer.h>
#include <wx/tokenzr.h>

using namespace std;

void cGrid::StayDown()
{
  int rownum = GetTable()->GetNumberRows();
  if(rownum >= 2) {
    if(IsRowShown(rownum - 2)) {
      MakeCellVisible(rownum - 1, 0);
    }
  }
}

cChatGrid::cChatGrid(wxWindow* Parent, cData& Data)
  : wxPanel(Parent), m_Data(Data)
{
  SetBackgroundStyle(wxBG_STYLE_PAINT);

  Connect(wxEVT_PAINT, wxPaintEventHandler(cChatGrid::OnPaint));
  Connect(wxEVT_SIZE, wxSizeEventHandler(cChatGrid::OnSize));
}

void cChatGrid::SwitchChannel(int Channel)
{
  m_Channel = Channel;
  Refresh();
}

void cChatGrid::OnSize(wxSizeEvent& Event)
{
  Refresh();
}

void cChatGrid::OnPaint(wxPaintEvent& Event)
{
  wxPaintDC pdc(this);
  wxBitmap bmp(pdc.GetSize().GetWidth(), pdc.GetSize().GetHeight());
  wxMemoryDC dc;
  dc.SelectObject(bmp);

  dc.SetBackground(*wxBLACK_BRUSH);
  dc.Clear();

  wxFont font(18, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
  dc.SetFont(font);
  int fontheight = dc.GetTextExtent("y").GetHeight();
  int fontwidth = dc.GetTextExtent("0").GetWidth();
  int height = dc.GetSize().GetHeight();
  int numLines = height / fontheight;
  int drawnLines = 0;

  int l = 0; // Index in array

  wxArrayString filterStrings;
  wxStringTokenizer tk(m_Filter, ",");
  while(tk.HasMoreTokens()) {
    filterStrings.Add(tk.GetNextToken());
    cout << "Add filter string: " << filterStrings[filterStrings.GetCount() - 1];
  }
  
  while(drawnLines < numLines) {
    int ln = m_Data.GetNumberOfLines(m_Channel) - 1 - l;
    l++;

    if(ln < 0) break;
    cChatLine* cl = m_Data.GetLine(ln, m_Channel);

    if(m_Filter != "") {
      bool ff = false;
      wxString txt = cl->GetText().Upper();
      for(int s = 0; s < filterStrings.GetCount(); s++) {
	if(txt.Find(filterStrings[s]) != wxNOT_FOUND) {
	  ff = true;
	  break;
	}
      }
      if(ff == false) continue;
    }

    cChannelProfile* profile = m_Data.GetChannelProfile(cl->GetChannelProfile(m_Channel));

    int y = height - (drawnLines + 1) * fontheight;
    drawnLines++;

    // Draw timestamp
    // TODO: Check channel profile for other type of time display (yet there isn't one :) )
    int timediff = (GetNow() - cl->GetTimestamp()) / 1000;
    wxString timetxt;
    wxColour timecol = wxColour(150, 150, 150);
    if(timediff < 120) {
      timetxt = itos(timediff / 60) + ":" + itospad(timediff % 60, 2);
      if(timediff < 60) {
	timecol = wxColour(100, 255, 100);
      }
      else {
	timecol = wxColour(255, 255, 150);
      }
    }
    else if(timediff < 3600) {
      timetxt = itos(timediff / 60) + "min";
      if(timediff < 300) {
	timecol = wxColour(255, 150, 150);
      }
    }
    else {
      timetxt = itos(timediff / 3600) + "h";
    }
    dc.SetTextForeground(timecol);
    dc.DrawText("[" + timetxt + "]", 0, y);

    // Draw the text
    int x = fontwidth * 8;
    wxString& text = cl->GetText();
    map<int, std::pair<int, int> >& variables = cl->GetVariables();

    // First determine the colours
    // There's probably a better way to do this, but I haven't found it yet: :)
    vector<wxColour> col;
    for(int i = 0; i < text.Length(); i++) {
      col.push_back(profile->GetDefaultColour());
    }
    for(auto it = variables.begin(); it != variables.end(); ++it) {
      if(profile->HasVariableColour(it->first)) {
	wxColour c = profile->GetVariableColour(it->first);
	for(int i = it->second.first; i < it->second.first + it->second.second; ++i) {
	  col[i] = c;
	}
      }
    }

    // Then draw the actual text
    int pos = 0;
    int overflow = 0;
    while(pos < text.Length() && overflow < 1000) {
      int epos = pos;
      if(pos < text.Length() - 1) {
	for(epos = pos + 1; epos < text.Length(); ++epos) {
	  if(col[epos] != col[pos]) {
	    break;
	  }
	}
	--epos;
      }
      dc.SetTextForeground(col[pos]);
      wxString part = text.Mid(pos, epos - pos + 1);
      dc.DrawText(part, x, y);
      wxSize ts = dc.GetTextExtent(part);
      x += ts.GetWidth();
      pos = epos + 1;
      overflow++;
    }

  }

  pdc.Blit(0, 0, pdc.GetSize().GetWidth(), pdc.GetSize().GetHeight(), &dc, 0, 0);

}
