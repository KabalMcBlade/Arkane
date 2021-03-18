-----------------------------------------------------------------------

© Copyright 2015 PureDev Software Ltd. All Rights Reserved.
Author:		Stewart Lynch
date:		06/04/2015
Website:	www.puredevsoftware.com
Version:	2.4

-----------------------------------------------------------------------
                              VMem
-----------------------------------------------------------------------

This software is provided 'as-is', without any express or implied warranty.
In no event will the author(s) be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

This code is released to the public domain, as explained at
http://creativecommons.org/publicdomain/zero/1.0/

-----------------------------------------------------------------------

VMem is a general purpose allocator designed to replace malloc/free/realloc
written by Stewart Lynch. Please send questions, comments, complaints,
performance analysis, etc to slynch@puredevsoftware.com.

-----------------------------------------------------------------------

note: Please read the README.txt FIRST.


To use VMem in an iOS application
---------------------------------

The simplest way to link to VMem is to include the combined cpp file into your main source file
	#define VMEM_OVERRIDE_NEW_DELETE
	#include "VMemSingleFile\VMem.cpp"

The source file must have a .mm extension. If it has just a .m extension, simply rename it.


To link as a library:
---------------------

Alternatively, you can link VMem as a library.

There are no pre-built libraries for iOS. This is because of the number of possible target
devices/configurations. It's simpler if you add the library into your workspace. To add the
VMem library:

* In XCode, right click on your root project and select "Add Files to..."
* Select the VMem_iOS.xcodeproj file
* In your projects Build Phases, add VMem to the "Target Dependencies" list
* In your projects Build Phases, add VMem to the "Link Binary With Libraries" list


IMPORTANT - PLEASE READ
-----------------------

Overriding new/delete is a bit tricky on iOS. By default it will also override the
functions in shared libraries. This can cause problems because of multiple definitions
of new/delete, memory will be allocated from one definition, and freed from another
definition. To stop this from happening do the following:

* copy the unexport.txt file from the VMem folder to your application folder (where your workspace and source files are)
* Go to the build settings for your application
* type 'unexport' into the search box
* you should see the 'Unexported Symbols fFile' setting
* enter 'unexport.txt'

This tells the compiler/linker to hide the overridden new/delete operators from shared libraries.

see here for more details:
https://developer.apple.com/library/mac/technotes/tn2185/_index.html#//apple_ref/doc/uid/DTS10004200-CH1-SECTION13

Once you have done that you need to override new/delete. One way to do this is to add the following
two lines to a source file in your mail application project:

#define VMEM_OVERRIDE_NEW_DELETE
#include "VMemAlloc.hpp"

The source file must have a .mm extension. If it has just a .m extension, simply rename it.

------------------------

If you have any problems getting VMem to work please contact Stewart Lynch at slynch@puredevsoftware.com

