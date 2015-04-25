/*
 * Copyright (c) 2004-2006 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "orte_config.h"

#include <sys/types.h>
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include "opal/util/argv.h"
#include "opal/class/opal_list.h"

#include "orte/mca/errmgr/errmgr.h"
#include "orte/dss/dss.h"

#include "orte/mca/rmaps/base/rmaps_private.h"

/*
 * JOB_MAP
 */
int orte_rmaps_base_print_map(char **output, char *prefix, orte_job_map_t *src, orte_data_type_t type)
{
    char *tmp, *tmp2, *tmp3, *pfx, *pfx2;
    orte_mapped_node_t *srcnode;
    orte_std_cntr_t i;
    opal_list_item_t *item;
    int rc;
    
    /* set default result */
    *output = NULL;

    /* protect against NULL prefix */
    if (NULL == prefix) {
        asprintf(&pfx2, " ");
    } else {
        asprintf(&pfx2, "%s", prefix);
    }

    asprintf(&tmp, "%sMap for job: %ld\tGenerated by mapping mode: %s\n%s\tStarting vpid: %ld\tVpid range: %ld\tNum app_contexts: %ld",
             pfx2, (long)src->job, (NULL == src->mapping_mode) ? "NULL" : src->mapping_mode,
             pfx2, (long)src->vpid_start, (long)src->vpid_range, (long)src->num_apps);

    asprintf(&pfx, "%s\t", pfx2);
    free(pfx2);

    for (i=0; i < src->num_apps; i++) {
        if (ORTE_SUCCESS != (rc = orte_dss.print(&tmp2, pfx, src->apps[i], ORTE_APP_CONTEXT))) {
            ORTE_ERROR_LOG(rc);
            free(pfx);
            free(tmp);
            return rc;
        }
        asprintf(&tmp3, "%s\n%s", tmp, tmp2);
        free(tmp);
        free(tmp2);
        tmp = tmp3;
    }
    
    asprintf(&tmp, "%s\n%sNum elements in nodes list: %ld", tmp3, pfx, (long)src->num_nodes);
    
    for (item = opal_list_get_first(&(src->nodes));
         item != opal_list_get_end(&(src->nodes));
         item = opal_list_get_next(item)) {
        srcnode = (orte_mapped_node_t*)item;
        if (ORTE_SUCCESS != (rc = orte_rmaps_base_print_mapped_node(&tmp2, pfx, srcnode, ORTE_MAPPED_NODE))) {
            ORTE_ERROR_LOG(rc);
            free(pfx);
            free(tmp);
            return rc;
        }
        asprintf(&tmp3, "%s\n%s", tmp, tmp2);
        free(tmp);
        free(tmp2);
        tmp = tmp3;
    }
    
    /* set the return */
    *output = tmp;

    free(pfx);
    return ORTE_SUCCESS;
}


/*
 * MAPPED_PROC
 */
int orte_rmaps_base_print_mapped_proc(char **output, char *prefix, orte_mapped_proc_t *src, orte_data_type_t type)
{
    char *tmp, *tmp2, *tmp3, *pfx, *pfx2;
    int rc;
    
    /* set default result */
    *output = NULL;
    
    /* protect against NULL prefix */
    if (NULL == prefix) {
        asprintf(&pfx2, " ");
    } else {
        asprintf(&pfx2, "%s", prefix);
    }
    
    asprintf(&tmp3, "%sMapped proc:\n%s\tProc Name:", pfx2, pfx2);
    
    asprintf(&pfx, "%s\t", pfx2);
    
    if (ORTE_SUCCESS != (rc = orte_dss.print(&tmp2, pfx, &(src->name), ORTE_NAME))) {
        ORTE_ERROR_LOG(rc);
        free(pfx);
        free(tmp3);
        return rc;
    }
    asprintf(&tmp, "%s\n%s\n%sProc Rank: %ld\tProc PID: %ld\tApp_context index: %ld\n", tmp3, tmp2, pfx,
                                                    (long)src->rank, (long)src->pid, (long)src->app_idx);
    free(tmp2);
    free(tmp3);
    
    /* set the return */
    *output = tmp;
    
    free(pfx);
    free(pfx2);
    return ORTE_SUCCESS;
}

/*
 * MAPPED_NODE
 */
int orte_rmaps_base_print_mapped_node(char **output, char *prefix, orte_mapped_node_t *src, orte_data_type_t type)
{
    int rc;
    char *tmp, *tmp2, *tmp3, *pfx, *pfx2;
    opal_list_item_t *item;
    orte_mapped_proc_t *srcproc;
    
    /* set default result */
    *output = NULL;
    
    /* protect against NULL prefix */
    if (NULL == prefix) {
        asprintf(&pfx2, " ");
    } else {
        asprintf(&pfx2, "%s", prefix);
    }

    asprintf(&tmp, "%sMapped node:\n%s\tCell: %ld\tNodename: %s\tLaunch id: %ld\tUsername: %s\n%s\tDaemon name:", pfx2, pfx2,
             (long)src->cell, (NULL == src->nodename ? "NULL" : src->nodename), (long)src->launch_id,
             (NULL == src->username ? "NULL" : src->username), pfx2);
    
    asprintf(&pfx, "%s\t", pfx2);
    free(pfx2);
    
    if (ORTE_SUCCESS != (rc = orte_dss.print(&tmp2, pfx, src->daemon, ORTE_NAME))) {
        ORTE_ERROR_LOG(rc);
        free(pfx);
        free(tmp);
        return rc;
    }
    
    asprintf(&tmp3, "%s\n\t%s\n%sOversubscribed: %s\tNum elements in procs list: %ld", tmp, tmp2, pfx,
             (src->oversubscribed ? "True" : "False"), (long)src->num_procs);
    free(tmp);
    free(tmp2);
    
   for (item = opal_list_get_first(&(src->procs));
         item != opal_list_get_end(&(src->procs));
         item = opal_list_get_next(item)) {
        srcproc = (orte_mapped_proc_t*)item;
        if (ORTE_SUCCESS != (rc = orte_rmaps_base_print_mapped_proc(&tmp2, pfx, srcproc, ORTE_MAPPED_PROC))) {
            ORTE_ERROR_LOG(rc);
            free(pfx);
            free(tmp);
            return rc;
        }
        asprintf(&tmp, "%s\n%s", tmp3, tmp2);
        free(tmp3);
        free(tmp2);
        tmp3 = tmp;
    }

    /* set the return */
    *output = tmp3;
    
    free(pfx);
    return ORTE_SUCCESS;
}
