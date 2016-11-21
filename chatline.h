#ifndef __CHATLINE__
#define __CHATLINE__

#include <map>
#include <vector>
#include <iostream>

#include <wx/wx.h>

class cChatLine
{
public:
  cChatLine(unsigned long long Timestamp, const wxString& Text);

  wxString& GetText() { return m_Text; }
  unsigned long long GetTimestamp() { return m_Timestamp; }
  void SetVariable(int Id, int Start, int Len);
  wxString GetVariableText(int Id);
  int GetVariableStart(int Id);
  std::map<int, std::pair<int, int> >& GetVariables() { return m_Variables; }
  void Strip(int Variable);
  void AddChannelProfile(int Channel, int ChannelProfile);
  int GetChannelProfile(int Channel);
  std::map<int, int>& GetChannelProfiles() { return m_ChannelProfiles; }

private:
  wxString m_Text;
  unsigned long long m_Timestamp;
  std::map<int, std::pair<int, int> > m_Variables;
  std::vector<int> m_Tags;
  std::map<int, int> m_ChannelProfiles; // first = channel id, 2nd = profile id
};

#endif
