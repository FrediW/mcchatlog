// TODO: A regex with variable, but without matching parantheses in regex, will crash
#include <wx/wx.h>
#include <iostream>
#include <wx/sound.h>
#include <sys/stat.h>
#include <vector>
#include <wx/regex.h>
#include <wx/dcbuffer.h>
#include <wx/clipbrd.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/stdpaths.h>
#include <wx/wrapsizer.h>

#include "config.h"
#include "data.h"
#include "tools.h"
#include "grid.h"

using namespace std;

cChatLine::cChatLine(unsigned long long Timestamp, const wxString& Text)
{
  m_Text = Text;
  m_Timestamp = Timestamp;
}

class cMainFrame : public wxFrame
{
public:
  cMainFrame();

  void OnHotkey(wxKeyEvent& Event);
  void OnTimer(wxTimerEvent& Event);

  void OnTestButton(wxCommandEvent& Event);
  void OnChannelButton(wxCommandEvent& Event);
  void OnClearFilterButton(wxCommandEvent& Event);
  void OnFilterChange(wxCommandEvent& Event);
  void OnUserCheckbox(wxCommandEvent& Event);

private:
  int GetLogfileSize();
  void ProcessDataBuffer();

  cChatGrid* m_Grid;
  vector<wxButton*> m_ChannelButtons;
  wxTextCtrl* m_Filter;
  vector<wxCheckBox*> m_UserCheckboxes;

  wxSound m_Sound;

  wxTimer m_Timer;

  wxString m_Logfile;
  int m_LogfileSize;
  wxString m_DataBuffer;

  vector<cChatLine> m_Log;

  unsigned long long m_LastUpdate;

  wxString m_NewPlayer;

  cData m_Data;
  
  config::cConfigFile m_ConfigFile;
};

void cMainFrame::OnChannelButton(wxCommandEvent& Event)
{
  int c = Event.GetId() - 10000;
  m_Grid->SwitchChannel(c);
  for(int x = 0; x < m_ChannelButtons.size(); x++) {
    if(x == c) {
      m_ChannelButtons[x]->SetBackgroundColour(*wxGREEN);
    }
    else {
      m_ChannelButtons[x]->SetBackgroundColour(*wxWHITE);
    }
  }
}

void cMainFrame::OnClearFilterButton(wxCommandEvent& Event)
{
  m_Filter->SetValue("");
}

void cMainFrame::OnFilterChange(wxCommandEvent& Event)
{
  m_Grid->ChangeFilter(m_Filter->GetValue());
}

cMainFrame::cMainFrame()
  : wxFrame(NULL, 0, "MCChatlog", wxDefaultPosition, wxSize(800, 500))
{
  wxFont font(15, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wxClientDC dc(this);
  dc.SetFont(font);
  int fontwidth = dc.GetTextExtent("0").GetWidth();

  m_ConfigFile.SetData(&m_Data);

  SetBackgroundColour(*wxBLACK);

  m_ConfigFile.LoadConfiguration("mcchat.config");

  m_Logfile = wxStandardPaths::Get().GetUserConfigDir() + "\\.minecraft-cv\\.minecraft\\logs\\latest.log";
  m_LogfileSize = 0; //GetLogfileSize();

  Show(true);
  RegisterHotKey(1000, wxACCEL_CTRL, 'L');
  Connect(wxEVT_HOTKEY, wxKeyEventHandler(cMainFrame::OnHotkey));

  m_LastUpdate = 0;

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  wxWrapSizer* menuSizer = new wxWrapSizer(wxHORIZONTAL);

  const vector<wxString>& channels = m_ConfigFile.GetChannelList();
  int cnr = 0;
  for(wxString ch: channels) {
    wxButton* b = new wxButton(this, 10000 + cnr, "&" + ch, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    if(cnr == 0) {
      b->SetBackgroundColour(*wxGREEN);
    }
    else {
      b->SetBackgroundColour(*wxWHITE);
    }
    b->SetFont(font);
    menuSizer->Add(b, 0, wxBOTTOM, 1);
    Connect(10000 + cnr, wxEVT_BUTTON, wxCommandEventHandler(cMainFrame::OnChannelButton));
    m_ChannelButtons.push_back(b);
    cnr++;
    menuSizer->AddSpacer(1);
  }

  menuSizer->AddSpacer(20);

  wxPanel* filterPanel = new wxPanel(this, wxID_ANY);
  filterPanel->SetBackgroundColour(*wxWHITE);
  wxBoxSizer* filterSizer = new wxBoxSizer(wxHORIZONTAL);
  m_Filter = new wxTextCtrl(filterPanel, wxID_ANY, "", wxDefaultPosition, wxSize(300, wxDefaultCoord), wxBORDER_NONE);
  m_Filter->SetFont(font);
  Connect(m_Filter->GetId(), wxEVT_TEXT, wxCommandEventHandler(cMainFrame::OnFilterChange));
  filterSizer->Add(m_Filter, 1, wxALIGN_CENTER_VERTICAL);
  filterPanel->SetSizerAndFit(filterSizer);
  menuSizer->Add(filterPanel, 0, wxEXPAND);
  wxButton* filterClearButton = new wxButton(this, wxID_ANY, "X", wxDefaultPosition, wxSize(fontwidth * 2, wxDefaultCoord), wxBORDER_NONE);
  Connect(filterClearButton->GetId(), wxEVT_BUTTON, wxCommandEventHandler(cMainFrame::OnClearFilterButton));
  filterClearButton->SetBackgroundColour(*wxWHITE);
  menuSizer->Add(filterClearButton, 0, wxEXPAND);

  for(wxString s: m_ConfigFile.GetUserCheckConditionList()) {
    wxCheckBox* cb = new wxCheckBox(this, wxID_ANY, s);
    cb->SetForegroundColour(*wxWHITE);
    menuSizer->Add(cb, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    m_UserCheckboxes.push_back(cb);
    Connect(cb->GetId(), wxEVT_CHECKBOX, wxCommandEventHandler(cMainFrame::OnUserCheckbox));
  }

  sizer->Add(menuSizer);

  m_Grid = new cChatGrid(this, m_Data);
  sizer->Add(m_Grid, 1, wxEXPAND);

  //wxButton* testbutton = new wxButton(this, wxID_ANY, "Test");
  //Connect(testbutton->GetId(), wxEVT_BUTTON, wxCommandEventHandler(cMainFrame::OnTestButton));
  //sizer->Add(
  SetSizerAndFit(sizer);

  Connect(wxEVT_TIMER, wxTimerEventHandler(cMainFrame::OnTimer));
  m_Timer.SetOwner(this);
  m_Timer.Start(50);
}

void cMainFrame::OnHotkey(wxKeyEvent& Event)
{
  // Raise();
  if(wxTheClipboard->Open()) {
    wxTheClipboard->SetData(new wxTextDataObject("/sh Welcome to Cubeville, " + m_NewPlayer + " :)"));
    wxTheClipboard->Close();
  }

  m_Sound.Play();
}

int cMainFrame::GetLogfileSize()
{
  FILE* file = fopen(m_Logfile.char_str(), "rb");
  fseek(file, 0, SEEK_END);
  int s = ftell(file);
  fclose(file);
  return s;
}

void cMainFrame::OnTestButton(wxCommandEvent& Event)
{

}

void cMainFrame::OnTimer(wxTimerEvent& Event)
{
  FILE* file = fopen(m_Logfile.char_str(), "rb");
  fseek(file, 0, SEEK_END);
  int s = ftell(file);
  if(s > m_LogfileSize) {
    int size = s - m_LogfileSize;
    fseek(file, m_LogfileSize, SEEK_SET);
    char* buffer = new char[size];
    fread(static_cast<void*>(buffer), size, 1, file);
    m_DataBuffer += wxString(buffer, size);
    delete[] buffer;
    ProcessDataBuffer();
  }
  // TODO: Kleiner -> Minecraft neu gestartet wahrscheinlich?!?
  m_LogfileSize = s;
  fclose(file);

  unsigned long long now = GetNow();
  if(now > m_LastUpdate + 2000) {
    Refresh();
    m_LastUpdate = now;
  }
}

void cMainFrame::ProcessDataBuffer()
{
  while(m_DataBuffer.Find('\r') != wxNOT_FOUND) {
    wxString nl = m_DataBuffer.BeforeFirst('\r');
    m_DataBuffer = m_DataBuffer.AfterFirst('\r');
    nl.Replace("\n", "");
    cChatLine* line = m_ConfigFile.Parse(nl);
    m_Data.AddChatLine(line);
  }
}

void cMainFrame::OnUserCheckbox(wxCommandEvent& Event)
{
  for(int i = 0; i < m_UserCheckboxes.size(); i++) {
    m_ConfigFile.SetUserCheckStatus(i, m_UserCheckboxes[i]->IsChecked());
  }
}

class cApp : public wxApp
{
public:
  virtual bool OnInit();
};

bool cApp::OnInit()
{
  cMainFrame* fr = new cMainFrame();
  SetTopWindow(fr);
  return true;
}

IMPLEMENT_APP(cApp)
