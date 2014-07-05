#include "WkCocos/Utils/Logger.h"

namespace WkCocos
{
	/****************
	* Old Logger implementation
	*/


	Logger::Logger(const std::string & LogPrefix, int indentlvl, int indentwidth)
		: _indentlvl(indentlvl), _indentwidth(indentwidth), _logprefix(LogPrefix)
	{
		//_consoleLog = true;
		//_fileLog = false;

#ifdef DEBUG

		std::clog << std::endl << "Logger created : " << _logprefix << std::endl;
#endif

	}

	//TO TEST
	bool Logger::setLogfile(const std::string & filename)
	{
		bool res = true;
		//_ofstr.open(filename.c_str(), std::ofstream::out);
		//if (!_ofstr)
		//{
		//	*this << "Logger ERROR : Failed to open " << filename;
		//	res = false;
		//}
		return res;
	}


	Logger& nl(Logger& log) // adds a new line with the prefix
	{
		std::string s(log._indentlvl * log._indentwidth, ' ');
		s += log._logprefix + " : ";

		log << "\n" << s;
		return log;
	}

	Logger & Logger::flush(void)
	{

		//if (_consoleLog)
		//	std::clog.flush();
		//if (_fileLog)
		//	_ofstr.flush();
		return *this;
	}

	//to enable ostream manipulators on Logger
	Logger& Logger::operator << (std::ostream& (*manip)(std::ostream&))
	{
		//if (_consoleLog)
		//	(*manip)(std::clog);
		//if (_fileLog)
		//	(*manip)(_ofstr);
		return *this;
	}

	//to enable ios manipulators on Logger
	Logger& Logger::operator << (std::ios_base& (*manip)(std::ios_base&))
	{
		//if (_consoleLog)
		//	(*manip)(std::clog);
		//if (_fileLog)
		//	(*manip)(_ofstr);
		return *this;
	}

	//to enable ostream manipulators on Logger
	Logger& Logger::operator << (Logger& (*manip)(Logger&))
	{
		(*manip)(*this);
		return *this;
	}

	Logger::~Logger()
	{
		flush();
		// do not delete appenders
		// creator has to handle it
		_appenders.clear();
#ifdef DEBUG

		std::clog << std::endl << "Logger terminated : " << _logprefix << std::endl;
#endif

	}

	Logger* Logger::_instance = nullptr;
	
	void Logger::create()
	{
		if (!_instance)
		{
			_instance = new Logger();
		}
	}

	Logger& Logger::getInstance()
	{
		return *_instance;
	}

	void Logger::destroy()
	{
		if (_instance)
		{
			delete _instance;
			_instance = nullptr;
		}
	}



} // WkCocos