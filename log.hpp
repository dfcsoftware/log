/*--------------------------------------------------
 * File: log.hpp
 *
 * Usage: Initialize class -
 *         Filer(LogLevel) for files, 
 *         Syslog(LogLevel) for syslog
 *         default LogLevel is INFO
 *
 *      - Possible levels:
 *         TRACE,  <- lowest
 *         INFO,
 *         WARN,
 *         CRITICAL <- higest
 *
 *      - Output level marked less than current
 *         will not be shown
 *
 *      - Redirect output with bit-shift operator
 *
 *      - Destructor actually does IO, 
 *         or you can force by calling Write()
 *
 * Example (see main for more):
 *  #include <log>
 *  Syslog LogSyslog(LogLevel::CRITICAL);
 *  Filer  LogFiler (LogLevel::WARN);
 *  Log &log = LogFiler;    // <= Log to File
 *  //Log &log = LogSyslog; // <= Log to Syslog
 *
 *  log.Writer(LogLevel::INFO) << "This is my info" << std::endl;
 *  log.Writer(LogLevel::WARN) << "Variable x=" << x << std::endl;
 *  log.Writer(LogLevel::CRITICAL) << "Wrapping up" << std::endl;
 *
 * Output to file:
 *  2020-11-09 10:58:34 WARN: Variable x=42
 *  2020-11-09 10:58:34 CRITICAL: Wrapping up
 *
 * Dependencies: 
 *  * std:: See #include below
 *
 * Author: Don Cohoon - November 2020
 *--------------------------------------------------
 */
#include <iostream> // io stream
#include <sstream>  // string stream
#include <fstream>  // file stream
#include <string>   // string
#include <syslog.h> // syslog
#include <ctime>    // time structs
#include <cstring>  // strcpy
#include <iomanip>  // put_time

enum class LogLevel
 {
   TRACE,
   INFO,
   WARN,
   CRITICAL
 }; // LogLevel
std::ostream& operator << (std::ostream& os, const LogLevel& obj)
{
   switch(obj) {
     case LogLevel::TRACE  :
         os << "TRACE";
         break; 
     case LogLevel::INFO  :
         os << "INFO";
         break; 
     case LogLevel::WARN  :
         os << "WARN";
         break; 
     case LogLevel::CRITICAL  :
         os << "CRITICAL";
         break; 
      default : //Optional
         os << "UNKNOWN";
   }
   return os;
} // operator LogLevel

const int syslogname_size = 12;
static char syslogname[syslogname_size];
//---------------------------------------------------
class Log
{
public:
  Log() { _level = LogLevel::INFO;  }
  Log(LogLevel level) { _level = level;  }
  ~Log() { Write(); }
  //
  virtual const char* ClassName() { return "Log"; }
  virtual void Write() { }
  virtual std::ostringstream& Writer( LogLevel l) = 0;
  virtual LogLevel GetLogLevel() = 0;
  virtual LogLevel SetLogLevel( LogLevel l ) = 0;
protected:
  std::ostringstream os;
  std::ostringstream nope;
private:
  LogLevel _level;
  int  _loglines = 0;
}; // Log

//---------------------------------------------------
class Syslog: public Log
{
public:
  Syslog( LogLevel level = LogLevel::INFO,
          const char *name = "Log",
	  int bufferlines = 5 )
    { 
      _level       = level;
      _bufferlines = bufferlines;
      openlog(name, LOG_NDELAY|LOG_PID, LOG_USER );
    }
  ~Syslog()
    { 
       Write();
    }
  //
  const char* ClassName() { return "Syslog"; }
  void Write();
  LogLevel GetLogLevel();
  LogLevel SetLogLevel( LogLevel l );
  void SetLogName( std::string logname );
  std::ostringstream& Writer( LogLevel l);
private:
  LogLevel _level;
  int _loglines = 0;
  int _bufferlines;
}; // Syslog
//---------------------------------------------------
class Filer: public Log
{
public:
  Filer( LogLevel level = LogLevel::INFO,
         std::string filename = "logfile.log",  
         int bufferlines = 5)
    { 
      _level       = level;
      _bufferlines = bufferlines;
      _logfile.open (filename,std::ios_base::app);
    }
  ~Filer()
    { 
      Write();
      _logfile.close();
    }
  //
  const char* ClassName() { return "File"; }
  void Write();
  LogLevel GetLogLevel();
  LogLevel SetLogLevel( LogLevel l );
  std::ostringstream& Writer( LogLevel l);
  void SetLogFile( std::string filename );
  void CloseLogFile( );
private:
  LogLevel _level;
  std::ofstream _logfile;
  int _loglines = 0;
  int _bufferlines;
}; // Filer

//---------------------------------------------------
void Filer::CloseLogFile( void )
{
  Write();
  _logfile.close();
} // Filer::CloseLogFile

//---------------------------------------------------
void Filer::SetLogFile( std::string filename )
{
  _logfile.close();
  _logfile.open (filename,std::ios_base::app);
} // Filer::SetLogFile

//---------------------------------------------------
void Syslog::SetLogName( std::string logname )
{
  closelog();
  // copying the contents of the
  // string to static char array
  char* cp = syslogname;
  strncpy(cp, logname.c_str(), syslogname_size);
  //std::cout << "syslogname:" << syslogname << std::endl;
  openlog(syslogname, LOG_NDELAY|LOG_PID, LOG_USER );
} // Filer::SetLogFile
//---------------------------------------------------
LogLevel Filer::SetLogLevel( LogLevel l )
{
  _level = l; 
  return _level;
} // Filer::SetLogLevel

//---------------------------------------------------
LogLevel Syslog::SetLogLevel( LogLevel l )
{
  _level = l; 
  return _level;
} // Syslog::SetLogLevel

//---------------------------------------------------
LogLevel Filer::GetLogLevel()
{
  return _level;
} // Filer::GetLogLevel

//---------------------------------------------------
LogLevel Syslog::GetLogLevel()
{
  return _level;
} // Syslog::GetLogLevel

//---------------------------------------------------
void Filer::Write()
{
  nope.str(std::string()); // discard nope's contents
  //
  _logfile << os.str();
  os.str(std::string()); // discard contents
} // Filer::Write

//---------------------------------------------------
std::ostringstream& Filer::Writer( LogLevel l)
{
  if ( l >= _level )
  { 
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    os << std::put_time(&tm, "%F %T") << " " << l << ": ";
    _loglines++;
    if ( _loglines > _bufferlines )
    {
       //std::cout << "Filer pending are " << _loglines << " flushing" << std::endl;
       Write();
       _loglines = 0;
    }
  }
  else
  {
    nope.str(std::string()); // discard nope's contents
    return nope; 
  }
  return os;
} // Filer::Writer

//---------------------------------------------------
void Syslog::Write()
{
  std::string lines;
  std::string line; 
  int count = 0; 
  int len = 0; 

  if ( os.str().c_str()[0] != '\0' ) 
  {
    lines = os.str(); 
    count = lines.find('\n'); 
    len   = lines.size();
    while ( (count = lines.find('\n')) > 0 ) 
    { //"#012");
      line = lines.substr(0,lines.find('\n'));
      // Call Syslog
      syslog(LOG_INFO, "%s", line.c_str());
      //
      lines = lines.substr(count+1,len);
      count = lines.find('\n'); 
      len = len - count;
    }
  }
  os.str(std::string()); // discard contents
} // Syslog::Write

//---------------------------------------------------
std::ostringstream& Syslog::Writer( LogLevel l)
{
  if ( l >= _level )
  { 
    _loglines++;
    if ( _loglines > _bufferlines )
    {
       //std::cout << "Syslog pending are " << _loglines << " flushing" << std::endl;
       Write();
       _loglines = 0;
    }
    os << l << ": ";
  } 
  else
  {
    nope.str(std::string()); // discard nope's contents
    return nope; 
  }
  return os;
} // Syslog::Writer

//---------------------------------------------------
