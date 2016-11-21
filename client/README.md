# DIG-SLC
DIG System Logging Client

## Building
**Requires:**
* CUTS
* ACE
* Sigar
* Boost

### Unix
```
$ACE_ROOT/bin/mwc.pl -type gnuace -feature_file $CUTS_ROOT/default.features DIG-SLC.mwc
make `cat $CUTS_ROOT/default.features`
```
### Windows
```
%ACE_ROOT%/bin/mwc.pl -type vc9 -feature_file %CUTS_ROOT%/default.features DIG-SLC.mwc
msbuild DIG_SLC.sln /p:Configuration=Release
copy Release/DIG-SLC.exe DIG-SLC.exe
```
