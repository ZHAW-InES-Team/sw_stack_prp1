/********************************************************************
*  
*  Copyright (c) 2007, Institute of Embedded Systems at 
*                      Zurich University of Applied Sciences 
*                      (http://ines.zhaw.ch)
*  
*  All rights reserved.
* 
* 
*  Redistribution and use in source and binary forms, with or  
*  without modification, are permitted provided that the 
*  following conditions are met:
*  
*  - Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer. 
*
*  - Redistributions in binary form must reproduce the above 
*    copyright notice, this list of conditions and the following 
*    disclaimer in the documentation and/or other materials provided
*    with the distribution. 
*
*  - Neither the name of the Zurich University of Applied Sciences
*    nor the names of its contributors may be used to endorse or 
*    promote products derived from this software without specific 
*    prior written permission. 
*
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
*  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
*  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
*  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
*  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*  
*********************************************************************/


/********************************************************************
*   _____       ______  _____                                       *
*  |_   _|     |  ____|/ ____|                                      *
*    | |  _ __ | |__  | (___    Institute of Embedded Systems       *
*    | | | '_ \|  __|  \___ \   Zurich University of Applied        *
*   _| |_| | | | |____ ____) |  Sciences                            *
*  |_____|_| |_|______|_____/   8401 Winterthur, Switzerland        *
*                                                                   *
*********************************************************************
* 
*  Project     : Parallel Redundancy Protocol
* 
*  Version     : 1.0
*  Author      : Sven Meier
* 
*********************************************************************
*  Change History
*
*  Date     | Name     | Modification
************|**********|*********************************************
*  17.12.07 | mesv     | file created
************|**********|*********************************************
*  14.01.08 | mesv     | added some comments
*********************************************************************/

#include "PRP_NodeTable_T.h"
#include "PRP_LogItf_T.h"
#include "PRP_Node_T.h"
#include "PRP_T.h"

/**
 * @fn void PRP_NodeTable_T_print(PRP_NodeTable_T* const me, uinteger32 level)
 * @brief Print the PRP_NodeTable status information
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   level uinteger32 importance
 */
void PRP_NodeTable_T_print(PRP_NodeTable_T* const me, uinteger32 level)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    PRP_PRP_LOGOUT(level, "%s\n", "====NodeTable=======================");
    PRP_PRP_LOGOUT(level, "cnt_nodes_:\t%u\n", me->cnt_nodes_);
    PRP_PRP_LOGOUT(level, "node_table_empty_:\t%u\n", me->node_table_empty_);
    PRP_PRP_LOGOUT(level, "%s\n", "====================================");
}

/**
 * @fn void PRP_NodeTable_T_set_mac_address_A(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac)
 * @brief Set mac address A
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T pointer to the node
 * @param   mac octet array[6] to the mac address
 */
void PRP_NodeTable_T_set_mac_address_A(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    if(node == NULL_PTR)
    {
        return;
    }

    if(mac == NULL_PTR)
    {
        return;
    }

    prp_memcpy(node->mac_address_A_, mac, PRP_ETH_ADDR_LENGTH);
}

/**
 * @fn void PRP_NodeTable_T_set_mac_address_B(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac)
 * @brief Set mac address B
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T pointer to the node
 * @param   mac octet array[6] to the mac address
 */
void PRP_NodeTable_T_set_mac_address_B(PRP_NodeTable_T* const me, PRP_Node_T* node, octet* mac)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    if(node == NULL_PTR)
    {
        return;
    }

    if(mac == NULL_PTR)
    {
        return;
    }

    prp_memcpy(node->mac_address_B_, mac, PRP_ETH_ADDR_LENGTH);
}

/**
 * @fn  PRP_Node_T* PRP_NodeTable_T_add_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
 * @brief Creates a new node with the values passed as arguments
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T to add to the table. Mac address A or B in the
 *          node structure are mandatory
 * @return  PRP_Node_T !NULL : OK (pointer to the memory of the new added node)
 *          PRP_Node_T NULL : ERROR
 */
PRP_Node_T* PRP_NodeTable_T_add_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
{
    PRP_Node_T* new_node;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    if(node == NULL_PTR)
    {
        return(NULL_PTR);
    }

    new_node = PRP_Node_T_create(); /* create a new node */

    if(new_node == NULL_PTR) /* memory full? */
    {
        return(NULL_PTR);
    }

    prp_memcpy(new_node, node, sizeof(PRP_Node_T)); /* copie the values to the new node */

    /* here you could add the nodes sorted, to make a binary search or so */
    if(me->last_node_ == NULL_PTR) /* table empty ? */
    {
        me->first_node_ = new_node; /* the first entry in table */
    }
    else
    {
        me->last_node_->next_node_ = new_node; /* reconnect nodes */
    }

    new_node->previous_node_ = me->last_node_;  /* added to the end */
    new_node->next_node_ = NULL_PTR;            /* added to the end */
    me->last_node_ = new_node;                  /* new last entry in the table */
    me->cnt_nodes_++;                           /* increment the number of entries in the table */
    me->node_table_empty_ = FALSE;              /* table not empty for sure */

    return(new_node);
}

/**
 * @fn void PRP_NodeTable_T_remove_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
 * @brief Removes the node passed as argument from the table.
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T pointer to the node to remove
 */
void PRP_NodeTable_T_remove_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    if(node == NULL_PTR)
    {
        return;
    }

    PRP_T_delete_node_table_entry(node);

    /* was deleted node first node in table? */
    if(node->previous_node_ == NULL_PTR)
    {
        me->first_node_ = node->next_node_; /* new first entry */
    }
    else
    {
        node->previous_node_->next_node_ = node->next_node_; /* reconnect nodes */
    }

    /* was deleted node last node in table? */
    if(node->next_node_ == NULL_PTR)
    {
        me->last_node_ = node->previous_node_; /* new last entry */
    }
    else
    {
        node->next_node_->previous_node_ = node->previous_node_; /* reconnect nodes */
    }

    me->cnt_nodes_--; /* decrement the number of entries in the table */
    if(me->cnt_nodes_ <= 0) /* check if table is empty */
    {
        me->node_table_empty_ = TRUE;
    }


    PRP_Node_T_destroy(node); /* Release memory */

    node = NULL_PTR;
}

/**
 * @fn PRP_Node_T* PRP_NodeTable_T_get_node(PRP_NodeTable_T* const me, octet* node_mac)
 * @brief Searches for a node in the table which matches the mac address
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node_mac octet pointer to the node's mac address
 * @return  PRP_Node_T !NULL : OK (pointer to the node)
 *          PRP_Node_T NULL : ERROR
 *
 * This function searches for a node in the table which matches the mac
 * address passed as argument. If found returns a pointer to the Node object
 * else NULL.
 */
PRP_Node_T* PRP_NodeTable_T_get_node(PRP_NodeTable_T* const me, octet* node_mac)
{
    PRP_Node_T* node;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    if(node_mac == NULL_PTR)
    {
        return(NULL_PTR);
    }

    /* here you could replace the linear search with a binary search or so */
    node = me->first_node_; /* beginning of the table */

    while(NULL_PTR != node) /* check in worst case whole table, linear search*/
    {
        if((0 == prp_memcmp(node->mac_address_A_, node_mac, PRP_ETH_ADDR_LENGTH)) ||
            (0 == prp_memcmp(node->mac_address_B_, node_mac, PRP_ETH_ADDR_LENGTH))) /* check if mac are equal => found */
        {
            return(node);
        }

        node = node->next_node_; /* go to the next node */
    }

    return(NULL_PTR); /* not found */
}

/**
 * @fn PRP_Node_T* PRP_NodeTable_T_get_first_node(PRP_NodeTable_T* const me)
 * @brief Returns a pointer to the first Node object in the table
 * 
 * @param   me PRP_NodeTable_T this pointer
 * @return  PRP_Node_T !NULL : OK (pointer to the node)
 *          PRP_Node_T NULL : ERROR (or table is empty)
 */
PRP_Node_T* PRP_NodeTable_T_get_first_node(PRP_NodeTable_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    return(me->first_node_);
}

/**
 * @fn PRP_Node_T* PRP_NodeTable_T_get_last_node(PRP_NodeTable_T* const me)
 * @brief Returns a pointer to the last Node object in the table
 *
 * @param   me PRP_NodeTable_T this pointer
 * @return  PRP_Node_T !NULL : OK (pointer to the node)
 *          PRP_Node_T NULL : ERROR (or table is empty)
 */
PRP_Node_T* PRP_NodeTable_T_get_last_node(PRP_NodeTable_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    return(me->last_node_);
}

/**
 * @fn PRP_Node_T* PRP_NodeTable_T_get_previous_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
 * @brief Returns a pointer to the previous Node object
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T pointer to the node
 * @return  PRP_Node_T !NULL : OK (pointer to the node)
 *          PRP_Node_T NULL : ERROR (or start of table)
 *
 * This function returns a pointer to the next Node object from the current
 * object, passed as argument. Node that is passed has to be received by
 * previously call get_first or get_last node (and call get_next or
 * get_previous node)
 */
PRP_Node_T* PRP_NodeTable_T_get_previous_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    if(node == NULL_PTR)
    {
        return(NULL_PTR);
    }

    return(node->previous_node_);
}

/**
 * @fn PRP_Node_T* PRP_NodeTable_T_get_next_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
 * @brief Returns a pointer to the next Node object.
 *
 * @param   me PRP_NodeTable_T this pointer
 * @param   node PRP_Node_T pointer to the node
 * @return  PRP_Node_T !NULL : OK (pointer to the node)
 *          PRP_Node_T NULL : ERROR (or end of table)
 *
 * This function returns a pointer to the next Node object from the current
 * object, passed as argument. Node that is passed has to be received by
 * previously call get_first or get_last node (and call get_next or
 * get_previous node)
 */
PRP_Node_T* PRP_NodeTable_T_get_next_node(PRP_NodeTable_T* const me, PRP_Node_T* node)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return(NULL_PTR);
    }

    if(node == NULL_PTR)
    {
        return(NULL_PTR);
    }

    return(node->next_node_);
}

/**
 * @fn void PRP_NodeTable_T_init(PRP_NodeTable_T* const me)
 * @brief Initialize the PRP_NodeTable interface
 *
 * @param   me PRP_NodeTable_T this pointer
 */
void PRP_NodeTable_T_init(PRP_NodeTable_T* const me)
{
    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    me->first_node_ = NULL_PTR;
    me->last_node_ = NULL_PTR;
    me->cnt_nodes_ = 0;
    me->node_table_empty_ = TRUE;
}

/**
 * @fn void PRP_NodeTable_T_cleanup(PRP_NodeTable_T* const me)
 * @brief Clean up the PRP_NodeTable interface
 *
 * @param   me PRP_NodeTable_T this pointer
 */
void PRP_NodeTable_T_cleanup(PRP_NodeTable_T* const me)
{
    PRP_Node_T* node;

    PRP_PRP_LOGOUT(3, "[%s] entering \n", __FUNCTION__);

    if(me == NULL_PTR)
    {
        return;
    }

    node = me->first_node_; /* beginning of the table */

    while(NULL_PTR != node) /* go through whole list */
    {
        /* deletes first node and sets the new first pointer*/
        PRP_NodeTable_T_remove_node(me, node);
        node = me->first_node_; /* new beginning of the table */
    }
}

