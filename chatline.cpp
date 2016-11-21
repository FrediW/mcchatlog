#include "chatline.h"

using namespace std;

void cChatLine::SetVariable(int Id, int Start, int Len)
{
  m_Variables[Id] = pair<int, int>(Start, Len);
}

void cChatLine::Strip(int Variable)
{
  if(m_Variables.count(Variable)) { // If the variable does not exist, just don't strip
    pair<int, int> location = m_Variables[Variable];
    m_Text = m_Text.Mid(location.first, location.second);
    m_Variables.clear();
  }
}

int cChatLine::GetChannelProfile(int Channel)
{
  return m_ChannelProfiles[Channel];
}

void cChatLine::AddChannelProfile(int Channel, int ChannelProfile)
{
  m_ChannelProfiles[Channel] = ChannelProfile;
}

wxString cChatLine::GetVariableText(int Id)
{
  const pair<int, int>& var = m_Variables[Id];
  return m_Text.Mid(var.first, var.second);
}

int cChatLine::GetVariableStart(int Id)
{
  return m_Variables[Id].first;
}
