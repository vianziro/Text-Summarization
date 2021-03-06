/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#include "ompi_config.h"

#include "ompi/mpi/c/bindings.h"
#include "ompi/mca/pml/pml.h"
#include "ompi/request/request.h"

#if OMPI_HAVE_WEAK_SYMBOLS && OMPI_PROFILING_DEFINES
#pragma weak MPI_Recv_init = PMPI_Recv_init
#endif

#if OMPI_PROFILING_DEFINES
#include "ompi/mpi/c/profile/defines.h"
#endif

static const char FUNC_NAME[] = "MPI_Recv_init";


int MPI_Recv_init(void *buf, int count, MPI_Datatype type, int source,
                  int tag, MPI_Comm comm, MPI_Request *request)
{
    int rc = MPI_SUCCESS;

    if ( MPI_PARAM_CHECK ) {
        OMPI_ERR_INIT_FINALIZE(FUNC_NAME);
        OMPI_CHECK_DATATYPE_FOR_RECV(rc, type, count);
        OMPI_CHECK_USER_BUFFER(rc, buf, type, count);
        
        if (ompi_comm_invalid(comm)) {
            return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_COMM, FUNC_NAME);
        } else if (((tag < 0) && (tag != MPI_ANY_TAG)) || (tag > mca_pml.pml_max_tag)) {
            rc = MPI_ERR_TAG;
        } else if ((source != MPI_ANY_SOURCE) && 
                   (MPI_PROC_NULL != source) &&
                   ompi_comm_peer_invalid(comm, source)) {
            rc = MPI_ERR_RANK;
        }
        
        OMPI_ERRHANDLER_CHECK(rc, comm, rc, FUNC_NAME);
    }

    if (MPI_PROC_NULL == source) { 
        *request = OBJ_NEW(ompi_request_t);
        /* Other fields were initialized by the constructor for
           ompi_request_t */
        (*request)->req_type = OMPI_REQUEST_NOOP;
        (*request)->req_status = ompi_request_empty.req_status;
        (*request)->req_complete = true;
        (*request)->req_state = OMPI_REQUEST_INACTIVE;
        (*request)->req_persistent = true;
        (*request)->req_free = ompi_request_persistent_proc_null_free;
        return MPI_SUCCESS;
    }

    rc = MCA_PML_CALL(irecv_init(buf,count,type,source,tag,comm,request));
    OMPI_ERRHANDLER_RETURN(rc, comm, rc, FUNC_NAME);
}
