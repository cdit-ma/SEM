GraphML specifications
http://graphml.graphdrawing.org/specification/


The Extensible Stylesheet Language Transforms (XSLT) created for transforming from xml models 
to execution artifacts may be used with any available XSLT processor that can process XSLT V1.0.
The transforms have been tested with Xalan-C, Xalan-J. Saxon home edition may be another 
alternative processor if Xalan is not available.


Using Xalan-C 1.11 for Windows or Linux
http://xalan.apache.org/xalan-c/index.html

>  Xalan -o HelloWorld.graphml HelloWorld.xme picml2graphml.xsl
    (note: for the picml2graphml.xsl transform the mga.dtd file must be in the same folder as the xme input file.)

>  Xalan -o HelloWorldyEd.graphml HelloWorld.graphml yEdAdd.xsl
>  Xalan -o HelloWorldEdit.graphml HelloWorldyEd.graphml yEdDelete.xsl
>  Xalan -p Multiplier 10 -o HelloWorldx10.graphml HelloWorld.graphml Scale.xsl
>  Xalan -p Multiplier 0.1 -o HelloWorldx0_1.graphml HelloWorld.graphml Scale.xsl
>  Xalan -p Algorithm 1 -o HelloWorldDeploy.graphml HelloWorld.graphml Deploy.xsl
>  Xalan -o HelloWorld.cdd HelloWorldDeploy.graphml graphml2cdd.xsl
>  Xalan -o HelloWorld.cdp HelloWorldDeploy.graphml graphml2cdp.xsl
>  Xalan -o HelloWorld.dts HelloWorldDeploy.graphml graphml2dts.xsl
>  Xalan -p File "'HelloWorldDeploy'" -o test.config HelloWorldDeploy.graphml graphml2testconfig.xsl
>  Xalan -p HardwareNode "'Rasppi204'" -p File "'HelloWorldDeploy'" -o deploy_Rasppi204.config HelloWorldDeploy.graphml graphml2deployconfig.xsl
>  Xalan -p HardwareNode "'Gouda22'" -o Gouda22%QoS.ddd HelloWorldDeploy.graphml graphml2ddd.xsl
>  Xalan -p ComponentInstance "'Assembly%Component'" -o Assembly%Component%QoS.dpd HelloWorldDeploy.graphml graphml2dpd.xsl
>  Xalan -o HelloWorldDeploy.graphml HelloWorldDeploy.graphml PreprocessIDL.xsl
>  Xalan -p File "'HelloWorld'" -o HelloWorld.idl HelloWorldDeploy.graphml graphml2idl.xsl
>  Xalan -p File "'HelloSenderImpl.mpc'" -o HelloSenderImpl.mpc HelloWorldDeploy.graphml graphml2mpc.xsl
>  Xalan -p FileList "'HelloSenderImpl.mpc,HelloReceiverImpl.mpc,HelloWorld_Components.mpc,HelloWorld.mpc'" -o HelloWorld.mwc HelloWorldDeploy.graphml graphml2mwc.xsl
>  Xalan -p File "'HelloSenderImpl.h'" -o HelloSenderImpl.h HelloWorldDeploy.graphml graphml2h.xsl
>  Xalan -p File "'HelloSenderImpl.cpp'" -o HelloSenderImpl.cpp HelloWorldDeploy.graphml graphml2cpp.xsl


Using Xalan-J on Windows
http://xalan.apache.org/xalan-j/index.html
> java -jar %MEDEA_ROOT%\Resources\Binaries\xalan.jar -in HelloWorld.xme -xsl %MEDEA_ROOT%\Resources\Transforms\picml2graphml.xsl -out HelloWorld.graphml
    (note: for the picml2graphml.xsl transform the mga.dtd file must be in the same folder as the xme input file.)

Alternativley the Windows environment may be set up to specify any jar locations
CLASSPATH=%CLASSPATH%;C:\MEDEA\Resources\Binaries\xalan.jar;C:\MEDEA\Resources\Binaries\serializer.jar;C:\MEDEA\Resources\Binaries\xml-apis.jar;C:\MEDEA\Resources\Binaries\xercesImpl.jar

If the Windows environment is set, use the org.apache.xalan.xslt.Process with java as follows;
> java org.apache.xalan.xslt.Process -in HelloWorld.xme -xsl %MEDEA_ROOT%\Resources\Transforms\picml2graphml.xsl -out HelloWorld.graphml
> java org.apache.xalan.xslt.Process -in HelloWorld.graphml -xsl %MEDEA_ROOT%\Resources\Transforms\yEdAdd.xsl -out HelloWorldyEd.graphml

For localhost deployment with no Hardware node defined in the model;
Use the following command line transforms for HelloWorld SEM 
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2cdd.xsl -PARAM MainNode "'1'" -OUT HelloWorld.cdd
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2cdp.xsl -PARAM MainNode "'1'" -OUT HelloWorld.cdp
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2testconfig.xsl -PARAM MainNode "'1'" -OUT test.config
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2deployconfig.xsl -PARAM MainNode "'1'" -OUT deploy_localhost.config
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2ddd.xsl -OUT MainNode%QoS.ddd
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2dpd.xsl -PARAM ComponentInstance "'HelloWorld_asm%Sender'" -OUT HelloWorld_asm%Sender%QoS.ddd
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2dpd.xsl -PARAM ComponentInstance "'HelloWorld_asm%Receiver'" -OUT HelloWorld_asm%Receiver%QoS.ddd
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\PreprocessIDL.xsl -OUT HelloWorldIDL.graphml
> java org.apache.xalan.xslt.Process -IN HelloWorldIDL.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2idl.xsl -PARAM File "HelloWorld" -OUT HelloWorld.idl
> java org.apache.xalan.xslt.Process -IN HelloWorldIDL.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2idl.xsl -PARAM File "HelloWorld_Components" -OUT HelloWorld_Components.idl
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2mpc.xsl -PARAM File "HelloSenderImpl.mpc" -OUT HelloSenderImpl.mpc
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2mpc.xsl -PARAM File "HelloReceiverImpl.mpc" -OUT HelloReceiverImpl.mpc
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2mpc.xsl -PARAM File "HelloWorld.mpc" -OUT HelloWorld.mpc
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2mpc.xsl -PARAM File "HelloWorld_Components" -OUT HelloWorld_Components.mpc
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2mwc.xsl -PARAM FileList "HelloSenderImpl.mpc,HelloReceiverImpl.mpc,HelloWorld_Components.mpc,HelloWorld.mpc" -OUT HelloWorld.mwc
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2h.xsl -PARAM File "HelloSenderImpl.h" -OUT HelloSenderImpl.h
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2h.xsl -PARAM File "HelloReceiverImpl.h" -OUT HelloReceiverImpl.h
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2cpp.xsl -PARAM File "HelloSenderImpl.cpp" -OUT HelloSenderImpl.cpp
> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\graphml2cpp.xsl -PARAM File "HelloReceiverImpl.cpp" -OUT HelloReceiverImpl.cpp


Using saxon9he java version on windows
http://saxon.sourceforge.net/

>  java  -jar saxon9he.jar -s:HelloWorld.xme -xsl:\picml2graphml.xsl -o:HelloWorld.graphml



Schematron transforms using Xalan-C
>  Xalan -p phase "'#ALL'" -p generate-paths "'false'" -o MEDEA.xsl MEDEA.sch iso_svrl_for_xslt1.xsl
>  Xalan -p phase "'IDL'" -p generate-paths "'false'" -o MEDEA_IDLnode.xsl MEDEA.sch iso_svrl_for_xslt1.xsl

>  Xalan -o report.xml HelloWorld.graphml MEDEA.xsl
>  Xalan -p selectID "'n0::n0::n0'" -o report.xml HelloWorld.graphml MEDEA.xsl

Schematron transforms using Xalan-J
> java org.apache.xalan.xslt.Process -IN %MEDEA_ROOT%\Resources\Transforms\MEDEA.sch -XSL %MEDEA_ROOT%\Resources\Transforms\iso_svrl_for_xslt1.xsl -PARAM phase "#ALL" -PARAM generate-paths "false" -OUT %MEDEA_ROOT%\Resources\Transforms\MEDEA.xsl

> java org.apache.xalan.xslt.Process -IN HelloWorld.graphml -XSL %MEDEA_ROOT%\Resources\Transforms\MEDEA.xsl -OUT report.xml
