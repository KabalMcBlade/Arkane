-----------------------------------------------------------------------

© Copyright 2015-2017 PureDev Software Ltd. All Rights Reserved.
Author:		Stewart Lynch
date:		25/02/2020
Website:	www.puredevsoftware.com
Version:	3.7

-----------------------------------------------------------------------
                              VMem
-----------------------------------------------------------------------



This file is part of VMem.

VMem is dual licensed. For use in open source software VMem can
be used under the GNU license. For use in commercial applications a
license can be purchased from Stewart Lynch.

If used under the GNU license VMem is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the License,
or (at your option) any later version.

VMem is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with VMem. If not, see <http://www.gnu.org/licenses/>.

If VMem is used under the commercial license please see the license
file distributed with this software, or see http://www.puredevsoftware.com/VMme_EULA.htm

-----------------------------------------------------------------------
Usage:

There are two ways to link to the VMem code:

	1. include the combined VMem.cpp
	2. link to the VMem library

Option 1 is the quickest and easiest method.


Option 1: Lnclude the combined VMem.cpp
---------------------------------------

In your main cpp file, include the combined VMem.cpp file (yes, the cpp file)

	#define VMEM_OVERRIDE_NEW_DELETE
	#include "VMemSingleFile\VMem.cpp"

And that's it!


Option 2: Link to the VMem library
----------------------------------

there are a number of pre-built libraries in the lib folder, for Windows,
Unix and OSX (for iOS please see the iOS README). Link to the appropiate
library, and add these two lines to your main cpp file:

	#define VMEM_OVERRIDE_NEW_DELETE
	#include "include\VMemAlloc.hpp"

Note, because you are not using the combined cpp file, you must include the VMemAlloc HEADER.


Building your own VMem library
------------------------------

For Windows there is a vcproj file which will build the VMem library.
For unix/gcc there is either the makefile or a codeblocks project file.
For OSX there is a xcode project.


If you have already overridden new/delete
-----------------------------------------

The options above use the VMEM_OVERRIDE_NEW_DELETE define. This tells VMem to override
new and delete. If you have already overridden new and delete you can't use this
option, so instead of defining VMEM_OVERRIDE_NEW_DELETE you must add the VMem calls
into your own new/delete overrides, for example:

	void* operator new(size_t size)
	{
		return VMem::Alloc(size);
	}

	void operator delete(void* p)
	{
		VMem::Free(p);
	}

	void* operator new[](size_t size)
	{
		return VMem::Alloc(size);
	}

	void operator delete[](void* p)
	{
		VMem::Free(p);
	}


-----------------------------------------------------------------------
VMem_Sample - this is a very simple app that shows how to use the single VMem.cpp file.

-----------------------------------------------------------------------
Notes:

* To avoid linker errors when using VMEM_OVERRIDE_NEW_DELETE, make sure that
VMem.hpp is included before any system headers that also override new/delete. You 
may need to turn off any pre-compiled headers for that file.


-----------------------------------------------------------------------

If you have any problems getting VMem to work please contact Stewart Lynch at slynch@puredevsoftware.com

