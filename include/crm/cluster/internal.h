/*
 * Copyright (C) 2004 Andrew Beekhof <andrew@beekhof.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CRM_CLUSTER_INTERNAL__H
#  define CRM_CLUSTER_INTERNAL__H

#  include <crm/cluster.h>

#  define AIS_IPC_NAME  "ais-crm-ipc"
#  define AIS_IPC_MESSAGE_SIZE 8192*128
#  define CRM_MESSAGE_IPC_ACK	0

typedef struct crm_ais_host_s AIS_Host;
typedef struct crm_ais_msg_s AIS_Message;

enum crm_ais_msg_class {
    crm_class_cluster = 0,
    crm_class_members = 1,
    crm_class_notify  = 2,
    crm_class_nodeid  = 3,
    crm_class_rmpeer  = 4,
    crm_class_quorum  = 5,
};

/* order here matters - its used to index into the crm_children array */
enum crm_ais_msg_types {
    crm_msg_none     = 0,
    crm_msg_ais      = 1,
    crm_msg_lrmd     = 2,
    crm_msg_cib      = 3,
    crm_msg_crmd     = 4,
    crm_msg_attrd    = 5,
    crm_msg_stonithd = 6,
    crm_msg_te       = 7,
    crm_msg_pe       = 8,
    crm_msg_stonith_ng = 9,
};

struct crm_ais_host_s {
    uint32_t id;
    uint32_t pid;
    gboolean local;
    enum crm_ais_msg_types type;
    uint32_t size;
    char uname[MAX_NAME];

} __attribute__ ((packed));

struct crm_ais_msg_s {
    cs_ipc_header_response_t header __attribute__ ((aligned(8)));
    uint32_t id;
    gboolean is_compressed;

    AIS_Host host;
    AIS_Host sender;

    uint32_t size;
    uint32_t compressed_size;
    /* 584 bytes */
    char data[0];

} __attribute__ ((packed));

struct crm_ais_nodeid_resp_s {
    cs_ipc_header_response_t header __attribute__ ((aligned(8)));
    uint32_t id;
    uint32_t counter;
    char uname[MAX_NAME];
    char cname[MAX_NAME];
} __attribute__ ((packed));

struct crm_ais_quorum_resp_s {
    cs_ipc_header_response_t header __attribute__ ((aligned(8)));
    uint64_t id;
    uint32_t votes;
    uint32_t expected_votes;
    uint32_t quorate;
} __attribute__ ((packed));

static inline enum crm_proc_flag
text2proc(const char *proc)
{
    /* We only care about these two so far */

    if(proc && strcmp(proc, "cib") == 0) {
        return crm_proc_cib;
    } else if(proc && strcmp(proc, "crmd") == 0) {
        return crm_proc_crmd;
    }
    
    return crm_proc_none;
}


static inline const char *
ais_dest(const struct crm_ais_host_s *host)
{
    if (host->local) {
        return "local";
    } else if (host->size > 0) {
        return host->uname;
    } else {
        return "<all>";
    }
}

#  define ais_data_len(msg) (msg->is_compressed?msg->compressed_size:msg->size)

static inline AIS_Message *
ais_msg_copy(const AIS_Message * source)
{
    AIS_Message *target = malloc(sizeof(AIS_Message) + ais_data_len(source));

    memcpy(target, source, sizeof(AIS_Message));
    memcpy(target->data, source->data, ais_data_len(target));

    return target;
}

static inline const char *
ais_error2text(int error)
{
    const char *text = "unknown";

#  if SUPPORT_COROSYNC
    switch (error) {
        case CS_OK:
            text = "None";
            break;
        case CS_ERR_LIBRARY:
            text = "Library error";
            break;
        case CS_ERR_VERSION:
            text = "Version error";
            break;
        case CS_ERR_INIT:
            text = "Initialization error";
            break;
        case CS_ERR_TIMEOUT:
            text = "Timeout";
            break;
        case CS_ERR_TRY_AGAIN:
            text = "Try again";
            break;
        case CS_ERR_INVALID_PARAM:
            text = "Invalid parameter";
            break;
        case CS_ERR_NO_MEMORY:
            text = "No memory";
            break;
        case CS_ERR_BAD_HANDLE:
            text = "Bad handle";
            break;
        case CS_ERR_BUSY:
            text = "Busy";
            break;
        case CS_ERR_ACCESS:
            text = "Access error";
            break;
        case CS_ERR_NOT_EXIST:
            text = "Doesn't exist";
            break;
        case CS_ERR_NAME_TOO_LONG:
            text = "Name too long";
            break;
        case CS_ERR_EXIST:
            text = "Exists";
            break;
        case CS_ERR_NO_SPACE:
            text = "No space";
            break;
        case CS_ERR_INTERRUPT:
            text = "Interrupt";
            break;
        case CS_ERR_NAME_NOT_FOUND:
            text = "Name not found";
            break;
        case CS_ERR_NO_RESOURCES:
            text = "No resources";
            break;
        case CS_ERR_NOT_SUPPORTED:
            text = "Not supported";
            break;
        case CS_ERR_BAD_OPERATION:
            text = "Bad operation";
            break;
        case CS_ERR_FAILED_OPERATION:
            text = "Failed operation";
            break;
        case CS_ERR_MESSAGE_ERROR:
            text = "Message error";
            break;
        case CS_ERR_QUEUE_FULL:
            text = "Queue full";
            break;
        case CS_ERR_QUEUE_NOT_AVAILABLE:
            text = "Queue not available";
            break;
        case CS_ERR_BAD_FLAGS:
            text = "Bad flags";
            break;
        case CS_ERR_TOO_BIG:
            text = "To big";
            break;
        case CS_ERR_NO_SECTIONS:
            text = "No sections";
            break;
    }
#  endif
    return text;
}

static inline const char *
msg_type2text(enum crm_ais_msg_types type)
{
    const char *text = "unknown";

    switch (type) {
        case crm_msg_none:
            text = "unknown";
            break;
        case crm_msg_ais:
            text = "ais";
            break;
        case crm_msg_cib:
            text = "cib";
            break;
        case crm_msg_crmd:
            text = "crmd";
            break;
        case crm_msg_pe:
            text = "pengine";
            break;
        case crm_msg_te:
            text = "tengine";
            break;
        case crm_msg_lrmd:
            text = "lrmd";
            break;
        case crm_msg_attrd:
            text = "attrd";
            break;
        case crm_msg_stonithd:
            text = "stonithd";
            break;
        case crm_msg_stonith_ng:
            text = "stonith-ng";
            break;
    }
    return text;
}

enum crm_ais_msg_types text2msg_type(const char *text);
char *get_ais_data(const AIS_Message * msg);
gboolean check_message_sanity(const AIS_Message * msg, const char *data);


#  if SUPPORT_HEARTBEAT
extern ll_cluster_t *heartbeat_cluster;
gboolean send_ha_message(ll_cluster_t * hb_conn, xmlNode * msg,
                                const char *node, gboolean force_ordered);
gboolean ha_msg_dispatch(ll_cluster_t * cluster_conn, gpointer user_data);

gboolean register_heartbeat_conn(ll_cluster_t * hb_cluster, char **uuid, char **uname,
                                        void (*hb_message) (HA_Message * msg, void *private_data),
                                        void (*hb_destroy) (gpointer user_data));
xmlNode *convert_ha_message(xmlNode * parent, HA_Message *msg, const char *field);
gboolean ccm_have_quorum(oc_ed_t event);
const char *ccm_event_name(oc_ed_t event);
crm_node_t *crm_update_ccm_node(const oc_ev_membership_t * oc, int offset, const char *state,
                                       uint64_t seq);
#  endif

#  if SUPPORT_COROSYNC

gboolean send_ais_message(xmlNode * msg, gboolean local,
                          const char *node, enum crm_ais_msg_types dest);

enum cluster_type_e find_corosync_variant(void);

void terminate_ais_connection(void);
gboolean init_ais_connection(gboolean(*dispatch) (AIS_Message *, char *, int),
                             void (*destroy) (gpointer), char **our_uuid, char **our_uname,
                             int *nodeid);
gboolean init_ais_connection_once(gboolean(*dispatch) (AIS_Message *, char *, int),
                                         void (*destroy) (gpointer), char **our_uuid,
                                         char **our_uname, int *nodeid);

#  endif

enum crm_quorum_source {
    crm_quorum_cman,
    crm_quorum_corosync,
    crm_quorum_pacemaker,
};

enum crm_quorum_source get_quorum_source(void);

void crm_update_peer_proc(const char *source, crm_node_t *peer, uint32_t flag, const char *status);

crm_node_t *crm_update_peer(
    const char *source, unsigned int id, uint64_t born, uint64_t seen,
    int32_t votes, uint32_t children, const char *uuid, const char *uname,
    const char *addr, const char *state);

gboolean init_cman_connection(
    gboolean(*dispatch) (unsigned long long, gboolean), void (*destroy) (gpointer));

gboolean init_quorum_connection(
    gboolean(*dispatch) (unsigned long long, gboolean), void (*destroy) (gpointer));

#endif
