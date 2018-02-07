#!/usr/bin/env python
# Script to run all kinds of commands

import os
import sys
import re

def setDefaults(parameters):
    """Set default paratemet for 'parameters' """
    parameters['debugType']         =   'Debug'
    parameters['releaseType']       =   'Release'
    parameters['buildType']         =   'Release' # Default is Release
    parameters['currentDir']        =   os.getcwd()

    parameters['doBuild']           =   True
    parameters['doSCA']             =   True
    parameters['doClean']           =   False
    parameters['buildDir']          =  'build'

    parameters['verbose']           =   False
    parameters['showProgress']      =   True
    parameters['color']             =   True
    parameters['execmd']            =   True    #If it is False, just print command, but do not execute

    parameters['DVROOT']            =   os.getenv('DVROOT')

    # Determine the operating system of the build machine and set some speical parameters
    if sys.platform.startswith('darwin'):
        parameters['osName']    =   'MacOSX'
    elif sys.platform.startswith('linux'):
        parameters['osName']    =   'Linux'
        subplatform = os.uname()[4]
        if re.match('.*i.86', subplatform):
            parameters['subplatform']=   'i386'
        elif re.match('.*86_64', subplatform):
            parameters['subplatform']=   'x86_64'
        else:
            parameters['subplatform']=   'Unknow'
    elif sys.platform.startswith('win32'):
        parameters['osName']        =   'Windows'
    else:
        sys.exit("Unknown system")

def dumpParameters(parameters):
    """Dump parameters in alphabetical order """
    for k in  sorted(parameters.keys()):
        print '  parameters[%18s] = ' % k, parameters[k]
    print " "

END_COLOR="\033[m"
HIGHLIGHTS = dict(
        list(zip([
            'bg_grey',
            'bg_red',
            'bg_green',
            'bg_yellow',
            'bg_blue',
            'bg_magenta',
            'bg_cyan',
            'bg_white'
            ],
            list(range(40, 48))
            ))
        )

COLORS = dict(
        list(zip([
            'black',
            'red',
            'green',
            'yellow',
            'blue',
            'magenta',
            'cyan',
            'white',
            ],
            list(range(30, 38))
            ))
        )

def coloredText(text, fg_color=None, bg_color=None):
    """Colorize text.

    Available text colors:
        red, green, yellow, blue, magenta, cyan, white.

    Available text highlights:
        bg_red, bg_green, bg_yellow, bg_blue, bg_magenta, bg_cyan, bg_white.

    Example:
        coloredText('Hello, World!', 'green')
    """
    if os.getenv('TERM') is not None and parameters['color']:
        #print(parameters['color'])
        if fg_color is not None:
            if bg_color is not None:
                text = '\033[%d;%dm%s' % (HIGHLIGHTS[bg_color], COLORS[fg_color], text)
            else:
                text = '\033[0;%dm%s' % (COLORS[fg_color], text)
            text += END_COLOR
    return text

def printColorText(text, fg_color=None, bg_color=None):
    text = coloredText(text, fg_color, bg_color)
    print(text)

def printColorText2(text1, text2, fg_color1=None, fg_color2=None):
    text =  coloredText(text1, fg_color1)
    text += coloredText(text2, fg_color2)
    print(text)

def printColorText3(text1, text2, text3, fg_color1=None, fg_color2=None, fg_color3=None):
    text =  coloredText(text1, fg_color1)
    text += coloredText(text2, fg_color2)
    text += coloredText(text3, fg_color3)
    print(text)

def printColorHelpItem(text1, text2, fg_color=None):
    printColorText2(text1, text2, fg_color)

def printErrMessage(text):
    printColorText(text, 'white', 'bg_red')

def Error(text):
    printErrMessage(text)
    sys.exit(1)

def Progress(text, parameters):
    if(parameters['showProgress']):
        printColorText('######## '+ text, 'cyan')

def Attention(text):
    printColorText(text, 'white', 'bg_magenta')

def PrintCmd(text):
    printColorText(text, 'magenta')

def FlagBoolVal(text):
    """ Parse arguments of the form --flag --flag=true --flag=on --flag=1 flag=false --flag=off flag=0 """
    value   =   False
    if re.match('.*=[Tt][Rr][Uu][Ee]|.*=[Oo][Nn]|.*[Yy][Ee][Ss]|.*=1', text):
        value = True
    elif re.match('.*=.*', text):
        value = False
    else:
        value = True
    return value

def PrintHelp(parameters, color):
    """ Print out a Usage message, if invoked with --help or if there is an error """
    print ('Usage python run.py [<flag> ...] [ <test> ... ]')
    print ('Where <flag> can be:')
    printColorHelpItem('--help|-h','                    Print help message', color)
    printColorHelpItem("--verbose[={true|false}]","     Write extra information in tests", color)
    printColorHelpItem("--debug","                      Build the debug version", color)
    printColorHelpItem("--release","                    Build the release version (default)", color)
    printColorHelpItem("--build[={true|false}]","       Build (default=true)", color)
    printColorHelpItem("--sca[={true|false}]","         Do source code analyzer (default=true)", color)
    printColorHelpItem("--clean[={true|false}]","       Perform clean before build (default=false)", color)
    printColorHelpItem("--color[={true|false}]","       Color output (default=true)", color)
    printColorHelpItem("--builddir=<dir>","             Use the given directory as build dir(default='build')", color)
    printColorHelpItem("--show[={true|false}]","        Show build(scan) progress (default=false)", color)
    printColorHelpItem("--noexec","                     Do not execute script, just print coomands which need execute", color)

    printColorHelpItem("-v","                           Shortcut for --verbose", color)


def exeCmd(parameters, cmdline):
    """ Execute command line if required, or print command line only """
    res = 0
    if parameters['execmd']:
        res = os.system(cmdline)
    else:
        PrintCmd(cmdline)
    return res

def processOptions(parameters):
    """Process input options """
    Progress ('Process command options', parameters)
    for item in sys.argv:
        if re.match('^--help$|^-h$', item):
            PrintHelp(parameters, 'blue')
            sys.exit(0)
        elif re.match('.*py$', item):
            continue
        elif re.match(r'^--verbose$', item):
            parameters['verbose']           =   FlagBoolVal(item)
        elif re.match(r'^-v', item):
            parameters['verbose']           =   True
        elif re.match('^--[Dd]ebug$', item):
            parameters['buildType']         =   parameters['debugType']
        elif re.match('^--[Rr]elease$', item):
            parameters['buildType']         =   parameters['releaseType']
        elif re.match('^--build$|^--build=.+', item):
            parameters['doBuild']           =   FlagBoolVal(item)
        elif re.match('^--[sS][cC][aA]$|^--sca=.+', item):
            parameters['doSCA']             =   FlagBoolVal(item)
        elif re.match('^--clean$|--clean=.+', item):
            parameters['doClean']           =   FlagBoolVal(item)
        elif re.match(r'^--color', item):
            parameters['color']             =   FlagBoolVal(item)
        elif re.match('^--[Nn]o[Ee]xec$', item):
            parameters['execmd']            =   False
        elif re.match('^--[Ss]how$|^--[Ss]how=.+', item):
            parameters['showProgress']      =   FlagBoolVal(item)
        elif re.match('^--build[dD]ir=.+', item):
            parameters['buildDir']          =   re.match('[^=]*=(.*)', item).group(1)   
        else:
            Error('Invalid options: '+ item)


def checkPara(parameters):
    """Check validity of options """
    res = 0
    return res


def checkSystemDeps(parameters):
    """Check system dependence: for example coverity, OCLint etc """
    res = 0
    return res

def cleanEnv(parameters):
	res = 0
	if (parameters['doClean']):
		cmdline = "rm -rf " + parameters['buildDir']
		res = exeCmd(parameters,cmdline)
		cmdline = "mkdir  " + parameters['buildDir'] + " && cd " + parameters['buildDir'] + " && cmake .." 
		res = exeCmd(parameters,cmdline)
	return res

def buildCases(parameters):
    """ Clean Gtest and xstool if required. And rebuild xstool and Gtest if required """
    res = 0
    
    if res:
        Error("Error when build Gtest! ")
    return res

                 
if __name__ == '__main__':
    # Set defult parameters
    parameters = {}
    setDefaults(parameters)

    # Process input options
    processOptions(parameters)
    
    # Check validity of options
    if(checkPara(parameters)):
        PrintHelp(parameters, 'blue')
        Error('Input parameters error!')
    
    # Check system dependence: for example directfb in ubuntu
    if(checkSystemDeps(parameters)):
        PrintHelp(parameters, 'blue')
        Error('System dependence check error. Please see information listed above!')
    
    # Dump used parameters if required
    if (parameters['verbose']):
        dumpParameters(parameters)
    
    # Do clean
    if cleanEnv(parameters):
        Error('Unknown error!')
    
    # Do real build
    buildCases(parameters)
    
    printColorText ( "\n######## Done.", 'green')
