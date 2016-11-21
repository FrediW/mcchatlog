#include "tools.h"

unsigned long long GetNow()
{
  unsigned long long ret;
  wxDateTime dt = wxDateTime::UNow();
  ret = dt.GetTicks();
  ret *= 1000;
  ret += dt.GetMillisecond();
  return ret;
}

wxString itos(long int Value, int Dec, bool Space)
{
  wxString ret;

  bool neg = false;
  if(Value < 0) {
    neg = true;
    Value *= -1;
  }
  do {
    ret = (char)((Value % 10) + '0') + ret;
    Value = (Value - Value % 10) / 10;
  } while(Value);
  while(ret.Length() < (unsigned int)Dec + 1)
    ret = "0" + ret;
  if(Dec)
    ret = ret.Left(ret.Length() - Dec) + "." + ret.Right(Dec);
  if(neg) {
    if(Space)
      ret = " " + ret;
    ret = "-" + ret;
  }
  return ret;
}

wxString itospad(long int Value, int Length)
{
  wxString ret = itos(Value, 0, false);
  while(ret.Length() < Length) {
    ret = '0' + ret;
  }
  return ret;
}

wxColour GetColourFromString(const wxString& Colour)
{
  wxColour c = Colour.Upper();
  if(c == "GREEN") {
    return wxColour(0, 255, 0);
  }
  else if(c == "LIGHTGREEN") {
    return wxColour(100, 255, 100);
  }
  else if(c == "RED") {
    return wxColour(255, 0, 0);
  }
  else if(c == "BLUE") {
    return wxColour(0, 0, 255);
  }
  else if(c == "YELLOW") {
    return wxColour(255, 255, 0);
  }
  else if(c == "PURPLE") {
    return wxColour(255, 0, 255);
  }
  else if(c == "CYAN") {
    return wxColour(0, 255, 255);
  }
  else if(c == "GREY") {
    return wxColour(190, 190, 190);
  }
  else if(c == "LIGHTGREY") {
    return wxColour(210, 210, 210);
  }
  else if(c == "WHITE") {
    return wxColour(230, 230, 230);
  }
  else if(c == "ORANGE") {
    return wxColour(255, 180, 0);
  }
  else if(c == "MAGENTA") {
    return wxColour(255, 30, 150);
  }
  return wxColour(150, 150, 150);
}
