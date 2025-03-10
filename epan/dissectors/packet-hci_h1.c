/* packet-hci_h1.c
 * Routines for the Bluetooth HCI H1 dissection
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "config.h"

#include <epan/packet.h>
#include <wiretap/wtap.h>

#include "packet-bluetooth.h"

static int proto_hci_h1;

static int hf_hci_h1_direction;

static int ett_hci_h1;

static dissector_table_t hci_h1_table;

static dissector_handle_t hci_h1_handle;

static const value_string hci_h1_type_vals[] = {
    {BTHCI_CHANNEL_COMMAND, "HCI Command"},
    {BTHCI_CHANNEL_ACL,     "ACL Data"},
    {BTHCI_CHANNEL_SCO,     "SCO Data"},
    {BTHCI_CHANNEL_EVENT,   "HCI Event"},
    {BTHCI_CHANNEL_ISO,     "ISO Data"},
    {0, NULL }
};
static const value_string hci_h1_direction_vals[] = {
    {-1, "Unknown"},
    {0,    "Sent"},
    {1,    "Rcvd"},
    {0, NULL}
};

void proto_register_hci_h1(void);
void proto_reg_handoff_hci_h1(void);

static int
dissect_hci_h1(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data)
{
    uint8_t            type;
    tvbuff_t          *next_tvb;
    proto_item        *ti = NULL;
    proto_tree        *hci_h1_tree = NULL;
    bluetooth_data_t  *bluetooth_data;

    bluetooth_data = (bluetooth_data_t *) data;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "HCI");

    col_clear(pinfo->cinfo, COL_INFO);

    DISSECTOR_ASSERT(bluetooth_data->previous_protocol_data_type == BT_PD_BTHCI);
    type = bluetooth_data->previous_protocol_data.bthci->channel;

    if (tree) {
        ti = proto_tree_add_item(tree, proto_hci_h1, tvb, 0, 0, ENC_NA);
        hci_h1_tree = proto_item_add_subtree(ti, ett_hci_h1);

        if(pinfo->p2p_dir == P2P_DIR_SENT ||
           pinfo->p2p_dir == P2P_DIR_RECV)
            proto_item_append_text(hci_h1_tree, " %s %s",
                           val_to_str(pinfo->p2p_dir,
                              hci_h1_direction_vals, "Unknown: %d"),
                           val_to_str(type,
                              hci_h1_type_vals,
                              "Unknown 0x%02x"));
        else
            proto_item_append_text(hci_h1_tree, " %s",
                           val_to_str(type,
                              hci_h1_type_vals,
                              "Unknown 0x%02x"));
    }

    if(pinfo->p2p_dir == P2P_DIR_SENT ||
       pinfo->p2p_dir == P2P_DIR_RECV)
        col_add_fstr(pinfo->cinfo, COL_INFO, "%s %s",
                 val_to_str(pinfo->p2p_dir,
                    hci_h1_direction_vals, "Unknown: %d"),
                     val_to_str(type, hci_h1_type_vals,
                    "Unknown 0x%02x"));
    else
        col_add_str(pinfo->cinfo, COL_INFO,
                 val_to_str(type, hci_h1_type_vals,
                    "Unknown 0x%02x"));

    ti = proto_tree_add_int(hci_h1_tree, hf_hci_h1_direction, tvb, 0, 0, pinfo->p2p_dir);
    proto_item_set_generated(ti);

    next_tvb = tvb_new_subset_remaining(tvb, 0);
    if (!dissector_try_uint_with_data(hci_h1_table, type, next_tvb, pinfo, tree, true, bluetooth_data)) {
        call_data_dissector(next_tvb, pinfo, tree);
    }

    return tvb_reported_length(tvb);
}


void
proto_register_hci_h1(void)
{
    static hf_register_info hf[] = {
        { &hf_hci_h1_direction,
            { "Direction",           "hci_h1.direction",
            FT_INT8, BASE_DEC, VALS(hci_h1_direction_vals), 0x0,
            "HCI Packet Direction Sent/Rcvd/Unknown", HFILL }
        }
    };

    static int *ett[] = {
        &ett_hci_h1,
    };

    proto_hci_h1 = proto_register_protocol("Bluetooth HCI H1",
            "HCI_H1", "hci_h1");

    hci_h1_handle = register_dissector("hci_h1", dissect_hci_h1, proto_hci_h1);

    proto_register_field_array(proto_hci_h1, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    hci_h1_table = register_dissector_table("hci_h1.type",
            "HCI h1 pdu type", proto_hci_h1, FT_UINT8, BASE_HEX);
}

void
proto_reg_handoff_hci_h1(void)
{
    dissector_add_uint("bluetooth.encap", WTAP_ENCAP_BLUETOOTH_HCI, hci_h1_handle);
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
