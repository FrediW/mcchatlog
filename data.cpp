#include "data.h"
#include "tools.h"
#include "grid.h"

using namespace std;

cChannelProfile::cChannelProfile(int Channel)
{
  m_Channel = Channel;
  m_DefaultColour = wxColour(150, 150, 150);
  m_BackgroundColour = *wxBLACK;
}

wxString cChannelProfile::ToString()
{
  wxString ret;
  ret = itos(m_Channel) + ";";
  ret += m_DefaultColour.GetAsString(wxC2S_CSS_SYNTAX) + ";";
  ret += m_BackgroundColour.GetAsString(wxC2S_CSS_SYNTAX) + ";";
  for(std::pair<int, wxColour> vc: m_VariableColours) {
    ret += itos(vc.first) + ":" + vc.second.GetAsString(wxC2S_CSS_SYNTAX) + ";";
  }
  return ret;
}

cData::cData()
{
}

void cData::AddChatLine(cChatLine* ChatLine)
{
  m_ChatLines.push_back(ChatLine);
  int index = m_ChatLines.size() - 1;
  const map<int, int>& channelProfiles = ChatLine->GetChannelProfiles();
  for(pair<int, int> c: channelProfiles) {
    int channel = c.first;
    while(m_ChannelIndex.size() < channel + 1) m_ChannelIndex.push_back(vector<int>());
    m_ChannelIndex[channel].push_back(index);
  }
}

int cData::GetNumberOfLines(int Channel)
{
  if(Channel < m_ChannelIndex.size()) {
    return m_ChannelIndex[Channel].size();
  }
  else {
    return 0;
  }
}

wxString cData::GetText(int Row, int Col, int Channel)
{
  if(Col == 0) {
    return "Time";
  }
  else {
    return m_ChatLines[m_ChannelIndex[Channel][Row]]->GetText();
  }
}

cChatLine* cData::GetLine(int Row, int Channel)
{
  return m_ChatLines[m_ChannelIndex[Channel][Row]];
}
