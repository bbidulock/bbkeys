// grab_defs.hh for bbkeys
//
//  Copyright (c) 1999-2001 by Jason Kasper (vanRijn) vR@movingparts.net
//  Copyright (c) 2001 by Ben Jansens <xor@orodu.net>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//
#ifndef __GRAB_DEFS_HH
#define __GRAB_DEFS_HH

enum {
	grabIconify = 0,
	grabRaise, 
	grabLower,
	grabClose, 
	grabWorkspace1, 
	grabWorkspace2, 
	grabWorkspace3, 
	grabWorkspace4, 
	grabWorkspace5, 
	grabWorkspace6, 
	grabWorkspace7, 
	grabWorkspace8, 
	grabWorkspace9, 
	grabWorkspace10, 
	grabWorkspace11, 
	grabWorkspace12, 
	grabNextWorkspace, 
	grabPrevWorkspace, 

	grabUpWorkspace,
	grabDownWorkspace,
	grabLeftWorkspace,
	grabRightWorkspace,

	grabNextWindow, 
	grabPrevWindow, 
	grabNextWindowAllWorkspaces,
	grabShade, 
	grabMaximize, 
	grabStick, 
	grabExecute, 
	grabVertMax, 
	grabHorizMax, 

	grabNudgeRight, 
	grabNudgeLeft, 
	grabNudgeUp, 
	grabNudgeDown, 

	grabBigNudgeRight, 
	grabBigNudgeLeft, 
	grabBigNudgeUp, 
	grabBigNudgeDown, 

	grabHorizInc, 
	grabVertInc, 
	grabHorizDec, 
	grabVertDec, 

	grabToggleDecor, 

	NUM_GRABS
};

#endif // __GRAB_DEFS_HH
