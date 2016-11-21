#include <vector>
#include <map>
#include <iostream>

#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/sound.h>

#include "configtokens.h"
#include "chatline.h"
#include "data.h"

namespace config
{
  
  class cParseException
  {
  public:
  cParseException(const wxString& Text)
    : m_Text(Text) { std::cout << Text << std::endl;}

  private:
    wxString m_Text;

  };


  class cToken
  {
  public:
    cToken(eConfigToken Id, const wxString& Text);

    const wxString& GetText() const { return m_Text; }
    int GetId() const { return m_Id; }

  private:
    eConfigToken m_Id;
    wxString m_Text;
  };


  class cParseParameter
  {
  public:
    cParseParameter(const wxString& Name) { m_Name = Name; }
    void SetSpecification(const wxString& Specification) { m_Specification = Specification; }
    void SetValue(const wxString& Value) { m_Value = Value; }
    void SetValueSpecification(const wxString& Specification) { m_ValueSpecification = Specification; }
    wxString GetName() { return m_Name; }
    wxString GetSpecification() { return m_Specification; }
    wxString GetValue() { return m_Value; }
    wxString GetValueSpecification() { return m_ValueSpecification; }

  private:
    wxString m_Name;
    wxString m_Specification;
    wxString m_Value;
    wxString m_ValueSpecification;
  };

  class cParseCommand
  {
  public:
    cParseCommand(const wxString& Name) { m_Name = Name; }

    void AddParameter(const cParseParameter& Parameter) { m_Parameters.push_back(Parameter); }
    
    wxString GetName() { return m_Name; }
    std::vector<cParseParameter>& GetParameters() { return m_Parameters; }

  private:
    wxString m_Name;
    std::vector<cParseParameter> m_Parameters;
  };

  class cCommand
  {
  public:
    virtual int GetType() { return 0; }
  };


  class cRegExCondition : public cCommand
  {
  public:
    cRegExCondition(const wxString& RegEx);
    void AddVariable(int VariableId) { m_Variables.push_back(VariableId); }
    void SetCheckVariable(int VariableId) { m_CheckVariable = VariableId; }
    const std::vector<int>& GetVariables() const { return m_Variables; }
    wxRegEx& GetRegEx() { return m_RegEx; }
    wxString& GetRegExString() { return m_RegExString; }
    int GetCheckVariable() { return m_CheckVariable; }

  private:
    wxRegEx m_RegEx;
    wxString m_RegExString;
    int m_CheckVariable; // -1: Complete texta
    std::vector<int> m_Variables;

  };

  class cUserCheckCondition : public cCommand
  {
  public:
    cUserCheckCondition(int Id) { m_Id = Id; }
    int GetId() { return m_Id; }

  private:
    int m_Id;

  };

  class cTagCommand : public cCommand
  {
  public:
    cTagCommand() {}
    void AddTag(int TagId) { m_TagList.push_back(TagId); }
    const std::vector<int> GetTagList() const { return m_TagList; }
    
  private:
    std::vector<int> m_TagList;

  };

  class cStripCommand : public cCommand
  {
  public:
    cStripCommand() {}
    void AddVariable(int VariableId) { m_VariableList.push_back(VariableId); }
    int GetVariable() { return m_VariableList[0]; }
    
  private:
    std::vector<int> m_VariableList;

  };

  class cChannelCommand : public cCommand
  {
  public:
    cChannelCommand(int ChannelProfile) { m_ChannelProfile = ChannelProfile; }
    int GetChannelProfile() const { return m_ChannelProfile; }

  private:
    int m_ChannelProfile;

  };

  class cSoundCommand : public cCommand
  {
  public:
    cSoundCommand(int SoundId) { m_SoundId = SoundId; }
    int GetSoundId() const { return m_SoundId; }

  private:
    int m_SoundId;
  };

  class cCommandLine
  {
  public:
    cCommandLine() {}
    void AddCommand(cCommand* Command) { m_Commands.push_back(Command); }

    const std::vector<cCommand*> GetCommands() const { return m_Commands; }

  private:
    std::vector<cCommand*> m_Commands;

  };


  class cTagSection
  {
  public:
    cTagSection(int TagId);

    void AddCommandLine(cCommandLine CommandLine) { m_CommandLines.push_back(CommandLine); }
    
    int GetId() const { return m_TagId; }
    const std::vector<cCommandLine>& GetCommandLines() const { return m_CommandLines; }

  private:
    std::vector<cCommandLine> m_CommandLines;
    int m_TagId;

  };


  class cConfigFile
  {
  public:
    cConfigFile() {}
    void LoadConfiguration(const wxString& Filename);

    cChatLine* Parse(const wxString& Text) const;

    const std::vector<wxString>& GetChannelList() const { return m_ChannelList; }
    const std::vector<wxString>& GetUserCheckConditionList() const { return m_UserCheckConditionList; }
    void SetUserCheckStatus(int Id, bool Value) { m_UserCheckConditionStatus[Id] = Value; }
    void SetData(cData* Data) { m_Data = Data; }

  protected:
    bool IsSectionHeader() const;
    int GetSectionHeaderTagId();

    int GetTagIdByName(const wxString& Tag);
    int GetChannelIdByName(const wxString& Channel);
    int GetVariableIdByName(const wxString& Variable);
    int GetSoundIdByName(const wxString& Sound);
    int GetUserCheckConditionIdByName(const wxString& Text);

    int GetIdByName(const wxString& Name, std::vector<wxString>& Map);
    int GetChannelProfileId(const wxString& ChannelProfile);

    int GetTokenId(int Offset = 0) const;
    wxString GetTokenText(int Offset = 0) const;
    wxString GetNextTokenText();
    int GetNextTokenId();
    void SkipToken();

    const cTagSection& GetTagSection(int TagId) const;
    bool TagSectionExists(int TagId) const;
    void ProcessTagSection(cChatLine& Line, int TagId) const;

    void AddCommandLineToLastTagSection(const cCommandLine& CommandLine) { m_TagSections[m_TagSections.size() - 1].AddCommandLine(CommandLine); }

    void PlaySound(int SoundId) const;

    std::vector<cTagSection> m_TagSections;

    std::vector<wxString> m_TagList;
    std::vector<wxString> m_VariableList;
    std::vector<wxString> m_ChannelList;
    std::vector<wxString> m_ChannelProfileList;
    std::vector<wxString> m_SoundList;
    std::vector<wxString> m_UserCheckConditionList;
    std::vector<bool> m_UserCheckConditionStatus;

    std::vector<cToken> m_TokenList;
    int m_TokenListOffset;

    std::vector<wxSound*> m_Sounds;
    cData* m_Data;
  };

}
