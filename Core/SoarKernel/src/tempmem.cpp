#include "portability.h"

/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/*************************************************************************
 *
 *  file:  tempmem.cpp
 *
 * =======================================================================
 *
 *             Temporary Memory and Slot routines
 *
 * =======================================================================
 */

#include <stdlib.h>

#include "tempmem.h"
#include "kernel.h"
#include "symtab.h"

#include "mem.h"
#include "agent.h"
#include "prefmem.h"
#include "decide.h"


/* **********************************************************************

                        Temporary Memory

********************************************************************** */

/* ======================================================================

                        Slot Management Routines

   Find_slot() looks for an existing slot for a given id/attr pair, and
   returns it if found.  If no such slot exists, it returns NIL.
   Make_slot() looks for an existing slot for a given id/attr pair,
   returns it if found, and otherwise creates a new slot and returns it.

   Mark_slot_as_changed() is called by the preference manager whenever
   the preferences for a slot change.  This updates the list of
   changed_slots and highest_goal_whose_context_changed for use by the
   decider.
====================================================================== */

slot* find_slot(Symbol* id, Symbol* attr)
{
    slot* s;
    
    if (!id)
    {
        return NIL;    /* fixes bug #135 kjh */
    }
    for (s = id->id->slots; s != NIL; s = s->next)
        if (s->attr == attr)
        {
            return s;
        }
    return NIL;
}

slot* make_slot(agent* thisAgent, Symbol* id, Symbol* attr)
{
    slot* s;
    int i;
    
    /* JC: Search for a slot first.  If it exists
    *  for the given symbol, then just return it
    */
    for (s = id->id->slots; s != NIL; s = s->next)
    {
        if (s->attr == attr)
        {
            return s;
        }
    }
    
    /* JC: need to create a new slot */
    allocate_with_pool(thisAgent, &thisAgent->slot_pool, &s);
    insert_at_head_of_dll(id->id->slots, s, next, prev);
    
    /* Context slots are goals and operators; operator slots get
     *  created with a goal (see create_new_context).
     */
    if ((id->id->isa_goal) && (attr == thisAgent->operator_symbol))
    {
        s->isa_context_slot = true;
    }
    else
    {
        s->isa_context_slot = false;
    }
    
    s->changed = NIL;
    s->acceptable_preference_changed = NIL;
    s->id = id;
    s->attr = attr;
    symbol_add_ref(thisAgent, id);
    symbol_add_ref(thisAgent, attr);
    s->wmes = NIL;
    s->all_preferences = NIL;
    s->CDPS = NIL;
    
    /* JC: This is the same as all_preferences
     *  except they are indexed by type.
     */
    for (i = 0; i < NUM_PREFERENCE_TYPES; i++)
    {
        s->preferences[i] = NIL;
    }
    
    s->impasse_type = NONE_IMPASSE_TYPE;
    s->impasse_id = NIL;
    s->acceptable_preference_wmes = NIL;
    s->marked_for_possible_removal = false;
    
    s->wma_val_references = NIL;
    
    return s;
}

void mark_slot_as_changed(agent* thisAgent, slot* s)
{
    dl_cons* dc;
    
    if (s->isa_context_slot)
    {
        if (thisAgent->highest_goal_whose_context_changed)
        {
            if (s->id->id->level <
                    thisAgent->highest_goal_whose_context_changed->id->level)
            {
                thisAgent->highest_goal_whose_context_changed = s->id;
            }
        }
        else
        {
            thisAgent->highest_goal_whose_context_changed = s->id;
        }
        s->changed = reinterpret_cast<dl_cons*>(s);   /* just make it nonzero */
    }
    else
    {
        if (! s->changed)
        {
            allocate_with_pool(thisAgent, &thisAgent->dl_cons_pool, &dc);
            dc->item = s;
            s->changed = dc;
            
            insert_at_head_of_dll(thisAgent->changed_slots, dc, next, prev);
        }
    }
}

/* -----------------------------------------------------------------
                      Slot Garbage Collection

   Old slots are garbage collected as follows:  whenever we notice that
   the last preference has been removed from a slot, we call
   mark_slot_for_possible_removal().  We don't deallocate the slot
   right away, because there might still be wmes in it, or we might
   be about to add a new preference to it (through some later action
   of the same production firing, for example).

   At the end of the phase, we call remove_garbage_slots(), which
   scans through each marked slot and garbage collects it if it has
   no wmes or preferences.
----------------------------------------------------------------- */

void mark_slot_for_possible_removal(agent* thisAgent, slot* s)
{
    if (s->marked_for_possible_removal)
    {
        return;
    }
    s->marked_for_possible_removal = true;
    push(thisAgent, s, thisAgent->slots_for_possible_removal);
}

/* MMA 8-2012: Clear out and deallocate the CDPS. */
void clear_CDPS(agent* thisAgent, slot* s)
{

    list* cond_current, *cond_old;
    preference* pref;
    
    /* The CDPS should never exist on a top-level slot, so we do
     * not need to worry about checking for DO_TOP_LEVEL_REF_CTS. */
    
    cond_old = cond_current = s->CDPS;
    s->CDPS = NIL;
    for (; cond_current != NIL; cond_current = cond_current->rest)
    {
        pref = static_cast<preference*>(cond_current->first);
        preference_remove_ref(thisAgent, pref);
    }
    free_list(thisAgent, cond_old);
    
}
/* MMA 8-2012 end */

void remove_garbage_slots(agent* thisAgent)
{
    cons* c;
    slot* s;
    
    while (thisAgent->slots_for_possible_removal)
    {
        c = thisAgent->slots_for_possible_removal;
        thisAgent->slots_for_possible_removal = thisAgent->slots_for_possible_removal->rest;
        s = static_cast<slot_struct*>(c->first);
        free_cons(thisAgent, c);
        
        if (s->wmes || s->all_preferences)
        {
            /* --- don't deallocate it if it still has any wmes or preferences --- */
            s->marked_for_possible_removal = false;
            continue;
        }
        
        /* --- deallocate the slot --- */
#ifdef DEBUG_SLOTS
        print_with_symbols(thisAgent, "\nDeallocate slot %y ^%y", s->id, s->attr);
#endif
        
        /* MMA 9-2012 */
        if (s->CDPS && thisAgent->sysparams[CHUNK_THROUGH_EVALUATION_RULES_SYSPARAM])
        {
            clear_CDPS(thisAgent, s);
        }
        /* MMA 9-2012 end */
        
        if (s->changed && (!s->isa_context_slot))
        {
            remove_from_dll(thisAgent->changed_slots, s->changed, next, prev);
            free_with_pool(&thisAgent->dl_cons_pool, s->changed);
        }
        remove_from_dll(s->id->id->slots, s, next, prev);
        symbol_remove_ref(thisAgent, s->id);
        symbol_remove_ref(thisAgent, s->attr);
        if (s->wma_val_references != NIL)
        {
            s->wma_val_references->~wma_sym_reference_map();
            free_with_pool(&(thisAgent->wma_slot_refs_pool), s->wma_val_references);
            s->wma_val_references = NIL;
        }
        free_with_pool(&thisAgent->slot_pool, s);
    }
}

