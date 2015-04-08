var txtCharset 		= "iso-8859-1";
				

var txtYouCanSetupUSBforDisk_Info  ="You can setup USB key to "+
				"access Rohos disk with it instead of a password. The password will be securely stored on the USB key. "+
				"The disk password will be entered automatically upon USB key insertion.";
var txtEnterYouDiskPass		="Enter Rohos disk password:";
var txtWrongConfirmationTryAgain="The new and confirmed passwords you entered do not match!\nRe-enter the new password in \x22Confirmation\x22 text box.";
var txtConnectUSBKeyXX=		"<b>Connect your USB device</b> (XX). ";
var txtNewPassToUSBKeyXX	="New password will be stored on the USB Key <b> XX </b>.";
var txtYouMayChangeDiskPassw	="You can change Rohos disk password.";

var txtPass			="Password:";
var txtNewPass			="New password:";
var txtConfirm			="Confirmation:";
var txtLetterWillChangeLater	="You should reconnect disk to apply new disk letter.";
var txtDiskLetterInfo		="To change disk letter you should first connect disk.";
var txtCryptoAlg		="Encryption algorithm: ";
var txtDiskImage		="Disk container: ";
var txtAutomountLabel		="<b>Connect Rohos disk automatically on system start.</b>";
var txtAutomountInfo		="You will be prompted to enter disk password on Windows start-up. If there will be USB Key for the disk it be connected automatically.";
	
var txtChangeDiskLetter		="<b>Change disk letter:</b> ";
var txtDiskNotMounted_Info	="<font color=red>Your favorite disk is not connected.</font>";
var txtEmptyPassNotAllowed	="Password cannot be empty.";
var txtNewDiskInfo		="Encrypted Rohos disk size <b> XX </b> Megabytes" +
	      		", disk letter <b> YYY </b>. Disk container path: <b> ZZZZ </b>." 
		  	
var txtDiskParams	=  "<a href='app:1011' title='Change disk properties.'>[Change...]</a>";

var txtCreateNewDisk	="Creating encrypted virtual drive. Rohos automatically has defined virtual drive configuration, however you can change it.";
var txtDiskPasswInfo	="Specify a new password to access the disk: ";
var txtConfirmPassw	="Confirm password:";

var txtErrorSetNewPassword = "Unable to set new password.\n";

			
var txtDiskWizardInfo   = "This wizard helps you create your personal encrypted disk. You can use it to store sensitive files and folders. The disk will be protected by a password. The wizard has determined best parameters for the disk but you may change it using 'details' link.";
var txtNextYoucanSetDiskOptions = "You can change disk parameters later on in Rohos Center window";
var txtPressNext	="Press 'Next'.";
var txtLogonWizard_Info	= "If you have a <b>USB key</b>, you can use it to access your Rohos disk, so you don't have to manually enter the password every time."+
			"<br><br><b>Plug it in</b> and choose check boxes to setup USB Key. If you don't have a USB flash drive, click 'Next'.";
var txtSetupUSBinfo	= "Setup this USB device :";
var txtDiskKey		= "To access your Rohos disk";
var txtWizard_UpdatesInfo = "With Rohos Center you can check for updates over an Internet connection."+
			"You will have a chance to see what's new in Rohos and update size, and to download and install update instantly."+
			"";
var txtStartInfo	="<b>Information:</b> <br><br>"+
			"<img src='trayicon.gif' border=0 align=left> You may close Rohos Center window and Rohos will continue to run in the taskbar (near the clock). <br><br>"+
			"<img src='save_as.gif' border=0 align=right> Find and use Rohos <b>disk icon</b> through <a href='app:1004@DiskIcons' title='Open Help'>Save As/Open</a> dialog within MS Office and others applications. <br><br>"+
			"It is strongly recommended to create a <b>Password Reset File</b> for Rohos disk to prevent forgotten password."+
			"<br><br><br>Press 'Finish' to create Rohos disk and open Rohos Center.<br>";
var txtStart		= "Finish";

var txtTitle_Disk	= "Disk Encryption";
var txtStep1		= "Step 1 of 3";
var txtStep2		= "Step 1 of 3";
var txtStep3		= "Step 2 of 3";
var txtLastStep		= "Finish";

var txtTitle_Updates	= "Rohos Updates";

var txtI_Need		= "I want to ...";
var txtEncryptFiles  	= "Encrypt files";
var txtEncryptFiles_Desc= "Learn how to instantly encrypt your files within any application or <a target=_blank href='http://www.rohos.com/protecting_ms_office_documents.htm'>MS Office</a>.";
var txtSetDiskOpt_Desk  = "Setup disk auto start, disconnect hotkeys and other options.";
var txtChangeDiskPasw   = "Change password";
var txtChangeDiskPasw_Desc= "Specify new password to access Rohos disk.";
var txtWWW_disk		="http://www.rohos.com/desktop_security_howto.htm";
var txtTips		="10 Tips ";
var txtTips_Desc 	="Visit <a target=_blank href='http://www.rohos.com/desktop_security_howto.htm' title='www.rohos.com/..'>Rohos web site</a>";
var txtSetupUSBdisk_Desc= "Setup USB stick as a hardware key to access Rohos disk.";
var txtDiskNotMounted	="<b>Primary Rohos disk is not connected.</b>" ;
var txtMountDisk	="Connect disk";
var txtDeleteDisk	="<a href='app:1100@DeleteDisk' title='Delete disk and its contents'>Delete disk</a> ";


var txtDiskNotCreated	= "<b>Rohos disk is not created as yet.</b>";
var txtCreateDisk	= "<a href='app:1100@CreateDisk' title='Create new Rohos Disk'> Create new disk... </a> | <a href='app:1100@BrowseDisk' title='Open already created Rohos Disk from container file'> Open exisiting disk... </a>";
var txtDiskMounted	= "<b>Disk is connected. </b> ";
var txtDiskInfo		= "Size: XX. Free space: YYY. ZZZZ";
var txtBrowseDisk2	= "<a href='app:1100@DisconnectDisk' title='Make the disk inaccessible' > Disconnect </a> &nbsp&nbsp&nbsp&nbsp    <a href='app:1100@ExploreDisk' title='Open explorer window on this disk'> Browse </a>";
var txtBrowseDisk	= "<a href='app:1100@DisconnectDisk' title='Make the disk inaccessible' > Disconnect </a> &nbsp&nbsp&nbsp&nbsp    <a href='app:1100@ExploreDisk' title='Open explorer window on this disk'> Browse </a> &nbsp&nbsp&nbsp&nbsp  ";

var txtBadDisk_Info	= "Disk is damaged. Please report this problem. <a href='app:1100@CreateDisk'> Create new disk </a>";

var txtUpdatesLink	= "<a href='app:0@Update' title='Check and install Rohos update'>Updates </a>";
var txtSupportLink	= "<a href='app:0@Support' title='Ask for support'>Support </a>";
var txtDiskTab		= "Disk<br>Encryption";
var txtLogonTab		= "Windows<br>logon";
var txtNext		= "Next >";
var txtCreateDiskBtn	= "Create disk";



var txtMainDiskOption = "Favorite disk";                      

var txtSkipWizard	= "<a href='app:0@SkipWizard' title='Skip it if this is second install of Rohos'>Skip this Wizard...</a>"

var txtQuickDismount = "<b>Hotkeys to disconnect disk:</b>";
var txtQuickDismount_Desc = "You can quickly disconnect (turn-off) encrypted disk by using hotkeys: ";
var txtDismountByHotKey = "Disconnect by HotKey: ";
var txtSetHotkey_Tip	= "define a new hotkey shortcut.";
var txtSetHotKey_	= "Change...";
var txtDismountByUSB	= "Disconnect by un-plugging USB flash drive:";

var txtRohosHeader1	= "Welcome to Rohos";
var txtRohosHeader2	= "Additional security";
var txtToProtectRohos	= "You can define additional security options to reduce the risk of unauthorized <br>usage of your private information:"
var txtWelcomeToRohos	= "Visit our <a href='http://www.rohos.com/howto.htm' target=_blank title='how-to articles, tips, and more in the following area...'>How-to center</a> to find more on: <br><li>How to <a href='http://www.rohos.com/prevent_forgotten_password.htm' target=_blank>prevent a forgotten password</a>; <li><a href='http://www.rohos.com/computer_protection_0.htm' target=_blank>The basics of computer protection</a> for newbies. <br> <li>Using USB drive for login ? - here's <a href='http://www.rohos.com/how-to-change-action-with-removable-media.htm' target=_blank> how to turn-off Auto-play for USB flash drives</a>."
var txtSetRohosPass	= "Setup a password for Rohos Center";

var txtCreateBackUpKey  = "Create Password Reset File";
var txtCreateBackUpKeyDesc  = " for the Rohos disk to prevent forgotten password. ";
var txtRestoreDisk	= "<a href= 'app:1500@RestoreDisk' title='Restoring Rohos disk by using reserved data.'>Restore Rohos disk.</a>";

var txtHideDisk		= "Create Rohos Disk within media file"
var txtHideDisk_Desc 	= "Place encrypted data inside AVI movie file to hide its presence."
var txtChooseAVI	= "Choose media file (AVI, MP3/4, MPG, WMV, OGG, MKV...)"
var txtChooseAVI_other	= "Choose another file..."
var txtHideNewDiskAVI   = "Rohos disk can be stored inside one big media file (avi, mpg, mp3, ...). The file contents will be modified but you will be able to play it partially."

var txtCreateDiskLink = "Make a shortcut to the disk on your desktop"

var txtDiskX_Choose = "Disk: XX.  [<a href='app:0@BrowseDisk' title='Specify disk container file to store its password on USB Key'>Specify...</a>]"


var txtEncryptUSB_Partition   = "Create protected partition on your USB flash drive."
var txtUSBDiskInfo		="Encrypted partition properties will be:<br> <li> Disk size: <b>XX</b> Megabytes" +
	      		"<li>Partition letter (disk): <b> YYY </b>"+
			"<li>Partition container file: <b>ZZZZ</b>." 

var txtCreateMoreDisk=	"<a href='app:1100@CreateDisk' title='Create additional Rohos disk.' style='PADDING-TOP: 10px'> Create another one... </a>"


var txtCheckDisk	= "<a href='app:1100@CheckDisk' title='Forcedly run Windows Check Disk utility to repair Rohos Disk.'>Check disk</a>";
var txtEncryptUSB 	= "Encrypt USB drive"

var txtEnlargeDisk      = " <a href='app:1100@ResizeDisk' title='Enlarge disk size'>Enlarge...</a>";
var txtToolsDisk = 	"<b><a href='' onclick='ShowToolsPopup(0,0);' >Tools...</a></b>"; 

var txtConnectMore = 	"<a href='app:1100@BrowseDisk' title='Connect another Rohos disk using container selection'> Connect more... </a>"

var txt_AnyDiskHtml =  "<b><a href='' onclick='ShowToolsPopup(0, this.diskName)' diskName=YYY> XX </a></b>";

var txtDisconnect = "<a href='app:1100@DisconnectDisk@XX' title='make the disk inaccessible' > Disconnect </a>"

var txtShowRohosIconTray	= "Show Rohos icon near the clock.";
var txtMountByUSB	= "<b>Connect Rohos Disk automatically </b> by plugging USB Key";
var txtDismountByLogoff = "Disconnect all when user logs off."
var txtBrowseDiskExplorer = "<a href='app:1100@ExploreDisk' title='Open explorer window on this disk'> Browse </a>";

var txt_AnyDiskHtml_new =  "<b><a href='' onclick='ShowToolsPopup(0, this.diskName)' diskName=YYY> XX </a></b>";
var txt_ChooseLanguage = "Language:";
var txt_EnableLogFile	= "Write log files for troubleshooting purposes"

var txtHideFolder 		= "Encrypt Application";
var txtHideFolder_Desc 		= "Hide and protect apps like Skype, Chrome or folders.";
var txtHideFolder_dialogtext 	= "Choose a folder that will be linked with Rohos Disk. This folder will be accessible only when Rohos Disk is on. During this operation all folder data will be moved into Rohos Disk.";
var txtSelectExistingFolder	= "Select a predefined App or Folder:";
var txtSpecifyFolder_manualy 	= "Or specify a folder path manually:"; 


var txtHide_Folder_Relink	= "<a href='app:1025@Restore' title='This will restore app/folder access after App reinstallation.'> Restore </a> "
var txtHide_Folder_Delete	= "<a href='app:1025@Delete' title='This will delete existing folder link only.'> Delete link </a> "
var txtHide_Folder_Unhide	= "<a href='app:1025@Unhide' title='Decrypt and Unhide folder into its original location and state'> Cancel Encryption </a> "

var txtReconnectOnResume	= "Lock disk on Sleep or inactivity timeout (mins) : "

var txtDiskSharedDrive	= "Enable shared disk drive (local and network)"
var txtPleaseCloseApp	= "Please close selected application or folder before starting encryption.";



// logon only strings **************************************************

//old

var txtGina		= "Rohos welcome screen (gina.dll)";
var txtWelcomeScreen	= "Windows XP welcome screen + Rohos";

var txtSetStrongPasswordDesc = "Generate strong password and save it to USB Key."
var txtClearUSBlogin	= "Disable Key..."

var txtChooseLogoffPicture  = "Choose a picture for shutdown dialog box (max. 100*100)";
var txtChooseUSB_butonPicture = "Choose USB Key icon on the logon window (max. 80*80)";
var txtMore		=	"More..."


var txtMSginaNative	= "Typical Windows login (msgina.dll)"
var txtSetupUSBdesc = 	"Setup your authentication device as an access key for Windows. Setup PIN code.";
var txtSetOptionsDesc = "Select the type of USB Key, security options and 2-factor authentication control.";
var txtSetupUsers = 	"Logon screen";
var txtSetupUsersDesc = "Edit logon screen background and user icons"

var txtToSelectUserPicture = 		"Select new picture for your account (not more than 10 Kb)";
var txtStartTime	= "Session started: ";
var txtWorkTime		= "Your on-computer work time: ";
var txtNoAction = 	"No action";
var txtLockWorkSta = 	"Lock computer";

var txtRemoveUSBAction = 	"<b>USB Key removal behaviour:</b> <A class=Glossary id=idRemoveUSBTip tabIndex=0 href='' >[?]</A> :";	
var txtRemoveUSBActionDescription = "This action will occur when you withdraw your USB Key from computer:";
var txtStandartLogon = 		"<b>Windows logon model</b>";
var txtStandartLogonRestore = 	"You can change Windows logon model (the way you log into Windows): ";

var txtYouCanSetupUSBforWin_Info=	"You can setup your USB device as a hardware key to your computer. "+ 
				"Further to log into Windows you will need to use this USB key. <br><br>"+
				"User: <b>XX</b>, <a href='app:0@SelectUser' title='Select another User account that can use the USB key for login.'>Select user...</a>";

var txtEnterYouWinPass=			"Please enter the user Windows password (optional) :";
var txtLoginPicture = 	"" ;
var txtLoginPictureDesc = "";
var txtLogoffAction = 	 "End your session (log off)";
var txtTurnOffAction =	 "Turn off computer";
var txtHibernateAction = "Hibernate computer";
var txtOnlyUSBloginText = "Allow to login only by using USB Key <A class=Glossary id=idOnlyUSBTip tabIndex=0 href='' >[ ? ]</a> ";
var txtOnlyUSBloginTip = "Allows to login only by using USB Key for selected users. In case of USB Key is broken you can login by using Emergency Login or another user account.";

var txtScreenSaverAction	= "Activate screensaver"
var txtDisplayShutdownDlg = "Install new shutdown dialog box";

var txtEnableSafeMode	= "Protect Safe Mode boot with USB key also";
var txtMaxPinAttempts  = "Maximum number of wrong PIN attempts";
var txtMaxPinAttemptsTip  = "The number of wrong PIN attempts before USB Key is blocked for login. Some of USB tokens has it's own PIN policy.";
var txtHideRohosIconAtLogin  = "Hide Rohos Logon icon from logon screen";


var txtSwitchUserAction= "Switch User";

var txtExceptUserList 	= "Except these users (comma separated list):";
var txtRequireWinPasswAlongWithUSBKey = "Require user to type Windows password along with a USB Key";
var txtWriteAccessLog 	= "Write log file for all authentication events (<a href='app:0@ShowLogFiles'> Open </a>)"


var txtYouCanSetupOTP_Info=	"You can setup Google Authenticator or Yubikey HOTP as a login key to your computer. "+ 
				"Further to log into Windows you will need to enter OTP and login or just OTP. <br><br>"+
				"User: <b>XX</b>, <a href='app:0@SelectUser' title='Select another User account that can use the USB key for login.'>Select user...</a>";


var txtNone =			"None";
var txtRequiredForAnyUser=	"For any user";
var txtRequiredForUserList=	"For listed users"
var txtRequiredForGroup=	"For 'rohos' user group in Active Directory"

var txtRequireForRemoteDesktopUsers	= "For Remote Desktop login"
var txtRequireForRemoteDesktopOutside	= "For Remote Desktop login outside LAN"

var txtManageUserKeysList=	"Users and keys"
var txtManageUserKeysList_Info=	"Manage the list of registered users and keys."



var txtYouCanSetupPhone_Info=	"You can setup your Phone a wireless key to your computer. "+ 
				"Further to log into Windows you will need to use the Phone. <br><br>"+
				"User: <b>XX</b>, <a href='app:0@SelectUser' title='Select another User account that can use the USB key for login.'>Select user...</a>";

var txtPhoneSignalFound =	"<b>XX signal received. Setup Done!</b>"
var txtPhoneSignalWait =	"Waiting for a phone signal...</b>"
var txtCustomScript = 		"Run script"

var txtScanQRCode = 		"Scan QR-Code by Rohos app on your phone:"


//common strings **************************************************


var txtUnblockPIN 	= "<a href= 'app:1500@UnblockPIN' title='Unblock to allow further usage of USB key'>[Unblock USB key...]</a>";
var txtErrorWhile 	= "Error while HTML scripting. Sorry for inconvenience."


var txtSetupUSB = 	"Setup a Key";
var txtSetOptions =	"Options";
var txtExit = 		"Exit";

var txtWWW_welcome = 	"http://www.rohos.com/howto.htm";

var txtHelpLabel = 	"<b><a href='' onclick=' ShowPopup(this.offsetLeft-50, this.offsetTop + 20);' >"+
				"Help...</a> </b>";

var txtHelp = 		"Help";
var txtWebSite = 	"Rohos Web site";
var txtForum = 		"Forum";
var txtAbout = 		"About...";
var txtRegister = 	"Get registered";


var txtCancel   		="Cancel";
var txtWrongPassTryAgain	="The password you entered is incorrect! Please try again.";
var txtConnectUSBKey=		"<b>Please insert your USB device</b>.";
var txtUSBKeyXXFound	=	"USB device <b>XX</b> has been found";


var txtTrialTimeIs40LeftXX_days = 	"<b>YYY-day trial period.</b> XX days left.";
var txtTrialTimeFinished = 		"<b>Trial period has expired</b>.";
var txtRegisterInfo = 			"<a href = 'http://www.rohos.com/desktop-security/buy_on-line.htm' target=_blank >Purchase License</a>.";
var txtRegisterInfo2 = 			"<a href = 'http://www.rohos.com/welcome-screen/buy_on-line.htm' target=_blank >Purchase License</a>.";
var txtSetupUSB_btn=			"Setup Key";

var txtFindMore		="Learn more...";
var txtUSBnotConfigured = "USB Key is not used";
var txtSetPIN		= "Change PIN code";
var txtSetPINDesc	= " to protect USB Key against unauthorized usage for login. USB Key will be blocked after 3 wrong PIN code entry. ";

var txtChooseUSB	= "&nbsp;&nbsp;&nbsp; [<a href='app:0@BrowseUSB' title='You can manually specify your USB device, if program does not detect it automatically.'>Change...</a>]"


var txtUsbKeyNotInstalled = "USB Key is not installed. Failed to load: "
var txtToggleUSBKeyRemovalText = "Perform this action when USB Key is plugged in again."

var txtFreeware= 	"Upgrade Now";
var txtUpgrade= 	"You may use two-factor authentication and hardware key variety and an upgrade.";
var txtUpgradeLink= 	"http://www.rohos.com/products/rohos-logon-free/upgrade/"