function Component()
{
	component.loaded.connect(this, addRegisterFileCheckBox);
	installer.finishButtonClicked.connect(this, openRegisteredFileIfChecked);
	component.graphmlFormat = "graphml"
}

Component.prototype.createOperations = function(){
	component.createOperations();
	if (systemInfo.productType === "windows") {
		component.addOperation("CreateShortcut", "@TargetDir@/MEDEA.exe", "@StartMenuDir@/MEDEA.lnk", "workingDirectory=@TargetDir@");
		component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Uninstall.lnk", "workingDirectory=@TargetDir@");
	}
}

// called as soon as the component was loaded
addRegisterFileCheckBox = function()
{
    // don't show when updating or uninstalling
    if (installer.isInstaller()) {
        installer.addWizardPageItem(component, "RegisterFileCheckBoxForm", QInstaller.TargetDirectory);
        component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text =
            component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text + "*." +component.graphmlFormat;
    }
}

// here we are creating the operation chain which will be processed at the real installation part later
Component.prototype.createOperations = function()
{
    // call default implementation to actually install the registeredfile
    component.createOperations();

    var isRegisterFileChecked = component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.checked;
    if (installer.value("os") === "win") {
        var iconId = 0;
        var medea_path = "@TargetDir@/MEDEA.exe";
        component.addOperation("RegisterFileType",
                               component.graphmlFormat,
                               medea_path + " '%1'",
                               "MEDEA GraphML Model Format",
                               "text/plain",
                               medea_path + "," + iconId,
                               "ProgId=QtProject.QtInstallerFramework." + component.graphmlFormat);
    }
}

openRegisteredFileIfChecked = function()
{
    if (!component.installed)
        return;

    if (installer.value("os") == "win" && installer.isInstaller() && installer.status == QInstaller.Success) {
        var isOpenRegisteredFileChecked = component.userInterface("OpenFileCheckBoxForm").OpenRegisteredFileCheckBox.checked;
        if (isOpenRegisteredFileChecked) {
            QDesktopServices.openUrl("file:///" + component.fileWithRegisteredType);
        }
    }
}

