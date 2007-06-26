/////////////////////////////////////////////////////////////////
// CommandLineInterface class file.
//
// Author: Jonathan Voigt, voigtjr@gmail.com
// Date  : 2004
//
/////////////////////////////////////////////////////////////////

#include <portability.h>

#include "cli_CommandLineInterface.h"

// FIXME: This needs to be dealt with in a portable manner.
#ifdef WIN32
#include "pcreposix.h"
#endif // WIN32

#include <assert.h>

#include "cli_Commands.h"
#include "cli_Aliases.h"

#include "gSKI_Error.h"
#include "gSKI_Agent.h"

// SML includes
#include "sml_Connection.h"
#include "sml_TagResult.h"
#include "sml_TagArg.h"
#include "sml_StringOps.h"
#include "sml_KernelSML.h"
#include "sml_AgentSML.h"
#include "sml_XMLTrace.h"

#include "agent.h"

using namespace cli;
using namespace sml;
using namespace std;

std::ostringstream CommandLineInterface::m_Result;	

EXPORT CommandLineInterface::CommandLineInterface() {

	// Map command names to processing function pointers
	m_CommandMap[Commands::kCLIAddWME]						= &cli::CommandLineInterface::ParseAddWME;
	m_CommandMap[Commands::kCLIAlias]						= &cli::CommandLineInterface::ParseAlias;
	m_CommandMap[Commands::kCLIAttributePreferencesMode]	= &cli::CommandLineInterface::ParseAttributePreferencesMode;
	m_CommandMap[Commands::kCLICD]							= &cli::CommandLineInterface::ParseCD;
	m_CommandMap[Commands::kCLIChunkNameFormat]				= &cli::CommandLineInterface::ParseChunkNameFormat;
	m_CommandMap[Commands::kCLICLog]						= &cli::CommandLineInterface::ParseCLog;
	m_CommandMap[Commands::kCLICommandToFile]				= &cli::CommandLineInterface::ParseCommandToFile;
	m_CommandMap[Commands::kCLIDefaultWMEDepth]				= &cli::CommandLineInterface::ParseDefaultWMEDepth;
	m_CommandMap[Commands::kCLIDirs]						= &cli::CommandLineInterface::ParseDirs;
	m_CommandMap[Commands::kCLIEcho]						= &cli::CommandLineInterface::ParseEcho;
	m_CommandMap[Commands::kCLIEchoCommands]				= &cli::CommandLineInterface::ParseEchoCommands;
	m_CommandMap[Commands::kCLIEditProduction]				= &cli::CommandLineInterface::ParseEditProduction;
	m_CommandMap[Commands::kCLIExcise]						= &cli::CommandLineInterface::ParseExcise;
	m_CommandMap[Commands::kCLIExplainBacktraces]			= &cli::CommandLineInterface::ParseExplainBacktraces;
	m_CommandMap[Commands::kCLIFiringCounts]				= &cli::CommandLineInterface::ParseFiringCounts;
	m_CommandMap[Commands::kCLIGDSPrint]					= &cli::CommandLineInterface::ParseGDSPrint;
	m_CommandMap[Commands::kCLIHelp]						= &cli::CommandLineInterface::ParseHelp;
	m_CommandMap[Commands::kCLIIndifferentSelection]		= &cli::CommandLineInterface::ParseIndifferentSelection;
	m_CommandMap[Commands::kCLIInitSoar]					= &cli::CommandLineInterface::ParseInitSoar;
	m_CommandMap[Commands::kCLIInputPeriod]					= &cli::CommandLineInterface::ParseInputPeriod;
	m_CommandMap[Commands::kCLIInternalSymbols]				= &cli::CommandLineInterface::ParseInternalSymbols;
	m_CommandMap[Commands::kCLILearn]						= &cli::CommandLineInterface::ParseLearn;
	m_CommandMap[Commands::kCLILoadLibrary]					= &cli::CommandLineInterface::ParseLoadLibrary;
	m_CommandMap[Commands::kCLILS]							= &cli::CommandLineInterface::ParseLS;
	m_CommandMap[Commands::kCLIMatches]						= &cli::CommandLineInterface::ParseMatches;
	m_CommandMap[Commands::kCLIMaxChunks]					= &cli::CommandLineInterface::ParseMaxChunks;
	m_CommandMap[Commands::kCLIMaxElaborations]				= &cli::CommandLineInterface::ParseMaxElaborations;
	m_CommandMap[Commands::kCLIMaxMemoryUsage]				= &cli::CommandLineInterface::ParseMaxMemoryUsage;
	m_CommandMap[Commands::kCLIMaxNilOutputCycles]			= &cli::CommandLineInterface::ParseMaxNilOutputCycles;
	m_CommandMap[Commands::kCLIMemories]					= &cli::CommandLineInterface::ParseMemories;
	m_CommandMap[Commands::kCLIMultiAttributes]				= &cli::CommandLineInterface::ParseMultiAttributes;
	m_CommandMap[Commands::kCLINumericIndifferentMode]		= &cli::CommandLineInterface::ParseNumericIndifferentMode;
	m_CommandMap[Commands::kCLIOSupportMode]				= &cli::CommandLineInterface::ParseOSupportMode;
	m_CommandMap[Commands::kCLIPopD]						= &cli::CommandLineInterface::ParsePopD;
	m_CommandMap[Commands::kCLIPreferences]					= &cli::CommandLineInterface::ParsePreferences;
	m_CommandMap[Commands::kCLIPrint]						= &cli::CommandLineInterface::ParsePrint;
	m_CommandMap[Commands::kCLIProductionFind]				= &cli::CommandLineInterface::ParseProductionFind;
	m_CommandMap[Commands::kCLIPushD]						= &cli::CommandLineInterface::ParsePushD;
	m_CommandMap[Commands::kCLIPWatch]						= &cli::CommandLineInterface::ParsePWatch;
	m_CommandMap[Commands::kCLIPWD]							= &cli::CommandLineInterface::ParsePWD;
	m_CommandMap[Commands::kCLIQuit]						= &cli::CommandLineInterface::ParseQuit;
	m_CommandMap[Commands::kCLIRemoveWME]					= &cli::CommandLineInterface::ParseRemoveWME;
	m_CommandMap[Commands::kCLIReteNet]						= &cli::CommandLineInterface::ParseReteNet;
	m_CommandMap[Commands::kCLIRun]							= &cli::CommandLineInterface::ParseRun;
	m_CommandMap[Commands::kCLISaveBacktraces]				= &cli::CommandLineInterface::ParseSaveBacktraces;
	m_CommandMap[Commands::kCLISetLibraryLocation]			= &cli::CommandLineInterface::ParseSetLibraryLocation;
	m_CommandMap[Commands::kCLISoar8]						= &cli::CommandLineInterface::ParseSoar8;
	m_CommandMap[Commands::kCLISoarNews]					= &cli::CommandLineInterface::ParseSoarNews;
	m_CommandMap[Commands::kCLISource]						= &cli::CommandLineInterface::ParseSource;
	m_CommandMap[Commands::kCLISP]							= &cli::CommandLineInterface::ParseSP;
	m_CommandMap[Commands::kCLISRand]						= &cli::CommandLineInterface::ParseSRand;
	m_CommandMap[Commands::kCLIStats]						= &cli::CommandLineInterface::ParseStats;
	m_CommandMap[Commands::kCLISetStopPhase]				= &cli::CommandLineInterface::ParseSetStopPhase;
	m_CommandMap[Commands::kCLIStopSoar]					= &cli::CommandLineInterface::ParseStopSoar;
	m_CommandMap[Commands::kCLITime]						= &cli::CommandLineInterface::ParseTime;
	m_CommandMap[Commands::kCLITimers]						= &cli::CommandLineInterface::ParseTimers;
	m_CommandMap[Commands::kCLIUnalias]						= &cli::CommandLineInterface::ParseUnalias;
	m_CommandMap[Commands::kCLIVerbose]						= &cli::CommandLineInterface::ParseVerbose;
	m_CommandMap[Commands::kCLIVersion]						= &cli::CommandLineInterface::ParseVersion;
	m_CommandMap[Commands::kCLIWaitSNC]						= &cli::CommandLineInterface::ParseWaitSNC;
	m_CommandMap[Commands::kCLIWarnings]					= &cli::CommandLineInterface::ParseWarnings;
	m_CommandMap[Commands::kCLIWatch]						= &cli::CommandLineInterface::ParseWatch;
	m_CommandMap[Commands::kCLIWatchWMEs]					= &cli::CommandLineInterface::ParseWatchWMEs;

	// Indicate which commands should be echoed so that all users can see them when doing a shared debugging session
	m_EchoMap[Commands::kCLIAddWME]						= true ;
	m_EchoMap[Commands::kCLIAlias]						= true ;
	m_EchoMap[Commands::kCLIAttributePreferencesMode]	= true ;
	m_EchoMap[Commands::kCLICD]							= true ;
	m_EchoMap[Commands::kCLIChunkNameFormat]			= true ;
	m_EchoMap[Commands::kCLICLog]						= true ;
	m_EchoMap[Commands::kCLICommandToFile]				= true ;
	m_EchoMap[Commands::kCLIDefaultWMEDepth]			= true ;
	m_EchoMap[Commands::kCLIEcho]						= true ;
	m_EchoMap[Commands::kCLIEchoCommands]				= true ;
	m_EchoMap[Commands::kCLIExcise]						= true ;
	m_EchoMap[Commands::kCLIIndifferentSelection]		= true ;
	m_EchoMap[Commands::kCLIInitSoar]					= true ;
	m_EchoMap[Commands::kCLIInputPeriod]				= true ;
	m_EchoMap[Commands::kCLILearn]						= true ;
	m_EchoMap[Commands::kCLILoadLibrary]				= true ; // TODO: figure out if we actually want to echo this
	m_EchoMap[Commands::kCLIMaxChunks]					= true ;
	m_EchoMap[Commands::kCLIMaxElaborations]			= true ;
	m_EchoMap[Commands::kCLIMaxNilOutputCycles]			= true ;
	m_EchoMap[Commands::kCLIMultiAttributes]			= true ;
	m_EchoMap[Commands::kCLINumericIndifferentMode]		= true ;
	m_EchoMap[Commands::kCLIOSupportMode]				= true ;
	m_EchoMap[Commands::kCLIPopD]						= true ;
	m_EchoMap[Commands::kCLIPushD]						= true ;
	m_EchoMap[Commands::kCLIQuit]						= true ;
	m_EchoMap[Commands::kCLIRemoveWME]					= true ;
	m_EchoMap[Commands::kCLIReteNet]					= true ;
	m_EchoMap[Commands::kCLIRun]						= true ;
	m_EchoMap[Commands::kCLISetLibraryLocation]			= true ;
	m_EchoMap[Commands::kCLISoar8]						= true ;
	m_EchoMap[Commands::kCLISource]						= true ;
	m_EchoMap[Commands::kCLISP]							= true ;
	m_EchoMap[Commands::kCLISRand]						= true ;
	m_EchoMap[Commands::kCLISetStopPhase]				= true ;
	m_EchoMap[Commands::kCLIStopSoar]					= true ;
	m_EchoMap[Commands::kCLITimers]						= true ;
	m_EchoMap[Commands::kCLIVerbose]					= true ;
	m_EchoMap[Commands::kCLIWaitSNC]					= true ;
	m_EchoMap[Commands::kCLIWatch]						= true ;
	m_EchoMap[Commands::kCLIWatchWMEs]					= true ;

	// Initialize other members
	m_pKernel = 0;
	m_SourceError = false;
	m_SourceDepth = 0;
	m_SourceDirDepth = 0;
	m_pLogFile = 0;
	m_KernelVersion.major = m_KernelVersion.minor = 0;
	m_LastError = CLIError::kNoError;
	gSKI::ClearError(&m_gSKIError);
	m_Initialized = true;
	m_PrintEventToResult = false;
	m_XMLEventToResult = false;
	m_XMLEventTag = 0 ;
	m_EchoResult = false ;
	m_pAgentSML = 0 ;
	m_CloseLogAfterOutput = false;
	m_VarPrint = false;

	m_XMLResult = new XMLTrace() ;
}

EXPORT CommandLineInterface::~CommandLineInterface() {
	if (m_pLogFile) {
		(*m_pLogFile) << "Log file closed due to shutdown." << std::endl;
		delete m_pLogFile;
	}

	delete m_XMLResult ;
	m_XMLResult = NULL ;
}

/*************************************************************
* @brief Returns true if the given command should always be echoed (to any listeners)
*        The current implementation doesn't support aliases or short forms of the commands.
* @param pCommandLine	The command line being tested
*************************************************************/
EXPORT bool CommandLineInterface::ShouldEchoCommand(char const* pCommandLine)
{
	if (!pCommandLine)
		return false ;

	std::string command = pCommandLine ;

	char const* pSpace = strchr(pCommandLine, ' ') ;
	if (pSpace)
	{
		// Trim everything from space on
		command.erase(pSpace-pCommandLine, command.length()) ;
	}

	// See if there's an entry in the echo map for this command
	// BADBAD: This won't work for short forms of the command or aliases; but making this test
	// happen later in the command line processing causes too many re-entrancy problem within the command line module.
	return (m_EchoMap.find(command) != m_EchoMap.end()) ;
}

/*************************************************************
* @brief Process a command.  Give it a command line and it will parse
*		 and execute the command using gSKI or system calls.
* @param pConnection The connection, for communication to the client
* @param pAgent The pointer to the gSKI agent interface
* @param pCommandLine The command line string, arguments separated by spaces
* @param echoResults If true send a copy of the result to the echo event
* @param pResponse Pointer to XML response object
*************************************************************/
EXPORT bool CommandLineInterface::DoCommand(Connection* pConnection, gSKI::Agent* pAgent, const char* pCommandLine, bool echoResults, ElementXML* pResponse) {
	// No way to return data
	if (!pConnection) return false;
	if (!pResponse) return false;

	// Log input
	if (m_pLogFile) {
		if (pAgent) (*m_pLogFile) << pAgent->GetName() << "> ";
		(*m_pLogFile) << pCommandLine << endl;
	}

	m_EchoResult = echoResults ;
	m_pAgentSML = m_pKernelSML->GetAgentSML(pAgent) ;

	// Process the command, ignoring its result (errors detected with m_LastError)
	DoCommandInternal(pAgent, pCommandLine);

	GetLastResultSML(pConnection, pResponse);

	// Close log if asked to
	if (m_CloseLogAfterOutput) {
		m_CloseLogAfterOutput = false;
		DoCLog(pAgent, LOG_CLOSE, 0, 0, true);
	}

	// Always returns true to indicate that we've generated any needed error message already
	return true;
}

void CommandLineInterface::GetLastResultSML(sml::Connection* pConnection, sml::ElementXML* pResponse) {
	assert(pConnection);
	assert(pResponse);

	// Handle source error output
	if (m_SourceError) {
		//SetError(CLIError::kSourceError);
		SetErrorDetail(m_SourceErrorDetail);
		m_SourceError = false;
	}

	if (m_LastError == CLIError::kNoError) {
        // Log output
        if (m_pLogFile) (*m_pLogFile) << m_Result.str() << endl;

        // The command succeeded, so return the result if raw output
		if (m_RawOutput) {
			pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.str().c_str());
			EchoString(pConnection, m_Result.str().c_str()) ;
		} else {
			// If there are tags in the response list, add them and return
			if (m_ResponseTags.size()) {
				TagResult* pTag = new TagResult();

				ElementXMLListIter iter = m_ResponseTags.begin();
				while (iter != m_ResponseTags.end()) {
					pTag->AddChild(*iter);
					m_ResponseTags.erase(iter);
					iter = m_ResponseTags.begin();
				}

				pResponse->AddChild(pTag);

			} else {
				// Otherwise, return result as simple result if there is one
				if (m_Result.str().size()) {
					pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.str().c_str());
					EchoString(pConnection, m_Result.str().c_str()) ;
				} else {
					// Or, simply return true
					pConnection->AddSimpleResultToSMLResponse(pResponse, sml_Names::kTrue);
				}
			}
		}
	} else {
		// The command failed, add the error message
		string errorDescription = CLIError::GetErrorDescription(m_LastError);
		if (m_LastErrorDetail.size()) {
			errorDescription += "\nError detail: ";
			errorDescription += m_LastErrorDetail;
		}
		if (m_Result.str().size()) {
			errorDescription += "\nResult before error happened:\n";
			errorDescription += m_Result.str();
		}
		if (gSKI::isError(m_gSKIError)) {
			errorDescription += "\ngSKI Error code: ";
			char buf[kMinBufferSize];
			Int2String(m_gSKIError.Id, buf, kMinBufferSize);
			errorDescription += buf;
			errorDescription += "\ngSKI Error text: ";
			errorDescription += m_gSKIError.Text;
			errorDescription += "\ngSKI Error details: ";
			errorDescription += m_gSKIError.ExtendedMsg;
		}
		pConnection->AddErrorToSMLResponse(pResponse, errorDescription.c_str(), m_LastError);
		EchoString(pConnection, errorDescription.c_str()) ;

        // Log error
        if (m_pLogFile) (*m_pLogFile) << errorDescription << endl;
	}

	// reset state
	m_Result.str("");
	m_ResponseTags.clear();	
	m_LastError = CLIError::kNoError;	
	m_LastErrorDetail.clear();			
	gSKI::ClearError(&m_gSKIError);
}

/************************************************************* 	 
* @brief Echo the given string through the smlEVENT_ECHO event
*		 if the call requested that commands be echoed.
*************************************************************/ 	 
void CommandLineInterface::EchoString(sml::Connection* pConnection, char const* pString)
{
	if (!m_EchoResult)
		return ;

	// BUGBUG: We may need to support this for kernel level commands without an agent
	if (m_pAgentSML)
		m_pAgentSML->FireEchoEvent(pConnection, pString) ;
}

/*************************************************************
* @brief Takes a command line and expands any aliases and returns
*		 the result.  The command is NOT executed.
* @param pCommandLine The command line string, arguments separated by spaces
* @param pExpandedLine The return value -- the expanded version of the command
*************************************************************/
bool CommandLineInterface::ExpandCommandToString(const char* pCommandLine, std::string* pExpandedLine)
{
	SetError(CLIError::kNoError);

	vector<string> argv;

	// 1) Parse command
	if (CLITokenize(pCommandLine, argv) == -1)
		return false ;

	// 2) Translate aliases
	if (!argv.empty())
	{
		m_Aliases.Translate(argv);
		
		// 3) Reassemble the command line
		for (unsigned int i = 0 ; i < argv.size() ; i++)
		{
			*pExpandedLine += argv[i] ;
			if (i != argv.size()-1) *pExpandedLine += " " ;
		}
	}

	return true ;
}

/*************************************************************
* @brief Takes a command line and expands any aliases and returns
*		 the result.  The command is NOT executed.
* @param pConnection The connection, for communication to the client
* @param pCommandLine The command line string, arguments separated by spaces
* @param pResponse Pointer to XML response object
*************************************************************/
EXPORT bool CommandLineInterface::ExpandCommand(sml::Connection* pConnection, const char* pCommandLine, sml::ElementXML* pResponse)
{
	std::string result ;

	bool ok = ExpandCommandToString(pCommandLine, &result) ;

	if (ok)
		pConnection->AddSimpleResultToSMLResponse(pResponse, result.c_str());

	return ok ;
}

bool CommandLineInterface::DoCommandInternal(gSKI::Agent* pAgent, const std::string& commandLine) {
	vector<string> argv;
	// Parse command:
	if (CLITokenize(commandLine, argv) == -1)  return false;	// Parsing failed

	// Execute the command
	return DoCommandInternal(pAgent, argv);
}

bool CommandLineInterface::DoCommandInternal(gSKI::Agent* pAgent, vector<string>& argv) {
	if (!argv.size()) return true;

	// Check for help flags
	bool helpFlag = false;
	if (CheckForHelp(argv)) {
		helpFlag = true;
	}

	// Expand aliases
	if (m_Aliases.Translate(argv)) {
		// Is the alias target implemented?
		if (m_CommandMap.find(argv[0]) == m_CommandMap.end()) {
			SetErrorDetail("(No such command: " + argv[0] + ")");
			return SetError(CLIError::kCommandNotImplemented);
		}

	} else {
		// Not an alias, check for partial match
		std::list<std::string> possibilities;
		std::list<std::string>::iterator liter;
		bool exactMatch = false;

		for(unsigned index = 0; index < (argv[0].size()); ++index) {

			if (index == 0) {
				// Bootstrap the list of possibilities
				CommandMapConstIter citer = m_CommandMap.begin();

				while (citer != m_CommandMap.end()) {
					if (citer->first[index] == argv[0][index]) {
						possibilities.push_back(citer->first);
					}
					++citer;
				}

			} else {
				// Update the list of possiblities

				// A more efficient search here would be nice.
				liter = possibilities.begin();
				while (liter != possibilities.end()) {
					if ((*liter)[index] != argv[0][index]) {
						// Remove this possibility from the list
						liter = possibilities.erase(liter);
					} else {
						// check for exact match
						if (argv[0] == (*liter)) {
							// Exact match, we're done
							argv[0] = (*liter);
							exactMatch = true;
							break;
						}
						++liter;
					}
				}
				if (exactMatch) break;
			}

			if (!possibilities.size()) {
				// Not implemented
				SetErrorDetail("(No such command: " + argv[0] + ")");
				return SetError(CLIError::kCommandNotImplemented);

			} 
		}

		if (!exactMatch) {
			if (possibilities.size() != 1) {
				// Ambiguous
				std::string detail = "Ambiguous command, possibilities: ";
				liter = possibilities.begin();
				while (liter != possibilities.end()) {
					detail += (*liter) + ' ';
					++liter;
				}
				SetErrorDetail(detail);
				return SetError(CLIError::kAmbiguousCommand);

			} else {
				// We have a partial match
				argv[0] = (*(possibilities.begin()));
			}
		}
	}

	// Show help if requested
	if (helpFlag) {
		std::string helpFile = m_LibraryDirectory + "/CLIHelp/" + argv[0];
		return GetHelpString(helpFile);
	}

	// Process command
	CommandFunction pFunction = m_CommandMap[argv[0]];
	assert(pFunction);
	
	// Initialize option parsing each call
	ResetOptions();

	// Make the Parse call
	return (this->*pFunction)(pAgent, argv);
}

bool CommandLineInterface::CheckForHelp(std::vector<std::string>& argv) {

	// Standard help check if there is more than one argument
	if (argv.size() > 1) {
		// Is one of the two help strings present?
		if (argv[1] == "-h" || argv[1] == "--help") {
			return true;
		}
	}
	return false;
}

EXPORT void CommandLineInterface::SetKernel(gSKI::Kernel* pKernel, gSKI::Version kernelVersion, sml::KernelSML* pKernelSML) {
	m_pKernel = pKernel;
	m_KernelVersion = kernelVersion;
	m_pKernelSML = pKernelSML;

	// Now that we have the kernel, set the home directory to the location of SoarKernelSML's parent directory,
	// SoarLibrary
#ifdef WIN32
	char dllpath[256];
	GetModuleFileName(static_cast<HMODULE>(m_pKernelSML->GetModuleHandle()), dllpath, 256);

	// This sets it to the path + the dll
	m_LibraryDirectory = dllpath;

	// This chops off the dll part to get just the path (...SoarLibrary/bin)
	m_LibraryDirectory = m_LibraryDirectory.substr(0, m_LibraryDirectory.find_last_of("\\"));

	// This takes the parent directory to get ...SoarLibrary
	m_LibraryDirectory = m_LibraryDirectory.substr(0, m_LibraryDirectory.find_last_of("\\"));

#else // WIN32
	// Hopefully ...SoarLibrary/bin
	GetCurrentWorkingDirectory(m_LibraryDirectory);

	// This takes the parent directory to get ...SoarLibrary
	m_LibraryDirectory = m_LibraryDirectory.substr(0, m_LibraryDirectory.find_last_of("/"));

#endif // WIN32
}

bool CommandLineInterface::GetCurrentWorkingDirectory(string& directory) {
	// Pull an arbitrary buffer size of 1024 out of a hat and use it
	char buf[1024];
	char* ret = getcwd(buf, 1024);

	// If getcwd returns 0, that is bad
	if (!ret) return SetError(CLIError::kgetcwdFail);

	// Store directory in output parameter and return success
	directory = buf;
	return true;
}

bool CommandLineInterface::IsInteger(const string& s) {
	string::const_iterator iter = s.begin();
	
	// Allow negatives
	if (s.length() > 1) {
		if (*iter == '-') {
			++iter;
		}
	}

	while (iter != s.end()) {
		if (!isdigit(*iter)) {
			return false;
		}
		++iter;
	}
	return true;
}

bool CommandLineInterface::RequireAgent(agent* pAgent) {
	// Requiring an agent implies requiring a kernel
	if (!RequireKernel()) return false;
	if (!pAgent) return SetError(CLIError::kAgentRequired);
	return true;
}

bool CommandLineInterface::RequireAgent(gSKI::Agent* pAgent) {
	// Requiring an agent implies requiring a kernel
	if (!RequireKernel()) return false;
	if (!pAgent) return SetError(CLIError::kAgentRequired);
	return true;
}

bool CommandLineInterface::RequireKernel() {
	if (!m_pKernel) return SetError(CLIError::kKernelRequired);
	return true;
}

void CommandLineInterface::AppendArgTag(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParam(pParam);
	pTag->SetType(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

void CommandLineInterface::AppendArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_back(pTag);
}

void CommandLineInterface::PrependArgTag(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParam(pParam);
	pTag->SetType(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_front(pTag);
}

void CommandLineInterface::PrependArgTagFast(const char* pParam, const char* pType, const char* pValue) {
	TagArg* pTag = new TagArg();
	pTag->SetParamFast(pParam);
	pTag->SetTypeFast(pType);
	pTag->SetValue(pValue);
	m_ResponseTags.push_front(pTag);
}

void CommandLineInterface::AddListenerAndDisableCallbacks(gSKI::Agent* pAgent) {
	if (m_pKernelSML) m_pKernelSML->DisablePrintCallback(pAgent);
	m_PrintEventToResult = true;
	if (!m_pLogFile && pAgent) pAgent->AddPrintListener(gSKIEVENT_PRINT, this);
}

void CommandLineInterface::RemoveListenerAndEnableCallbacks(gSKI::Agent* pAgent) {
	if (!m_pLogFile && pAgent) pAgent->RemovePrintListener(gSKIEVENT_PRINT, this);
	m_PrintEventToResult = false;
	if (m_pKernelSML) m_pKernelSML->EnablePrintCallback(pAgent);
}

void CommandLineInterface::AddXMLListenerAndDisableCallbacks(gSKI::Agent* pAgent) {
	if (m_pKernelSML) m_pKernelSML->DisablePrintCallback(pAgent);
	m_XMLEventToResult = true;
	if (pAgent) pAgent->AddXMLListener(gSKIEVENT_XML_TRACE_OUTPUT, this);
}

void CommandLineInterface::RemoveXMLListenerAndEnableCallbacks(gSKI::Agent* pAgent) {
	if (pAgent) pAgent->RemoveXMLListener(gSKIEVENT_XML_TRACE_OUTPUT, this);
	m_XMLEventToResult = false;
	if (m_pKernelSML) m_pKernelSML->EnablePrintCallback(pAgent);

	if (m_XMLEventTag)
	{
		m_ResponseTags.push_back(m_XMLEventTag->DetatchObject()) ;
		delete m_XMLEventTag ;
		m_XMLEventTag = NULL ;
	}
}

bool CommandLineInterface::SetError(cli::ErrorCode code) {
	m_LastError = code;
	return false;
}
bool CommandLineInterface::SetErrorDetail(const std::string detail) {
	m_LastErrorDetail = detail;
	return false;
}

void CommandLineInterface::ResultToArgTag() {
	AppendArgTagFast(sml_Names::kParamMessage, sml_Names::kTypeString, m_Result.str().c_str());
	m_Result.str("");
}

bool CommandLineInterface::StripQuotes(std::string& str) {
    if ((str.size() >= 2) && (str[0] == '"') && (str[str.length() - 1] == '"')) {
        str = str.substr(1, str.length() - 2);
        return true;
    }
    return false;
}

void CommandLineInterface::ResetOptions() {
	m_Argument = 0;
	m_NonOptionArguments = 0;	
}

bool CommandLineInterface::ProcessOptions(std::vector<std::string>& argv, Options* options) {

	// default to indifference
	m_Option = 0;
	m_OptionArgument = "";

	// increment current argument and check bounds
	while (static_cast<unsigned>(++m_Argument) < argv.size()) {

		// args less than 2 characters cannot mean anything to us
		if (argv[m_Argument].size() < 2) {
			++m_NonOptionArguments;
			continue;
		}

		if (argv[m_Argument][0] == '-') {
			if (argv[m_Argument][1] == '-') {
				// possible long m_Option
				if (argv[m_Argument].size() > 2) {
					// long m_Option, save it
					std::string longOption = argv[m_Argument].substr(2);

					// check for partial match
					std::list<Options> possibilities;
					std::list<Options>::iterator liter;

					for(unsigned index = 0; index < longOption.size(); ++index) {

						if (index == 0) {
							// Bootstrap the list of possibilities
							for (int i = 0; options[i].shortOpt != 0; ++i) {
								if (options[i].longOpt[index] == longOption[index]) {
									possibilities.push_back(options[i]);
								}
							}

						} else {
							// Update the list of possiblities

							// A more efficient search here would be nice.
							liter = possibilities.begin();
							while (liter != possibilities.end()) {
								if ((*liter).longOpt[index] != longOption[index]) {
									// Remove this possibility from the list
									liter = possibilities.erase(liter);
								} else {
									// check for exact match
									if (longOption == (*liter).longOpt) {
										// exact match, we're done
										m_Option = liter->shortOpt;
										MoveBack(argv, m_Argument, m_NonOptionArguments);
										if (!HandleOptionArgument(argv, liter->longOpt, liter->argument)) return false;
										return true;
									}
									++liter;
								}
							}
						}

						if (!possibilities.size()) {
							SetErrorDetail("No such m_Option: " + longOption);
							return SetError(CLIError::kUnrecognizedOption);
						} 
					}

					if (possibilities.size() != 1) {
						// Ambiguous
						std::string detail = "Ambiguous m_Option, possibilities: ";
						liter = possibilities.begin();
						while (liter != possibilities.end()) {
							detail += (*liter).longOpt + ' ';
							++liter;
						}
						SetErrorDetail(detail);
						return SetError(CLIError::kAmbiguousOption);

					}
					// We have a partial match
					m_Option = (*(possibilities.begin())).shortOpt;
					MoveBack(argv, m_Argument, m_NonOptionArguments);
					if (!HandleOptionArgument(argv, (*(possibilities.begin())).longOpt, (*(possibilities.begin())).argument)) return false;
					return true;
				}

				// end of options special flag '--'
				// FIXME: remove -- argument?
				m_Option = -1; // done
				return true; // no error
			}

			// short m_Option(s)
			for (int i = 0; options[i].shortOpt != 0; ++i) {
				if (argv[m_Argument][1] == options[i].shortOpt) {
					if (argv[m_Argument].size() > 2) {
						std::vector<std::string>::iterator target = argv.begin();
						target += m_Argument;

						std::string original = *target;
						*target = (*target).substr(0,2);
						++target;

						argv.insert(target, "-" + original.substr(2));
					}
					m_Option = options[i].shortOpt;
					MoveBack(argv, m_Argument, m_NonOptionArguments);
					if (!HandleOptionArgument(argv, options[i].longOpt, options[i].argument)) return false;
					return true;
				}
			}
			SetErrorDetail("No such m_Option: " + argv[m_Argument][1]);
			return SetError(CLIError::kUnrecognizedOption);
		}
		++m_NonOptionArguments;
	}

	// out of arguments
	m_Option = -1;	// done
	return true;	// no error
}

void CommandLineInterface::MoveBack(std::vector<std::string>& argv, int what, int howFar) {

	assert(what >= howFar);
	assert(what > 0);
	assert(howFar >= 0);

	if (howFar == 0) {
		return;
	}

	std::vector<std::string>::iterator target = argv.begin();
	target += what;

	std::vector<std::string>::iterator dest = target - howFar;

	argv.insert(dest, *target);

	target = argv.begin();
	target += what + 1;

	argv.erase(target);
}

bool CommandLineInterface::HandleOptionArgument(std::vector<std::string>& argv, const char* option, int arg) {
	switch (arg) {
		case 0:
			break;
		case 1:
			// required argument
			if (static_cast<unsigned>(++m_Argument) >= argv.size()) {
				std::string detail(option);
				SetErrorDetail("Option '" + detail + "' requires an argument.");
				return SetError(CLIError::kMissingOptionArg);
			}
			m_OptionArgument = argv[m_Argument];
			MoveBack(argv, m_Argument, m_NonOptionArguments);
			break;
		case 2:
		default:
			// optional argument
			if (static_cast<unsigned>(++m_Argument) < argv.size()) {
				if (argv[m_Argument].size()) {
					if (argv[m_Argument][0] != '-') {
						m_OptionArgument = argv[m_Argument];
						MoveBack(argv, m_Argument, m_NonOptionArguments);
					} 
				}
			}
			if (!m_OptionArgument.size()) {
				--m_Argument;
			}
			break;
	}
	return true;
}

/** 
* @brief Event callback function
*
* This method recieves callbacks when the xml event occurs for an agent.
*
* @param eventId	  Id of the event that occured (can only be gSKIEVENT_XML_TRACE_OUTPUT)
* @param agentPtr	  Pointer to the agent that fired the print event
* @param funcType     Pointer to c-style string containing the function type (i.e. addTag, addAttributeValuePair, endTag)
* @param attOrTag     Pointer to c-style string containing the tag to add or remove or the attribute to add
* @param value		  Pointer to c-style string containing the value to add (may be NULL if just adding/ending a tag)
*/
void CommandLineInterface::HandleEvent(egSKIXMLEventId eventId, gSKI::Agent* agentPtr, const char* funcType, const char* attOrTag, const char* value) {
	unused(eventId) ;
	unused(agentPtr) ;

	// Collect up the incoming XML events into an XML object
	if (!m_XMLEventTag)
		m_XMLEventTag = new XMLTrace() ;

	// We need to decide what type of operation this is and we'd like to do that
	// fairly efficiently so we'll switch based on the first character of the name.
	char ch = funcType[0] ;

	switch (ch)
	{
	case 'b' : 
		if (strcmp(sml_Names::kFunctionBeginTag, funcType) == 0)
		{
			m_XMLEventTag->BeginTag(attOrTag) ;
		}
		break ;
	case 'e':
		if (strcmp(sml_Names::kFunctionEndTag, funcType) == 0)
		{
			m_XMLEventTag->EndTag(attOrTag) ;
		}
		break ;
	case 'a':
		if (strcmp(sml_Names::kFunctionAddAttribute, funcType) == 0)
		{
			m_XMLEventTag->AddAttribute(attOrTag, value) ;
		}
		break ;
	default:
		// This is an unknown function type
		assert(ch == 'b' || ch == 'e' || ch == 'a') ;
		break ;
	}
}

void CommandLineInterface::HandleEvent(egSKIPrintEventId, gSKI::Agent*, const char* msg) {
	if (m_PrintEventToResult || m_pLogFile) {
		if (m_VarPrint) {
			// Transform if varprint, see print command
			std::string message(msg);

			regex_t comp;
			regcomp(&comp, "[A-Z][0-9]+", REG_EXTENDED);

			regmatch_t match;
			memset(&match, 0, sizeof(regmatch_t));

			while (regexec(&comp, message.substr(match.rm_eo, message.size() - match.rm_eo).c_str(), 1, &match, 0) == 0) {
				message.insert(match.rm_so, "<");
				message.insert(match.rm_eo + 1, ">");
				match.rm_eo += 2;
			}  

			regfree(&comp);

			// Simply append to message result
			if (m_PrintEventToResult) {
				CommandLineInterface::m_Result << message;
			}
		} else {
			if (m_PrintEventToResult) {
				CommandLineInterface::m_Result << msg;
			}
		}
	}
}

CommandLineInterface* cli::GetCLI()
{
	return sml::KernelSML::GetKernelSML()->GetCommandLineInterface() ;
}

/*************************************************************
* @brief Methods to create an XML element by starting a tag, adding attributes and
*		 closing the tag.
*		 These tags are automatically collected into the result of the current command.
*	
* NOTE: The attribute names must be compile time constants -- i.e. they remain in scope
*		at all times (so we don't have to copy them).
*************************************************************/
void CommandLineInterface::XMLBeginTag(char const* pTagName)
{
	m_XMLResult->BeginTag(pTagName) ;
}

void CommandLineInterface::XMLAddAttribute(char const* pAttribute, char const* pValue)
{
	m_XMLResult->AddAttribute(pAttribute, pValue) ;
}

void CommandLineInterface::XMLEndTag(char const* pTagName)
{
	m_XMLResult->EndTag(pTagName) ;
}

/*************************************************************
* @brief	Occassionally it's helpful to be able to back up
*			in the XML and add some extra elements.
*
*			These calls should only be used once a tag has been completed,
*			so the sequence is something like:
*			beginTag() ;
*			...
*			endTag() ;
*			moveToLastChild() ;	// Goes back to previous tag
*			add an extra attribute() ;
*			moveToParent() ;	// Go back to parent
*			... continue on
*************************************************************/
bool CommandLineInterface::XMLMoveCurrentToParent()
{
	return m_XMLResult->MoveCurrentToParent() ;
}

bool CommandLineInterface::XMLMoveCurrentToChild(int index)
{
	return m_XMLResult->MoveCurrentToChild(index) ;
}

bool CommandLineInterface::XMLMoveCurrentToLastChild()
{
	return m_XMLResult->MoveCurrentToLastChild() ;
}

// The copies over the m_XMLResult object to the response XML object and sets the
// tag name to the command that was just executed.
// The result is XML in this format (e.g. for matches):
// <result><matches>...</matches></result>
// where ... contains the XML specific to that command.
void CommandLineInterface::XMLResultToResponse(char const* pCommandName)
{
	// Extract the XML object from the xmlTrace object and
	// add it as a child of this message.  This is just moving a few pointers around, nothing is getting copied.
	ElementXML_Handle xmlHandle = m_XMLResult->Detach() ;
	ElementXML* pXMLResult = new ElementXML(xmlHandle) ;
	pXMLResult->SetTagName(pCommandName) ;

	m_ResponseTags.push_back(pXMLResult) ;

	// Clear the XML result, so it's ready for use again.
	m_XMLResult->Reset() ;
}

int CommandLineInterface::CLITokenize(string cmdline, vector<string>& argumentVector) {

	int ret = Tokenize(cmdline, argumentVector);
	
	if (ret >= 0) {
		return ret; // no error
	}

	// there is an error

	// handle easy errors with a switch
	switch (ret) {
		case -1:
			SetError(CLIError::kNewlineBeforePipe);
			break;

		case -2:
			SetErrorDetail("An extra '}' was found.");
			SetError(CLIError::kExtraClosingParen);
			break;

		case -3:
			SetErrorDetail("An extra ')' was found.");
			SetError(CLIError::kExtraClosingParen);
			break;

		default:
			{
				int errorCode = abs(ret);

				const int QUOTES_MASK = 4;
				const int BRACKETS_MASK = 8;
				const int PARENS_MASK = 16;
				const int PIPES_MASK = 32;

				std::string errorDetail = "These quoting characters were not closed: ";
				bool foundError = false;
				if (QUOTES_MASK & errorCode) {
					foundError = true;
					errorDetail += "\"";
				}
				if (BRACKETS_MASK & errorCode) {
					foundError = true;
					errorDetail += "{";
				}
				if (PARENS_MASK & errorCode) {
					foundError = true;
					errorDetail += "(";
				}
				if (PIPES_MASK & errorCode) {
					foundError = true;
					errorDetail += "|";
				}
				assert(foundError);
				SetErrorDetail(errorDetail);
			}
			SetError(CLIError::kUnmatchedBracketOrQuote);
			break;
	}

	return ret;
}

