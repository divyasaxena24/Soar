#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/////////////////////////////////////////////////////////////////
// CommandLineInterface class
//
// Author: Jonathan Voigt
// Date  : Sept 2004
//
/////////////////////////////////////////////////////////////////

#include "cli_CommandLineInterface.h"

#ifdef WIN32
#include <direct.h>
#endif // WIN32

#include <assert.h>

#include "cli_GetOpt.h"
#include "cli_Constants.h"
#include "cli_Aliases.h"

// BADBAD: I think we should be using an error class instead to work with error objects.
#include "../../gSKI/src/gSKI_Error.h"
#include "IgSKI_Agent.h"

// SML includes
#include "sml_Connection.h"
#include "sml_TagResult.h"
#include "sml_TagArg.h"
#include "sml_StringOps.h"
#include "sml_KernelSML.h"

using namespace cli;
using namespace sml;

std::ostringstream CommandLineInterface::m_Result;	

EXPORT CommandLineInterface::CommandLineInterface() {

	// Create getopt object
	m_pGetOpt = new GetOpt;

	// Map command names to processing function pointers
	m_CommandMap[Constants::kCLIAddWME]					= &cli::CommandLineInterface::ParseAddWME;
	m_CommandMap[Constants::kCLIAlias]					= &cli::CommandLineInterface::ParseAlias;
	m_CommandMap[Constants::kCLICD]						= &cli::CommandLineInterface::ParseCD;
	m_CommandMap[Constants::kCLIChunkNameFormat]		= &cli::CommandLineInterface::ParseChunkNameFormat;
	m_CommandMap[Constants::kCLIDefaultWMEDepth]		= &cli::CommandLineInterface::ParseDefaultWMEDepth;
	m_CommandMap[Constants::kCLIDirs]					= &cli::CommandLineInterface::ParseDirs;
	m_CommandMap[Constants::kCLIEcho]					= &cli::CommandLineInterface::ParseEcho;
	m_CommandMap[Constants::kCLIExcise]					= &cli::CommandLineInterface::ParseExcise;
	m_CommandMap[Constants::kCLIExplainBacktraces]		= &cli::CommandLineInterface::ParseExplainBacktraces;
	m_CommandMap[Constants::kCLIFiringCounts]			= &cli::CommandLineInterface::ParseFiringCounts;
	m_CommandMap[Constants::kCLIGDSPrint]				= &cli::CommandLineInterface::ParseGDSPrint;
	m_CommandMap[Constants::kCLIHelp]					= &cli::CommandLineInterface::ParseHelp;
	m_CommandMap[Constants::kCLIIndifferentSelection]	= &cli::CommandLineInterface::ParseIndifferentSelection;
	m_CommandMap[Constants::kCLIInitSoar]				= &cli::CommandLineInterface::ParseInitSoar;
	m_CommandMap[Constants::kCLIInternalSymbols]		= &cli::CommandLineInterface::ParseInternalSymbols;
	m_CommandMap[Constants::kCLILearn]					= &cli::CommandLineInterface::ParseLearn;
	m_CommandMap[Constants::kCLILog]					= &cli::CommandLineInterface::ParseLog;
	m_CommandMap[Constants::kCLILS]						= &cli::CommandLineInterface::ParseLS;
	m_CommandMap[Constants::kCLIMatches]				= &cli::CommandLineInterface::ParseMatches;
	m_CommandMap[Constants::kCLIMaxChunks]				= &cli::CommandLineInterface::ParseMaxChunks;
	m_CommandMap[Constants::kCLIMaxElaborations]		= &cli::CommandLineInterface::ParseMaxElaborations;
	m_CommandMap[Constants::kCLIMaxNilOutputCycles]		= &cli::CommandLineInterface::ParseMaxNilOutputCycles;
	m_CommandMap[Constants::kCLIMemories]				= &cli::CommandLineInterface::ParseMemories;
	m_CommandMap[Constants::kCLIMultiAttributes]		= &cli::CommandLineInterface::ParseMultiAttributes;
	m_CommandMap[Constants::kCLINumericIndifferentMode]	= &cli::CommandLineInterface::ParseNumericIndifferentMode;
	m_CommandMap[Constants::kCLIOSupportMode]			= &cli::CommandLineInterface::ParseOSupportMode;
	m_CommandMap[Constants::kCLIPopD]					= &cli::CommandLineInterface::ParsePopD;
	m_CommandMap[Constants::kCLIPreferences]			= &cli::CommandLineInterface::ParsePreferences;
	m_CommandMap[Constants::kCLIPrint]					= &cli::CommandLineInterface::ParsePrint;
	m_CommandMap[Constants::kCLIProductionFind]			= &cli::CommandLineInterface::ParseProductionFind;
	m_CommandMap[Constants::kCLIPushD]					= &cli::CommandLineInterface::ParsePushD;
	m_CommandMap[Constants::kCLIPWatch]					= &cli::CommandLineInterface::ParsePWatch;
	m_CommandMap[Constants::kCLIPWD]					= &cli::CommandLineInterface::ParsePWD;
	m_CommandMap[Constants::kCLIQuit]					= &cli::CommandLineInterface::ParseQuit;
	m_CommandMap[Constants::kCLIRemoveWME]				= &cli::CommandLineInterface::ParseRemoveWME;
	m_CommandMap[Constants::kCLIReteNet]				= &cli::CommandLineInterface::ParseReteNet;
	m_CommandMap[Constants::kCLIRun]					= &cli::CommandLineInterface::ParseRun;
	m_CommandMap[Constants::kCLISaveBacktraces]			= &cli::CommandLineInterface::ParseSaveBacktraces;
	m_CommandMap[Constants::kCLISoar8]					= &cli::CommandLineInterface::ParseSoar8;
	m_CommandMap[Constants::kCLISoarNews]				= &cli::CommandLineInterface::ParseSoarNews;
	m_CommandMap[Constants::kCLISource]					= &cli::CommandLineInterface::ParseSource;
	m_CommandMap[Constants::kCLISP]						= &cli::CommandLineInterface::ParseSP;
	m_CommandMap[Constants::kCLIStats]					= &cli::CommandLineInterface::ParseStats;
	m_CommandMap[Constants::kCLIStopSoar]				= &cli::CommandLineInterface::ParseStopSoar;
	m_CommandMap[Constants::kCLITime]					= &cli::CommandLineInterface::ParseTime;
	m_CommandMap[Constants::kCLITimers]					= &cli::CommandLineInterface::ParseTimers;
	m_CommandMap[Constants::kCLIVerbose]				= &cli::CommandLineInterface::ParseVerbose;
	m_CommandMap[Constants::kCLIVersion]				= &cli::CommandLineInterface::ParseVersion;
	m_CommandMap[Constants::kCLIWaitSNC]				= &cli::CommandLineInterface::ParseWaitSNC;
	m_CommandMap[Constants::kCLIWarnings]				= &cli::CommandLineInterface::ParseWarnings;
	m_CommandMap[Constants::kCLIWatch]					= &cli::CommandLineInterface::ParseWatch;
	m_CommandMap[Constants::kCLIWatchWMEs]				= &cli::CommandLineInterface::ParseWatchWMEs;

	// Set Home to current directory
	GetCurrentWorkingDirectory(m_HomeDirectory);

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
}

EXPORT CommandLineInterface::~CommandLineInterface() {
	if (m_pGetOpt) delete m_pGetOpt;
	if (m_pLogFile) {
		(*m_pLogFile) << "Log file closed due to shutdown." << std::endl;
		delete m_pLogFile;
	}
}

/*************************************************************
* @brief Process a command.  Give it a command line and it will parse
*		 and execute the command using gSKI or system calls.
* @param pConnection The connection, for communication to the client
* @param pAgent The pointer to the gSKI agent interface
* @param pCommandLine The command line string, arguments separated by spaces
* @param pResponse Pointer to XML response object
*************************************************************/
EXPORT bool CommandLineInterface::DoCommand(Connection* pConnection, gSKI::IAgent* pAgent, const char* pCommandLine, ElementXML* pResponse) {
	// No way to return data
	if (!pConnection) return false;
	if (!pResponse) return false;

	// Log input
	if (m_pLogFile) {
		if (pAgent) (*m_pLogFile) << pAgent->GetName() << "> ";
		(*m_pLogFile) << pCommandLine << endl;
	}

	// Process the command, ignoring its result (errors detected with m_LastError)
	DoCommandInternal(pAgent, pCommandLine);

	// Log output
	if (m_pLogFile) (*m_pLogFile) << m_Result.str() << endl;

	GetLastResultSML(pConnection, pResponse);

	// Always returns true to indicate that we've generated any needed error message already
	return true;
}

void CommandLineInterface::GetLastResultSML(sml::Connection* pConnection, sml::ElementXML* pResponse) {
	assert(pConnection);
	assert(pResponse);

	// Handle source error output
	if (m_SourceError) {
		SetError(CLIError::kSourceError);
		SetErrorDetail(m_SourceErrorDetail);
		m_SourceError = false;
	}

	if (m_LastError == CLIError::kNoError) {
		// The command succeeded, so return the result if raw output
		if (m_RawOutput) {
			pConnection->AddSimpleResultToSMLResponse(pResponse, m_Result.str().c_str());

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
	}

	// reset state
	m_Result.str("");
	m_ResponseTags.clear();	
	m_LastError = CLIError::kNoError;	
	m_LastErrorDetail.clear();			
	gSKI::ClearError(&m_gSKIError);
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
	SetError(CLIError::kNoError);

	std::string result ;
	vector<string> argv;

	// 1) Parse command
	if (Tokenize(pCommandLine, argv) == -1)
		return false ;

	// 2) Translate aliases
	if (!argv.empty())
	{
		m_Aliases.Translate(argv);
		
		// 3) Reassemble the command line
		for (unsigned int i = 0 ; i < argv.size() ; i++)
		{
			result += argv[i] ;
			if (i != argv.size()-1) result += " " ;
		}
	}

	pConnection->AddSimpleResultToSMLResponse(pResponse, result.c_str());

	return true ;
}

bool CommandLineInterface::DoCommandInternal(gSKI::IAgent* pAgent, const std::string& commandLine) {
	vector<string> argv;
	// Parse command:
	if (Tokenize(commandLine, argv) == -1)  return false;	// Parsing failed

	// Execute the command
	return DoCommandInternal(pAgent, argv);
}

bool CommandLineInterface::DoCommandInternal(gSKI::IAgent* pAgent, vector<string>& argv) {
	if (!argv.size()) return true;

	// Translate aliases
	m_Aliases.Translate(argv);

	// Is the command implemented?
	if (m_CommandMap.find(argv[0]) == m_CommandMap.end()) return SetError(CLIError::kCommandNotFound);

	// Check for help flags
	if (CheckForHelp(argv)) {
		m_Result << "Help deprecated until release, please see\n\thttp://winter.eecs.umich.edu/soarwiki";
		return SetError(CLIError::kNoUsageFile);
	}

	// Process command
	CommandFunction pFunction = m_CommandMap[argv[0]];
	assert(pFunction);
	
	// Initialize GetOpt
	m_pGetOpt->Initialize();

	// Make the Parse call
	return (this->*pFunction)(pAgent, argv);
}

int CommandLineInterface::Tokenize(string cmdline, vector<string>& argumentVector) {
	int argc = 0;
	string::iterator iter;
	string arg;
	bool quotes = false;
	int brackets = 0;

	for (;;) {

		// Is there anything to work with?
		if(cmdline.empty()) break;

		// Remove leading whitespace
		iter = cmdline.begin();
		while (isspace(*iter)) {
			cmdline.erase(iter);

			if (!cmdline.length()) break; //Nothing but space left
			
			// Next character
			iter = cmdline.begin();
		}

		// Was it actually trailing whitespace?
		if (!cmdline.length()) break;// Nothing left to do

		// We have an argument
		++argc;
		arg.clear();
		// Use space as a delimiter unless inside quotes or brackets (nestable)
		while (!isspace(*iter) || quotes || brackets) {
			// Eat quotes but leave brackets
			if (*iter == '"') {
				// Flip the quotes flag
				quotes = !quotes;

			} else {
				if (*iter == '{') {
					++brackets;
				} else if (*iter == '}') {
					--brackets;
					if (brackets < 0) {
						SetError(CLIError::kExtraClosingParen);
						return -1;
					}
				}
			}

			// Add to argument (if we eat quotes, this has to be moved into the else above
			arg += (*iter);

			// Delete the character and move on on
			cmdline.erase(iter);
			iter = cmdline.begin();

			// Are we at the end of the string?
			if (iter == cmdline.end()) {

				// Did they close their quotes or brackets?
				if (quotes || brackets) {
					SetError(CLIError::kUnmatchedBracketOrQuote);
					return -1;
				}
				break;
			}
		}

		// Store the arg
		argumentVector.push_back(arg);
	}

	// Return the number of args found
	return argc;
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

/*************************************************************
* @brief Set the kernel this command line module is interfacing with.
* @param pKernel The pointer to the gSKI kernel interface
* @param kernelVersion The gSKI version, available from the KernelFactory
* @param pKernelSML The pointer to the KernelSML object, optional, used to disable print callbacks
*************************************************************/
EXPORT void CommandLineInterface::SetKernel(gSKI::IKernel* pKernel, gSKI::Version kernelVersion, sml::KernelSML* pKernelSML) {
	m_pKernel = pKernel;
	m_KernelVersion = kernelVersion;
	m_pKernelSML = pKernelSML;
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

bool CommandLineInterface::RequireAgent(gSKI::IAgent* pAgent) {
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

void CommandLineInterface::AddListenerAndDisableCallbacks(gSKI::IAgent* pAgent) {
	if (m_pKernelSML) m_pKernelSML->DisablePrintCallback(pAgent);
	if (pAgent) pAgent->AddPrintListener(gSKIEVENT_PRINT, this);
}

void CommandLineInterface::RemoveListenerAndEnableCallbacks(gSKI::IAgent* pAgent) {
	if (pAgent) pAgent->RemovePrintListener(gSKIEVENT_PRINT, this);
	if (m_pKernelSML) m_pKernelSML->EnablePrintCallback(pAgent);
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
