/*--------------------------------------------------
 * File: log.cpp
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
 *  * Log:: <log.hpp>
 *
 * Author: Don Cohoon - November 2020
 *--------------------------------------------------
 */
#include <iostream> // io stream
#include <log.hpp>  // log

//---------------------------------------------------
int main(int argc, char *argv[], char *envp[])
{
   //---------------------------------------------------
   // File Log - example
   Filer  LogFiler (LogLevel::INFO,"/var/log/log.log");
   Log &logF = LogFiler;   

   std::cout << "Logging to: " << logF.ClassName()
            << "; Log level: " << logF.GetLogLevel() 
            << std::endl;
   int x = 42;
   logF.Writer(LogLevel::INFO)     << "This is " << argv[0] << " info" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Variable x=" << x << std::endl;
   logF.Writer(LogLevel::CRITICAL) << "Wrapping up" << std::endl;
   logF.SetLogLevel(LogLevel::WARN);
   std::cout << "Changed Log level: " << logF.GetLogLevel() << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop" << std::endl;
   logF.Writer(LogLevel::INFO)     << "Should be invisable" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop2" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop3" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop4" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop5" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop6" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop7" << std::endl;
   logF.Writer(LogLevel::WARN)     << "Whoop whoop8" << std::endl;
  
   //---------------------------------------------------
   // Syslog - example
   Syslog LogSyslog(LogLevel::WARN, argv[0]);
   //Syslog LogSyslog(LogLevel::WARN);
   Log &logS = LogSyslog;   

   std::cout << "Logging to: " << logS.ClassName()
            << "; Log level: " << logS.GetLogLevel() 
            << std::endl;
   logS.Writer(LogLevel::INFO)     << "This is " << argv[0] << " info" << std::endl;
   logS.Writer(LogLevel::WARN)     << "Variable x=" << x << std::endl;
   logS.Writer(LogLevel::CRITICAL) << "Wrapping up" << std::endl;
   logS.SetLogLevel(LogLevel::WARN);
   std::cout << "Changed Log level: " << logS.GetLogLevel() << std::endl;
   logS.Writer(LogLevel::WARN)     << "Whoop whoop" << std::endl;
   logS.Writer(LogLevel::INFO)     << "Should be invisable" << std::endl;
   logS.Writer(LogLevel::WARN)     << "Whoop whoop2" << std::endl; 
   logS.Writer(LogLevel::WARN)     << "Whoop whoop3" << std::endl; 
   logS.Writer(LogLevel::WARN)     << "Whoop whoop4" << std::endl; 
   logS.Writer(LogLevel::WARN)     << "Whoop whoop5" << std::endl; 
}
