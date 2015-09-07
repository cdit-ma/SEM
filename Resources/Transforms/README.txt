GraphML specifications
http://graphml.graphdrawing.org/specification/

Using saxon9he java version on windows
http://saxon.sourceforge.net/

>  java  -jar saxon9he.jar -s:HelloWorld.xme -xsl:picml2IDLgraphml.xsl -o:HelloWorldIDL.graphml
>  java  -jar saxon9he.jar -s:HelloWorldIDL.graphml -xsl:IDLgraphml2yEd.xsl -o:yEd.graphml

>  java  -jar saxon9he.jar -s:HelloWorld.xme -xsl:picml2WEDgraphml.xsl -o:HelloWorldWED.graphml


Using Xalan-C 1.11 for Windows or Linux
http://xalan.apache.org/xalan-c/index.html

>  Xalan -o HelloWorld.graphml HelloWorld.xme picml2graphml.xsl

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

Using Xalan-J on windows
http://xalan.apache.org/xalan-j/index.html

>  java org.apache.xalan.xslt.Process -in HelloWorldASM.graphml -xsl yEdAdd.xsl -out HelloWorldASMyEd.graphml



To produce hardware nodes graphml from Jenkins 

> python Jenkins-Groovy-Runner.py -s http://129.127.9.91:8188/ -u dig -p !Secret123! --groovy Jenkins_Construct_Graphml.groovy --output nodes.graphml

> python Jenkins-Groovy-Runner.py -s http://129.127.9.91:2488/ -u dig -p !Secret123! --groovy Jenkins_Construct_GraphMLNodesList.groovy --output nodes.graphml



Schematron transforms 

>  Xalan -p phase "'#ALL'" -p generate-paths "'false'" -o MEDEA.xsl MEDEA.sch iso_svrl_for_xslt1.xsl

>  Xalan -p phase "'IDL'" -p generate-paths "'false'" -o MEDEA_IDLnode.xsl MEDEA.sch iso_svrl_for_xslt1.xsl

>  Xalan -o report.xml HelloWorld.graphml MEDEA.xsl

>  Xalan -p selectID "'n0::n0::n0'" -o report.xml HelloWorld.graphml MEDEA.xsl