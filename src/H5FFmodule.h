/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have          *
 * access to either file, you may request a copy from help@hdfgroup.org.     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Programmer:	Mohamad Chaarawi
 *              Ocotber 2015
 *
 * Purpose:	This file contains declarations which define macros for the
 *		H5FF package.  Including this header means that the source file
 *		is part of the H5FF package.
 */
#ifndef _H5FFmodule_H
#define _H5FFmodule_H

/* Define the proper control macros for the generic FUNC_ENTER/LEAVE and error
 *      reporting macros.
 */
#define H5FF_MODULE
#define H5_MY_PKG       H5FF
#define H5_MY_PKG_ERR   H5E_F
#define H5_MY_PKG_INIT  YES

#endif /* _H5FFmodule_H */
