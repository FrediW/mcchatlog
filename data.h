#ifndef __DATA__
#define __DATA__

#include <vector>
#include <iostream>

#include <wx/grid.h>

#include "chatline.h"
#include "tools.h"

#include <iostream>

class cChatTable;

class cChannelProfile
{
 public:
  cChannelProfile(int Channel);

  int GetChannel() { return m_Channel; }

  wxString ToString();

  void SetDefaultColour(const wxColour& Colour) { m_DefaultColour = Colour; }
  void SetDefaultColour(const wxString& Colour) { m_DefaultColour = GetColourFromString(Colour); }
  wxColour& GetDefaultColour() { return m_DefaultColour; }

  void AddVariableColour(int Variable, const wxColour& Colour) { m_VariableColours[Variable] = Colour; }
  void AddVariableColour(int Variable, const wxString& Colour) { AddVariableColour(Variable, GetColourFromString(Colour)); }
  bool HasVariableColour(int Variable) { return m_VariableColours.count(Variable); }
  wxColour GetVariableColour(int Variable) { return m_VariableColours[Variable]; }

 private:
  int m_Channel;
  wxColour m_DefaultColour;
  wxColour m_BackgroundColour;
  std::map<int, wxColour> m_VariableColours;

};

class cData
{
 public:
  cData();

  void AddChatLine(cChatLine* ChatLine);
  int GetNumberOfLines(int Channel);
  cChatLine* GetLine(int Row, int Channel);
  wxString GetText(int Row, int Col, int Channel);
  void AddChatView(cChatTable* ChatView) { m_ChatView.push_back(ChatView); }
  void AddChannelProfile(cChannelProfile* ChannelProfile) { m_ChannelProfiles.push_back(ChannelProfile); }
  bool ChannelProfileExists(int ChannelProfile) { return ChannelProfile < m_ChannelProfiles.size(); }
  cChannelProfile* GetChannelProfile(int ProfileId) { return m_ChannelProfiles[ProfileId]; }

 private:
  std::vector<cChatLine*> m_ChatLines;
  std::vector<cChatTable*> m_ChatView;
  std::vector<std::vector<int> > m_ChannelIndex;
  std::vector<cChannelProfile*> m_ChannelProfiles;

};

#endif
