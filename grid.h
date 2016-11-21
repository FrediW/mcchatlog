#ifndef __GRID__
#define __GRID__

#include <vector>

#include <wx/wx.h>
#include <wx/grid.h>

#include "data.h"

class cGrid : public wxGrid
{
public:
  cGrid(wxWindow* Parent) : wxGrid(Parent, -1) {}
  virtual void StayDown();
};

class cChatGrid : public wxPanel
{
 public:
  cChatGrid(wxWindow* Parent, cData& Data);
  void SwitchChannel(int Channel);
  void OnPaint(wxPaintEvent& Event);
  void OnSize(wxSizeEvent& Event);
  void ChangeFilter(const wxString& Filter) { m_Filter = Filter.Upper(); Refresh(); }

 private:
  int m_Channel;
  std::vector<int> m_ColumnWidth;
  cData& m_Data;
  wxString m_Filter;

};

#endif
