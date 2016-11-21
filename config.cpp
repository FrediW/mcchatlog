#include "config.h"
#include "FlexLexer.h"
#include "tools.h"
#include "data.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace config
{

  cToken::cToken(eConfigToken Id, const wxString& Text)
  {
    m_Id = Id;
    m_Text = Text;
  }

  cTagSection::cTagSection(int TagId)
  {
    m_TagId = TagId;
  }

  cRegExCondition::cRegExCondition(const wxString& RegEx)
  {
    m_RegExString = RegEx;
    m_RegEx.Compile(RegEx, wxRE_ADVANCED);
    m_CheckVariable = -1;
    if(!m_RegEx.IsValid()) {
      throw cParseException("Regular expression " + RegEx + " is not valid!");
    }
  }

  void cConfigFile::LoadConfiguration(const wxString& Filename)
  {
    GetTagIdByName("Logfile"); // Set logfile tag id to zero
    GetChannelIdByName("General"); // Set general channel to 0

    ifstream configFile(Filename);

    // Tokenize the config file
    yyFlexLexer lex(&configFile);
    while(int x = lex.yylex()) {
      config::cToken tk(static_cast<eConfigToken>(x), lex.YYText());
      m_TokenList.push_back(tk);
    }
    m_TokenListOffset = 0;

    // The file must start with a header
    if(!IsSectionHeader()) {
      throw cParseException("First line of config file must be a section header!");
    }

    while(m_TokenListOffset < m_TokenList.size()) { // TODO: Maybe we need something ... here?
      if(IsSectionHeader()) {
	int tagId = GetSectionHeaderTagId();
	m_TagSections.push_back(cTagSection(tagId));
      }
      else { // Parse command line

	vector<cParseCommand> commands;

	while(GetTokenId() != CT_EndOfLine) {
	  if(GetTokenId() != CT_Word) throw cParseException("Lines in sections must start with a keyword");

	  {
	    cParseCommand newCommand(GetNextTokenText());
	    commands.push_back(newCommand);
	  }
	  cParseCommand& currentCommand = commands[commands.size() - 1];

	  if(GetTokenId() == CT_EndOfLine) break;
	  if(GetTokenId() == CT_Space) {
	    SkipToken();
	    continue;
	  }

	  if(GetNextTokenId() != CT_OpenParen) throw new cParseException("Function/Condition names must be followed by opening paranthesis");

	  // Parameter list
	  while(GetTokenId() != CT_CloseParen) {

	    // Parameter name
	    if(GetTokenId() != CT_Word) throw new cParseException("A parameter list must start with a keyword/expression");
	    cParseParameter parameter(GetNextTokenText());

	    // Optional parameter name specification (in parentheses)
	    if(GetTokenId() == CT_OpenParen) {
	      SkipToken();
	      if(GetTokenId() != CT_Word) throw new cParseException("Parameter name specification invalid");
	      parameter.SetSpecification(GetNextTokenText());
	      if(GetNextTokenId() != CT_CloseParen) throw new cParseException("Parameter specification must be finished by closing parenthesis");
	    }

	    // Optional parameter value
	    if(GetTokenId() == CT_Colon) {
	      SkipToken();
	      if(GetTokenId() != CT_Word) throw new cParseException("Parameter value invalid");
	      parameter.SetValue(GetNextTokenText());
	      // Optional parameter value specification (in parentheses)
	      if(GetTokenId() == CT_OpenParen) {
		SkipToken();
		if(GetTokenId() != CT_Word) throw new cParseException("Parameter value specification invalid");
		parameter.SetValueSpecification(GetNextTokenText());
		if(GetNextTokenId() != CT_CloseParen) throw new cParseException("Parameter value specification must be finished by closing parenthesis");
	      }

	    }
	    
	    currentCommand.AddParameter(parameter);

	    if(GetTokenId() == CT_CloseParen) break;
	    if(GetNextTokenId() != CT_Comma) throw new cParseException("Parameter list must be finished by closing parenthesis or continued with comma");
	  }
	  SkipToken(); // close parenthesis

	  if(GetTokenId() == CT_Space) {
	    SkipToken();
	    continue;
	  }
	  else if(GetTokenId() != CT_EndOfLine) {
	    throw new cParseException("Function/Condition must be followed by whitespace unless it's the last one in the line");
	  }
	}

	SkipToken(); // end of line
	
	// Parse the commands and create the class instances
	if(commands.size()) {
	  cCommandLine commandLine;
	  
	  for(cParseCommand& command: commands) {
	    const wxString& name = command.GetName();
	    vector<cParseParameter>& pars = command.GetParameters();
	    if(name == "Regex") {
	      int offset = 0;
	      int checkvar = -1;
	      if(pars.size() == 0) throw cParseException("Regex without parameters does not work");
	      if(pars[0].GetName() == "Variable") {
		if(pars[0].GetValue() == "") throw cParseException("Variable paramter needs a variable name");
		checkvar = GetVariableIdByName(pars[0].GetValue());
		offset++;
	      }
	      if(pars.size() <= offset) throw cParseException("Regex needs at least one expression parameter");
	      cRegExCondition* re = new cRegExCondition(pars[offset].GetName());
	      if(checkvar != -1) re->SetCheckVariable(checkvar);
	      for(int p = offset + 1; p < pars.size(); p++) re->AddVariable(GetVariableIdByName(pars[p].GetName()));
	      commandLine.AddCommand(re);
	    }
	    else if(name == "Tag") {
	      cTagCommand* tc = new cTagCommand();
	      for(int p = 0; p < pars.size(); p++) tc->AddTag(GetTagIdByName(pars[p].GetName()));
	      commandLine.AddCommand(tc);
	    }
	    else if(name == "Strip") {
	      cStripCommand* sc = new cStripCommand();
	      for(int p = 0; p < pars.size(); p++) sc->AddVariable(GetVariableIdByName(pars[p].GetName()));
	      commandLine.AddCommand(sc);
	    }
	    else if(name == "Sound") {
	      if(pars.size() != 1) {
		throw cParseException("Sound command takes exactly one parameter");
	      }
	      cSoundCommand* sc = new cSoundCommand(GetSoundIdByName(pars[0].GetName()));
	      commandLine.AddCommand(sc);
	    }
	    else if(name == "UserCheck") {
	      if(pars.size() != 1) throw cParseException("UserCheck needs one parameter");
	      wxString name = pars[0].GetName();
	      cUserCheckCondition* ucc = new cUserCheckCondition(GetUserCheckConditionIdByName(name));
	      commandLine.AddCommand(ucc);
	    }
	    else if(name == "Channel") {
	      if(pars.size() >= 1) {
		cChannelProfile* profile = new cChannelProfile(GetChannelIdByName(pars[0].GetName()));
		for(cParseParameter& par: pars) {
		  if(par.GetName() == "Colour") {
		    if(par.GetSpecification() == "") {
		      profile->SetDefaultColour(par.GetValue());
		    }
		    else {
		      profile->AddVariableColour(GetVariableIdByName(par.GetSpecification()), par.GetValue());
		    }
		  }
		}
		int profileId = GetChannelProfileId(profile->ToString());
		if(!m_Data->ChannelProfileExists(profileId)) {
		  m_Data->AddChannelProfile(profile);
		}
		else {
		  delete profile;
		}
		cChannelCommand* cc = new cChannelCommand(profileId);
		commandLine.AddCommand(cc);
	      }
	      else {
		throw cParseException("Channel needs at least one parameter");
	      }
	    }
	  }

	  AddCommandLineToLastTagSection(commandLine);
	}

      }
    }
  }

  bool cConfigFile::IsSectionHeader() const
  {
    if(GetTokenId() != CT_OpenBrack) return false;
    if(GetTokenId(1) != CT_Word) return false;
    if(GetTokenId(2) != CT_CloseBrack) return false;
    if(GetTokenId(3) != CT_EndOfLine) return false;
    return true;
  }

  int cConfigFile::GetSectionHeaderTagId()
  {
    // TODO: Call IsSectionHeader first? Or trust the programmer that he/she did?
    m_TokenListOffset++;
    int ret = GetTagIdByName(m_TokenList[m_TokenListOffset].GetText());
    m_TokenListOffset += 2;
    if(m_TokenListOffset < m_TokenList.size()) m_TokenListOffset++;
    return ret;
  }

  int cConfigFile::GetIdByName(const wxString& Name, std::vector<wxString>& List)
  {
    for(int p = 0; p < List.size(); p++) {
      if(List[p] == Name) return p;
    }
    List.push_back(Name);
    return List.size() - 1;
  }

  int cConfigFile::GetTagIdByName(const wxString& Tag)
  {
    return GetIdByName(Tag, m_TagList);
  }

  int cConfigFile::GetChannelIdByName(const wxString& Channel)
  {
    return GetIdByName(Channel, m_ChannelList);
  }

  int cConfigFile::GetVariableIdByName(const wxString& Variable)
  {
    return GetIdByName(Variable, m_VariableList);
  }

  int cConfigFile::GetChannelProfileId(const wxString& ChannelProfileDescr)
  {
    return GetIdByName(ChannelProfileDescr, m_ChannelProfileList);
  }

  int cConfigFile::GetSoundIdByName(const wxString& Sound)
  {
    int id = GetIdByName(Sound, m_SoundList);
    if(id >= m_Sounds.size()) {
      m_Sounds.push_back(new wxSound(Sound + ".wav"));
    }
    return id;
  }

  int cConfigFile::GetUserCheckConditionIdByName(const wxString& Text)
  {
    int id = GetIdByName(Text, m_UserCheckConditionList);
    if(id >= m_UserCheckConditionStatus.size()) {
      m_UserCheckConditionStatus.push_back(false);
    }
    return id;
  }

  int cConfigFile::GetTokenId(int Offset) const
  {
    if(m_TokenListOffset + Offset >= m_TokenList.size()) {
      return CT_EndOfFile;
    }
    else {
      return m_TokenList[m_TokenListOffset + Offset].GetId();
    }
  }

  wxString cConfigFile::GetTokenText(int Offset) const
  {
    if(m_TokenListOffset + Offset >= m_TokenList.size()) {
      return "";
    }
    else {
      return m_TokenList[m_TokenListOffset + Offset].GetText();
    }
  }

  wxString cConfigFile::GetNextTokenText()
  {
    wxString ret = GetTokenText();
    SkipToken();
    return ret;
  }

  int cConfigFile::GetNextTokenId()
  {
    if(m_TokenListOffset < m_TokenList.size()) {
      m_TokenListOffset++;
      return m_TokenList[m_TokenListOffset - 1].GetId();
    }
    else {
      return CT_EndOfFile;
    }
  }

  void cConfigFile::SkipToken()
  {
    // could be optimized
    GetNextTokenId();
  }

  const cTagSection& cConfigFile::GetTagSection(int TagId) const
  {
    for(const cTagSection& tagSection: m_TagSections) {
      if(tagSection.GetId() == TagId) {
	return tagSection;
      }
    }
    throw cParseException("Cant find tag id (this should never happen)");
  }

  bool cConfigFile::TagSectionExists(int TagId) const
  {
    for(const cTagSection& tagSection: m_TagSections) {
      if(tagSection.GetId() == TagId) {
	return true;
      }
    }
    return false;
  }

  cChatLine* cConfigFile::Parse(const wxString& Text) const
  {
    cChatLine* line = new cChatLine(GetNow(), Text);
    ProcessTagSection(*line, 0);
    return line;
  }

  void cConfigFile::ProcessTagSection(cChatLine& Line, int TagId) const
  {
    if(!TagSectionExists(TagId)) {
      return;
    }

    // TODO: Check if this tag has already been processed

    const cTagSection& section = GetTagSection(TagId);
    const vector<cCommandLine>& commandLines = section.GetCommandLines();

    for(int lineno = 0; lineno < commandLines.size(); lineno++) {
      const cCommandLine& commandLine = commandLines[lineno];

      for(cCommand* cmd: commandLine.GetCommands()) {

	{
	  cRegExCondition* re = dynamic_cast<cRegExCondition*>(cmd);
	  if(re) {
	    // TODO: This should maybe be moved to the chatline class
	    wxRegEx& r = re->GetRegEx();
	    wxString tck;
	    int offset = 0;
	    if(re->GetCheckVariable() == -1) {
	      tck = Line.GetText();
	      offset = 0;
	    }
	    else {
	      tck = Line.GetVariableText(re->GetCheckVariable());
	      offset = Line.GetVariableStart(re->GetCheckVariable());
	    }
	    if(r.Matches(tck)) {
	      vector<int> variableIds = re->GetVariables();
	      for(int vno = 0; vno < variableIds.size(); vno++) {
		size_t start, len;
		if(r.GetMatch(&start, &len, vno + 1)) {
		  Line.SetVariable(variableIds[vno], start + offset, len);
		}
	      }
	      continue;
	    }
	    else {
	      break; // No match -> Go on with next line
	    }
	  }
	}

	{
	  cUserCheckCondition* ucc = dynamic_cast<cUserCheckCondition*>(cmd);
	  if(ucc) {
	    if(!m_UserCheckConditionStatus[ucc->GetId()]) {
	      break;
	    }
	  }
	}

	{
	  cTagCommand* tc = dynamic_cast<cTagCommand*>(cmd);
	  if(tc) {
	    for(int tag: tc->GetTagList()) {
	      ProcessTagSection(Line, tag);
	    }
	    continue;
	  }
	}

	{
	  cStripCommand* sc = dynamic_cast<cStripCommand*>(cmd);
	  if(sc) {
	    Line.Strip(sc->GetVariable()); // TODO: Permit multiple variables
	    continue;
	  }
	}

	{
	  cChannelCommand* cc = dynamic_cast<cChannelCommand*>(cmd);
	  if(cc) {
	    int profile = cc->GetChannelProfile();
	    int channel = m_Data->GetChannelProfile(profile)->GetChannel();
	    Line.AddChannelProfile(channel, profile);
	    continue;
	  }
	}

	{
	  cSoundCommand* sc = dynamic_cast<cSoundCommand*>(cmd);
	  if(sc) {
	    int soundId = sc->GetSoundId();
	    PlaySound(soundId);
	    continue;
	  }
	}

      }
    }

  }

  void cConfigFile::PlaySound(int SoundId) const
  {
    if(SoundId < m_Sounds.size()) {
      m_Sounds[SoundId]->Play();
    }
  }

}
