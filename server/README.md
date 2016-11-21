# DIG-SLS
DIG System Logging Server

## Building
**Requires:**
* CUTS
* ACE
* SQLite3
* Boost

### Unix
```
$ACE_ROOT/bin/mwc.pl -type gnuace -feature_file $CUTS_ROOT/default.features DIG-SLS.mwc
make `cat $CUTS_ROOT/default.features`
```
### Windows
```
%ACE_ROOT%/bin/mwc.pl -type vc9 -feature_file %CUTS_ROOT%/default.features DIG-SLS.mwc
msbuild DIG_SLS.sln /p:Configuration=Release
copy Release/DIG-SLS.exe DIG-SLS.exe
```
