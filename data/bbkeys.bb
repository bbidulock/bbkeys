!**********************************************************************
!** bbkeys.bb: Style file for bbkeys when using Blackbox           **
!**********************************************************************

! set this to true if you want the old bbkeys behavior
! of not grabbbing keystrokes when caps/shift/num-locks are pressed
bbkeys.honorModifiers:        False

! set this to false to use the linear style of window cycling
!bbkeys.menu.stackedCycling:	False

! set this to false to not show the popup window menu for stacked window cycling
!bbkeys.menu.showCycleMenu:	False

! set this to true to allow stacked cycling to go to any window on any
! workspace. all of the windows will appear in the menu if it is being shown.
! it is highly recommended that you turn on showCycleMenu is you are going
! to have this on
!bbkeys.menu.showAllWorkspaces:	True

! *** menu options for the stacked cycling popup window menu....
!bbkeys.menu.justify: right

! ** auto reconfigure when config-file has changed **
!bbkeys.autoConfig:                False
! ** number of seconds between checks **
!bbkeys.autoConfig.checkTimeout:   10

!** position of bbkeys window **
bbkeys.position:               +0-0

!** bbkeys on top of other windows **
bbkeys.raised:                 True

!** set the window docus style none, border or texture
!bbkeys.window.focusStyle:      texture

! *************************************************************
! ** By default the values below this point are taken from   **
! ** the Blackbox style.                                     **
! ** If you want to override something just uncomment it     **
! ** and change the value                                    **
! *************************************************************

!** set bevelwidth
!bbkeys.bevelWidth:             4

!** define frame style **
!bbkeys.frame:                 Raised Gradient Vertical Bevel1
!bbkeys.frame.color:           slategrey
!bbkeys.frame.colorTo:         darkslategrey

!bbkeys.active.window.borderColor:	lightgrey
!bbkeys.inactive.window.borderColor:	black
!bbkeys.active.desktop.borderColor:	lightgrey
