/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
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

/* This test is to be built and run with the HDF5 version 1.10 and
   beyond. The resulting files are used by the 1.8 library to test for
   backwards compatibility for files created with the avoid truncate
   feature. After generating the tests, the developer should run the
   h5extend tool from tools/misc/h5extend on TESTFILE4 and
   TESTFILE4_MULTI before moving the generated files to the 1.8
   branch:

*  h5extend tfile_avoidt_h5extend_v18.h5;
*  h5extend multifile_avoidt_h5extend_v18.h5 

   After this, all the test files can be moved to the 1.8 library
   under test/testfiles/avoid_truncate/* .

   The tavoid_truncate test should be run in the 1.8 library and
   should pass. Once that happens, TESTFILE4 and TESTFILE4_MULTI files
   should be moved back into the 1.10 library under
   test/testfiles/avoid_truncate/. The truncation test should then be
   run and should pass.

*/

/*********************************************************************
 * Purpose: Create 4 testfiles both with Avoid Truncate Feature turned
 * on:

 * The first test file will have an EOA message where EOA < EOF. The
 * 1.8 library should fail when opening the file in any mode.
 *
 * The second test file will have an EOA/EOFS message where EOA >
 * EOF. The 1.8 library should fail when opening the file for RW and
 * succeed for RONLY.
 *
 * The third test file should have an EOA message but the EOA ==
 * EOF. The 1.8 library should succeed in accessing the file and
 * ignore the EOA extension message, but mark it with
 * FLAG_WAS_UNKNOWN. Later the 1.10 library should access that file
 * and check the message flag and update it with the correct EOA
 * values, and change the FLAG to MARK_IF_UNKOWN.
 *
 * The fourth test file should not have an EOA message and should be
 * properly truncated. we will run the h5extend tool on to truncate it
 * properly, remove the extension message and make it accessible with
 * the 1.8 branch.
 *
 * Developer:    Mohamad Chaarawi
 *               December 2014
 *
 *********************************************************************/

#include "hdf5.h"

#define TESTFILE1 "tfile_avoidt_v18_fail.h5"
#define TESTFILE1_MULTI "multifile_avoidt_v18_fail.h5"

#define TESTFILE2 "tfile_avoidt_v18_rdonly.h5"
#define TESTFILE2_MULTI "multifile_avoidt_v18_rdonly.h5"

#define TESTFILE3 "tfile_avoidt_v18.h5"
#define TESTFILE3_MULTI "multifile_avoidt_v18.h5"

#define TESTFILE4 "tfile_avoidt_h5extend_v18.h5"
#define TESTFILE4_MULTI "multifile_avoidt_h5extend_v18.h5"

static int create_file (hid_t fapl, const char* filename);
static int access_file (hid_t fapl, const char* filename, hbool_t increase_eoa);

int main(void)
{
    hid_t fapl;

    /* create 3 files with sec2 driver. at this point eof should be equal to eoa. */
    if(create_file(H5P_DEFAULT, TESTFILE1) < 0) {
        fprintf(stderr, "Failed to create %s\n", TESTFILE1);
        return -1;
    }
    if(create_file(H5P_DEFAULT, TESTFILE2) < 0) {
        fprintf(stderr, "Failed to create %s\n", TESTFILE2);
        return -1;
    }
    if(create_file(H5P_DEFAULT, TESTFILE3) < 0) {
        fprintf(stderr, "Failed to create %s\n", TESTFILE3);
        return -1;
    }
    if(create_file(H5P_DEFAULT, TESTFILE4) < 0) {
        fprintf(stderr, "Failed to create %s\n", TESTFILE4);
        return -1;
    }

    /* access the first and fourth files to delete a dataset
       decreasing the EOA. This should make the eoa != eof since Avoid
       truncate is enabled. The 1.8 library should not be able to
       access this file if it has not been fixed with the h5extend
       tool */
    if(access_file(H5P_DEFAULT, TESTFILE1, -1) < 0)
        return -1;
    if(access_file(H5P_DEFAULT, TESTFILE4, -1) < 0)
        return -1;

    /* access the second file to create a dataset with allocation time
       early increasing the EOA. This should make the eoa > eof since
       Avoid truncate is enabled. The 1.8 library should not be able
       to access this file in RDWR mode if it has not been fixed with
       the h5extend tool. It should be able to read from it though
       normally. */
    if(access_file(H5P_DEFAULT, TESTFILE2, 1) < 0)
        return -1;

    /* Do the same as above but with the multi-vfd. */
    /* create a multi file driver */
    if ((fapl=H5Pcreate(H5P_FILE_ACCESS))<0)
        return -1;
    if (H5Pset_fapl_multi(fapl, NULL, NULL, NULL, NULL, 1) < 0)
        return -1;

    if(create_file(fapl, TESTFILE1_MULTI) < 0)
        return -1;
    if(create_file(fapl, TESTFILE2_MULTI) < 0)
        return -1;
    if(create_file(fapl, TESTFILE3_MULTI) < 0)
        return -1;
    if(create_file(fapl, TESTFILE4_MULTI) < 0)
        return -1;

    if(access_file(fapl, TESTFILE1_MULTI, -1) < 0)
        return -1;
    if(access_file(fapl, TESTFILE4_MULTI, -1) < 0)
        return -1;
    if(access_file(fapl, TESTFILE2_MULTI, 1) < 0)
        return -1;

    if(H5Pclose(fapl) < 0)
        return -1;

    /* Return */
    return 0;
} /* main */

static int create_file (hid_t fapl, const char* filename)
{
    /* Variables */
    hid_t fcpl,fid,sid,did1,did2,did3 = -1;         /* Object Descriptors */

    /* Create a file creation property list */
    if ((fcpl = H5Pcreate(H5P_FILE_CREATE)) < 0) return -1;

    /* Enable 'avoid truncate' feature */
    if (H5Pset_avoid_truncate(fcpl, H5F_AVOID_TRUNCATE_ALL) < 0) return -1;

    /* Create a file that avoids truncation */
    if ((fid = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl)) < 0) {
        fprintf(stderr, "Failed H5Fcreate %s\n", filename);
        return -1;
    }

    /* Close the fcpl */
    if (H5Pclose(fcpl) < 0) {
        fprintf(stderr, "Failed H5Pclose\n");
        return -1;
    }

    /* Create dataspace for dataset */
    if ((sid = H5Screate(H5S_SCALAR)) < 0) {
        fprintf(stderr, "Failed H5Screate\n");
        return -1;
    }

    /* Create dataset */
    if ((did1 = H5Dcreate2(fid, "Dataset1", H5T_NATIVE_INT, sid, H5P_DEFAULT,
                          H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        fprintf(stderr, "Failed to create Dataset1 in %s\n", filename);
        return -1;
    }

    /* Create dataset */
    if ((did2 = H5Dcreate2(fid, "Dataset2", H5T_NATIVE_INT, sid, H5P_DEFAULT,
                          H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        fprintf(stderr, "Failed to create Dataset2 in %s\n", filename);
        return -1;
    }

    /* Create dataset */
    if ((did3 = H5Dcreate2(fid, "Dataset3", H5T_NATIVE_INT, sid, H5P_DEFAULT,
                          H5P_DEFAULT, H5P_DEFAULT)) < 0) {
        fprintf(stderr, "Failed to create Dataset3 in %s\n", filename);
        return -1;
    }

    /* Close the file, dataspace, and dataset */
    H5Sclose(sid);
    H5Dclose(did1);
    H5Dclose(did2);
    H5Dclose(did3);
    H5Fclose(fid);

    return 0;
}

static int access_file (hid_t fapl, const char* filename, hbool_t increase_eoa)
{
    hid_t fid;

    /* Re-open file */
    if ((fid = H5Fopen(filename, H5F_ACC_RDWR, fapl)) < 0) {
        fprintf(stderr, "Failed H5Fopen %s\n", filename);
        return -1;
    }

    if(1 == increase_eoa) {
        hid_t did4, sid,dcpl;
        hsize_t dims[2];

        /* Create the data space */
        dims[0] = 64;
        dims[1] = 128;
        sid = H5Screate_simple(2, dims, NULL);

        if((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0) {
            fprintf(stderr, "Failed to create a DCPL\n");
            return -1;
        }
        if(H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY) < 0) {
            fprintf(stderr, "Failed H5Pset_alloc_time\n");
            return -1;
        }

        /* Create dataset */
        if ((did4 = H5Dcreate2(fid, "Dataset4", H5T_NATIVE_INT, sid, H5P_DEFAULT,
                               dcpl, H5P_DEFAULT)) < 0) {
            fprintf(stderr, "Failed to create Dataset4 in %s\n", filename);
            return -1;
        }
        H5Pclose(dcpl);
        H5Sclose(sid);
        H5Dclose(did4);
    }
    else {
        /* Unlink the dataset, reducing the 'EOA' value (but not EOF) */
        if (H5Ldelete(fid, "Dataset3", H5P_DEFAULT) < 0) {
            fprintf(stderr, "Failed to delete Dataset3 in %s\n", filename);
            return -1;
        }
    }

    /* Close file */
    if (H5Fclose(fid) < 0) {
        fprintf(stderr, "Failed H5Fclose %s\n", filename);
        return -1;
    }

    return 0;
}
