function Component()
{
        component.loaded.connect(this, addRegisterFileCheckBox);
	component.graphmlFormat = "graphml"
}

// called as soon as the component was loaded
addRegisterFileCheckBox = function(){
	// don't show when updating or uninstalling
	if (installer.isInstaller()) {
		installer.addWizardPageItem(component, "RegisterFileCheckBoxForm", QInstaller.TargetDirectory);
		component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text =
		component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text + "*." +component.graphmlFormat;
	}
}

// here we are creating the operation chain which will be processed at the real installation part later
Component.prototype.createOperations = function(){
	// call default implementation to actually install the registeredfile
	component.createOperations();
	var isRegisterFileChecked = component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.checked;

	//Register File type on Windows
	if (systemInfo.productType === "windows") {
		var iconId = 1;
		var medea_path = "@TargetDir@/MEDEA.exe";
		component.addOperation("RegisterFileType",
		component.graphmlFormat,
                medea_path + " \"%1\"",
		"MEDEA GraphML Model Format",
		"text/plain",
		medea_path + "," + iconId,
                "ProgId=MEDEA." + component.graphmlFormat);
	}
	//Register into Start-Menu on Windows
	if (systemInfo.productType === "windows") {
		component.addOperation("CreateShortcut", "@TargetDir@/MEDEA.exe", "@StartMenuDir@/MEDEA.lnk", "workingDirectory=@TargetDir@");
                component.addOperation("CreateShortcut", "@TargetDir@/UninstallMEDEA.exe", "@StartMenuDir@/Uninstall.lnk", "workingDirectory=@TargetDir@");
	}
}

