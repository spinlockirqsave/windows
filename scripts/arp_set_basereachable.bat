:: @peterg
:: This script search ipv4 interfaces for something named "Local(...)"
:: (we make the assumption this is Local Area Connection) and sets ARP
:: basereachable timeout to parameter passed into script or asks the user
:: to specify multiplier and sets basereachable to multiplier*15000 in case
:: parameter is not passed

:: BaseReachable time which manages expiration of ARP entries in local host ARP table.
:: Windows generates random number between 0.5 - 1.5 and sets this expiration time to be
:: the number*BaseReachable milliseconds. Some our machines talks to 20-40 other machines
:: (maybe more, I don't know) and default for Windows 7 is set to 30s, so most machines
:: will ask for records about every minutes, which generates a lot of traffic.

SETLOCAL
@ECHO OFF

SET res=
SET MULTIPLIER=
SET BASEREACHABLE=

IF %1.==. (
Echo ARP reachable base timeout argument is missing
Echo Usage:
Echo %0 reachablebase
Echo Do yo want to set reachable base timeout to some multiple of 15s?

goto :resCheck
:resPrompt
set /p "res=y/n: "
:resCheck
if not defined res goto resPrompt

IF /I "%res%"=="y" (
  SET MULTIPLIER=
  goto :multiplierCheck
  :multiplierPrompt
  set /p "MULTIPLIER=multiplier: "
  :multiplierCheck
  if not defined MULTIPLIER goto multiplierPrompt

  SET /a BASEREACHABLE=15000*MULTIPLIER
  SET MULTIPLIER=
  Echo Setting BASEREACHABLE to %BASEREACHABLE%
  goto :mainLogic
) ELSE (
  ECHO Nothing has been changed. 
  ECHO Exiting...
  SET res=
  SET MULTIPLIER=
  PAUSE
  EXIT /B
)
) ELSE (
  SET BASEREACHABLE=
  goto :basereachableCheck
  :basereachablePrompt
  SET /a BASEREACHABLE=%1
  :basereachableCheck
  if not defined BASEREACHABLE goto basereachablePrompt
  Echo Setting BASEREACHABLE to %BASEREACHABLE%
)

:mainLogic
SET res=
@ECHO ON
netsh interface ipv4 show interfaces

@Echo Off
For /f "skip=2 tokens=1234*" %%a In ('NetSh Interface IPv4 Show Interfaces') Do (
    Call :UseNetworkAdapter %%a %%b %%c %%d "%%e"
)
Exit /B

:UseNetworkAdapter
:: %1 = Interface id
:: %2 = Met
:: %3 = MTU
:: %4 = State
:: %5 = Name (quoted); %~5 = Name (unquoted)
If %5==Local (
    :: change ARP reachable base timeout
    Echo Choosing %1 %2 %3 %4 "%5"
	Echo netsh interface ipv4 set interface %1 basereachable=%BASEREACHABLE%
	netsh interface ipv4 set interface %1 basereachable=%BASEREACHABLE%
	SET res=
    SET MULTIPLIER=
    SET BASEREACHABLE=
	PAUSE
)
Exit /B

@ECHO OFF
PAUSE